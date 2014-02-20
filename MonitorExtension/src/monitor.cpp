#include "hphp/runtime/base/base-includes.h"
#include "hphp/runtime/base/stream-wrapper-registry.h"
#include <hphp/runtime/vm/event-hook.h>
#include "ExternalEventsRporter.h"
#include "hphp/runtime/monitor/monitor.h"
#include "ZendTraceCollector.h"
#include "hphp/runtime/base/class-info.h"
#include <iostream>
#include <fstream>

namespace HPHP
{
const StaticString
  s_class("class"),
          s_function("function"),
          s_file("file"),
          s_type("type"),
          s_line("line");

// Statics needed here
class monitorExtension : public Extension
{
protected:
  static ZendTraceCollector m_collector;
  
protected:
  /**
   * @brief return the current function name from the FP 
   */
  static const char* getFunctionName(const HPHP::ActRec* ar, int funcType) {
    const char* name;
    switch (funcType) {
    case EventHook::NormalFunc:
      name = ar->m_func->fullName()->data();
      if (name[0] == '\0') {
        // We're evaling some code for internal purposes, most
        // likely getting the default value for a function parameter
        name = "{main}";
      }
      break;
    case EventHook::PseudoMain:
      name = makeStaticString(
               std::string("run_init::") + ar->m_func->unit()->filepath()->data())
             ->data();
      break;
    case EventHook::Eval:
      name = "_";
      break;
    default:
      not_reached();
    }
    return name;
  }
  
  /**
   * @brief return file,line pair from activation record
   */
  static void getZendFunctionInfo(const HPHP::ActRec* ar, int funcType, ZendFunctionInfo &zfi) {
    
    ClassInfo::MethodInfo info;
    ar->m_func->getFuncInfo( &info );
    zfi.m_linenumber = info.line1; // line1: start of function, line2: end of function
    
    // parepare the arguments field
    zfi.m_args += "(";
    for(size_t i=0; i<info.parameters.size(); ++i) {
      const ClassInfo::ParameterInfo *param = info.parameters.at(i);
      if ( param->attribute & ClassInfo::IsReference ) {
        zfi.m_args += "&";
      }
      zfi.m_args += "$";
      zfi.m_args += param->name;
      zfi.m_args += ", ";
    }
    
    if ( !info.parameters.empty() ) {
      zfi.m_args.erase(zfi.m_args.length()-2, 2); // remove the ", " we added earlier
    }
    zfi.m_args += ")";

    const Func* foo = ar->func();
    if ( foo && foo->unit() && foo->unit()->filepath() ) {
      zfi.m_filename = foo->unit()->filepath()->data();
    }
    zfi.m_name = getFunctionName(ar, funcType);
  }
  
  /**
   * @brief extract a Zend Server usable backtrace information from an extended exception class
   */
  static void extractBacktraceFromExpcetion(const HPHP::ExtendedException& exc, Zend::BacktraceList_t& bt) {
    Array stackTrace = exc.getBackTrace();
    int i = 0;
    for (ArrayIter it(stackTrace); it; ++it, ++i) {
      Zend::EventStackEntry stackEntry;
      CArrRef frame = it.second().toArray();
      if (frame.exists(s_function)) {
        stackEntry._functionName = frame[s_function].toString().c_str();
        if (frame.exists(s_class)) {
          stackEntry._objectName = frame[s_class].toString().c_str();
        }
      }
      stackEntry._fileName = frame[s_file].toString().c_str();
      stackEntry._line     = frame[s_line].toInt64();
      stackEntry._depth    = i;
      bt.push_back( stackEntry );
    }

    Zend::BacktraceList_t::reverse_iterator iter = bt.rbegin();
    std::string savedFunc = "main";
    std::string savedClass;
    for(; iter != bt.rend(); ++iter) {
      iter->_objectName.swap( savedClass );
      iter->_functionName.swap( savedFunc );
    }
  }


public:
  monitorExtension() : Extension("monitor", "1.0 Release") {}
  virtual void moduleInit() {
    // HHVM_FE(hw_print);
    loadSystemlib();
    EventHook::Enable();
    fprintf(stderr, "Zend Monitor Extension loaded\n");
    
    // Initialize Zend event reporting
    Zend::InitializeReporting();
    
    // Register our callbacks
    ZendMonitor::setPfnFatalError   ( &monitorExtension::onFatalError    );
    ZendMonitor::setPfnFunctionEnter( &monitorExtension::onFunctionEnter );
    ZendMonitor::setPfnFunctionLeave( &monitorExtension::onFunctionLeave );
  }
  
  virtual void requestInit() {
    
    // Perform Request Init
    m_collector.clear();
    
    // enable collection
    m_collector.setCollecting(true);
    
  }
  
  virtual void requestShutdown() {
    Zend::SendEvents(g_vmContext->getRequestUrl(), "");
    
    // print to file
    std::ofstream file("/tmp/hhvm.trace", std::ios::trunc | std::ios::binary | std::ios::out );
    m_collector.dump( file );
    file.close();
    
    m_collector.clear(); // will also stop collecting
  }

  /**
   * @brief will get called by HHVM::ZendMonitor
   * @param exc
   */
  static void onFatalError( const Exception* exc ) {
    Zend::BacktraceList_t stackTrace;
    const ExtendedException* extExc = dynamic_cast<const ExtendedException*>( exc );
    if ( extExc ) {
      extractBacktraceFromExpcetion( *extExc, stackTrace );
    }
    Zend::ReportZendErrorEvent( Zend::kERROR, stackTrace, exc->getMessage());
    
    // Notify tracer
    m_collector.onFatalError( exc->getMessage() );
    
    // Dump what we got so far
    std::ofstream file("/tmp/hhvm.trace", std::ios::trunc | std::ios::binary | std::ios::out );
    m_collector.dump( file );
    file.close();
        
    // disable collection until next r-init
    m_collector.setCollecting(false);
  }
  
  /**
   * @brief called by HHVM whenever a function is entered
   */
  static void onFunctionEnter(const ActRec* ar, int funcType ) {
    if ( m_collector.isCollecting() ) {
      ZendFunctionInfo zfi;
      getZendFunctionInfo(ar, funcType, zfi);
      m_collector.enterFunction( zfi );
    }
  }

  /**
   * @brief called by HHVM whenever a function is exit
   * Note that in some cases this handler might not get called (e.g. onFatalError)
   */
  static void onFunctionLeave(const ActRec* ar, int funcType ) {
    if ( m_collector.isCollecting() ) {
      ZendFunctionInfo zfi;
      getZendFunctionInfo(ar, funcType, zfi);
      m_collector.leaveFunction( zfi );
    }
  }

} s_monitor_extension;

// static initialization
HPHP::ZendTraceCollector monitorExtension::m_collector;

// Uncomment for non-bundled module
HHVM_GET_MODULE(monitor);

//////////////////////////////////////////////////////////////////////////////
} // namespace HPHP

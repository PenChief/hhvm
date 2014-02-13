#include "hphp/runtime/base/base-includes.h"
#include "hphp/runtime/base/stream-wrapper-registry.h"
#include <hphp/runtime/vm/event-hook.h>
#include "ExternalEventsRporter.h"
#include "hphp/runtime/monitor/monitor.h"

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
        name = "{internal}";
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
    ZendMonitor::setFatalErrorHandler( &monitorExtension::onFatalError );

  }
  virtual void requestInit() {
    // Perform Request Init
  }
  virtual void requestShutdown() {
    Zend::SendEvents(g_vmContext->getRequestUrl(), "");
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
  }


} s_monitor_extension;

// Uncomment for non-bundled module
HHVM_GET_MODULE(monitor);

//////////////////////////////////////////////////////////////////////////////
} // namespace HPHP

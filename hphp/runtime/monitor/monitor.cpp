#include "monitor.h"
#include "hphp/runtime/vm/event-hook.h"
#include "hphp/runtime/ext/ext_error.h"
#include "hphp/runtime/base/extended-logger.h"
#include <ExternalEventsRporter.h>

const HPHP::StaticString
  s_class("class"),
  s_function("function"),
  s_file("file"),
  s_type("type"),
  s_line("line");

const char* HPHP::ZendMonitor::getFunctionName(const HPHP::ActRec* ar, int funcType)
{
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
#define SAFE_CSTR(s) (s.c_str() == NULL ? "NULL" : s.c_str() )

void HPHP::ZendMonitor::extractBacktraceFromExpcetion(const HPHP::ExtendedException& exc, Zend::BacktraceList_t& bt)
{
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

void HPHP::ZendMonitor::onFatalError(const HPHP::Exception* exc)
{
  // try to extract the backtrace from the exception
  Zend::BacktraceList_t stackTrace;
  const ExtendedException* extExc = dynamic_cast<const ExtendedException*>( exc );
  if ( extExc ) {
    extractBacktraceFromExpcetion( *extExc, stackTrace );
  }
  Zend::ReportZendErrorEvent( Zend::kERROR, stackTrace, exc->getMessage());
}

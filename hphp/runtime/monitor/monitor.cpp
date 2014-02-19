#include "monitor.h"
#include "hphp/runtime/vm/event-hook.h"
#include "hphp/runtime/ext/ext_error.h"
#include "hphp/runtime/base/extended-logger.h"

// static initialization
HPHP::ZendMonitor::PFUNC_ERROR_HANDLER        HPHP::ZendMonitor::m_pfnFatalError = NULL;
HPHP::ZendMonitor::PFUNC_FUNCTION_LEAVE_ENTER HPHP::ZendMonitor::m_pfnFunctionEnter = NULL;
HPHP::ZendMonitor::PFUNC_FUNCTION_LEAVE_ENTER HPHP::ZendMonitor::m_pfnFunctionLeave = NULL;

void HPHP::ZendMonitor::onFatalError(const HPHP::Exception* exc)
{
  // try to extract the backtrace from the exception
  if ( m_pfnFatalError ) {
    m_pfnFatalError( exc );
  }
}

void HPHP::ZendMonitor::onFunctionEnter(const HPHP::ActRec* ar, int funcType)
{
  if ( m_pfnFunctionEnter ) {
    m_pfnFunctionEnter(ar, funcType);
  }
}

void HPHP::ZendMonitor::onFunctionExit(const HPHP::ActRec* ar, int funcType)
{
  if ( m_pfnFunctionLeave ) {
    m_pfnFunctionLeave(ar, funcType);
  }
}

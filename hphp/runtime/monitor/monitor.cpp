#include "monitor.h"
#include "hphp/runtime/vm/event-hook.h"
#include "hphp/runtime/ext/ext_error.h"
#include "hphp/runtime/base/extended-logger.h"

HPHP::ZendMonitor::PFUNC_ERROR_HANDLER HPHP::ZendMonitor::m_pfnFatalError = NULL;

void HPHP::ZendMonitor::onFatalError(const HPHP::Exception* exc)
{
  // try to extract the backtrace from the exception
  if ( m_pfnFatalError ) {
    m_pfnFatalError( exc );
  }
}

void HPHP::ZendMonitor::setFatalErrorHandler(PFUNC_ERROR_HANDLER pfn)
{
  m_pfnFatalError = pfn;
}

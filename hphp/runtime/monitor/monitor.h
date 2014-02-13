#ifndef MONITOR_H
#define MONITOR_H

#include "hphp/runtime/base/execution-context.h"
#include "hphp/runtime/vm/bytecode.h"
#include "hphp/runtime/base/rds.h"
#include "hphp/runtime/base/exceptions.h"
#include <atomic>

namespace HPHP
{
class ZendMonitor
{
public:
  typedef void (*PFUNC_ERROR_HANDLER)(const HPHP::Exception* exc);
  
protected:
  static PFUNC_ERROR_HANDLER m_pfnFatalError;
  

public:
  /**
   * @brief callback function will called by HHVM when a fatal error occurs
   */
  static void onFatalError(const HPHP::Exception* exc) ;
  
  /**
   * @brief set a user error handler function
   */
  static void setFatalErrorHandler(PFUNC_ERROR_HANDLER pfn);
};

}

#endif // MONITOR_H

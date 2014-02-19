#ifndef MONITOR_H
#define MONITOR_H

#include "hphp/runtime/base/execution-context.h"
#include "hphp/runtime/vm/bytecode.h"
#include "hphp/runtime/base/rds.h"
#include "hphp/runtime/base/exceptions.h"
#include <atomic>

class Profiler;
namespace HPHP
{
class ZendMonitor
{
public:
  typedef void (*PFUNC_ERROR_HANDLER)(const HPHP::Exception* exc);
  typedef void (*PFUNC_FUNCTION_LEAVE_ENTER)(const HPHP::ActRec* ar, int funcType);

protected:
  static PFUNC_ERROR_HANDLER        m_pfnFatalError;
  static PFUNC_FUNCTION_LEAVE_ENTER m_pfnFunctionEnter;
  static PFUNC_FUNCTION_LEAVE_ENTER m_pfnFunctionLeave;

public:
  /**
   * @brief callback function will called by HHVM when a fatal error occurs
   */
  static void onFatalError(const HPHP::Exception* exc) ;
  /**
   * @brief function entered hook
   * @param ar the activation record. Can be used to extract the function name and other useful information
   * @param funcType
   */
  static void onFunctionEnter( const HPHP::ActRec* ar, int funcType);
  /**
   * @brief function leave hook
   * @param ar the activation record. Can be used to extract the function name and other useful information
   * @param funcType
   */
  static void onFunctionExit( const HPHP::ActRec* ar, int funcType);

  // --------------------------------------
  // User handlers
  // --------------------------------------

  static void setPfnFatalError(PFUNC_ERROR_HANDLER pfnFatalError) {
    m_pfnFatalError = pfnFatalError;
  }
  static void setPfnFunctionEnter(PFUNC_FUNCTION_LEAVE_ENTER pfnFunctionEnter) {
    m_pfnFunctionEnter = pfnFunctionEnter;
  }
  static void setPfnFunctionLeave(PFUNC_FUNCTION_LEAVE_ENTER pfnFunctionLeave) {
    m_pfnFunctionLeave = pfnFunctionLeave;
  }
};

}

#endif // MONITOR_H

#ifndef MONITOR_H
#define MONITOR_H

#include "hphp/runtime/base/execution-context.h"
#include "hphp/runtime/vm/bytecode.h"
#include "hphp/runtime/base/rds.h"
#include "hphp/runtime/base/exceptions.h"
#include <atomic>
#include <ExternalEventsRporter.h>

namespace HPHP
{
class ZendMonitor
{
protected:
  /**
   * @brief extract backtrace usable by zend server from HHVM extended exception class
   */
  static void extractBacktraceFromExpcetion(const HPHP::ExtendedException& exc, Zend::BacktraceList_t& bt) ;

  /**
   * @brief extract the function name from ActRec & funcType
   */
  static const char* getFunctionName(const HPHP::ActRec* ar, int funcType) ;

public:
  /**
   * @brief callback function will called by HHVM when a fatal error occurs
   */
  static void onFatalError(const HPHP::Exception* exc) ;
};

}

#endif // MONITOR_H

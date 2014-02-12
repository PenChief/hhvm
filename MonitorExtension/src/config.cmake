if ( NOT DEFINED ENV{ZS_SRC_DIR} )
  message(FATAL_ERROR "*** ERROR: please set ZS_SRC_DIR to Zend Server root directory" )
endif ()
set( ZS_SRC_DIR $ENV{ZS_SRC_DIR} )
include_directories( ${ZS_SRC_DIR}/ZendEventsReporter/src )

HHVM_EXTENSION(monitor monitor.cpp)
HHVM_SYSTEMLIB(monitor monitor.php)


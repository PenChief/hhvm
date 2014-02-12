CMAKE_MINIMUM_REQUIRED(VERSION 2.8.7 FATAL_ERROR)
PROJECT(hphp C CXX ASM)

## Zend Server event reporting mechanism
if ( NOT DEFINED ENV{ZS_SRC_DIR} )
  message(FATAL_ERROR "ERROR! please set ZS_SRC_DIR environment variable to point to Zend Server build folder")
endif ( )
set ( ZS_SRC_DIR $ENV{ZS_SRC_DIR} )
message("-- ZS_SRC_DIR is set to ${ZS_SRC_DIR}")

IF(NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
	message(FATAL_ERROR "HHVM requires a 64bit OS")
ENDIF()

set(HPHP_HOME "$ENV{HPHP_HOME}")
if (NOT HPHP_HOME)
	set(HPHP_HOME "${CMAKE_CURRENT_SOURCE_DIR}")
endif()
message("Using HPHP_HOME == ${HPHP_HOME}")

IF(NOT EXISTS "${HPHP_HOME}/CMake/HPHPSetup.cmake")
	message(FATAL_ERROR "Invalid HPHP_HOME. Set it to the root of your hhvm tree, or run `cmake .` from there.")
ENDIF()

SET(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake" ${CMAKE_MODULE_PATH})

include("${CMAKE_CURRENT_SOURCE_DIR}/CMake/HPHPFunctions.cmake")
include(CheckFunctionExists)

add_subdirectory(hphp)


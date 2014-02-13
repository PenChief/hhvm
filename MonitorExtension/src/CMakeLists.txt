CMAKE_MINIMUM_REQUIRED(VERSION 2.8.5 FATAL_ERROR)
PROJECT(hphp C CXX ASM)

set(HPHP_HOME "/home/eran/devl/facebook/hhvm")
set(CMAKE_MODULE_PATH
    "${HPHP_HOME}/CMake"
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_MODULE_PATH})
set(HHVM_INCLUDE_DIRS "/usr/include;/usr/include;/usr/include;/usr/include/x86_64-linux-gnu;/usr/include;/usr/include/mysql;/usr/include/mysql;/usr/include;/usr/include;/home/eran/devl/facebook/include;/usr/include;/usr/include;/usr/include/freetype2;/usr/include;/usr/include/libxml2;/usr/include;/home/eran/devl/facebook/hhvm/hphp/third_party/libsqlite3;/home/eran/devl/facebook/hhvm/hphp/third_party/timelib;/home/eran/devl/facebook/hhvm/hphp/third_party/libafdt/src;/home/eran/devl/facebook/hhvm/hphp/third_party/libmbfl;/home/eran/devl/facebook/hhvm/hphp/third_party/libmbfl/mbfl;/home/eran/devl/facebook/hhvm/hphp/third_party/libmbfl/filter;/home/eran/devl/facebook/hhvm/hphp/third_party/lz4;/home/eran/devl/facebook/hhvm/hphp/third_party/double-conversion/src;/home/eran/devl/facebook/hhvm/hphp/third_party/folly;/home/eran/devl/facebook/hhvm/hphp/third_party/libzip;/usr/include/x86_64-linux-gnu;/usr/include;/usr/include;/usr/include;/usr/include;/usr/local/include;/usr/include;/usr/include;/usr/include;/usr/include;/usr/include;/usr/include;/usr/include/c-client;/usr/local/include;/usr/include;/usr/include/security;/home/eran/devl/facebook/hhvm/hphp;/home/eran/devl/facebook/hhvm/hphp/system/gen;/home/eran/devl/facebook/hhvm/hphp;/home/eran/devl/facebook/hhvm/hphp/lib/system/gen;/home/eran/devl/facebook/hhvm;/usr/include")
set(HHVM_DEFINITIONS ";HAVE_LIBDL;HAVE_BOOST1_49;HAVE_FEATURES_H=1;PHP_MYSQL_UNIX_SOCK_ADDR=\"/var/run/mysqld/mysqld.sock\";PNG_SKIP_SETJMP_CHECK;HAVE_GD_JPG;HAVE_GD_PNG;HAVE_CURL_MULTI_WAIT;USE_JEMALLOC=1;NO_TCMALLOC=1;HAVE_ELF_GETSHDRSTRNDX;_REENTRANT=1;_PTHREADS=1;__STDC_FORMAT_MACROS;FOLLY_HAVE_WEAK_SYMBOLS=1;_GNU_SOURCE;DEBUG;ENABLE_FASTCGI=1;HPHP_OSS=1;PACKAGE=hhvm;PACKAGE_VERSION=Release;FOLLY_NO_CONFIG=1;FOLLY_HAVE_CLOCK_GETTIME=1;FOLLY_HAVE_FEATURES_H=1;HHVM")
include(HPHPFunctions)
include(HPHPCompiler)
include(config)

foreach (dir ${HHVM_INCLUDE_DIRS})
	include_directories(${dir})
endforeach()

foreach (def ${HHVM_DEFINITIONS})
	add_definitions("-D${def}")
endforeach()

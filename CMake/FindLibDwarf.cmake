# - Try to find libdwarf
# Once done this will define
#
#  LIBDWARF_FOUND - system has libdwarf
#  LIBDWARF_INCLUDE_DIRS - the libdwarf include directory
#  LIBDWARF_LIBRARIES - Link these to use libdwarf
#  LIBDWARF_DEFINITIONS - Compiler switches required for using libdwarf
#

# Locate libelf library at first
if (NOT LIBELF_FOUND)
   find_package (LibElf REQUIRED)
endif (NOT LIBELF_FOUND)

if (LIBDWARF_LIBRARIES AND LIBDWARF_INCLUDE_DIRS)
  set (LibDwarf_FIND_QUIETLY TRUE)
endif (LIBDWARF_LIBRARIES AND LIBDWARF_INCLUDE_DIRS)

if ( HHVM_SHARED_RUNTIME )
  find_path (DWARF_INCLUDE_DIR
      NAMES
        libdwarf.h dwarf.h
      PATHS
        /home/eran/root/include
        /usr/include
        /usr/include/libdwarf
        /opt/local/include
        /sw/include
        ENV CPATH) # PATH and INCLUDE will also work
      
else ( HHVM_SHARED_RUNTIME )

  ## When building as static library 
  ## use the default search path
  find_path (DWARF_INCLUDE_DIR
    NAMES
      libdwarf.h dwarf.h
    PATHS
      /usr/include
      /usr/include/libdwarf
      /usr/local/include
      /opt/local/include
      /sw/include
      ENV CPATH) # PATH and INCLUDE will also work
      
endif ( HHVM_SHARED_RUNTIME )

if (DWARF_INCLUDE_DIR)
    set (LIBDWARF_INCLUDE_DIRS  ${DWARF_INCLUDE_DIR})
endif ()

if ( HHVM_SHARED_RUNTIME )
  message("-- Using SHARED runtime, searching /usr/local/lib first")
  find_library (LIBDWARF_LIBRARIES
      NAMES
        dwarf
      PATHS
        /home/eran/root/lib
        /usr/lib
        /opt/local/lib
        /sw/lib
        ENV LIBRARY_PATH   # PATH and LIB will also work
        ENV LD_LIBRARY_PATH)
  include (FindPackageHandleStandardArgs)
  
else ( HHVM_SHARED_RUNTIME )
  message("-- Using STATIC runtime ")
  find_library (LIBDWARF_LIBRARIES
      NAMES
        dwarf
      PATHS
        /usr/lib
        /usr/local/lib
        /opt/local/lib
        /sw/lib
        ENV LIBRARY_PATH   # PATH and LIB will also work
        ENV LD_LIBRARY_PATH)
  include (FindPackageHandleStandardArgs)
  
endif ( HHVM_SHARED_RUNTIME )

# handle the QUIETLY and REQUIRED arguments and set LIBDWARF_FOUND to TRUE
# if all listed variables are TRUE
FIND_PACKAGE_HANDLE_STANDARD_ARGS(LibDwarf DEFAULT_MSG
    LIBDWARF_LIBRARIES
    LIBDWARF_INCLUDE_DIRS)

if (LIBDWARF_LIBRARIES AND LIBDWARF_INCLUDE_DIRS)
  set(CMAKE_REQUIRED_INCLUDES ${LIBDWARF_INCLUDE_DIRS})
  set(CMAKE_REQUIRED_LIBRARIES ${LIBDWARF_LIBRARIES})
  include(CheckSymbolExists)
  CHECK_SYMBOL_EXISTS(dwarf_encode_leb128 "libdwarf.h" LIBDWARF_HAVE_ENCODE_LEB128)
endif()

mark_as_advanced(LIBDW_INCLUDE_DIR DWARF_INCLUDE_DIR)
mark_as_advanced(LIBDWARF_INCLUDE_DIRS LIBDWARF_LIBRARIES)
mark_as_advanced(LIBDWARF_HAVE_ENCODE_LEB128)

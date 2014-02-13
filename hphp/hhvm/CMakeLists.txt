set(CXX_SOURCES)
auto_sources(files "*.cpp" "")
list(APPEND CXX_SOURCES ${files})

add_executable(hhvm ${CXX_SOURCES} )

## set RPATH to the installation folder
set (LINKER_OPTIONS -Wl,-rpath,"${CMAKE_INSTALL_PREFIX}/lib")
message( "-- rpath is set to ${LINKER_OPTIONS}")

target_link_libraries(hhvm ${LINKER_OPTIONS} ${HHVM_LINK_LIBRARIES})
embed_all_systemlibs(hhvm "${CMAKE_CURRENT_SOURCE_DIR}/hhvm")
add_dependencies(hhvm systemlib)

HHVM_INSTALL(hhvm bin)

## install libphp_runtime.so -> ${CMAKE_INSTALL_PREFIX}/lib (defaults to /usr/local/lib)
HHVM_INSTALL(hphp_runtime_static lib) 

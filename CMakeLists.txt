cmake_minimum_required(VERSION 2.6.0)

# pull in the pods macros. See cmake/pods.cmake for documentation
set(POD_NAME p2l-trace-analysis)
include(cmake/pods.cmake)


add_definitions( -std=c++0x -Wall -fdiagnostics-show-option )
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g3")

option ( USE_PEDANTIC "Turn on -pendantic mode in gcc. This will spit out *lots* of warnings from lcm :-(, but hopefully none from the rest of the code" OFF)
if( USE_PEDANTIC )
    add_definitions( -pedantic )
endif (USE_PEDANTIC)


add_library( p2l-trace-analysis SHARED
  src/trace_util.cpp
)
pods_install_headers( 
  src/trace_util.hpp
  DESTINATION
  trace-analysis
)
pods_use_pkg_config_packages( p2l-trace-analysis boost-1.54.0 boost-1.54.0-filesystem)
pods_install_libraries( p2l-trace-analysis )
pods_install_pkg_config_file(p2l-trace-analysis
    CFLAGS
    LIBS -lp2l-trace-analysis
    REQUIRES boost-1.54.0 boost-1.54.0-filesystem
    VERSION 0.0.1)



# the console
add_executable( p2l-trace-analysis-console
  src/console.cpp)
pods_use_pkg_config_packages( p2l-trace-analysis-console p2l-trace-analysis boost-1.54.0-regex)
pods_install_executables( p2l-trace-analysis-console )



# add the test subdirectory
add_subdirectory( test )

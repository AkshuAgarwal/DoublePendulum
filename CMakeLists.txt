cmake_minimum_required( VERSION 3.7 )
project( double_pendulum )

set( double_pendulum_VERSION_MAJOR 1 )
set( double_pendulum_VERSION_MINOR 0 )

set( CMAKE_CXX_STANDARD 17 )
set( CMAKE_CXX_STANDARD_REQUIRED True )

add_subdirectory( lib/SDL EXCLUDE_FROM_ALL )

add_executable( double_pendulum main.cpp )

target_link_libraries( double_pendulum PRIVATE SDL3::SDL3 )


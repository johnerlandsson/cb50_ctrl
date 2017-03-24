cmake_minimum_required(VERSION 3.6 FATAL_ERROR)
project(cb50_ctrl)

IF( NOT CMAKE_BUILD_TYPE )
    SET( CMAKE_BUILD_TYPE Release ... FORCE )
ENDIF()

add_subdirectory("cb50_ctrl")

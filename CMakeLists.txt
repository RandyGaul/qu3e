cmake_minimum_required(VERSION 2.8.8)
project(qu3e)

set( CMAKE_EXPORT_COMPILE_COMMANDS ON)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11")

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    )

add_subdirectory(src)

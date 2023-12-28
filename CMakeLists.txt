cmake_minimum_required(VERSION 3.12)
project(zipper VERSION 2.1.0.0)

add_subdirectory(external)
add_subdirectory(src)

option(ZIPPER_BUILD_TEST "Build zipper unit test" OFF)

if(ZIPPER_BUILD_TEST)
    add_subdirectory(tests)
endif()

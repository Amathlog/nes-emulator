cmake_minimum_required(VERSION 3.12)

project(NesEmulator)

# Enable c++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(EXTERNAL_FOLDER "External")
set(MAIN_FOLDER "NesEmulator")

if(MSVC)
  add_definitions(/MP)
endif()

# set(CMAKE_CXX_FLAGS "-Wall")
if(WIN32)
set(CMAKE_CXX_FLAGS_DEBUG "/ZI /Od")
set(CMAKE_CXX_FLAGS_RELEASE "/Os")
else(WIN32)
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
endif(WIN32)

option(BUILD_TESTS "Build the tests" OFF)

add_subdirectory("src")

if (BUILD_TESTS)
    add_subdirectory("tests")
endif(BUILD_TESTS)
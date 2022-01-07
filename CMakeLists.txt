cmake_minimum_required(VERSION 3.7)

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
set(CMAKE_CXX_FLAGS_RELEASE "/O2")
else(WIN32)
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")
endif(WIN32)

# Generate Tonic lib
include_directories(${NesEmulator_SOURCE_DIR}/external/Tonic/src Tonic)
file(GLOB_RECURSE TONIC_SRC
    ${NesEmulator_SOURCE_DIR}/external/Tonic/src/*
)

set(TONIC_DIR ${NesEmulator_SOURCE_DIR}/external/Tonic/src)
include_directories(${TONIC_DIR})
include_directories(${NesEmulator_SOURCE_DIR}/external/include)
file(GLOB_RECURSE TONIC_SRC
    "${TONIC_DIR}/*"
)

add_library(Tonic STATIC ${TONIC_SRC})
if(UNIX)
  target_compile_options(Tonic PRIVATE "-Wimplicit-int-float-conversion")
endif(UNIX)
if(WIN32)
  target_compile_options(Tonic PRIVATE /wd4068 /wd4267 /wd4244 /wd4305 /wd4018)
endif(WIN32)
set_target_properties(Tonic PROPERTIES FOLDER ${EXTERNAL_FOLDER})


# Generate Maximilian lib
include_directories(${NesEmulator_SOURCE_DIR}/external/Maximilian/src Maximilian)
set(MAXIMILIAN_DIR ${NesEmulator_SOURCE_DIR}/external/Maximilian/src)
file(GLOB_RECURSE MAXIMILIAN_SRC
    ${MAXIMILIAN_DIR}/*
)

add_library(Maximilian STATIC ${MAXIMILIAN_SRC})

add_subdirectory("src")
add_subdirectory("tests")

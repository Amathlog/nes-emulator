include_directories(${NesEmulator_SOURCE_DIR}/include)

file(GLOB_RECURSE CORELIB_SRC
    ${NesEmulator_SOURCE_DIR}/include/core/*.h
    ${NesEmulator_SOURCE_DIR}/src/core/*.cpp
)

add_library(NesEmulator_Core ${CORELIB_SRC})
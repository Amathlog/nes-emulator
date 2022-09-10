add_compile_definitions(_SILENCE_CXX17_STRSTREAM_DEPRECATION_WARNING)

include_directories(${NesEmulator_SOURCE_DIR}/include)
include_directories("${NesEmulator_SOURCE_DIR}/external/DSP-Cpp-filters/lib")

file(GLOB_RECURSE CORELIB_SRC
    ${NesEmulator_SOURCE_DIR}/include/core/*.h
    ${NesEmulator_SOURCE_DIR}/src/core/*.cpp
)

add_library(NesEmulator_Core STATIC ${CORELIB_SRC})
set_target_properties(NesEmulator_Core PROPERTIES FOLDER ${MAIN_FOLDER})
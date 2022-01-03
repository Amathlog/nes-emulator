set(SFML_STATIC_LIBRARIES TRUE)
#add_subdirectory(${NesEmulator_SOURCE_DIR}/external/SFML build)

add_compile_definitions(_SILENCE_CXX17_STRSTREAM_DEPRECATION_WARNING)

# include_directories(${NesEmulator_SOURCE_DIR}/external/SFML/include SFML)
include_directories(${NesEmulator_SOURCE_DIR}/include)

file(GLOB_RECURSE CORELIB_SRC
    ${NesEmulator_SOURCE_DIR}/include/core/*.h
    ${NesEmulator_SOURCE_DIR}/src/core/*.cpp
)

# link_directories(${NesEmulator_SOURCE_DIR}/external/SFML)
add_library(NesEmulator_Core STATIC ${CORELIB_SRC})
# target_link_libraries(NesEmulator_Core sfml-system sfml-window sfml-audio sfml-graphics)
target_link_libraries(NesEmulator_Core Tonic)
set_target_properties(NesEmulator_Core PROPERTIES FOLDER ${MAIN_FOLDER})
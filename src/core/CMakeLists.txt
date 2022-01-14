#set(SFML_STATIC_LIBRARIES TRUE)
#add_subdirectory(${NesEmulator_SOURCE_DIR}/external/SFML build)

add_compile_definitions(_SILENCE_CXX17_STRSTREAM_DEPRECATION_WARNING)

# include_directories(${NesEmulator_SOURCE_DIR}/external/SFML/include SFML)
set(NESEMULATOR_CORE_INCLUDES ${NesEmulator_SOURCE_DIR}/include)
include_directories(${NESEMULATOR_CORE_INCLUDES})

set(NESEMULATOR_CORE_PROJECT NesEmulator_Core)

file(GLOB_RECURSE CORELIB_SRC
    ${NesEmulator_SOURCE_DIR}/include/core/*.h
    ${NesEmulator_SOURCE_DIR}/src/core/*.cpp
)

# link_directories(${NesEmulator_SOURCE_DIR}/external/SFML)
add_library(${NESEMULATOR_CORE_PROJECT} STATIC ${CORELIB_SRC})
# target_link_libraries(NesEmulator_Core sfml-system sfml-window sfml-audio sfml-graphics)
target_link_libraries(${NESEMULATOR_CORE_PROJECT} Tonic)
set_target_properties(${NESEMULATOR_CORE_PROJECT} PROPERTIES FOLDER ${MAIN_FOLDER})
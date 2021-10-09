include_directories(${NesEmulator_SOURCE_DIR}/include)

file(GLOB_RECURSE EXE_SRC
    "${NesEmulator_SOURCE_DIR}/src/exe/*.cpp"
)

link_directories(${NesEmulator_SOURCE_DIR}/LibProject/src)
add_executable(NesEmulator ${EXE_SRC})
target_link_libraries(NesEmulator NesEmulator_Core)
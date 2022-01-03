# Add GTest
set(BUILD_GMOCK OFF)
add_subdirectory(${NesEmulator_SOURCE_DIR}/external/googletest build)
set_target_properties(gtest gtest_main uninstall PROPERTIES FOLDER ${EXTERNAL_FOLDER}/gtest)
enable_testing()
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

include_directories(${NesEmulator_SOURCE_DIR}/include/core)

file(GLOB_RECURSE TESTS_SRC
    "${NesEmulator_SOURCE_DIR}/tests/*.cpp"
)

add_executable(NesEmulatorUnitTests ${TESTS_SRC})
target_link_libraries(NesEmulatorUnitTests NesEmulator_Core Tonic)
target_link_libraries(NesEmulatorUnitTests gtest_main)
set_target_properties(NesEmulatorUnitTests PROPERTIES FOLDER ${MAIN_FOLDER})

#include(GoogleTest)
#gtest_discover_tests(NesEmulatorUnitTests)
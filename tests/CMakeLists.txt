# Add GTest
add_subdirectory(${NesEmulator_SOURCE_DIR}/external/googletest build)
enable_testing()
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

include_directories(${NesEmulator_SOURCE_DIR}/include/core)

file(GLOB_RECURSE TESTS_SRC
    "${NesEmulator_SOURCE_DIR}/tests/*.cpp"
)

add_executable(NesEmulatorUnitTests ${TESTS_SRC})
target_link_libraries(NesEmulatorUnitTests NesEmulator_Core)
target_link_libraries(NesEmulatorUnitTests gtest_main)

#include(GoogleTest)
#gtest_discover_tests(NesEmulatorUnitTests)
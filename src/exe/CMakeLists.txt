# Qt dependency
find_package(Qt5 COMPONENTS Core Widgets REQUIRED)

SET(CMAKE_AUTOMOC ON)
SET(CMAKE_AUTOUIC ON)
SET(CMAKE_AUTORCC ON)


include_directories(${NesEmulator_SOURCE_DIR}/include)

file(GLOB_RECURSE EXE_SRC
    "${NesEmulator_SOURCE_DIR}/src/exe/*.cpp"
)

file(GLOB_RECURSE EXE_UI
    "{NesEmulator_SOURCE_DIR}/src/exe/ui/*"
)

file(GLOB_RECURSE EXE_INCLUDE
    "{NesEmulator_SOURCE_DIR}/include/exe/*.h"
)


link_directories(${NesEmulator_SOURCE_DIR}/LibProject/src)
add_executable(NesEmulator ${EXE_SRC} ${EXE_UI} ${EXE_INCLUDE})
target_link_libraries(NesEmulator NesEmulator_Core Qt5::Core Qt5::Widgets)
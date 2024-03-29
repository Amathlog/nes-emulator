# Include OpenGL
if(UNIX)
    set(OpenGL_GL_PREFERENCE "GLVND")
endif(UNIX)

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS})

# Include RTAudio
if(UNIX)
    set(RTAUDIO_API_PULSE ON)
    set(RTAUDIO_API_WASAPI OFF)
endif(UNIX)
if(WIN32)
    set(RTAUDIO_API_WASAPI ON)
    set(RTAUDIO_API_PULSE OFF)
endif(WIN32)
set(RTAUDIO_BUILD_TESTING OFF)
add_subdirectory("${NesEmulator_SOURCE_DIR}/external/rtaudio" rtaudio)
include_directories("${NesEmulator_SOURCE_DIR}/external/rtaudio")
set_target_properties(rtaudio PROPERTIES FOLDER ${EXTERNAL_FOLDER})
if(WIN32)
  target_compile_options(rtaudio PRIVATE /wd4251 /wd4996 /wd4267)
endif(WIN32)

# Include GLFW and GLAD
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

set(GLFW_DIR "${NesEmulator_SOURCE_DIR}/external/glfw")
set(GLAD_DIR "${NesEmulator_SOURCE_DIR}/external/glad")
add_subdirectory("${GLFW_DIR}" glfw)
include_directories("${GLFW_DIR}/include")
include_directories("${GLAD_DIR}/include")

# Build ImGui
set(IMGUI_DIR "${NesEmulator_SOURCE_DIR}/external/imgui")
include_directories("${IMGUI_DIR}")
include_directories("${IMGUI_DIR}/backends")

file (GLOB IMGUI_SRC
    "${IMGUI_DIR}/*cpp"
    "${IMGUI_DIR}/backends/imgui_impl_glfw.cpp"
    "${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp"
)

add_library(ImGui STATIC ${IMGUI_SRC})
set_target_properties(ImGui PROPERTIES FOLDER ${EXTERNAL_FOLDER})

# Build ImGuiFileBrowser
set(IMGUI_FE_DIR "${NesEmulator_SOURCE_DIR}/external/ImGui-Addons/FileBrowser")
include_directories("${IMGUI_FE_DIR}")
file (GLOB IMGUI_FE_SRC
    "${IMGUI_FE_DIR}/*.cpp"
)

add_library(ImGuiFE STATIC ${IMGUI_FE_SRC})
set_target_properties(ImGuiFE PROPERTIES FOLDER ${EXTERNAL_FOLDER})
target_link_libraries(ImGuiFE ImGui)
if(WIN32)
  target_compile_options(ImGuiFE PRIVATE /wd4267 /wd4996 /wd4244)
endif(WIN32)

include_directories(${NesEmulator_SOURCE_DIR}/include)
include_directories("${NesEmulator_SOURCE_DIR}/external/DSP-Cpp-filters/lib")

file(GLOB_RECURSE NEW_EXE_SRC
    "${NesEmulator_SOURCE_DIR}/src/new_exe/*.cpp"
    "${NesEmulator_SOURCE_DIR}/include/new_exe/*.h"
    "${GLAD_DIR}/src/glad.c"
)

link_directories(${NesEmulator_SOURCE_DIR}/LibProject/src)
add_executable(NesEmulatorGL ${NEW_EXE_SRC})
target_link_libraries(NesEmulatorGL NesEmulator_Core ${OPENGL_LIBRARIES} glfw ${CMAKE_DL_LIBS} rtaudio ImGuiFE ImGui)
set_target_properties(NesEmulatorGL PROPERTIES FOLDER ${MAIN_FOLDER})
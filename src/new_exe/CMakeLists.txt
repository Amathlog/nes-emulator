# Include OpenGL
if(UNIX)
    set(OpenGL_GL_PREFERENCE "GLVND")
endif(UNIX)

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS})


# Include GLUT
# if(WIN32)
#     set(GLUT_INCLUDE_DIR "external/glut/include")
#     set(OPENGL_LIBRARY_DIR "${CMAKE_SOURCE_DIR}/external/glut/lib")
#     # set(GLUT_LIBRARY "${CMAKE_SOURCE_DIR}/external/glut/lib/x64/freeglut")
#     find_package(GLUT REQUIRED)
#     include_directories(${GLUT_INCLUDE_DIR})
# else(WIN32)
#     find_package(GLUT REQUIRED)
#     include_directories(${GLUT_INCLUDE_DIRS})
# endif(WIN32)

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

# Include ImGui
set(IMGUI_DIR "${NesEmulator_SOURCE_DIR}/external/imgui")
set(IMGUI_FE_DIR "${NesEmulator_SOURCE_DIR}/external/ImGui-Addons/FileBrowser")
include_directories("${IMGUI_DIR}")
include_directories("${IMGUI_FE_DIR}")
include_directories("${IMGUI_DIR}/backends")

include_directories(${NesEmulator_SOURCE_DIR}/include)

file(GLOB_RECURSE NEW_EXE_SRC
    "${NesEmulator_SOURCE_DIR}/src/new_exe/*.cpp" 
    "${GLAD_DIR}/src/glad.c"
)

file (GLOB IMGUI_SRC
    "${IMGUI_DIR}/*cpp"
    "${IMGUI_DIR}/backends/imgui_impl_glfw.cpp"
    "${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp"
)

file (GLOB IMGUI_FE_SRC
    "${IMGUI_FE_DIR}/*.cpp"
)

# file(GLOB_RECURSE EXE_UI
#     "{NesEmulator_SOURCE_DIR}/src/exe/ui/*"
# )

file(GLOB_RECURSE NEW_EXE_INCLUDE
    "{NesEmulator_SOURCE_DIR}/include/new_exe/*.h"
)


link_directories(${NesEmulator_SOURCE_DIR}/LibProject/src)
add_executable(NesEmulatorGL ${NEW_EXE_SRC} ${IMGUI_SRC} ${IMGUI_FE_SRC} ${NEW_EXE_INCLUDE})
target_link_libraries(NesEmulatorGL NesEmulator_Core ${OPENGL_LIBRARIES} glfw ${CMAKE_DL_LIBS})
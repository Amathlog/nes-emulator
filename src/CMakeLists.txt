add_subdirectory(core)

# Don't add "exe" on Windows, to avoid installing Qt.
if(UNIX)
add_subdirectory(exe)
endif(UNIX)

add_subdirectory(new_exe)
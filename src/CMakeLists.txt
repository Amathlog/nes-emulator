add_subdirectory(core)

# Don't add "exe" on Windows, to avoid installing Qt.
# Now deprecated
# if(UNIX)
# add_subdirectory(exe)
# endif(UNIX)

option(NESEMULATOR_ONLY_CORE "Build only the core lib" OFF)

if (NOT NESEMULATOR_ONLY_CORE)
    add_subdirectory(new_exe)
endif(NOT NESEMULATOR_ONLY_CORE)
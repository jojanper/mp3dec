set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 11)

# Include RPATHs in the build tree.
set(CMAKE_SKIP_BUILD_RPATH FALSE)

# Don't use the install path for the RPATH.
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)

# Add paths to linker search and installed RPATH.
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

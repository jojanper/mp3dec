# Version info
set(VERSION_MAJOR 0)
set(VERSION_MINOR 1)
set(VERSION_PATCH 0)

# cpack

# Name of (base) package
set(PACKAGE_NAME "${CMAKE_PROJECT_NAME}-${CMAKE_SYSTEM_NAME}-${CMAKE_BUILD_TYPE}-${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")

# General information for package(s)
set(CPACK_PACKAGE_DESCRIPTION "MP3 decoder library")
set(CPACK_PACKAGE_VENDOR "Juha Ojanperä")
set(CPACK_PACKAGE_CONTACT "juha.ojanpera@gmail.com")

# 2 packages created: static and shared
set(CPACK_COMPONENTS_ALL static shared)

# Package name postfixes
set(CPACK_COMPONENT_STATIC_GROUP "static")
set(CPACK_COMPONENT_SHARED_GROUP "shared")

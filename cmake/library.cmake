# Create object library.
#
# lib_name - Name of library
# lib_srcs - Library sources
# defines  - Private defines
function(build_object_library lib_name lib_srcs defines)
  add_library(${lib_name} OBJECT ${lib_srcs})
  set_target_properties(${lib_name} PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib
  )
  target_compile_definitions(${lib_name} PRIVATE ${defines})
endfunction()

# Create static library.
#
# lib_name       - Name of library
# lib_srcs       - Library sources
# link_libs      - Additional link libraries
# build_includes - Include directories for building the library
# defines        - Private defines
function(build_static_library lib_name lib_srcs link_libs build_includes defines)
  add_library(${lib_name} STATIC ${lib_srcs})
  set_target_properties(${lib_name} PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib
  )
  target_link_libraries(${lib_name} ${link_libs})

  target_compile_definitions(${lib_name} PRIVATE ${defines})

  # Include directories for the target; both public and internal
  target_include_directories(${lib_name}
    PUBLIC
      $<INSTALL_INTERFACE:include>
    PRIVATE
      ${build_includes}
  )
endfunction()

# Create shared library.
#
# lib_name       - Name of library
# lib_srcs       - Library sources
# link_libs      - Additional link libraries
# build_includes - Include directories for building the library
# defines        - Private defines
function(build_shared_library lib_name lib_srcs link_libs build_includes defines)
  add_library(${lib_name} SHARED ${lib_srcs})
  set_target_properties(${lib_name} PROPERTIES
    COMPILE_DEFINITIONS "SHARED_LIBRARY"
    VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_BUILD}
    SOVERSION ${VERSION_MAJOR}
    ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib
    LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin
  )
  target_link_libraries(${lib_name} PRIVATE ${link_libs})
  target_compile_definitions(${lib_name} PRIVATE ${defines})

  # Include directories for the target; both public and internal
  target_include_directories(${lib_name}
    PUBLIC
      $<INSTALL_INTERFACE:include>
    PRIVATE
      ${build_includes}
  )
  strip_library(${lib_name})
endfunction()

# Install target.
#
# target       - Target item
# component    - Install component name
# export_cmake - Export target script name
# namespace    - Prefix for imported targets
function(install_target_binary target component export_cmake namespace)
  install(TARGETS ${target}
    # Register the installed target to exports
    EXPORT ${target}-targets
    RUNTIME DESTINATION bin COMPONENT ${component}
    LIBRARY DESTINATION lib COMPONENT ${component}
    ARCHIVE DESTINATION lib COMPONENT ${component}
  )

  # Export the target to a .cmake script
  install(EXPORT ${target}-targets
    FILE ${export_cmake}.cmake
    NAMESPACE ${namespace}
    DESTINATION cmake COMPONENT ${component}
  )
endfunction()

# Install files.
#
# destination - Destination name
# target      - Target file(s)
function(install_target_files destination target component)
  install(FILES ${target}
    DESTINATION ${destination} COMPONENT ${component}
  )
endfunction()

# Build unit test binary.
#
# test_name - Test binary name
# test_src  - Sources
# link_libs - Additional link libraries
function(build_unittest test_name test_srcs link_libs)
  add_executable(${test_name} ${test_srcs})
  target_link_libraries(${test_name} ${link_libs} ${GTEST_ALL_LIBS})
  add_test(NAME ${test_name} COMMAND $<TARGET_FILE:${test_name}>)
  set_target_properties(${test_name} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin
  )
endfunction()

# Build target binary.
#
# test_name    - Binary name
# test_src     - Sources
# link_libs    - Additional link libraries
# compile_defs - Compilation definitions, if any
function(build_exe exe_name exe_srcs link_libs compile_defs)
  add_executable(${exe_name} ${exe_srcs})
  target_link_libraries(${exe_name} ${link_libs})
  set_target_properties(${exe_name} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin
    LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib
    ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib
    COMPILE_DEFINITIONS "${compile_defs}"
  )
endfunction()

# Create static library from 3rd party (static) library
#
# lib_name    - Name of target library
# release_lib - Release lib path
# debug_lib   - Debug lib path
function(create_static_lib_export lib_name release_lib debug_lib)
  add_library(${lib_name} STATIC IMPORTED GLOBAL)
  set_target_properties(${lib_name} PROPERTIES IMPORTED_LOCATION_RELEASE ${release_lib})
  set_target_properties(${lib_name} PROPERTIES IMPORTED_LOCATION_DEBUG ${debug_lib})
endfunction()

# Create shared library from 3rd party (shared) library
#
# lib_name    - Name of target library
# release_lib - Release lib path
# debug_lib   - Debug lib path
function(create_shared_lib_export lib_name release_lib debug_lib)
  add_library(${lib_name} SHARED IMPORTED GLOBAL)
  set_target_properties(${lib_name} PROPERTIES IMPORTED_LOCATION_RELEASE ${release_lib})
  set_target_properties(${lib_name} PROPERTIES IMPORTED_LOCATION_DEBUG ${debug_lib})
endfunction()

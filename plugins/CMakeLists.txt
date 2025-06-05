cmake_minimum_required(VERSION 3.16)
set(CMAKE_POLICY_VERSION_MINIMUM
    "3.16"
    CACHE STRING "CMake policy version" FORCE)

project(
  xFlow
  VERSION 1.0
  LANGUAGES C CXX)

set(X_FLOW_PLUGIN_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${X_FLOW_PLUGIN_OUTPUT_DIRECTORY})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${X_FLOW_PLUGIN_OUTPUT_DIRECTORY})
enable_testing()
add_subdirectory(c)
add_subdirectory(qt)

# --------------------------------------------------------------------------------------------------
# Copying the script files to the plugins directory
add_custom_target(
  copy_scripts ALL
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/js
          ${X_FLOW_PLUGIN_OUTPUT_DIRECTORY}
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/lua
          ${X_FLOW_PLUGIN_OUTPUT_DIRECTORY}
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/python
          ${X_FLOW_PLUGIN_OUTPUT_DIRECTORY})

# --------------------------------------------------------------------------------------------------
# Remove useless files from the plugins directory
set(plugins_dir ${CMAKE_BINARY_DIR}/plugins)
add_custom_target(
  cooked_plugins ALL
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${X_FLOW_PLUGIN_OUTPUT_DIRECTORY} ${plugins_dir}
  COMMAND ${CMAKE_COMMAND} "-DargTargetDir=${plugins_dir}" -P ${CMAKE_SOURCE_DIR}/cmake/rm.cmake
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

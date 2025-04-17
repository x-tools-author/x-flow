cmake_minimum_required(VERSION 3.16)
set(CMAKE_POLICY_VERSION_MINIMUM
    "3.16"
    CACHE STRING "CMake policy version" FORCE)

project(
  xFlow
  VERSION 1.0
  LANGUAGES C CXX)

set(X_FLOW_PLUGIN_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${X_FLOW_PLUGIN_OUTPUT_DIRECTORY})
enable_testing()
add_subdirectory(c)
add_subdirectory(qt)

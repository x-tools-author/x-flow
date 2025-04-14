cmake_minimum_required(VERSION 3.16)
set(CMAKE_POLICY_VERSION_MINIMUM
    "3.16"
    CACHE STRING "CMake policy version" FORCE)

project(
  xFlow
  VERSION 1.0
  LANGUAGES C CXX)

add_subdirectory(c)
add_subdirectory(qt)

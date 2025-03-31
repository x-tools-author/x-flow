cmake_minimum_required(VERSION 3.20)
project(
  c_plugin-demo
  VERSION 1.0
  LANGUAGES C)

# --------------------------------------------------------------------------------------------------
# crc lib
set(lib_crc_file "libcrc-2.0")
execute_process(COMMAND ${CMAKE_COMMAND} -E tar xzf ${lib_crc_file}.zip
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/3rd)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/3rd/${lib_crc_file}/include)
file(GLOB_RECURSE crc_srcs ${CMAKE_CURRENT_SOURCE_DIR}/3rd/${lib_crc_file}/src/*.c)
add_library(libcrc STATIC ${crc_srcs})

# --------------------------------------------------------------------------------------------------
# google test
set(google_test_file "googletest-1.16.0")
execute_process(COMMAND ${CMAKE_COMMAND} -E tar xzf ${google_test_file}.zip
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/3rd)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/3rd/${google_test_file}/googletest/include)
add_subdirectory(3rd/${google_test_file})

# --------------------------------------------------------------------------------------------------
# crc calculator
add_subdirectory(crccalculator)

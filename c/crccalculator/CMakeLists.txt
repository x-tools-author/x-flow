cmake_minimum_required(VERSION 3.20)
project(
  c_plugin_crc_calculator
  VERSION 1.0
  LANGUAGES C CXX)

add_compile_definitions(CRC_CALCULATOR_DLL_EXPORTS)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/crccalculator/lib)
add_library(${PROJECT_NAME} SHARED src/c_plugin_crc_calculator.h src/c_plugin_crc_calculator.c)
target_link_libraries(${PROJECT_NAME} PUBLIC libcrc)

# --------------------------------------------------------------------------------------------------
include(GoogleTest)
enable_testing()
add_executable(${PROJECT_NAME}_test
               ${CMAKE_CURRENT_SOURCE_DIR}/src/c_plugin_crc_calculator_unittest.cpp)
target_link_libraries(${PROJECT_NAME}_test PRIVATE gtest gtest_main ${PROJECT_NAME})
add_test(${PROJECT_NAME}_test ${PROJECT_NAME}_test)
gtest_discover_tests(${PROJECT_NAME}_test)

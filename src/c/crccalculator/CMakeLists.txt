cmake_minimum_required(VERSION 3.20)
project(
  c_plugin-crc-calculator
  VERSION 1.0
  LANGUAGES C)

add_compile_definitions(CRC_CALCULATOR_DLL_EXPORTS)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/crccalculator/lib)
add_library(${PROJECT_NAME} SHARED src/c-plugin-crc-calculator.c)

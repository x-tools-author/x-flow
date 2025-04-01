cmake_minimum_required(VERSION 3.20)
project(
  crc32_calc
  VERSION 1.0
  LANGUAGES C CXX)

add_compile_definitions(CRC_CALCULATOR_DLL_EXPORTS)
add_library(${PROJECT_NAME} SHARED src/c_plugin_crc_calculator.h src/c_plugin_crc_calculator.c)
target_link_libraries(${PROJECT_NAME} PUBLIC crc32c)
if(${CMAKE_BUILD_TYPE} STREQUAL "Release")
  add_custom_target(
    ${PROJECT_NAME}_deploy COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PROJECT_NAME}>
                                   ${CMAKE_CURRENT_SOURCE_DIR}/crccalculator/lib)
endif()

# --------------------------------------------------------------------------------------------------
include(GoogleTest)
include(CTest)
enable_testing()
add_executable(${PROJECT_NAME}_test
               ${CMAKE_CURRENT_SOURCE_DIR}/src/c_plugin_crc_calculator_unittest.cpp)
target_link_libraries(${PROJECT_NAME}_test PRIVATE gtest gtest_main ${PROJECT_NAME})
add_test(${PROJECT_NAME}_test ${PROJECT_NAME}_test)

cmake_minimum_required(VERSION 3.20)
project(
  c_plugin-demo
  VERSION 1.0
  LANGUAGES C CXX)

# --------------------------------------------------------------------------------------------------
# crc32 lib
set(lib_crc_file "crc32c-main")
execute_process(COMMAND ${CMAKE_COMMAND} -E tar xzf ${lib_crc_file}.zip
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/3rd)
set(CRC32C_BUILD_BENCHMARKS
    OFF
    CACHE BOOL "Build benchmarks")
set(CRC32C_BUILD_TESTS
    OFF
    CACHE BOOL "Build tests")
set(CRC32C_USE_GLOG
    OFF
    CACHE BOOL "Use glog for logging")
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/3rd/${lib_crc_file}/include)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/3rd/${lib_crc_file})

# --------------------------------------------------------------------------------------------------
# google test
set(google_test_file "googletest-1.16.0")
execute_process(COMMAND ${CMAKE_COMMAND} -E tar xzf ${google_test_file}.zip
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/3rd)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/3rd/${google_test_file}/googletest/include)
add_subdirectory(3rd/${google_test_file})

# --------------------------------------------------------------------------------------------------
add_compile_definitions(CRC_CALCULATOR_DLL_EXPORTS)
add_library(${PROJECT_NAME} SHARED src/crc32_calc.h src/crc32_calc.c)
target_link_libraries(${PROJECT_NAME} PUBLIC crc32c)
if(${CMAKE_BUILD_TYPE} STREQUAL "Release")
  add_custom_target(
    ${PROJECT_NAME}_deploy COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PROJECT_NAME}>
                                   ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}/lib)
endif()

# --------------------------------------------------------------------------------------------------
# include(GoogleTest) include(CTest)
enable_testing()
add_executable(${PROJECT_NAME}_test ${CMAKE_CURRENT_SOURCE_DIR}/src/crc32_calc_unittest.cpp)
target_link_libraries(${PROJECT_NAME}_test PRIVATE gtest gtest_main ${PROJECT_NAME})
add_test(name ${PROJECT_NAME}_test COMMAND ${PROJECT_NAME}_test)

cmake_minimum_required(VERSION 3.16)
project(
  xFlowCPlugins
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
add_compile_definitions(X_FLOW_DLL_EXPORTS)
enable_testing()

include(bypass/bypass.cmake)
include(crc32_calc/crc32_calc.cmake)

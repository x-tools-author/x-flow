# --------------------------------------------------------------------------------------------------
add_library(crc32_calc SHARED ${CMAKE_CURRENT_SOURCE_DIR}/crc32_calc/crc32_calc.h
                              ${CMAKE_CURRENT_SOURCE_DIR}/crc32_calc/crc32_calc.c)
target_link_libraries(crc32_calc PUBLIC crc32c)

# --------------------------------------------------------------------------------------------------
# Tests
enable_testing()
add_executable(crc32_calc_unittest ${CMAKE_CURRENT_SOURCE_DIR}/crc32_calc/crc32_calc_unittest.cpp)
target_link_libraries(crc32_calc_unittest PRIVATE gtest gtest_main crc32_calc)
add_test(NAME crc32_calc_unittest COMMAND crc32_calc_unittest)

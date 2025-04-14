# --------------------------------------------------------------------------------------------------
add_library(crc32_calc SHARED ${CMAKE_CURRENT_SOURCE_DIR}/crc32_calc/crc32_calc.h
                              ${CMAKE_CURRENT_SOURCE_DIR}/crc32_calc/crc32_calc.c)
target_link_libraries(crc32_calc PUBLIC crc32c)
set_target_properties(crc32_calc PROPERTIES LIBRARY_OUTPUT_DIRECTORY
                                            ${X_FLOW_PLUGIN_OUTPUT_DIRECTORY})

# --------------------------------------------------------------------------------------------------
# Tests
enable_testing()
add_executable(${PROJECT_NAME}_test ${CMAKE_CURRENT_SOURCE_DIR}/crc32_calc/crc32_calc_unittest.cpp)
target_link_libraries(${PROJECT_NAME}_test PRIVATE gtest gtest_main crc32_calc)
add_test(NAME ${PROJECT_NAME}_test COMMAND ${PROJECT_NAME}_test)

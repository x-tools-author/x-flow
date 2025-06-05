# --------------------------------------------------------------------------------------------------
add_library(bypass SHARED ${CMAKE_CURRENT_SOURCE_DIR}/bypass/bypass.h
                          ${CMAKE_CURRENT_SOURCE_DIR}/bypass/bypass.c)
target_link_libraries(bypass PUBLIC crc32c)

# --------------------------------------------------------------------------------------------------
# Tests
enable_testing()
add_executable(bypass_unittest ${CMAKE_CURRENT_SOURCE_DIR}/bypass/bypass_unittest.cpp)
target_link_libraries(bypass_unittest PRIVATE gtest gtest_main bypass)
add_test(NAME bypass_unittest COMMAND bypass_unittest)

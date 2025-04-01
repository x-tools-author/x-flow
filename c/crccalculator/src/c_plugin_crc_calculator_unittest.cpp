/***************************************************************************************************
 * Copyright 2025-2025 x-tools-author(x-tools@outlook.com). All rights reserved.
 *
 * The file is encoded using "utf8 with bom", it is a part of xFlow project.
 *
 * xFlow is licensed according to the terms in the file LICENCE(GPL V3) in the root of the source
 * code directory.
 **************************************************************************************************/
#include <gtest/gtest.h>

#include <string>

extern "C" {
#include "c_plugin_crc_calculator.h"
}

bool test_crc32_calculator(const unsigned char *input_data,
                           int input_data_length,
                           bool is_big_endian,
                           uint32_t expected_crc_result)
{
    struct x_flow_parameters *params = (x_flow_parameters *) x_flow_new_parameters();
    if (params == NULL) {
        return false;
    }

    // Set parameters
    int format_ascii = 0;
    x_flow_set_parameters(params, 0, (void *) &format_ascii, sizeof(int));
    x_flow_set_parameters(params, 1, (void *) input_data, input_data_length);
    x_flow_set_parameters(params, 2, (void *) &is_big_endian, sizeof(bool));

    // Allocate output buffer
    int output_length = x_flow_get_output_buffer_length();
    unsigned char *output_data = (unsigned char *) malloc(output_length);
    if (output_data == NULL) {
        x_flow_free_parameters(params);
        return false;
    }

    // Process the input data
    int ret = x_flow_handle_in_data(input_data, input_data_length, output_data, (void *) params);
    if (ret != input_data_length + 4) {
        free(output_data);
        return false;
    }

    // Check CRC result
    uint32_t crc_result = *(uint32_t *) (output_data + input_data_length);
    if (crc_result != expected_crc_result) {
        free(output_data);
        return false;
    }

    return true;
}

TEST(CrcCalculator, CRC32_00)
{
    uint8_t buf[32];
    std::memset(buf, 0, sizeof(buf));
    EXPECT_EQ(true, test_crc32_calculator(buf, sizeof(buf), false, 0x8a9136aa));
}

TEST(CrcCalculator, CRC32_00_big_endian)
{
    uint8_t buf[32];
    std::memset(buf, 0, sizeof(buf));
    EXPECT_EQ(true, test_crc32_calculator(buf, sizeof(buf), true, 0xaa36918a));
}

TEST(CrcCalculator, CRC32_ff)
{
    uint8_t buf[32];
    std::memset(buf, 0xff, sizeof(buf));
    EXPECT_EQ(true, test_crc32_calculator(buf, sizeof(buf), false, 0x62a8ab43));
}

TEST(CrcCalculator, CRC32_ff_big_endian)
{
    uint8_t buf[32];
    std::memset(buf, 0xff, sizeof(buf));
    EXPECT_EQ(true, test_crc32_calculator(buf, sizeof(buf), true, 0x43aba862));
}

TEST(CrcCalculator, CRC32_00_32)
{
    uint8_t buf[32];
    for (size_t i = 0; i < 32; ++i) {
        buf[i] = static_cast<uint8_t>(i);
    }

    EXPECT_EQ(true, test_crc32_calculator(buf, sizeof(buf), false, 0x46dd794e));
}

TEST(CrcCalculator, CRC32_00_32_big_endian)
{
    uint8_t buf[32];
    for (size_t i = 0; i < 32; ++i) {
        buf[i] = static_cast<uint8_t>(i);
    }

    EXPECT_EQ(true, test_crc32_calculator(buf, sizeof(buf), true, 0x4e79dd46));
}

TEST(CrcCalculator, CRC32_32_00)
{
    uint8_t buf[32];
    for (size_t i = 0; i < 32; ++i) {
        buf[i] = static_cast<uint8_t>(31 - i);
    }

    EXPECT_EQ(true, test_crc32_calculator(buf, sizeof(buf), false, 0x113fdb5c));
}

TEST(CrcCalculator, CRC32_32_00_big_endian)
{
    uint8_t buf[32];
    for (size_t i = 0; i < 32; ++i) {
        buf[i] = static_cast<uint8_t>(31 - i);
    }

    EXPECT_EQ(true, test_crc32_calculator(buf, sizeof(buf), true, 0x5cdb3f11));
}

TEST(CrcCalculator, CRC32)
{
    uint8_t buf[48] = {
        0x01, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00,
        0x00, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00, 0x18, 0x28, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    };

    EXPECT_EQ(true, test_crc32_calculator(buf, sizeof(buf), false, 0xd9963a56));
}

TEST(CrcCalculator, CRC32_big_endian)
{
    uint8_t buf[48] = {
        0x01, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00,
        0x00, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00, 0x18, 0x28, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    };

    EXPECT_EQ(true, test_crc32_calculator(buf, sizeof(buf), true, 0x563a96d9));
}
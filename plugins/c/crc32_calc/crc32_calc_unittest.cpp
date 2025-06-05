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
#include "crc32_calc.h"
}

bool test_crc32_calculator(const char *input_data,
                           int input_data_length,
                           uint32_t expected_crc_result)
{
    // Allocate output buffer
    int output_length = 1024;
    unsigned char *output_data = (unsigned char *) malloc(output_length);
    // Process the input data
    int ret = x_flow_plugin_handle_data(input_data, input_data_length, output_data, output_length);
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

    free(output_data);
    return true;
}

TEST(CrcCalculator, CRC32_00)
{
    uint8_t buf[32];
    std::memset(buf, 0, sizeof(buf));
    EXPECT_EQ(true, test_crc32_calculator((const char *) buf, sizeof(buf), 0xaa36918a));
}

TEST(CrcCalculator, CRC32_ff)
{
    uint8_t buf[32];
    std::memset(buf, 0xff, sizeof(buf));
    EXPECT_EQ(true, test_crc32_calculator((const char *) buf, sizeof(buf), 0x43aba862));
}

TEST(CrcCalculator, CRC32_00_32)
{
    uint8_t buf[32];
    for (size_t i = 0; i < 32; ++i) {
        buf[i] = static_cast<uint8_t>(i);
    }

    EXPECT_EQ(true, test_crc32_calculator((const char *) buf, sizeof(buf), 0x4e79dd46));
}

TEST(CrcCalculator, CRC32_32_00)
{
    uint8_t buf[32];
    for (size_t i = 0; i < 32; ++i) {
        buf[i] = static_cast<uint8_t>(31 - i);
    }

    EXPECT_EQ(true, test_crc32_calculator((const char *) buf, sizeof(buf), 0x5cdb3f11));
}

TEST(CrcCalculator, CRC32)
{
    uint8_t buf[48] = {
        0x01, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00,
        0x00, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00, 0x18, 0x28, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    };

    EXPECT_EQ(true, test_crc32_calculator((const char *) buf, sizeof(buf), 0x563a96d9));
}
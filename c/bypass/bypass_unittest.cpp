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
#include "bypass.h"
}

bool test_bypass(const char *input_data,
                 int input_data_length,
                 unsigned char *output_data,
                 int output_data_length)
{
    int len = x_flow_plugin_handle_data(input_data,
                                        input_data_length,
                                        output_data,
                                        output_data_length);

    if (len != input_data_length) {
        return false;
    }

    if (len > output_data_length) {
        return false;
    }

    if (len > 0) {
        if (std::memcmp(input_data, output_data, len) != 0) {
            return false;
        }
    }

    return true;
}

TEST(Bypass, Bypass_00)
{
    uint8_t inBuf[32];
    std::memset(inBuf, 0, sizeof(inBuf));
    uint8_t outBuf[32];
    std::memset(outBuf, 0, sizeof(outBuf));

    EXPECT_EQ(true, test_bypass((const char *) inBuf, sizeof(inBuf), outBuf, sizeof(outBuf)));
}

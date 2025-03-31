/***************************************************************************************************
 * Copyright 2025 x-tools-author(x-tools@outlook.com). All rights reserved.
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

std::string hello_world()
{
    return "Hello World";
}

TEST(HelloWorld, hello_world)
{
    ASSERT_EQ("Hello World", hello_world());
}
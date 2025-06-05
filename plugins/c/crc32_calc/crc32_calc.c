/***************************************************************************************************
 * Copyright 2025-2025 x-tools-author(x-tools@outlook.com). All rights reserved.
 *
 * The file is encoded using "utf8 with bom", it is a part of xFlow project.
 *
 * xFlow is licensed according to the terms in the file LICENCE(GPL V3) in the root of the source
 * code directory.
 **************************************************************************************************/
#include "crc32_calc.h"

#include <crc32c/crc32c.h>

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

X_FLOW_API const char* x_flow_plugin_get_api_version()
{
    static const char* plugin_api_version = "1.0";
    return plugin_api_version;
}

X_FLOW_API const char* x_flow_plugin_get_caption()
{
    static const char* caption = "CRC32 Calculator";
    return caption;
}

X_FLOW_API const char* x_flow_plugin_get_name()
{
    static const char* name = "x-tools.x-flow.plugin.crc32_calc";
    return name;
}

X_FLOW_API int x_flow_plugin_get_in_ports()
{
    return 1;
}

X_FLOW_API int x_flow_plugin_get_out_ports()
{
    return 1;
}

X_FLOW_API int x_flow_plugin_handle_data(const char* intput_data,
                                         int input_length,
                                         unsigned char* output_data,
                                         int output_data_length)
{
    memcpy_s(output_data, output_data_length, intput_data, input_length);
    uint32_t result = crc32c_value((const uint8_t*) intput_data, input_length);
    output_data[input_length] = (result >> 24) & 0xFF;
    output_data[input_length + 1] = (result >> 16) & 0xFF;
    output_data[input_length + 2] = (result >> 8) & 0xFF;
    output_data[input_length + 3] = result & 0xFF;

    return input_length + 4;
}

#ifdef __cplusplus
}
#endif

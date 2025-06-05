/***************************************************************************************************
 * Copyright 2025-2025 x-tools-author(x-tools@outlook.com). All rights reserved.
 *
 * The file is encoded using "utf8 with bom", it is a part of xFlow project.
 *
 * xFlow is licensed according to the terms in the file LICENCE(GPL V3) in the root of the source
 * code directory.
 **************************************************************************************************/
#include "bypass.h"

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
    static const char* caption = "Bypass(c)";
    return caption;
}

X_FLOW_API const char* x_flow_plugin_get_name()
{
    static const char* name = "x-tools.x-flow.plugin.bypass.c";
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
    return input_length;
}

#ifdef __cplusplus
}
#endif

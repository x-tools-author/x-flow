/***************************************************************************************************
 * Copyright 2025-2025 x-tools-author(x-tools@outlook.com). All rights reserved.
 *
 * The file is encoded using "utf8 with bom", it is a part of xFlow project.
 *
 * xFlow is licensed according to the terms in the file LICENCE(GPL V3) in the root of the source
 * code directory.
 **************************************************************************************************/
#pragma once

#ifdef _MSC_VER
#ifdef X_FLOW_DLL_EXPORTS
#define X_FLOW_API __declspec(dllexport)
#else
#define X_FLOW_API __declspec(dllimport)
#endif
#else
#define X_FLOW_API
#endif

#ifdef __cplusplus
extern "C" {
#endif

X_FLOW_API const char* x_flow_plugin_get_api_version();
X_FLOW_API const char* x_flow_plugin_get_caption();
X_FLOW_API const char* x_flow_plugin_get_name();
X_FLOW_API int x_flow_plugin_get_in_ports();
X_FLOW_API int x_flow_plugin_get_out_ports();
X_FLOW_API int x_flow_plugin_handle_data(const char* input_data,
                                         int input_data_length,
                                         unsigned char* output_data,
                                         int output_data_length);
#ifdef __cplusplus
}
#endif
/***************************************************************************************************
 * Copyright 2025-2025 x-tools-author(x-tools@outlook.com). All rights reserved.
 *
 * The file is encoded using "utf8 with bom", it is a part of xFlow project.
 *
 * xFlow is licensed according to the terms in the file LICENCE(GPL V3) in the root of the source
 * code directory.
 **************************************************************************************************/
#pragma once

#include <stdbool.h>

#ifdef _MSC_VER
#ifdef CRC32_CALC_DLL_EXPORTS
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

struct crc32_calc_parameters
{
    int format; // 0: ASCII, 1: HEX
    const char* data;
    int data_length;
    bool is_big_endian;
};

/**
 * * @brief Parameters structure for xFlow.
 * * @note This structure is used to pass parameters to the xFlow functions.
 */
X_FLOW_API void* crc32_calc_malloc_parameters();

/**
 * * @brief Free the parameters structure.
 * * @param parameters Pointer to the parameters structure.
 * * @note This function frees the memory allocated for the parameters structure.
 */
X_FLOW_API void crc32_calc_free_parameters(void* parameters);

/**
 * * @brief Set parameters for xFlow.
 * * @param parameters Pointer to the parameters structure.
 * * @param parameter_index Index of the parameter to set.(The index of "plugin-parameters", which defined in conf.json.)
 * * @param value Pointer to the value to set.
 * * @param value_length Length of the value to set.
 * * @note This function sets the specified parameter in the parameters structure.
 */
X_FLOW_API void crc32_calc_set_parameters(void* parameters,
                                          int parameter_index,
                                          void* value,
                                          int value_length);

/**
 * * @brief Get the length of the output buffer.
 * * @return Length of the output buffer.
 */
X_FLOW_API int crc32_calc_get_output_buffer_length();

/**
 * * @brief Handle input data and generate output data.
 * * @param intput_data Pointer to the input data.(Do not free it. It will be freed by the caller.)
 * * @param input_length Length of the input data.
 * * @param output_data Pointer to the output data buffer, the size of the buffer is the value return by crc32-calc_get_output_buffer_length().
 * * @param parameters Pointer to the parameters structure, which newed by crc32-calc_new_parameters().
 * * @note This function processes the input data and generates the output data based on the parameters.
 * * @return Length of the output data.
 * * @note The output data is generated based on the input data and the parameters.
 */
X_FLOW_API int crc32_calc_handle_in_data(const unsigned char* intput_data,
                                         int input_length,
                                         unsigned char* output_data,
                                         void* parameters);
#ifdef __cplusplus
}
#endif
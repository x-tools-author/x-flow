/***************************************************************************************************
 * Copyright 2025-2025 x-tools-author(x-tools@outlook.com). All rights reserved.
 *
 * The file is encoded using "utf8 with bom", it is a part of xFlow project.
 *
 * xFlow is licensed according to the terms in the file LICENCE(GPL V3) in the root of the source
 * code directory.
 **************************************************************************************************/
#include "crc32_calc.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <crc32c/crc32c.h>

X_FLOW_API void* crc32_calc_new_parameters()
{
    struct crc32_calc_parameters* parameters = (struct crc32_calc_parameters*) malloc(
        sizeof(struct crc32_calc_parameters) + sizeof(const char*) * 2);
    if (parameters == NULL) {
        return NULL;
    }
    parameters->format = 0;
    parameters->data = NULL;
    parameters->data_length = 0;
    return parameters;
}

X_FLOW_API void crc32_calc_free_parameters(void* parameters)
{
    if (parameters == NULL) {
        return;
    }

    struct crc32_calc_parameters* params = (struct crc32_calc_parameters*) parameters;
    free(params);
}

X_FLOW_API void crc32_calc_set_parameters(void* parameters,
                                          int parameter_index,
                                          void* value,
                                          int value_length)
{
    if (parameters == NULL) {
        return;
    }

    struct crc32_calc_parameters* params = (struct crc32_calc_parameters*) parameters;
    switch (parameter_index) {
    case 0:
        params->format = *(int*) value;
        break;
    case 1:
        params->data = (const char*) (value);
        params->data_length = value_length;
        break;
    case 2:
        params->is_big_endian = *(bool*) value;
        break;
    default:
        break;
    }
}

X_FLOW_API int crc32_calc_get_output_buffer_length()
{
    return 1024;
}

X_FLOW_API int crc32_calc_handle_in_data(const unsigned char* intput_data,
                                         int input_length,
                                         unsigned char* output_data,
                                         void* parameters)
{
    if (parameters == NULL) {
        return 0;
    }

    int output_length = crc32_calc_get_output_buffer_length();
    struct crc32_calc_parameters* params = (struct crc32_calc_parameters*) parameters;
    bool is_big_endian = params->is_big_endian;
    memcpy_s(output_data, output_length, intput_data, input_length);
    uint32_t result = crc32c_value((const uint8_t*) intput_data, input_length);
    if (params->is_big_endian) {
        output_data[input_length] = (result >> 24) & 0xFF;
        output_data[input_length + 1] = (result >> 16) & 0xFF;
        output_data[input_length + 2] = (result >> 8) & 0xFF;
        output_data[input_length + 3] = result & 0xFF;
    } else {
        output_data[input_length] = result & 0xFF;
        output_data[input_length + 1] = (result >> 8) & 0xFF;
        output_data[input_length + 2] = (result >> 16) & 0xFF;
        output_data[input_length + 3] = (result >> 24) & 0xFF;
    }

    return input_length + 4;
}

#ifdef __cplusplus
}
#endif

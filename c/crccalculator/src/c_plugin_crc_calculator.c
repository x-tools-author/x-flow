/***************************************************************************************************
 * Copyright 2025 x-tools-author(x-tools@outlook.com). All rights reserved.
 *
 * The file is encoded using "utf8 with bom", it is a part of xFlow project.
 *
 * xFlow is licensed according to the terms in the file LICENCE(GPL V3) in the root of the source
 * code directory.
 **************************************************************************************************/
#include "c_plugin_crc_calculator.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "checksum.h"

void crc_calculate(const unsigned char* input_data,
                   int input_length,
                   unsigned char* crc_data,
                   int* crc_data_length,
                   bool is_big_endian,
                   int algorithm)
{
    if (algorithm == CRC_8) {
        unsigned char crc = 0;
        crc = crc_8(input_data, input_length);

        crc_data = (unsigned char*) malloc(1);
        if (crc_data == NULL) {
            crc_data_length = 0;
            return;
        }

        *crc_data_length = 1;
        crc_data[0] = crc;
    } else if (algorithm >= CRC_16 && algorithm <= CRC_16_XMODEM) {
        uint16_t crc = 0;
        switch (algorithm) {
        case CRC_16:
            crc = crc_16(input_data, input_length);
            break;
        case CRC_16_CCITT:
            crc = crc_ccitt_1d0f(input_data, input_length);
            break;
        case CRC_16_CCITT_FALSE:
            crc = crc_ccitt_ffff(input_data, input_length);
            break;
        case CRC_16_DNP:
            crc = crc_dnp(input_data, input_length);
            break;
        case CRC_16_KERMIT:
            crc = crc_kermit(input_data, input_length);
            break;
        case CRC_16_MODBUS:
            crc = crc_modbus(input_data, input_length);
            break;
        case CRC_16_SICK:
            crc = crc_sick(input_data, input_length);
            break;
        case CRC_16_XMODEM:
            crc = crc_xmodem(input_data, input_length);
            break;
        }

        crc_data = (unsigned char*) malloc(2);
        if (crc_data == NULL) {
            *crc_data_length = 0;
            return;
        }

        *crc_data_length = 2;
        if (is_big_endian) {
            crc_data[0] = (crc >> 8) & 0xFF;
            crc_data[1] = crc & 0xFF;
        } else {
            crc_data[0] = crc & 0xFF;
            crc_data[1] = (crc >> 8) & 0xFF;
        }
    } else if (algorithm == CRC_32) {
        uint32_t crc = 0;
        crc = crc_32(input_data, input_length);

        crc_data = (unsigned char*) malloc(4);
        if (crc_data == NULL) {
            crc_data_length = 0;
            return;
        }

        *crc_data_length = 4;
        if (is_big_endian) {
            crc_data[0] = (crc >> 24) & 0xFF;
            crc_data[1] = (crc >> 16) & 0xFF;
            crc_data[2] = (crc >> 8) & 0xFF;
            crc_data[3] = crc & 0xFF;
        } else {
            crc_data[0] = crc & 0xFF;
            crc_data[1] = (crc >> 8) & 0xFF;
            crc_data[2] = (crc >> 16) & 0xFF;
            crc_data[3] = (crc >> 24) & 0xFF;
        }
    } else {
        crc_data = NULL;
        *crc_data_length = 0;
    }
}

struct x_flow_parameters
{
    int format;
    int algorithm;
    const char* data;
    int data_length;
};

X_FLOW_API void* x_flow_new_parameters()
{
    struct x_flow_parameters* parameters = (struct x_flow_parameters*) malloc(
        sizeof(struct x_flow_parameters) + sizeof(const char*) * 2);
    if (parameters == NULL) {
        return NULL;
    }
    parameters->format = 0;
    parameters->algorithm = 0;
    parameters->data = NULL;
    parameters->data_length = 0;
    return parameters;
}

X_FLOW_API void x_flow_free_parameters(void* parameters)
{
    if (parameters == NULL) {
        return;
    }

    struct x_flow_parameters* params = (struct x_flow_parameters*) parameters;
    if (params->data != NULL) {
        free((void*) params->data);
        params->data = NULL;
    }

    free(params);
}

X_FLOW_API void x_flow_set_parameters(void* parameters,
                                      int parameter_index,
                                      void* value,
                                      int value_length)
{
    if (parameters == NULL) {
        return;
    }

    struct x_flow_parameters* params = (struct x_flow_parameters*) parameters;
    switch (parameter_index) {
    case 0:
        params->format = *(int*) value;
        break;
    case 1:
        params->algorithm = *(int*) value;
        break;
    case 2:
        if (params->data != NULL) {
            free((void*) params->data);
            params->data = NULL;
        }
        params->data = (const char*) malloc(value_length);
        if (params->data != NULL) {
            memcpy_s((void*) params->data, value_length, value, value_length);
            params->data_length = value_length;
        }
        break;
    default:
        break;
    }
}

X_FLOW_API void x_flow_handle_in_data(const unsigned char* intput_data,
                                      int input_length,
                                      unsigned char* output_data,
                                      int* output_length,
                                      void* parameters)
{
    if (parameters == NULL) {
        return;
    }

    struct x_flow_parameters* params = (struct x_flow_parameters*) parameters;
    // Handle the input data based on the parameters
    // For example, you can use the format and algorithm to process the data
    // and store the result in output_data.

    unsigned char* crc_data = NULL;
    int crc_data_length = 0;
    bool is_big_endian = false;        // Set this based on your requirements
    int algorithm = params->algorithm; // Use the algorithm from parameters
    crc_calculate(intput_data, input_length, crc_data, &crc_data_length, is_big_endian, algorithm);

    int output_length_temp = input_length + crc_data_length;
    output_data = (unsigned char*) malloc(output_length_temp);
    if (output_data == NULL) {
        *output_length = 0;
        return;
    }

    *output_length = output_length_temp;
    memcpy_s(output_data, output_length_temp, intput_data, input_length);

    // CRC data is appended to the end of the output data
    // The output data is a pointer to the allocated memory, so we need to copy the CRC data to it
    const unsigned char* crc_dst = output_data + input_length;
    int crc_dst_length = output_length_temp - input_length;
    memcpy_s((void*) crc_dst, crc_dst_length, crc_data, crc_data_length);
}

#ifdef __cplusplus
}
#endif

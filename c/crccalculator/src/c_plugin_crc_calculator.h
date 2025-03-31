/***************************************************************************************************
 * Copyright 2025 x-tools-author(x-tools@outlook.com). All rights reserved.
 *
 * The file is encoded using "utf8 with bom", it is a part of xFlow project.
 *
 * xFlow is licensed according to the terms in the file LICENCE(GPL V3) in the root of the source
 * code directory.
 **************************************************************************************************/
#pragma once

#ifdef _MSC_VER
#ifdef CRC_CALCULATOR_DLL_EXPORTS
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

enum crc_algorithm {
    CRC_8,
    CRC_16,
    CRC_16_CCITT,
    CRC_16_CCITT_FALSE,
    CRC_16_DNP,
    CRC_16_KERMIT,
    CRC_16_MODBUS,
    CRC_16_SICK,
    CRC_16_XMODEM,
    CRC_32,
};

/**
 * * @brief Parameters structure for xFlow.
 * * @note This structure is used to pass parameters to the xFlow functions.
 */
X_FLOW_API void* x_flow_new_parameters();

/**
 * * @brief Free the parameters structure.
 * * @param parameters Pointer to the parameters structure.
 * * @note This function frees the memory allocated for the parameters structure.
 */
X_FLOW_API void x_flow_free_parameters(void* parameters);

/**
 * * @brief Set parameters for xFlow.
 * * @param parameters Pointer to the parameters structure.
 * * @param parameter_index Index of the parameter to set.(The index of "plugin-parameters", which defined in conf.json.)
 * * @param value Pointer to the value to set.
 * * @param value_length Length of the value to set.
 * * @note This function sets the specified parameter in the parameters structure.
 */
X_FLOW_API void x_flow_set_parameters(void* parameters,
                                      int parameter_index,
                                      void* value,
                                      int value_length);

/**
 * * @brief Handle input data and generate output data.
 * * @param intput_data Pointer to the input data.(Do not free it. It will be freed by the caller.)
 * * @param input_length Length of the input data.
 * * @param output_data Pointer to the output data buffer.(You need to allocate memory for it. It will be freed by the caller.)
 * * @param output_length Pointer to the length of the output data.
 * * @param parameters Pointer to the parameters structure, which newed by x_flow_new_parameters().
 * * @note This function processes the input data and generates the output data based on the parameters.
 */
X_FLOW_API void x_flow_handle_in_data(const unsigned char* intput_data,
                                      int input_length,
                                      unsigned char* output_data,
                                      int* output_length,
                                      void* parameters);
#ifdef __cplusplus
}
#endif
/***************************************************************************************************
 * Copyright 2025 x-tools-author(x-tools@outlook.com). All rights reserved.
 *
 * The file is encoded using "utf8 with bom", it is a part of xFlow project.
 *
 * xFlow is licensed according to the terms in the file LICENCE(GPL V3) in the root of the source
 * code directory.
 **************************************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

#ifdef CRC_CALCULATOR_DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

DLL_API void crc_calculator_calculate_crc(
    const char *input, int input_len, const char *out, int output_len, void *parameters)
{}

#ifdef __cplusplus
}
#endif

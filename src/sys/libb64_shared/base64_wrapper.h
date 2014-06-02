// $Id$
/**
 * @file    base64_wrapper.h
 * @brief   Redwood specific wrapper for the libb64 encode and decode functions.
 *
 * This wrapper around the libb64 cencode.c is necessary due to the way that the
 * libb64 was written.  While the functions are indeed ANSI C, the process to
 * encode and decode is multistep and not very straight forward.  This wrapper
 * gives a single function interface to all the necessary functionality.
 *
 * The C++ wrapper is already provided via the encode.hpp and decode.hpp
 *
 * @date    03/01/2013
 * @author  Harry Rostovtsev
 * @email   harry_rostovtsev@datacard.com
 * Copyright (C) 2013 Datacard. All rights reserved.
 */
// $Log$
#ifndef BASE64_WRAPPER_H_
#define BASE64_WRAPPER_H_

#include "cencode.h"
#include "cdecode.h"

/**
 * Encodes a char array into base64
 *
 * @param[in]     in: Pointer to char array containing data to encode
 * @param[in]     in_len:  Length of the data to encode (in bytes)
 * @param[in|out] in: Pointer to char array to put encoded data.
 * @param[in]     in_len:  Max length of the encoded data.  This is necessary
 * since encoded data ends up longer than the original.
 *
 * @return n_encoded:
 *      @arg > 0: Number of bytes in the resulting encoded string.
 *      @arg < 0: Errors.
 *
 * Example usage:
 * @code
 *   uint16_t test_in_len = 10;
 *   char test_in[] = "\x06\x08\x02\x10\x00\x18\x02\x02\x08\x00";
 *   cout << "input (size is " << test_in_len << ") ";
 *   for (int i=0; i<(test_in_len); i++) {
 *       printf("%02x ",test_in[i]);
 *   }
 *   printf("\n");
 *
 *   uint16_t test_out_len = 20;
 *   char test_out[test_out_len];
 *   int encoded_sz = base64_encode(test_in, test_in_len, test_out, test_out_len);
 *   cout << "output (size is " << encoded_sz << ") ";
 *   for (int i=0; i<(encoded_sz); i++) {
 *       printf("%c ",test_out[i]);
 *   }
 *   printf("\n");
 *
 * @endcode
 */
int base64_encode(char *in, int in_len, char *out, int out_max_len);

/**
 * Encodes a char array into base64
 *
 * @param[in]     in: Pointer to char array containing encoded data
 * @param[in]     in_len:  Length of the encoded data (in bytes)
 * @param[in|out] in: Pointer to char array to put decoded data.
 * @param[in]     in_len:  Max length of the decoded data.
 *
 * @return n_encoded:
 *      @arg > 0: Number of bytes in the resulting decoded string.
 *      @arg < 0: Errors.
 *
 * Example usage:
 * @code
 *      char decoded_serial_buf[MAX_MSG_LEN];
 *      int decoded_sz = base64_decode(encoded_serial_buf, encoded_sz, decoded_serial_buf, MAX_MSG_LEN);
 *
 * @endcode
 */
int base64_decode(char *in, int in_len, char *out, int out_max_len);

#endif                                                   /* BASE64_WRAPPER_H_ */
/******** Copyright (C) 2013 Datacard. All rights reserved *****END OF FILE****/

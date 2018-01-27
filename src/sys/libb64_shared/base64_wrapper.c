// $Id$
/**
 * @file    base64_wrapper.c
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
 * @email   rost0031@gmail.com
 * Copyright (C) 2013 Harry Rostovtsev. All rights reserved.
 */
// $Log$

#include "base64_wrapper.h"


int base64_encode(char *in, int in_len, char *out, int out_max_len)
{
    base64_encodestate state;
    int outBuf_len = (in_len / 3) * 4;

    if (outBuf_len > out_max_len) {
        return -1;
    }

    base64_init_encodestate(&state);
    int r1 = base64_encode_block(in, in_len, out, &state);
    int r2 = base64_encode_blockend(out+r1, &state);
    base64_init_encodestate(&state);

    int n_encoded = r1 + r2;

    return n_encoded;

}

int base64_decode(char *in, int in_len, char *out, int out_max_len)
{
    base64_decodestate dstate;
    base64_init_decodestate(&dstate);

    int n_decoded = base64_decode_block(in, in_len, out, &dstate);
    base64_init_decodestate(&dstate);

    return n_decoded;
}

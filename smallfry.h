/*
 * Copyright (c) 2014, Derek Buitenhuis
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef SMALLFRY_H
#define SMALLFRY_H

#include <stdint.h>

#include "ijgdec.h"

typedef struct smallfryctx {
    uint8_t *inbuf;
    uint8_t *outbuf;

    uint8_t *decinput;
    uint8_t *decoutput;

    uint8_t *inplanar;
    uint8_t *outplanar;

    jinfo res;

    uint64_t isize;
    uint64_t osize;
} smallfryctx;

smallfryctx *smallfryallocctx(uint8_t *inbuf, uint64_t size);
void smallfryfreectx(smallfryctx **in);
int smallfryoptimize(smallfryctx *ctx);
uint64_t smallfrygetsize(smallfryctx *ctx);
uint8_t *smallfrygetbuffer(smallfryctx *ctx);

#endif

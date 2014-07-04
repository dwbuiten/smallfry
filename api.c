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

#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "smallfry.h"
#include "ijgdec.h"
#include "ijgenc.h"
#include "metric.h"

void smallfryfreectx(smallfryctx **in)
{
    smallfryctx *ctx = *in;

    if (!ctx)
        return;

    if (ctx->outbuf)
        free(ctx->outbuf);

    if (ctx->decinput)
        free(ctx->decinput);

    if (ctx->decoutput)
        free(ctx->decoutput);

    if (ctx->inplanar)
        free(ctx->inplanar);

    if (ctx->outplanar)
        free(ctx->outplanar);

    free(ctx);

    *in = NULL;
}

smallfryctx *smallfryallocctx(uint8_t *inbuf, uint64_t size)
{
    smallfryctx *ret;
    int bufsize;

    ret = calloc(sizeof(smallfryctx), 1);
    if (!ret)
        goto allocfail;

    ret->inbuf = inbuf;
    ret->isize = size;
    ret->osize = 0;

    ret->res = jpeg_get_res(ret->inbuf, ret->isize);
    if (!ret->res.width)
        goto allocfail;

    bufsize = ret->res.width * ret->res.height * 3;

    ret->outbuf    = malloc(bufsize);
    ret->decinput  = malloc(bufsize);
    ret->decoutput = malloc(bufsize);
    ret->inplanar  = malloc(bufsize / 3);
    ret->outplanar = malloc(bufsize / 3);
    if (!ret->outbuf || !ret->decinput || !ret->decoutput || !ret->inplanar || !ret->outplanar)
        goto allocfail;

    return ret;

allocfail:
    smallfryfreectx(&ret);
    return NULL;
}

static void process_planar(uint8_t *interleaved, uint8_t *planar, jinfo res)
{
    int i;
    int j = 0;
    uint8_t *out = planar;

    for (i = 0; i < res.width * res.height * 3; i += 3) {
        out[j] = interleaved[i];
        j++;
    }
}

int smallfryoptimize(smallfryctx *ctx)
{
    const double threshold = 99.91 + 4.13 / 2;

    int ret;
    int qmin = 0, qmax = 100;
    double cur = threshold + 1;

    ret = jpeg_decode(ctx->inbuf, ctx->decinput, ctx->isize);
    if (ret < 0)
        return EINVAL;

    process_planar(ctx->decinput, ctx->inplanar, ctx->res);

    while (qmin <= qmax) {
        int size;
        int quality = (qmin + qmax) / 2;

        size = jpeg_encode(ctx->decinput, ctx->outbuf, ctx->res, quality);

        jpeg_decode(ctx->outbuf, ctx->decoutput, size);

        process_planar(ctx->decoutput, ctx->outplanar, ctx->res);

        cur = getmetric(ctx->inplanar, ctx->outplanar, ctx->res.width, ctx->res.height);

        if (cur < threshold)
            qmin = quality + 1;
        else
            qmax = quality - 1;
    }

    printf("Using quality = %d.\n", qmax + 1);

    ctx->osize = jpeg_encode(ctx->decinput, ctx->outbuf, ctx->res, qmax + 1);

    return 0;
}

uint64_t smallfrygetsize(smallfryctx *ctx)
{
    return ctx->osize;
}

uint8_t *smallfrygetbuffer(smallfryctx *ctx)
{
    return ctx->outbuf;
}

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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "smallfry.h"

#ifdef _MSC_VER
#pragma warning(disable: 4996)
#define fseeko _fseeki64
#define ftello _ftelli64
#endif

int main(int argc, char **argv)
{
    smallfryctx *ctx;
    FILE *in, *out;
    uint8_t *inbuf;
    uint64_t size;
    int ret;

    if (argc < 3) {
        printf("Usage: %s in.jpg out.jpg\n", argv[0]);
        return -1;
    }

    in = fopen(argv[1], "rb");
    if (!in) {
        printf("Cannot open %s.\n", argv[1]);
        return -1;
    }

    out = fopen(argv[2], "wb");
    if (!out) {
        fclose(in);
        printf("Cannot open %s.\n", argv[2]);
        return -1;
    }

    fseeko(in, 0, SEEK_END);
    size = ftello(in);
    fseeko(in, 0, SEEK_SET);

    inbuf = malloc(size);
    if (!inbuf) {
        fclose(out);
        fclose(in);
        printf("Cannot allocate input buffer.\n");
        return -1;
    }

    fread(inbuf, 1, size, in);
    fclose(in);

    ctx = smallfryallocctx(inbuf, size);
    if (!ctx) {
        fclose(out);
        free(inbuf);
        printf("Context allocation failed.\n");
        return -1;
    }

    ret = smallfryoptimize(ctx);
    if (ret)
        printf("Cannot decode.\n");

    fwrite(smallfrygetbuffer(ctx), 1, smallfrygetsize(ctx), out);
    fclose(out);

    smallfryfreectx(&ctx);
    free(inbuf);

    return 0;
}

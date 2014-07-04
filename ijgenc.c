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

#include <jerror.h>
#include <jpeglib.h>

#include "ijgdec.h"
#include "ijgenc.h"

static void init(j_compress_ptr cinfo)
{
}

static boolean empty(j_compress_ptr cinfo)
{
    return TRUE;
}

static void term(j_compress_ptr cinfo)
{
}

int jpeg_encode(uint8_t *in, uint8_t *out, jinfo res, int quality)
{
    JSAMPROW curline[1];
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
    struct jpeg_destination_mgr d;
    int size;

    cinfo.err  = jpeg_std_error(&jerr);

    jpeg_create_compress(&cinfo);

    cinfo.dest             = &d;
    cinfo.image_width      = res.width;
    cinfo.image_height     = res.height;
    cinfo.input_components = 3;
    cinfo.in_color_space   = JCS_YCbCr;

    cinfo.use_moz_defaults = TRUE;

    jpeg_set_defaults(&cinfo);

    cinfo.use_flat_quant_tbl = TRUE;
    cinfo.lambda_log_scale1 = 10.5;
    cinfo.lambda_log_scale2 = 13.0;
   
 cinfo.use_lambda_weight_tbl = TRUE;

    cinfo.dest->init_destination    = &init;
    cinfo.dest->empty_output_buffer = &empty;
    cinfo.dest->term_destination    = &term;
    cinfo.dest->free_in_buffer      = res.width * res.height * 3;
    cinfo.dest->next_output_byte    = (JOCTET *) out;

    cinfo.comp_info[0].v_samp_factor = res.vsamp;
    cinfo.comp_info[0].h_samp_factor = res.hsamp;

    jpeg_set_quality(&cinfo, quality, TRUE);

    jpeg_start_compress(&cinfo, TRUE);

    while (cinfo.next_scanline < cinfo.image_height) {
        curline[0] = &in[cinfo.next_scanline * res.width * 3];
        jpeg_write_scanlines(&cinfo, curline, 1);
    }

    jpeg_finish_compress(&cinfo);

    size = (res.width * res.height * 3) - cinfo.dest->free_in_buffer;

    jpeg_destroy_compress(&cinfo);

    return size;
}

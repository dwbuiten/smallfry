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
#include <string.h>

#include <jerror.h>
#include <jpeglib.h>

#include "ijgdec.h"

static const JOCTET EOI_BUFFER[1] = { JPEG_EOI };

static void init(j_decompress_ptr cinfo)
{
}

static boolean input(j_decompress_ptr cinfo)
{
    cinfo->src->next_input_byte = EOI_BUFFER;
    cinfo->src->bytes_in_buffer = 1;

    return TRUE;
}

static void skip(j_decompress_ptr cinfo, long num_bytes)
{
    if (cinfo->src->bytes_in_buffer < num_bytes) {
        cinfo->src->next_input_byte = EOI_BUFFER;
        cinfo->src->bytes_in_buffer = 1;
    } else {
        cinfo->src->next_input_byte += num_bytes;
        cinfo->src->bytes_in_buffer -= num_bytes;
    }
}

static void term(j_decompress_ptr cinfo)
{
}

jinfo jpeg_get_res(uint8_t *inb, uint64_t insize)
{
    struct jpeg_decompress_struct cinfo;
    struct jpeg_source_mgr s;
    struct jpeg_error_mgr jerr;
    jinfo ret;

    jpeg_create_decompress(&cinfo);

    cinfo.err = jpeg_std_error(&jerr);
    cinfo.src = &s;

    cinfo.src->init_source       = init;
    cinfo.src->fill_input_buffer = input;
    cinfo.src->skip_input_data   = skip;
    cinfo.src->resync_to_restart = jpeg_resync_to_restart;
    cinfo.src->term_source       = term;
    cinfo.src->bytes_in_buffer   = insize;
    cinfo.src->next_input_byte   = (const JOCTET *) inb;

    jpeg_read_header(&cinfo, TRUE);
    jpeg_start_decompress(&cinfo);

    ret.width  = cinfo.output_width;
    ret.height = cinfo.output_height;
    ret.vsamp  = cinfo.comp_info[0].v_samp_factor;
    ret.hsamp  = cinfo.comp_info[0].h_samp_factor;

    jpeg_destroy_decompress(&cinfo);

    return ret;
}

int jpeg_decode(uint8_t *inb, uint8_t *o, uint64_t insize)
{
    JSAMPARRAY outb;
    struct jpeg_decompress_struct cinfo;
    struct jpeg_source_mgr s;
    struct jpeg_error_mgr jerr;

    jpeg_create_decompress(&cinfo);

    cinfo.err = jpeg_std_error(&jerr);
    cinfo.src = &s;

    cinfo.src->init_source       = init;
    cinfo.src->fill_input_buffer = input;
    cinfo.src->skip_input_data   = skip;
    cinfo.src->resync_to_restart = jpeg_resync_to_restart;
    cinfo.src->term_source       = term;
    cinfo.src->bytes_in_buffer   = insize;
    cinfo.src->next_input_byte   = (const JOCTET *) inb;

    jpeg_read_header(&cinfo, TRUE);

    cinfo.out_color_space = JCS_YCbCr;

    jpeg_start_decompress(&cinfo);

    outb = (*cinfo.mem->alloc_sarray)((j_common_ptr) &cinfo, JPOOL_IMAGE,
                                      cinfo.output_width * cinfo.output_components, 1);
    if (!outb)
        return -1;

    while (cinfo.output_scanline < cinfo.output_height) {
        jpeg_read_scanlines(&cinfo, outb, 1);
        memcpy(o + cinfo.output_width * cinfo.output_components * (cinfo.output_scanline - 1),
               outb[0], cinfo.output_width * cinfo.output_components);
    }

    jpeg_finish_decompress(&cinfo);

    jpeg_destroy_decompress(&cinfo);

    return 0;
}

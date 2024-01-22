/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * NITRO is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, If not,
 * see <http://www.gnu.org/licenses/>.
 *
 */

 // J2K isn't part of "nitf" (yet?) so use NITRO, not NITF prefix
#ifndef NITRO_j2k_TileWriter_h_INCLUDED_ 
#define NITRO_j2k_TileWriter_h_INCLUDED_

#include "j2k/Defines.h"

J2K_CXX_GUARD

typedef struct _j2k_stream_t
{
    void /*obj_stream_t*/* opj_stream;
} j2k_stream_t;

#define NITRO_J2K_STREAM_CHUNK_SIZE 0x100000 /** 1 mega by default */ // c.f. OPJ_J2K_STREAM_CHUNK_SIZE in <openjpeg.h>

J2KAPI(j2k_stream_t*) j2k_stream_create(size_t chunkSize, J2K_BOOL isInputStream);
J2KAPI(void) j2k_stream_destroy(j2k_stream_t* pStream);

//----------------------------------------------------------------------------------------------------------------

typedef struct _j2k_image_t
{
    void /*opj_image_t*/* opj_image;
} j2k_image_t;

typedef struct _j2k_image_comptparm // c.f. opj_image_comptparm in <openjpeg.h>
{
    uint32_t dx;     /** XRsiz: horizontal separation of a sample of ith component with respect to the reference grid */
    uint32_t dy;     /** YRsiz: vertical separation of a sample of ith component with respect to the reference grid */
    uint32_t w;     /** data width */
    uint32_t h;     /** data height */
    uint32_t x0;     /** x component offset compared to the whole image */
    uint32_t y0;     /** y component offset compared to the whole image */
    uint32_t prec;     /** precision */
    uint32_t bpp;  /** image depth in bits */
    J2K_BOOL sgnd;     /** signed (1) / unsigned (0) */
} j2k_image_comptparm;

typedef enum J2K_COLOR_SPACE { // c.f. OBJ_COLOR_SPACE in <openjpeg.h>
    J2K_CLRSPC_UNKNOWN = -1,    /**< not supported by the library */
    J2K_CLRSPC_UNSPECIFIED = 0, /**< not specified in the codestream */
    J2K_CLRSPC_SRGB = 1,        /**< sRGB */
    J2K_CLRSPC_GRAY = 2,        /**< grayscale */
    J2K_CLRSPC_SYCC = 3,        /**< YUV */
    J2K_CLRSPC_EYCC = 4,        /**< e-YCC */
    J2K_CLRSPC_CMYK = 5         /**< CMYK */
} J2K_COLOR_SPACE;

J2KAPI(j2k_image_t*) j2k_image_tile_create(uint32_t numcmpts, const j2k_image_comptparm* cmptparms, J2K_COLOR_SPACE clrspc);
J2KAPI(J2K_BOOL) j2k_image_init(j2k_image_t* pImage, int x0, int y0, int x1, int y1, int numcmpts, J2K_COLOR_SPACE color_space);
J2KAPI(void) j2k_image_destroy(j2k_image_t* pImage);

//----------------------------------------------------------------------------------------------------------------

typedef struct _j2k_codec
{
    void /*opj_codec_t*/* opj_codec;
} j2k_codec_t;

typedef struct _j2k_cparameters_t
{
    void /*opj_cparameters_t*/* opj_cparameters;
} j2k_cparameters_t;

/**
 * Callback function prototype for events
 * @param msg               Event message
 * @param client_data       Client object where will be return the event message
 * */
typedef void (*j2k_msg_callback)(const char* msg, void* client_data);

J2KAPI(j2k_codec_t*) j2k_create_compress(void);
J2KAPI(void) j2k_destroy_codec(j2k_codec_t* pEncoder);
J2KAPI(j2k_cparameters_t*) j2k_set_default_encoder_parameters(void);
J2KAPI(void) j2k_destroy_encoder_parameters(j2k_cparameters_t* pParameters);
J2KAPI(NRT_BOOL) j2k_initEncoderParameters(j2k_cparameters_t* pParameters,
    size_t tileRow, size_t tileCol, double compressionRatio, size_t numResolutions);
J2KAPI(NRT_BOOL) j2k_set_error_handler(j2k_codec_t* p_codec, j2k_msg_callback p_callback, void* p_user_data);
J2KAPI(NRT_BOOL) j2k_setup_encoder(j2k_codec_t* p_codec, const j2k_cparameters_t* parameters, j2k_image_t* image);

//----------------------------------------------------------------------------------------------------------------

typedef size_t(*j2k_stream_write_fn)(void* p_buffer, size_t p_nb_bytes, void* p_user_data);
typedef int64_t(*j2k_stream_skip_fn)(int64_t p_nb_bytes, void* p_user_data);
typedef NRT_BOOL(*j2k_stream_seek_fn)(int64_t p_nb_bytes, void* p_user_data);
typedef void (*j2k_stream_free_user_data_fn)(void* p_user_data);

J2KAPI(void) j2k_stream_set_write_function(j2k_stream_t* p_stream, j2k_stream_write_fn p_function);
J2KAPI(void) j2k_stream_set_skip_function(j2k_stream_t* p_stream, j2k_stream_skip_fn p_function);
J2KAPI(void) j2k_stream_set_seek_function(j2k_stream_t* p_stream, j2k_stream_seek_fn p_function);

J2KAPI(void) j2k_stream_set_user_data(j2k_stream_t* p_stream, void* p_data, j2k_stream_free_user_data_fn p_function);

J2KAPI(NRT_BOOL) j2k_flush(j2k_codec_t* p_codec, j2k_stream_t* p_stream);
J2KAPI(NRT_BOOL) j2k_start_compress(j2k_codec_t* p_codec, j2k_image_t* p_image, j2k_stream_t* p_stream);
J2KAPI(NRT_BOOL) j2k_end_compress(j2k_codec_t* p_codec, j2k_stream_t* p_stream);

J2KAPI(NRT_BOOL) j2k_write_tile(j2k_codec_t* p_codec, uint32_t p_tile_index, const uint8_t* p_data, uint32_t p_data_size, j2k_stream_t* p_stream);

J2K_CXX_ENDGUARD

#endif // NITRO_j2k_TileWriter_h_INCLUDED_

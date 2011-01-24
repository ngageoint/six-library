/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2010, General Dynamics - Advanced Information Systems
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

#ifdef HAVE_J2K_H

#include <import/nitf.h>
#include <import/j2k.h>

NITF_CXX_GUARD

NITFPRIV(nitf_CompressionControl*) implOpen(nitf_ImageSubheader*, nitf_Error*);

NITFPRIV(NITF_BOOL) implStart(nitf_CompressionControl *control,
                              nitf_Uint64 offset,
                              nitf_Uint64 dataLength,
                              nitf_Uint64 *blockMask,
                              nitf_Uint64 *padMask,
                              nitf_Error *error);

NITFPRIV(NITF_BOOL) implWriteBlock(nitf_CompressionControl * control,
                                   nitf_IOInterface *io,
                                   nitf_Uint8 *data,
                                   NITF_BOOL pad,
                                   NITF_BOOL noData,
                                   nitf_Error *error);

NITFPRIV(NITF_BOOL) implEnd( nitf_CompressionControl * control,
                             nitf_IOInterface *io,
                             nitf_Error *error);

NITFPRIV(void) implDestroy(nitf_CompressionControl ** control);


static char *ident[] =
{
    "COMPRESSION", "C8", NULL
};

static nitf_CompressionInterface interfaceTable =
{
    implOpen, implStart, implWriteBlock, implEnd, implDestroy, NULL
};

typedef struct _ImplControl
{
    nitf_BlockingInfo blockInfo; /* Kept for convenience */
    j2k_Writer *writer;          /* j2k Writer */
    nitf_Uint64 offset;          /* File offset to data */
    nitf_Uint64 fileLength;      /* Length of compressed data in file */
}
ImplControl;

NITF_CXX_ENDGUARD


NITFAPI(char**) J2KCompress_init(nitf_Error *error)
{
    return ident;
}

NITFAPI(void) C8_cleanup(void)
{
    /* TODO */
}


NITFAPI(void*) C8_construct(char *compressionType,
                            nitf_Error* error)
{
    if (strcmp(compressionType, "C8") != 0)
    {
        nitf_Error_init(error,
                        "Unsupported compression type",
                        NITF_CTXT,
                        NITF_ERR_DECOMPRESSION);

        return NULL;
    }
    return((void *) &interfaceTable);
}


NITFPRIV(nitf_CompressionControl*) implOpen(nitf_ImageSubheader *subheader,
                                            nitf_Error *error)
{
    nitf_Error_init(error, "Not implemented",
                    NITF_CTXT, NITF_ERR_INVALID_OBJECT);
    /* TODO */
    return NULL;
}

NITFPRIV(NITF_BOOL) implStart(nitf_CompressionControl *control,
                              nitf_Uint64 offset,
                              nitf_Uint64 dataLength,
                              nitf_Uint64 *blockMask,
                              nitf_Uint64 *padMask,
                              nitf_Error *error)
{
    ImplControl *implControl = (ImplControl*)control;
    nitf_Error_init(error, "Not implemented",
                    NITF_CTXT, NITF_ERR_INVALID_OBJECT);
    /* TODO */
    return NITF_FAILURE;
}

NITFPRIV(NITF_BOOL) implWriteBlock(nitf_CompressionControl * control,
                                   nitf_IOInterface *io,
                                   nitf_Uint8 *data,
                                   NITF_BOOL pad,
                                   NITF_BOOL noData,
                                   nitf_Error *error)
{
    ImplControl *implControl = (ImplControl*)control;
    nitf_Error_init(error, "Not implemented",
                    NITF_CTXT, NITF_ERR_INVALID_OBJECT);
    /* TODO */
    return NITF_FAILURE;
}

NITFPRIV(NITF_BOOL) implEnd( nitf_CompressionControl * control,
                             nitf_IOInterface *io,
                             nitf_Error *error)
{
    ImplControl *implControl = (ImplControl*)control;
    nitf_Error_init(error, "Not implemented",
                    NITF_CTXT, NITF_ERR_INVALID_OBJECT);
    /* TODO */
    return NITF_FAILURE;
}

NITFPRIV(void) implDestroy(nitf_CompressionControl ** control)
{
    /* TODO */
}

#endif

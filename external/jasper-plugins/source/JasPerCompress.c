/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
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

#include <stdio.h>
#include <string.h>

#include <import/nitf.h>
#include <jasper/jasper.h>
#include "JasPerIOInterface.h"

NITF_CXX_GUARD

typedef struct _ImplControl
{
    nitf_Uint32 nRows; /*!< Number of rows in the image */
    nitf_Uint32 nCols; /*!< Number of columns in the image */
    nitf_Uint32 nBands; /*!< Number of bands in the image */
    nitf_Uint32 nBits; /*!< Number of bits per pixel */
    nitf_Uint32 nBytes; /*!< Number of bits per pixel */
    nitf_Uint32 nppbh; /*!< Number of pixels per block horizontal */
    nitf_Uint32 nppbv; /*!< Number of pixels per block vertical */
    nitf_Uint32 nbpr; /*!< Number of blocks per row */
    nitf_Uint32 nbpc; /*!< Number of blocks per column */
    char irep[NITF_IREP_SZ + 1];
    nitf_Uint64 offset; /*!< Saved start argument */
    nitf_Uint64 dataLength; /*!< Saved start argument */
    jas_image_t *jasImage;
    nitf_Field *comratField;
    nitf_Uint32 curBlock;
} ImplControl;

NITFPRIV(void*) implMemAlloc(size_t size, nitf_Error* error)
{
    void * p = NITF_MALLOC(size);
    memset(p, 0, size);
    if (!p)
    {
        nitf_Error_init(error,
                NITF_STRERROR( NITF_ERRNO ),
                NITF_CTXT,
                NITF_ERR_MEMORY);
    }
    return p;
}

NITFPRIV(void) implMemFree(void* p)
{
    if (p) NITF_FREE(p);
}

NITFPRIV(nitf_CompressionControl*) implOpen(nitf_ImageSubheader * subheader,
        nitf_Error * error)
{
    ImplControl *icntl = NULL;
    nitf_Uint32 nRows;
    nitf_Uint32 nCols;
    nitf_Uint32 nBands;
    nitf_Uint32 nbpp;
    nitf_Uint32 abpp;
    nitf_Uint32 nbpr;
    nitf_Uint32 nbpc;
    nitf_Uint32 nppbh;
    nitf_Uint32 nppbv;
    char pvtype[NITF_PVTYPE_SZ+1];
    char ic[NITF_IC_SZ+1];
    char imode[NITF_IMODE_SZ+1];
    char irep[NITF_IREP_SZ+1];

    if(!nitf_Field_get(subheader->NITF_NROWS, &nRows,
                    NITF_CONV_INT, sizeof(nitf_Uint32), error))
    {
        return NULL;
    }
    if(!nitf_Field_get(subheader->NITF_NCOLS, &nCols,
                    NITF_CONV_INT, sizeof(nitf_Uint32), error))
    {
        return NULL;
    }

    nBands = nitf_ImageSubheader_getBandCount(subheader, error);

    if(!nitf_Field_get(subheader->NITF_NBPP, &nbpp,
                    NITF_CONV_INT, sizeof(nitf_Uint32), error))
    {
        return NULL;
    }
    if(!nitf_Field_get(subheader->NITF_ABPP, &abpp,
                    NITF_CONV_INT, sizeof(nitf_Uint32), error))
    {
        return NULL;
    }
    if(!nitf_Field_get(subheader->NITF_NPPBH, &nppbh,
                    NITF_CONV_INT, sizeof(nitf_Uint32), error))
    {
        return NULL;
    }
    if(!nitf_Field_get(subheader->NITF_NPPBV, &nppbv,
                    NITF_CONV_INT, sizeof(nitf_Uint32), error))
    {
        return NULL;
    }
    if(!nitf_Field_get(subheader->NITF_NBPR, &nbpr,
                    NITF_CONV_INT, sizeof(nitf_Uint32), error))
    {
        return NULL;
    }
    if(!nitf_Field_get(subheader->NITF_NBPC, &nbpc,
                    NITF_CONV_INT, sizeof(nitf_Uint32), error))
    {
        return NULL;
    }

    if(!nitf_Field_get(subheader->NITF_IREP, irep, NITF_CONV_STRING,
                    NITF_IREP_SZ+1, error))
    {
        return NULL;
    }
    if(!nitf_Field_get(subheader->NITF_PVTYPE, pvtype, NITF_CONV_STRING,
                    NITF_PVTYPE_SZ+1, error))
    {
        return NULL;
    }
    if(!nitf_Field_get(subheader->NITF_IC, ic, NITF_CONV_STRING,
                    NITF_IC_SZ+1, error))
    {
        return NULL;
    }
    if(!nitf_Field_get(subheader->NITF_IMODE, imode, NITF_CONV_STRING,
                    NITF_IMODE_SZ+1, error))
    {
        return NULL;
    }

    nitf_Field_trimString(pvtype);
    if(strcmp(pvtype, "INT") != 0 && strcmp(pvtype, "SI") != 0
            && strcmp(pvtype, "B") != 0)
    {
        nitf_Error_init(error,
                "For J2k compression, PVTYPE must be INT, SI or B",
                NITF_CTXT, NITF_ERR_COMPRESSION);
        return NULL;
    }

    nitf_Field_trimString(imode);
    if(strcmp(imode, "B") != 0)
    {
        nitf_Error_init(error,
                "For J2k compression, IMODE must be B",
                NITF_CTXT, NITF_ERR_INVALID_OBJECT);
        return NULL;
    }

    nitf_Field_trimString(ic);
    if(strcmp(ic, "C8") != 0)
    {
        nitf_Error_init(error,
                "For J2k compression, IC must be C8",
                NITF_CTXT, NITF_ERR_COMPRESSION);
        return NULL;
    }

    nitf_Field_trimString(irep);
    if (strcmp(irep, "RGB") == 0)
    {
        if (nBands < 3)
        {
            nitf_Error_init(error,
                    "For RGB irep, must have at least 3 bands",
                    NITF_CTXT, NITF_ERR_COMPRESSION);
            return NULL;
        }
    }
    else if (strcmp(irep, "RGB/LUT") == 0)
    {
        nitf_Error_init(error,
                "RGB/LUT not yet supported",
                NITF_CTXT, NITF_ERR_COMPRESSION);
        return NULL;
    }

    /*  Allocate the compression control structure */
    icntl = implMemAlloc(sizeof(ImplControl), error);
    if (!icntl)
    {
        return NULL;
    }

    icntl->nRows = nRows;
    icntl->nCols = nCols;
    icntl->nBands = nBands;
    icntl->nBits = nbpp;
    icntl->nBytes = NITF_NBPP_TO_BYTES(nbpp);
    icntl->nppbh = nppbh;
    icntl->nppbv = nppbv;
    icntl->nbpr= nbpr;
    icntl->nbpc= nbpc;

    strcpy(icntl->irep, irep);
    icntl->jasImage = NULL;

    icntl->curBlock = 0;
    icntl->comratField = subheader->NITF_COMRAT;

    return icntl;
}

NITFPRIV(NITF_BOOL) implStart(nitf_CompressionControl *control,
        nitf_Uint64 offset,
        nitf_Uint64 dataLength,
        nitf_Uint64 *blockMask,
        nitf_Uint64 *padMask,
        nitf_Error *error)
{
    jas_image_cmptparm_t *cmptparms;
    ImplControl *icntl;
    nitf_Uint32 i, j, k, cmp, numComponents;

    icntl = (ImplControl*)control;
    icntl->offset = offset;
    icntl->dataLength = dataLength;

    /* Not sure if I should multiply by bands */
    numComponents = icntl->nbpr * icntl->nbpc * icntl->nBands;

    cmptparms = implMemAlloc(sizeof(jas_image_cmptparm_t) * numComponents, error);
    if (!cmptparms)
    return NITF_FAILURE;

    for(i = 0, cmp = 0; i < icntl->nbpr; ++i)
    {
        for(j = 0; j < icntl->nbpc; ++j)
        {
            /* TODO Is this really how to deal with multi bands? */
            for(k = 0; k < icntl->nBands; ++k, ++cmp)
            {
                cmptparms[cmp].tlx = icntl->nppbh * j;
                cmptparms[cmp].tly = icntl->nppbv * i;
                cmptparms[cmp].hstep = 1;
                cmptparms[cmp].vstep = 1;
                cmptparms[cmp].width = icntl->nppbh;
                cmptparms[cmp].height = icntl->nppbv;
                cmptparms[cmp].prec = icntl->nBits;
                cmptparms[cmp].sgnd = 0; /* TODO */
            }
        }
    }

    icntl->jasImage = jas_image_create(numComponents, cmptparms,
            JAS_CLRSPC_UNKNOWN);
    if (!icntl->jasImage)
    {
        /*  Or initialize an error!  */
        nitf_Error_init(error,
                "Error creating jasper image",
                NITF_CTXT,
                NITF_ERR_COMPRESSION);
        return NITF_FAILURE;
    }

    /* cleanup */
    implMemFree(cmptparms);

    /* TODO support more types */
    if (strcmp(icntl->irep, "RGB") == 0)
    {
        jas_image_setclrspc(icntl->jasImage, JAS_CLRSPC_GENRGB);
        jas_image_setcmpttype(icntl->jasImage, 0,
                JAS_IMAGE_CT_COLOR(JAS_IMAGE_CT_RGB_R));
        jas_image_setcmpttype(icntl->jasImage, 1,
                JAS_IMAGE_CT_COLOR(JAS_IMAGE_CT_RGB_G));
        jas_image_setcmpttype(icntl->jasImage, 2,
                JAS_IMAGE_CT_COLOR(JAS_IMAGE_CT_RGB_B));
    }
    else
    {
        jas_image_setclrspc(icntl->jasImage, JAS_CLRSPC_GENGRAY);
        for(i = 0; i < numComponents; ++i)
        {
            jas_image_setcmpttype(icntl->jasImage, i,
                    JAS_IMAGE_CT_COLOR(JAS_IMAGE_CT_GRAY_Y));
        }
    }

    return NITF_SUCCESS;
}

#define PRIV_WRITE_MATRIX(_SZ) { \
int rc; \
nitf_Uint##_SZ* data##_SZ = (nitf_Uint##_SZ*)data; \
jas_matrix_t* matrix##_SZ = jas_matrix_create(icntl->nppbv, icntl->nppbh); \
for (i = 0; i < icntl->nppbv; ++i){ \
    for (j = 0; j < icntl->nppbh; ++j){ \
         jas_matrix_set(matrix##_SZ, i, j, (jas_seqent_t)*data##_SZ++); \
    } \
} \
rc = jas_image_writecmpt (icntl->jasImage, icntl->curBlock++,blockCol*icntl->nppbh, \
                          blockRow*icntl->nppbv, icntl->nppbh, icntl->nppbv, \
                          matrix##_SZ); \
jas_matrix_destroy (matrix##_SZ); \
if (rc != 0) { \
    nitf_Error_init(error, "Unable to write image component", NITF_CTXT, \
                    NITF_ERR_COMPRESSION); \
    return NITF_FAILURE; \
} }\


NITFPRIV(NITF_BOOL) implWriteBlock(nitf_CompressionControl * control,
        nitf_IOInterface *io,
        nitf_Uint8 *data,
        NITF_BOOL pad,
        NITF_BOOL noData,
        nitf_Error *error)
{
    ImplControl *icntl;
    nitf_Uint32 i, j, blockCol, blockRow, blockBand, curBlock;
    size_t totalBytes;

    icntl = (ImplControl *) control;

    blockBand = icntl->curBlock % icntl->nBands;
    curBlock = icntl->curBlock / icntl->nBands;
    blockRow = curBlock / icntl->nbpr;
    blockCol = curBlock % icntl->nbpr;

    if (icntl->nBytes == 1)
    {
        PRIV_WRITE_MATRIX(8);
    }
    else if (icntl->nBytes == 2)
    {
        PRIV_WRITE_MATRIX(16);
    }
    else if (icntl->nBytes == 4)
    {
        PRIV_WRITE_MATRIX(32);
    }
    else
    {
        nitf_Error_init(error,
                "Unknown pixel size",
                NITF_CTXT,
                NITF_ERR_COMPRESSION);
        return NITF_FAILURE;
    }
    return NITF_SUCCESS;
}

NITFPRIV(NITF_BOOL) implEnd( nitf_CompressionControl * control,
        nitf_IOInterface *io, nitf_Error *error)
{
    ImplControl *icntl;
    jas_stream_t *stream;
    int rc;
    nitf_Off offset;
    size_t compressedSize, rawSize;
    nitf_Uint32 comratInt;
    float comrat;

    icntl = (ImplControl *) control;
    offset = nitf_IOInterface_tell(io, error);
    stream = JasPer_wrapIOInterface(io, JAS_STREAM_WRITE| JAS_STREAM_BINARY,
                                    error);
    if (!stream)
    {
        return NITF_FAILURE;
    }

    rc = jas_image_encode(icntl->jasImage, stream,
            jas_image_strtofmt((char*)"jpc"), NULL);
    if (rc != 0)
    {
        nitf_Error_init(error,
                "Error encoding image",
                NITF_CTXT,
                NITF_ERR_COMPRESSION);
        return NITF_FAILURE;
    }
    jas_stream_flush(stream);
    rc = jas_stream_close(stream);

    if (rc != 0)
    {
        nitf_Error_init(error,
                "Error closing image",
                NITF_CTXT,
                NITF_ERR_COMPRESSION);
        return NITF_FAILURE;
    }

    jas_image_clearfmts();
    jas_image_destroy(icntl->jasImage);

    /* figure out the compression ratio */
    compressedSize = (size_t)(nitf_IOInterface_tell(io, error) - offset);
    rawSize = icntl->nRows * icntl->nCols * icntl->nBands * icntl->nBytes;

    /*
        Nxyz = JPEG 2000 numerically lossless, where
        "xyz" indicates the expected achieved bit rate (in
        bits per pixel per band) for the final layer of each tile.
        The decimal point is implicit and assumed to be one
        digit from the right (i.e. xy.z).
     */
    comrat = (1.0f * compressedSize * icntl->nBits) / rawSize;
    comratInt = (nitf_Uint32)(comrat * 10.0f + 0.5f);

    /* write the comrat field */
    NITF_SNPRINTF(icntl->comratField->raw, icntl->comratField->length + 1,
                  "N%03d", comratInt);
    /*printf("compression ratio: %f, %s\n", comrat, icntl->comratField->raw);*/

    return NITF_SUCCESS;
}

NITFPRIV(void) implDestroy(nitf_CompressionControl ** control)
{
    ImplControl *implControl = (ImplControl*) *control;
    implMemFree(implControl);
    *control = NULL;
}

static nitf_CompressionInterface interfaceTable = { implOpen, implStart,
                                                    implWriteBlock, implEnd,
                                                    implDestroy, NULL };

static char *ident[] = { "COMPRESSION", "C8", NULL };

NITFAPI(char**) JasPerCompress_init(nitf_Error *error)
{
    /*  Initialize jasper  */
    if (jas_init())
    {
        /*  Or initialize an error!  */
        nitf_Error_init(error,
                "Error initializing jasper library",
                NITF_CTXT,
                NITF_ERR_COMPRESSION);
        return NULL;
    }

    /*  Return the identifier structure  */
    return ident;
}

/* This is a required function for the plugin registry */

NITFAPI(void*) C8_construct(char *compressionType,nitf_Error *error)
{
    if (strcmp(compressionType, "C8") != 0)
    {
        nitf_Error_init(error,"Unsupported compression type",
                NITF_CTXT,NITF_ERR_COMPRESSION);
        return NULL;
    }

    return((void *) &interfaceTable);
}

NITFAPI(void) C8_cleanup(void)
{
    /*  Cleanup the jasper library  */
    jas_cleanup();
}

NITF_CXX_ENDGUARD

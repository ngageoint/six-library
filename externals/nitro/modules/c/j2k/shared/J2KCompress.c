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

#ifdef HAVE_J2K_H

#if _MSC_VER
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#pragma warning(push)
#pragma warning(disable: 5039) // '...': pointer or reference to potentially throwing function passed to '...' 
#include <windows.h>
#pragma warning(pop)
#undef min
#undef max

#pragma warning(disable: 5045) // Compiler will insert Spectre mitigation for memory load if /Qspectre switch specified
#endif // _MSC_VER

#include <import/nitf.h>
#include <import/j2k.h>

NITF_CXX_GUARD

NITFPRIV(nitf_CompressionControl*) implOpen(nitf_ImageSubheader*, nrt_HashTable*, nitf_Error*);

NITFPRIV(NITF_BOOL) implStart(nitf_CompressionControl *control,
                              uint64_t offset,
                              uint64_t dataLength,
                              uint64_t *blockMask,
                              uint64_t *padMask,
                              nitf_Error *error);

NITFPRIV(NITF_BOOL) implWriteBlock(nitf_CompressionControl * control,
                                   nitf_IOInterface *io,
                                   const uint8_t *data,
                                   NITF_BOOL pad,
                                   NITF_BOOL noData,
                                   nitf_Error *error);

NITFPRIV(NITF_BOOL) implEnd( nitf_CompressionControl * control,
                             nitf_IOInterface *io,
                             nitf_Error *error);

NITFPRIV(void) implDestroy(nitf_CompressionControl ** control);


static const char *ident[] =
{
    NITF_PLUGIN_COMPRESSION_KEY, "C8", NULL
};

static nitf_CompressionInterface interfaceTable =
{
    implOpen, implStart, implWriteBlock, implEnd, implDestroy, NULL
};

#if _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4820) // '...': '...' bytes padding added after data member '...'
#endif
typedef struct _ImplControl
{
    nitf_BlockingInfo blockInfo; /* Kept for convenience */
    j2k_Container *container;    /* j2k Container */
    j2k_Writer *writer;          /* j2k Writer */
    uint64_t offset;
    uint64_t dataLength;
    uint32_t curBlock;
    nitf_Field *comratField;     /* kept so we can update it */
}ImplControl;
#if _MSC_VER
#pragma warning(pop)
#endif

NITF_CXX_ENDGUARD


NITFAPI(const char**) J2KCompress_init(nitf_Error *error)
{
    (void)error;
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
                                            nrt_HashTable* userOptions,
                                            nitf_Error *error)
{
    j2k_WriterOptions options;
    ImplControl* implControl = NULL;

    uint32_t nRows;
    uint32_t nCols;
    uint32_t nBands;
    uint32_t nbpp;
    uint32_t abpp;
    uint32_t nbpr;
    uint32_t nbpc;
    uint32_t nppbh;
    uint32_t nppbv;
    char pvtype[NITF_PVTYPE_SZ+1];
    char ic[NITF_IC_SZ+1];
    //char comrat[NITF_COMRAT_SZ+1];
    char imode[NITF_IMODE_SZ+1];
    char irep[NITF_IREP_SZ+1];
    int imageType;
    J2K_BOOL isSigned = 0;
    uint32_t idx = 0;
    j2k_Component** components = NULL;

    /* reset the options */
    memset(&options, 0, sizeof(j2k_WriterOptions));
    if(userOptions)
    {
        if(!j2k_Writer_setOptions(&options, userOptions, error))
            goto CATCH_ERROR;
    }

    if(!nitf_Field_get(subheader->NITF_NROWS, &nRows,
                    NITF_CONV_INT, sizeof(uint32_t), error))
    {
        goto CATCH_ERROR;
    }
    if(!nitf_Field_get(subheader->NITF_NCOLS, &nCols,
                    NITF_CONV_INT, sizeof(uint32_t), error))
    {
        goto CATCH_ERROR;
    }

    if (0 == (nBands = nitf_ImageSubheader_getBandCount(subheader, error)))
    {
        goto CATCH_ERROR;
    }

    if(!nitf_Field_get(subheader->NITF_NBPP, &nbpp,
                    NITF_CONV_INT, sizeof(uint32_t), error))
    {
        goto CATCH_ERROR;
    }
    if(!nitf_Field_get(subheader->NITF_ABPP, &abpp,
                    NITF_CONV_INT, sizeof(uint32_t), error))
    {
        goto CATCH_ERROR;
    }
    if(!nitf_Field_get(subheader->NITF_NPPBH, &nppbh,
                    NITF_CONV_INT, sizeof(uint32_t), error))
    {
        goto CATCH_ERROR;
    }
    if(!nitf_Field_get(subheader->NITF_NPPBV, &nppbv,
                    NITF_CONV_INT, sizeof(uint32_t), error))
    {
        goto CATCH_ERROR;
    }
    if(!nitf_Field_get(subheader->NITF_NBPR, &nbpr,
                    NITF_CONV_INT, sizeof(uint32_t), error))
    {
        goto CATCH_ERROR;
    }
    if(!nitf_Field_get(subheader->NITF_NBPC, &nbpc,
                    NITF_CONV_INT, sizeof(uint32_t), error))
    {
        goto CATCH_ERROR;
    }

    if(!nitf_Field_get(subheader->NITF_IREP, irep, NITF_CONV_STRING,
                    NITF_IREP_SZ+1, error))
    {
        goto CATCH_ERROR;
    }
    if(!nitf_Field_get(subheader->NITF_PVTYPE, pvtype, NITF_CONV_STRING,
                    NITF_PVTYPE_SZ+1, error))
    {
        goto CATCH_ERROR;
    }
    if(!nitf_Field_get(subheader->NITF_IC, ic, NITF_CONV_STRING,
                    NITF_IC_SZ+1, error))
    {
        goto CATCH_ERROR;
    }
    /*if(!nitf_Field_get(subheader->NITF_COMRAT, comrat, NITF_CONV_STRING,
                    NITF_COMRAT_SZ+1, error))
    {
        goto CATCH_ERROR;
        }*/
    if(!nitf_Field_get(subheader->NITF_IMODE, imode, NITF_CONV_STRING,
                    NITF_IMODE_SZ+1, error))
    {
        goto CATCH_ERROR;
    }

    nitf_Field_trimString(pvtype);
    if(strcmp(pvtype, "INT") != 0 && strcmp(pvtype, "SI") != 0
            && strcmp(pvtype, "B") != 0)
    {
        nitf_Error_init(error,
                "For J2k compression, PVTYPE must be INT, SI or B",
                NITF_CTXT, NITF_ERR_COMPRESSION);
        goto CATCH_ERROR;
    }
    if (strcmp(pvtype, "SI") == 0)
    {
        isSigned = J2K_TRUE;
    }

    nitf_Field_trimString(imode);
    if(strcmp(imode, "B") != 0)
    {
        nitf_Error_init(error,
                "For J2k compression, IMODE must be B",
                NITF_CTXT, NITF_ERR_INVALID_OBJECT);
        goto CATCH_ERROR;
    }

    nitf_Field_trimString(ic);
    if(strcmp(ic, "C8") != 0)
    {
        nitf_Error_init(error,
                "For J2k compression, IC must be C8",
                NITF_CTXT, NITF_ERR_COMPRESSION);
        goto CATCH_ERROR;
    }

    /*nitf_Field_trimString(comrat);
    if(strlen(comrat) != 0)
    {
        if (comrat[0] == 'N')
        {
           // numerically lossless
           double bitRate = NITF_ATO32(&comrat[1]) / 10.0;
           options.compressionRatio = bitRate / abpp;
        }
    }*/

    nitf_Field_trimString(irep);
    if (strcmp(irep, "RGB") == 0)
    {
        if (nBands < 3)
        {
            nitf_Error_init(error,
                    "For RGB irep, must have at least 3 bands",
                    NITF_CTXT, NITF_ERR_COMPRESSION);
            goto CATCH_ERROR;
        }
        imageType = J2K_TYPE_RGB;
    }
    else if (strcmp(irep, "RGB/LUT") == 0)
    {
        nitf_Error_init(error,
                "RGB/LUT not yet supported",
                NITF_CTXT, NITF_ERR_COMPRESSION);
        goto CATCH_ERROR;
    }
    else
    {
        /* TODO should be named J2K_TYPE_GRAY */
        imageType = J2K_TYPE_MONO;
    }

    implControl = (ImplControl*)NITF_MALLOC(sizeof(ImplControl));
    if (!implControl)
    {
        nitf_Error_init(error, NITF_STRERROR( NITF_ERRNO ),
                        NITF_CTXT, NITF_ERR_MEMORY);
        goto CATCH_ERROR;
    }
    memset(implControl, 0, sizeof(ImplControl));

    implControl->comratField = subheader->NITF_COMRAT;

    /* initialize the container */
    components = (j2k_Component**)J2K_MALLOC(
        sizeof(j2k_Component*) * nBands);
    if (!components)
    {
        nrt_Error_init(error, NRT_STRERROR(NRT_ERRNO), NRT_CTXT,
                       NRT_ERR_MEMORY);
        goto CATCH_ERROR;
    }

    for(idx = 0; idx < nBands; ++idx)
    {
        components[idx] = j2k_Component_construct(nCols, nRows, abpp,
                                                        isSigned, 0, 0, 1, 1,
                                                        error);
        if (!components[idx])
        {
            goto CATCH_ERROR;
        }
    }

    implControl->container = j2k_Container_construct(nCols,
                                                           nRows,
                                                           nBands,
                                                           components,
                                                           nppbh,
                                                           nppbv,
                                                           imageType,
                                                           error);
    if (!implControl->container)
    {
        goto CATCH_ERROR;
    }

    implControl->writer = j2k_Writer_construct(implControl->container,
                                                     &options, error);
    if (!implControl->writer)
    {
        goto CATCH_ERROR;
    }

    return((nitf_CompressionControl*) implControl);

    CATCH_ERROR:
    {
        if(implControl)
        {
            implDestroy((nitf_CompressionControl **)&implControl);
        }
        return NULL;
    }
}

NITFPRIV(NITF_BOOL) implStart(nitf_CompressionControl *control,
                              uint64_t offset,
                              uint64_t dataLength,
                              uint64_t *blockMask,
                              uint64_t *padMask,
                              nitf_Error *error)
{
    (void)blockMask;
    (void)padMask;
    (void)error;

    ImplControl *implControl = (ImplControl*)control;

    implControl->offset = offset;
    implControl->dataLength = dataLength;
    implControl->curBlock = 0;

    return NITF_SUCCESS;
}

NITFPRIV(NITF_BOOL) implWriteBlock(nitf_CompressionControl * control,
                                   nitf_IOInterface *io,
                                   const uint8_t *data,
                                   NITF_BOOL pad,
                                   NITF_BOOL noData,
                                   nitf_Error *error)
{
    (void)io;
    (void)pad;
    (void)noData;

    ImplControl *implControl = (ImplControl*)control;
    uint32_t tileX, tileY, tileWidth, tileHeight, tilesX;
    uint32_t nComponents, nBytes, bufSize;

    /* TODO this needs to change... won't work on separated images */
    tileWidth = j2k_Container_getTileWidth(implControl->container, error);
    tileHeight = j2k_Container_getTileHeight(implControl->container, error);
    tilesX = j2k_Container_getTilesX(implControl->container, error);
    nComponents = j2k_Container_getNumComponents(implControl->container, error);
    nBytes = (j2k_Container_getPrecision(implControl->container, error) - 1) / 8 + 1;

    tileX = implControl->curBlock % tilesX;
    tileY = implControl->curBlock / tilesX;
    bufSize = tileWidth * tileHeight * nComponents * nBytes;

    if (!j2k_Writer_setTile(implControl->writer, tileX, tileY, data, bufSize,
                            error))
    {
        goto CATCH_ERROR;
    }
    implControl->curBlock++;
    return NITF_SUCCESS;

    CATCH_ERROR:
    {
        return NITF_FAILURE;
    }
}

NITFPRIV(NITF_BOOL) implEnd( nitf_CompressionControl * control,
                             nitf_IOInterface *io,
                             nitf_Error *error)
{
    ImplControl *implControl = (ImplControl*)control;
    nitf_Off offset;
    size_t compressedSize, rawSize;
    uint32_t comratInt;
    float comrat;
    uint32_t width, height, nComponents, nBytes, nBits;

    width = j2k_Container_getWidth(implControl->container, error);
    height = j2k_Container_getHeight(implControl->container, error);
    nComponents = j2k_Container_getNumComponents(implControl->container, error);
    nBits = j2k_Container_getPrecision(implControl->container, error);

    nBytes = (nBits - 1) / 8 + 1;

    offset = nitf_IOInterface_tell(io, error);

    if (!j2k_Writer_write(implControl->writer, io, error))
        return NITF_FAILURE;

    /* figure out the compression ratio */
    compressedSize = (size_t)(nitf_IOInterface_tell(io, error) - offset);
    rawSize = (size_t)width * height * nComponents * nBytes;

    /*
        Nxyz = JPEG 2000 numerically lossless, where
        "xyz" indicates the expected achieved bit rate (in
        bits per pixel per band) for the final layer of each tile.
        The decimal point is implicit and assumed to be one
        digit from the right (i.e. xy.z).
     */
    comrat = (float) (((double)(compressedSize * nBits)) / (double)rawSize);
    comratInt = (uint32_t)(comrat * 10.0f + 0.5f);

    /* write the comrat field */
    NITF_SNPRINTF(implControl->comratField->raw,
                  implControl->comratField->length + 1,
                  "N%03d", comratInt);

    return NITF_SUCCESS;
}

NITFPRIV(void) implDestroy(nitf_CompressionControl ** control)
{
    if (control && *control)
    {
        ImplControl *implControl = (ImplControl*)*control;
        if (implControl->container)
        {
            j2k_Container_destruct(&implControl->container);
        }
        if (implControl->writer)
        {
            j2k_Writer_destruct(&implControl->writer);
        }
        NITF_FREE(*control);
        *control = NULL;
    }
}

#endif

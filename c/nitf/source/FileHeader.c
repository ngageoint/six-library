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

#include "nitf/FileHeader.h"

#define _NITF_CONSTRUCT_FIELD(OWNER, ID, TYPE) \
    OWNER->ID = nitf_Field_construct(ID##_SZ, TYPE, error); \
    if (!OWNER->ID) goto CATCH_ERROR;

#define _NITF_CLONE_FIELD(DEST, SOURCE, ID) \
    DEST->ID = nitf_Field_clone(SOURCE->ID, error); \
    if (!DEST->ID) goto CATCH_ERROR;

#define _NITF_DESTRUCT_FIELD(OWNER, ID) \
    if (OWNER->ID) nitf_Field_destruct(OWNER->ID);

NITFAPI(nitf_FileHeader *) nitf_FileHeader_construct(nitf_Error * error)
{
    /*  Start by allocating the header */
    nitf_FileHeader *header = (nitf_FileHeader *)
                              NITF_MALLOC(sizeof(nitf_FileHeader));

    /*  Return now if we have a problem above */
    if (!header)
    {
        nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO),
                        NITF_CTXT, NITF_ERR_MEMORY);

        goto CATCH_ERROR;
    }
    /*  First, we initialize all of our sections to NULL */
    /*  This will prevent problems when we allocate them */
    header->imageInfo = NULL;
    header->graphicInfo = NULL;
    header->labelInfo = NULL;
    header->textInfo = NULL;
    header->dataExtensionInfo = NULL;
    header->reservedExtensionInfo = NULL;
    header->securityGroup = NULL;
    header->userDefinedSection = NULL;
    header->extendedSection = NULL;

    header->securityGroup = nitf_FileSecurity_construct(error);
    if (!header->securityGroup)
    	goto CATCH_ERROR;

    /*  Zero out a known block of memory, by ID */
    _NITF_CONSTRUCT_FIELD(header, NITF_FHDR, NITF_BCS_A);

    _NITF_CONSTRUCT_FIELD(header, NITF_FVER, NITF_BCS_A);
    _NITF_CONSTRUCT_FIELD(header, NITF_CLEVEL, NITF_BCS_N);
    _NITF_CONSTRUCT_FIELD(header, NITF_STYPE, NITF_BCS_A);
    _NITF_CONSTRUCT_FIELD(header, NITF_OSTAID, NITF_BCS_A);
    _NITF_CONSTRUCT_FIELD(header, NITF_FDT, NITF_BCS_A);
    _NITF_CONSTRUCT_FIELD(header, NITF_FTITLE, NITF_BCS_A);
    _NITF_CONSTRUCT_FIELD(header, NITF_FSCLAS, NITF_BCS_A);

    _NITF_CONSTRUCT_FIELD(header, NITF_FSCOP, NITF_BCS_N);
    _NITF_CONSTRUCT_FIELD(header, NITF_FSCPYS, NITF_BCS_N);
    _NITF_CONSTRUCT_FIELD(header, NITF_ENCRYP, NITF_BCS_A);
    _NITF_CONSTRUCT_FIELD(header, NITF_FBKGC, NITF_BINARY);
    _NITF_CONSTRUCT_FIELD(header, NITF_ONAME, NITF_BCS_A);
    _NITF_CONSTRUCT_FIELD(header, NITF_OPHONE, NITF_BCS_A);

    _NITF_CONSTRUCT_FIELD(header, NITF_FL, NITF_BCS_N);
    _NITF_CONSTRUCT_FIELD(header, NITF_HL, NITF_BCS_N);
    _NITF_CONSTRUCT_FIELD(header, NITF_NUMI, NITF_BCS_N);
    _NITF_CONSTRUCT_FIELD(header, NITF_NUMS, NITF_BCS_N);
    _NITF_CONSTRUCT_FIELD(header, NITF_NUMX, NITF_BCS_N);
    _NITF_CONSTRUCT_FIELD(header, NITF_NUMT, NITF_BCS_N);
    _NITF_CONSTRUCT_FIELD(header, NITF_NUMDES, NITF_BCS_N);
    _NITF_CONSTRUCT_FIELD(header, NITF_NUMRES, NITF_BCS_N);

    _NITF_CONSTRUCT_FIELD(header, NITF_UDHDL, NITF_BCS_N);
    _NITF_CONSTRUCT_FIELD(header, NITF_UDHOFL, NITF_BCS_N);
    _NITF_CONSTRUCT_FIELD(header, NITF_XHDL, NITF_BCS_N);
    _NITF_CONSTRUCT_FIELD(header, NITF_XHDLOFL, NITF_BCS_N);


    header->userDefinedSection = nitf_Extensions_construct(error);
    if (!header->userDefinedSection)
        goto CATCH_ERROR;
    header->extendedSection = nitf_Extensions_construct(error);
    if (!header->extendedSection)
        goto CATCH_ERROR;

    return header;

CATCH_ERROR:
    /* destruct if it was allocated */
    if (header)
        nitf_FileHeader_destruct(&header);
    return NULL;
}


NITFAPI(nitf_FileHeader *) nitf_FileHeader_clone(nitf_FileHeader * source,
        nitf_Error * error)
{
    nitf_Uint32 numImages, numGraphics, numLabels;
    nitf_Uint32 numTexts, numDES, numRES;
    nitf_Uint32 i;

    /*  Start with a NULL pointer  */
    nitf_FileHeader *header = NULL;
    if (source)
    {
        /*  Start by allocating the header */
        header = (nitf_FileHeader *) NITF_MALLOC(sizeof(nitf_FileHeader));
        if (!header)
        {
            nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO),
                            NITF_CTXT, NITF_ERR_MEMORY);
            return NULL;
        }

        header->securityGroup =
            nitf_FileSecurity_clone(source->securityGroup, error);

        header->imageInfo = NULL;
        header->graphicInfo = NULL;
        header->labelInfo = NULL;
        header->textInfo = NULL;
        header->dataExtensionInfo = NULL;
        header->reservedExtensionInfo = NULL;

        /*  These objects all start out NULL,               */
        /*  and if they arent assigned, they remain NULL    */
        /*  This poses a problem when we clone, so for now, */
        /*  Im checking to make sure I have something to    */
        /*  clone in the first place.                       */

        NITF_TRY_GET_UINT32(source->numImages, &numImages, error);
        NITF_TRY_GET_UINT32(source->numGraphics, &numGraphics, error);
        NITF_TRY_GET_UINT32(source->numLabels, &numLabels, error);
        NITF_TRY_GET_UINT32(source->numTexts, &numTexts, error);
        NITF_TRY_GET_UINT32(source->numDataExtensions, &numDES, error);
        NITF_TRY_GET_UINT32(source->numReservedExtensions, &numRES, error);

        if (numImages > 0)
        {
            header->imageInfo =
                (nitf_ComponentInfo **)
                NITF_MALLOC(sizeof(nitf_ComponentInfo *) * numImages);

            if (!header->imageInfo)
            {
                nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO),
                                NITF_CTXT, NITF_ERR_MEMORY);
                goto CATCH_ERROR;
            }

            for (i = 0; i < numImages; ++i)
            {
                header->imageInfo[i] =
                    nitf_ComponentInfo_clone(source->imageInfo[i], error);
                if (!header->imageInfo[i])
                    goto CATCH_ERROR;
            }
        }

        if (numGraphics > 0)
        {
            header->graphicInfo =
                (nitf_ComponentInfo **)
                NITF_MALLOC(sizeof(nitf_ComponentInfo *) * numGraphics);

            if (!header->graphicInfo)
            {
                nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO),
                                NITF_CTXT, NITF_ERR_MEMORY);
                goto CATCH_ERROR;
            }

            for (i = 0; i < numGraphics; ++i)
            {
                header->graphicInfo[i] =
                    nitf_ComponentInfo_clone(source->graphicInfo[i],
                                             error);
                if (!header->graphicInfo[i])
                    goto CATCH_ERROR;
            }
        }

        if (numLabels > 0)
        {
            header->labelInfo =
                (nitf_ComponentInfo **)
                NITF_MALLOC(sizeof(nitf_ComponentInfo *) * numLabels);

            if (!header->labelInfo)
            {
                nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO),
                                NITF_CTXT, NITF_ERR_MEMORY);
                goto CATCH_ERROR;
            }

            for (i = 0; i < numLabels; ++i)
            {
                header->labelInfo[i] =
                    nitf_ComponentInfo_clone(source->labelInfo[i], error);
                if (!header->labelInfo[i])
                    goto CATCH_ERROR;
            }
        }

        if (numTexts > 0)
        {
            header->textInfo =
                (nitf_ComponentInfo **)
                NITF_MALLOC(sizeof(nitf_ComponentInfo *) * numTexts);

            if (!header->textInfo)
            {
                nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO),
                                NITF_CTXT, NITF_ERR_MEMORY);
                goto CATCH_ERROR;
            }

            for (i = 0; i < numTexts; ++i)
            {
                header->textInfo[i] =
                    nitf_ComponentInfo_clone(source->textInfo[i], error);
                if (!header->textInfo[i])
                    goto CATCH_ERROR;
            }
        }

        if (numDES > 0)
        {
            header->dataExtensionInfo =
                (nitf_ComponentInfo **)
                NITF_MALLOC(sizeof(nitf_ComponentInfo *) * numDES);

            if (!header->dataExtensionInfo)
            {
                nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO),
                                NITF_CTXT, NITF_ERR_MEMORY);
                goto CATCH_ERROR;
            }

            for (i = 0; i < numDES; ++i)
            {
                header->dataExtensionInfo[i] =
                    nitf_ComponentInfo_clone(source->dataExtensionInfo[i],
                                             error);
                if (!header->dataExtensionInfo[i])
                    goto CATCH_ERROR;
            }
        }

        if (numRES > 0)
        {
            header->reservedExtensionInfo =
                (nitf_ComponentInfo **)
                NITF_MALLOC(sizeof(nitf_ComponentInfo *) * numRES);

            if (!header->dataExtensionInfo)
            {
                nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO),
                                NITF_CTXT, NITF_ERR_MEMORY);
                goto CATCH_ERROR;
            }

            for (i = 0; i < numRES; ++i)
            {
                header->reservedExtensionInfo[i] =
                    nitf_ComponentInfo_clone(source->
                                             reservedExtensionInfo[i],
                                             error);
                if (!header->reservedExtensionInfo[i])
                    goto CATCH_ERROR;
            }
        }

        /* init these to NULL for safety */
        header->userDefinedSection = NULL;
        header->extendedSection = NULL;

        /*  Copy over extension segments  */
        if (source->userDefinedSection)
        {
            header->userDefinedSection =
                nitf_Extensions_clone(source->userDefinedSection, error);
            if (!header->userDefinedSection)
                goto CATCH_ERROR;
        }

        if (source->extendedSection)
        {
            header->extendedSection =
                nitf_Extensions_clone(source->extendedSection, error);
            if (!header->extendedSection)
                goto CATCH_ERROR;
        }

        /*  Copy a known block of memory, by ID */
        _NITF_CLONE_FIELD(header, source, NITF_FHDR);

        _NITF_CLONE_FIELD(header, source, NITF_FVER);
        _NITF_CLONE_FIELD(header, source, NITF_CLEVEL);
        _NITF_CLONE_FIELD(header, source, NITF_STYPE);
        _NITF_CLONE_FIELD(header, source, NITF_OSTAID);
        _NITF_CLONE_FIELD(header, source, NITF_FDT);
        _NITF_CLONE_FIELD(header, source, NITF_FTITLE);
        _NITF_CLONE_FIELD(header, source, NITF_FSCLAS);

        _NITF_CLONE_FIELD(header, source, NITF_FSCOP);
        _NITF_CLONE_FIELD(header, source, NITF_FSCPYS);
        _NITF_CLONE_FIELD(header, source, NITF_ENCRYP);
        _NITF_CLONE_FIELD(header, source, NITF_FBKGC);
        _NITF_CLONE_FIELD(header, source, NITF_ONAME);
        _NITF_CLONE_FIELD(header, source, NITF_OPHONE);

        /*  Now copy the value fields  */
        _NITF_CLONE_FIELD(header, source, NITF_FL);
        _NITF_CLONE_FIELD(header, source, NITF_HL);
        _NITF_CLONE_FIELD(header, source, NITF_NUMI);
        _NITF_CLONE_FIELD(header, source, NITF_NUMS);
        _NITF_CLONE_FIELD(header, source, NITF_NUMX);
        _NITF_CLONE_FIELD(header, source, NITF_NUMT);
        _NITF_CLONE_FIELD(header, source, NITF_NUMDES);
        _NITF_CLONE_FIELD(header, source, NITF_NUMRES);

        _NITF_CLONE_FIELD(header, source, NITF_UDHDL);
        _NITF_CLONE_FIELD(header, source, NITF_UDHOFL);
        _NITF_CLONE_FIELD(header, source, NITF_XHDL);
        _NITF_CLONE_FIELD(header, source, NITF_XHDLOFL);

        /*  Return the cloned header  */
        return header;
    }

CATCH_ERROR:
    nitf_FileHeader_destruct(&header);
    return NULL;
}


/*
 *  Destruct all sub-objects, and make sure that we
 *  kill this object too.
 */
NITFAPI(void) nitf_FileHeader_destruct(nitf_FileHeader ** fh)
{
    nitf_Uint32 numImages, numGraphics, numLabels;
    nitf_Uint32 numTexts, numDES, numRES;
    nitf_Uint32 i;

    nitf_Error error;

    if (!*fh)
        return;

    if ((*fh)->userDefinedSection)
        nitf_Extensions_destruct(&(*fh)->userDefinedSection);

    if ((*fh)->extendedSection)
        nitf_Extensions_destruct(&(*fh)->extendedSection);

    NITF_TRY_GET_UINT32((*fh)->numImages, &numImages, &error);
    NITF_TRY_GET_UINT32((*fh)->numGraphics, &numGraphics, &error);
    NITF_TRY_GET_UINT32((*fh)->numLabels, &numLabels, &error);
    NITF_TRY_GET_UINT32((*fh)->numTexts, &numTexts, &error);
    NITF_TRY_GET_UINT32((*fh)->numDataExtensions, &numDES, &error);
    NITF_TRY_GET_UINT32((*fh)->numReservedExtensions, &numRES, &error);

    /* need this for the above calls */
CATCH_ERROR:

    for (i = 0; i < numImages && (*fh)->imageInfo; ++i)
        nitf_ComponentInfo_destruct(&(*fh)->imageInfo[i]);

    for (i = 0; i < numGraphics && (*fh)->graphicInfo; ++i)
        nitf_ComponentInfo_destruct(&(*fh)->graphicInfo[i]);

    for (i = 0; i < numLabels && (*fh)->labelInfo; ++i)
        nitf_ComponentInfo_destruct(&(*fh)->labelInfo[i]);

    for (i = 0; i < numTexts && (*fh)->textInfo; ++i)
        nitf_ComponentInfo_destruct(&(*fh)->textInfo[i]);

    for (i = 0; i < numDES && (*fh)->dataExtensionInfo; ++i)
        nitf_ComponentInfo_destruct(&(*fh)->dataExtensionInfo[i]);

    for (i = 0; i < numRES && (*fh)->reservedExtensionInfo; ++i)
        nitf_ComponentInfo_destruct(&(*fh)->reservedExtensionInfo[i]);

    if ((*fh)->imageInfo)
    {
        NITF_FREE((*fh)->imageInfo);
        (*fh)->imageInfo = NULL;
    }
    if ((*fh)->graphicInfo)
    {
        NITF_FREE((*fh)->graphicInfo);
        (*fh)->graphicInfo = NULL;
    }

    if ((*fh)->labelInfo)
    {
        NITF_FREE((*fh)->labelInfo);
        (*fh)->labelInfo = NULL;
    }

    if ((*fh)->textInfo)
    {
        NITF_FREE((*fh)->textInfo);
        (*fh)->textInfo = NULL;
    }
    if ((*fh)->dataExtensionInfo)
    {
        NITF_FREE((*fh)->dataExtensionInfo);
        (*fh)->dataExtensionInfo = NULL;
    }
    if ((*fh)->reservedExtensionInfo)
    {
        NITF_FREE((*fh)->reservedExtensionInfo);
        (*fh)->reservedExtensionInfo = NULL;
    }
    if ((*fh)->securityGroup)
    {
        nitf_FileSecurity_destruct(&(*fh)->securityGroup);
        (*fh)->securityGroup = NULL;
    }

    _NITF_DESTRUCT_FIELD(&(*fh), NITF_FHDR);
    _NITF_DESTRUCT_FIELD(&(*fh), NITF_FVER);
    _NITF_DESTRUCT_FIELD(&(*fh), NITF_CLEVEL);
    _NITF_DESTRUCT_FIELD(&(*fh), NITF_STYPE);
    _NITF_DESTRUCT_FIELD(&(*fh), NITF_OSTAID);
    _NITF_DESTRUCT_FIELD(&(*fh), NITF_FDT);
    _NITF_DESTRUCT_FIELD(&(*fh), NITF_FTITLE);
    _NITF_DESTRUCT_FIELD(&(*fh), NITF_FSCLAS);
    _NITF_DESTRUCT_FIELD(&(*fh), NITF_FSCOP);
    _NITF_DESTRUCT_FIELD(&(*fh), NITF_FSCPYS);
    _NITF_DESTRUCT_FIELD(&(*fh), NITF_ENCRYP);
    _NITF_DESTRUCT_FIELD(&(*fh), NITF_FBKGC);
    _NITF_DESTRUCT_FIELD(&(*fh), NITF_ONAME);
    _NITF_DESTRUCT_FIELD(&(*fh), NITF_OPHONE);
    _NITF_DESTRUCT_FIELD(&(*fh), NITF_FL);
    _NITF_DESTRUCT_FIELD(&(*fh), NITF_HL);
    _NITF_DESTRUCT_FIELD(&(*fh), NITF_NUMI);
    _NITF_DESTRUCT_FIELD(&(*fh), NITF_NUMS);
    _NITF_DESTRUCT_FIELD(&(*fh), NITF_NUMX);
    _NITF_DESTRUCT_FIELD(&(*fh), NITF_NUMT);
    _NITF_DESTRUCT_FIELD(&(*fh), NITF_NUMDES);
    _NITF_DESTRUCT_FIELD(&(*fh), NITF_NUMRES);
    _NITF_DESTRUCT_FIELD(&(*fh), NITF_UDHDL);
    _NITF_DESTRUCT_FIELD(&(*fh), NITF_UDHOFL);
    _NITF_DESTRUCT_FIELD(&(*fh), NITF_XHDL);
    _NITF_DESTRUCT_FIELD(&(*fh), NITF_XHDLOFL);

    NITF_FREE(*fh);
    *fh = NULL;
}


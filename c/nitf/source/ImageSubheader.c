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

#include "nitf/ImageSubheader.h"

#define _NITF_CONSTRUCT_FIELD(OWNER, ID, TYPE) \
    OWNER->ID = nitf_Field_construct(ID##_SZ, TYPE, error); \
    if (!OWNER->ID) goto CATCH_ERROR;

#define _NITF_CLONE_FIELD(DEST, SOURCE, ID) \
    DEST->ID = nitf_Field_clone(SOURCE->ID, error); \
    if (!DEST->ID) goto CATCH_ERROR;

#define _NITF_DESTRUCT_FIELD(OWNER, ID) \
    if (OWNER->ID) nitf_Field_destruct(OWNER->ID);

#define _NITF_BLOCK_DIM_MAX (8192)
#define _NITF_BLOCK_DEFAULT_MAX (1024)
#define _NITF_BLOCK_DEFAULT_MIN (1024)

NITFAPI(nitf_ImageSubheader *)
nitf_ImageSubheader_construct(nitf_Error * error)
{
    /*  Start by allocating the header */
    nitf_ImageSubheader *subhdr = (nitf_ImageSubheader *)
                                  NITF_MALLOC(sizeof(nitf_ImageSubheader));

    /*  Return now if we have a problem above */
    if (!subhdr)
    {
        nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO),
                        NITF_CTXT, NITF_ERR_MEMORY);

        return NULL;
    }

    subhdr->securityGroup = NULL;
    subhdr->imageComments = NULL;

    subhdr->userDefinedSection = NULL;
    subhdr->extendedSection = NULL;

    subhdr->securityGroup = nitf_FileSecurity_construct(error);
    if (!subhdr->securityGroup)
        goto CATCH_ERROR;

    subhdr->imageComments = nitf_List_construct(error);
    if (!subhdr->imageComments)
        goto CATCH_ERROR;


    _NITF_CONSTRUCT_FIELD(subhdr, NITF_IM, NITF_BCS_A);
    _NITF_CONSTRUCT_FIELD(subhdr, NITF_IID1, NITF_BCS_A);
    _NITF_CONSTRUCT_FIELD(subhdr, NITF_IDATIM, NITF_BCS_A);
    _NITF_CONSTRUCT_FIELD(subhdr, NITF_TGTID, NITF_BCS_A);
    _NITF_CONSTRUCT_FIELD(subhdr, NITF_IID2, NITF_BCS_A);
    _NITF_CONSTRUCT_FIELD(subhdr, NITF_ISCLAS, NITF_BCS_A);

    _NITF_CONSTRUCT_FIELD(subhdr, NITF_ENCRYP, NITF_BCS_A);
    _NITF_CONSTRUCT_FIELD(subhdr, NITF_ISORCE, NITF_BCS_A);

    _NITF_CONSTRUCT_FIELD(subhdr, NITF_NROWS, NITF_BCS_N);
    _NITF_CONSTRUCT_FIELD(subhdr, NITF_NCOLS, NITF_BCS_N);

    _NITF_CONSTRUCT_FIELD(subhdr, NITF_PVTYPE, NITF_BCS_A);
    _NITF_CONSTRUCT_FIELD(subhdr, NITF_IREP, NITF_BCS_A);
    _NITF_CONSTRUCT_FIELD(subhdr, NITF_ICAT, NITF_BCS_A);

    _NITF_CONSTRUCT_FIELD(subhdr, NITF_ABPP, NITF_BCS_N);

    _NITF_CONSTRUCT_FIELD(subhdr, NITF_PJUST, NITF_BCS_A);
    _NITF_CONSTRUCT_FIELD(subhdr, NITF_ICORDS, NITF_BCS_A);
    _NITF_CONSTRUCT_FIELD(subhdr, NITF_IGEOLO, NITF_BCS_A);

    _NITF_CONSTRUCT_FIELD(subhdr, NITF_NICOM, NITF_BCS_N);

    _NITF_CONSTRUCT_FIELD(subhdr, NITF_IC, NITF_BCS_A);
    _NITF_CONSTRUCT_FIELD(subhdr, NITF_COMRAT, NITF_BCS_A);

    _NITF_CONSTRUCT_FIELD(subhdr, NITF_NBANDS, NITF_BCS_N);
    _NITF_CONSTRUCT_FIELD(subhdr, NITF_XBANDS, NITF_BCS_N);

    subhdr->bandInfo = NULL;

    _NITF_CONSTRUCT_FIELD(subhdr, NITF_ISYNC, NITF_BCS_N);
    _NITF_CONSTRUCT_FIELD(subhdr, NITF_IMODE, NITF_BCS_A);

    _NITF_CONSTRUCT_FIELD(subhdr, NITF_NBPR, NITF_BCS_N);
    _NITF_CONSTRUCT_FIELD(subhdr, NITF_NBPC, NITF_BCS_N);
    _NITF_CONSTRUCT_FIELD(subhdr, NITF_NPPBH, NITF_BCS_N);
    _NITF_CONSTRUCT_FIELD(subhdr, NITF_NPPBV, NITF_BCS_N);
    _NITF_CONSTRUCT_FIELD(subhdr, NITF_NBPP, NITF_BCS_N);

    _NITF_CONSTRUCT_FIELD(subhdr, NITF_IDLVL, NITF_BCS_N);
    _NITF_CONSTRUCT_FIELD(subhdr, NITF_IALVL, NITF_BCS_N);
    _NITF_CONSTRUCT_FIELD(subhdr, NITF_ILOC, NITF_BCS_N);
    _NITF_CONSTRUCT_FIELD(subhdr, NITF_IMAG, NITF_BCS_A);

    _NITF_CONSTRUCT_FIELD(subhdr, NITF_UDIDL, NITF_BCS_N);
    _NITF_CONSTRUCT_FIELD(subhdr, NITF_UDOFL, NITF_BCS_N);
    _NITF_CONSTRUCT_FIELD(subhdr, NITF_IXSHDL, NITF_BCS_N);
    _NITF_CONSTRUCT_FIELD(subhdr, NITF_IXSOFL, NITF_BCS_N);

    subhdr->userDefinedSection = nitf_Extensions_construct(error);
    if (!subhdr->userDefinedSection)
        goto CATCH_ERROR;
    subhdr->extendedSection = nitf_Extensions_construct(error);
    if (!subhdr->extendedSection)
        goto CATCH_ERROR;

    return subhdr;

CATCH_ERROR:
    nitf_ImageSubheader_destruct(&subhdr);
    return NULL;
}


NITFAPI(nitf_ImageSubheader *)
nitf_ImageSubheader_clone(nitf_ImageSubheader * source, nitf_Error * error)
{
    nitf_Uint32 nbands, i;

    nitf_ImageSubheader *subhdr = NULL;
    if (source)
    {
        subhdr =
            (nitf_ImageSubheader *)
            NITF_MALLOC(sizeof(nitf_ImageSubheader));
        if (!subhdr)
        {
            nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO),
                            NITF_CTXT, NITF_ERR_MEMORY);
            return NULL;
        }

        subhdr->securityGroup =
            nitf_FileSecurity_clone(source->securityGroup, error);
        if (!subhdr->securityGroup)
            goto CATCH_ERROR;

        subhdr->imageComments = nitf_List_clone(source->imageComments,
                                                (NITF_DATA_ITEM_CLONE) nitf_Field_clone, error);
        if (!subhdr->imageComments)
            goto CATCH_ERROR;

        /*  Copy some fields  */
        _NITF_CLONE_FIELD(subhdr, source, NITF_IM);
        _NITF_CLONE_FIELD(subhdr, source, NITF_IID1);
        _NITF_CLONE_FIELD(subhdr, source, NITF_IDATIM);
        _NITF_CLONE_FIELD(subhdr, source, NITF_TGTID);
        _NITF_CLONE_FIELD(subhdr, source, NITF_IID2);
        _NITF_CLONE_FIELD(subhdr, source, NITF_ISCLAS);

        _NITF_CLONE_FIELD(subhdr, source, NITF_ENCRYP);
        _NITF_CLONE_FIELD(subhdr, source, NITF_ISORCE);

        /*  And some values (ints in this case)  */
        _NITF_CLONE_FIELD(subhdr, source, NITF_NROWS);
        _NITF_CLONE_FIELD(subhdr, source, NITF_NCOLS);

        /*  And some more fields  */
        _NITF_CLONE_FIELD(subhdr, source, NITF_PVTYPE);
        _NITF_CLONE_FIELD(subhdr, source, NITF_IREP);
        _NITF_CLONE_FIELD(subhdr, source, NITF_ICAT);

        /*  Even more fields  */
        _NITF_CLONE_FIELD(subhdr, source, NITF_PJUST);
        _NITF_CLONE_FIELD(subhdr, source, NITF_ICORDS);
        _NITF_CLONE_FIELD(subhdr, source, NITF_IGEOLO);

        _NITF_CLONE_FIELD(subhdr, source, NITF_NICOM);
        _NITF_CLONE_FIELD(subhdr, source, NITF_IC);
        _NITF_CLONE_FIELD(subhdr, source, NITF_COMRAT);

        _NITF_CLONE_FIELD(subhdr, source, NITF_NBANDS);
        _NITF_CLONE_FIELD(subhdr, source, NITF_XBANDS);

        subhdr->bandInfo = NULL;

        {
            /* Scope this locally */
            nbands = nitf_ImageSubheader_getBandCount(source, error);
            if (nbands > 0 && nbands != NITF_INVALID_BAND_COUNT)
            {
                subhdr->bandInfo =
                    (nitf_BandInfo **) NITF_MALLOC(sizeof(nitf_BandInfo *)
                                                   * nbands);
                if (!subhdr->bandInfo)
                {
                    nitf_ImageSubheader_destruct(&subhdr);
                    nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO),
                                    NITF_CTXT, NITF_ERR_MEMORY);
                    return NULL;

                }
            }
            if (nbands != NITF_INVALID_BAND_COUNT)
                for (i = 0; i < nbands; i++)
                    subhdr->bandInfo[i] =
                        nitf_BandInfo_clone(source->bandInfo[i], error);
        }

        _NITF_CLONE_FIELD(subhdr, source, NITF_ISYNC);
        _NITF_CLONE_FIELD(subhdr, source, NITF_IMODE);

        _NITF_CLONE_FIELD(subhdr, source, NITF_ABPP);
        _NITF_CLONE_FIELD(subhdr, source, NITF_NBPR);
        _NITF_CLONE_FIELD(subhdr, source, NITF_NBPC);
        _NITF_CLONE_FIELD(subhdr, source, NITF_NPPBH);
        _NITF_CLONE_FIELD(subhdr, source, NITF_NPPBV);
        _NITF_CLONE_FIELD(subhdr, source, NITF_NBPP);

        _NITF_CLONE_FIELD(subhdr, source, NITF_IDLVL);
        _NITF_CLONE_FIELD(subhdr, source, NITF_IALVL);
        _NITF_CLONE_FIELD(subhdr, source, NITF_ILOC);
        _NITF_CLONE_FIELD(subhdr, source, NITF_IMAG);

        _NITF_CLONE_FIELD(subhdr, source, NITF_UDIDL);
        _NITF_CLONE_FIELD(subhdr, source, NITF_UDOFL);
        _NITF_CLONE_FIELD(subhdr, source, NITF_IXSHDL);
        _NITF_CLONE_FIELD(subhdr, source, NITF_IXSOFL);

        /* init these to NULL for safety */
        subhdr->userDefinedSection = NULL;
        subhdr->extendedSection = NULL;

        if (source->userDefinedSection)
        {

            subhdr->userDefinedSection =
                nitf_Extensions_clone(source->userDefinedSection, error);

            if (!subhdr->userDefinedSection)
                goto CATCH_ERROR;
        }

        if (source->extendedSection)
        {
            subhdr->extendedSection =
                nitf_Extensions_clone(source->extendedSection, error);

            if (!subhdr->extendedSection)
                goto CATCH_ERROR;
        }

        return subhdr;
    }

CATCH_ERROR:
    nitf_ImageSubheader_destruct(&subhdr);
    return NULL;

}

NITFAPI(nitf_CornersType)
nitf_ImageSubheader_getCornersType(nitf_ImageSubheader* subheader)
{
    nitf_CornersType type = NITF_CORNERS_UNKNOWN;
    switch (subheader->NITF_ICORDS->raw[0])
    {
    case 'U':
        type = NITF_CORNERS_UTM;
        break;

    case 'N':
        type = NITF_CORNERS_UTM_UPS_N;
        break;

    case 'S':
        type = NITF_CORNERS_UTM_UPS_S;
        break;

    case 'D':
        type = NITF_CORNERS_DECIMAL;
        break;

    case 'G':
        type = NITF_CORNERS_GEO;
        break;

    }
    return type;
}

NITFAPI(NITF_BOOL)
nitf_ImageSubheader_setCornersFromLatLons(nitf_ImageSubheader* subheader,
                                          nitf_CornersType type,
                                          double corners[4][2],
                                          nitf_Error* error)
{

    char cornerRep = nitf_Utils_cornersTypeAsCoordRep(type);
    char *igeolo = subheader->NITF_IGEOLO->raw;
    unsigned int i = 0;
    unsigned int where = 0;

    if (type == NITF_CORNERS_GEO)
    {

        for (; i < 4; i++)
        {
            nitf_Utils_decimalLatToGeoCharArray(corners[i][0], &igeolo[where]);
            where += 7;
            nitf_Utils_decimalLonToGeoCharArray(corners[i][1], &igeolo[where]);
            where += 8;
        }

    }
    else if (type == NITF_CORNERS_DECIMAL)
    {
        for (; i < 4; i++)
        {
            nitf_Utils_decimalLatToCharArray(corners[i][0], &igeolo[where]);
            where += 7;
            nitf_Utils_decimalLonToCharArray(corners[i][1], &igeolo[where]);
            where += 8;
        }
    }
    else
    {
        nitf_Error_initf(error, NITF_CTXT, NITF_ERR_INVALID_PARAMETER,
                         "Can only support IGEOLO 'D' or 'G' for this operation.  Found %c",
                         cornerRep);
        return NITF_FAILURE;
    }

    /* Go ahead and set ICORDS */
    subheader->NITF_ICORDS->raw[0] = cornerRep;
    return NITF_SUCCESS;

}


NITFAPI(NITF_BOOL)
nitf_ImageSubheader_getCornersAsLatLons(nitf_ImageSubheader* subheader,
                                        double corners[4][2],
                                        nitf_Error *error)
{
    nitf_CornersType type = nitf_ImageSubheader_getCornersType(subheader);
    char *igeolo = subheader->NITF_IGEOLO->raw;
    unsigned int i = 0;
    unsigned int where = 0;

    if (type == NITF_CORNERS_GEO)
    {

        for (; i < 4; i++)
        {
            int d, m;
            double s;
            char lat[8];
            char lon[9];
            lat[7] = 0;
            lon[8] = 0;
            memcpy(lat, &igeolo[where], 7);
            where += 7;
            if (!nitf_Utils_parseGeographicString(lat, &d, &m, &s, error))
                return NITF_FAILURE;

            corners[i][0] = nitf_Utils_geographicToDecimal(d, m, s);

            memcpy(lon, &igeolo[where], 8);
            where += 8;

            if (!nitf_Utils_parseGeographicString(lon, &d, &m, &s, error))
                return NITF_FAILURE;

            corners[i][1] = nitf_Utils_geographicToDecimal(d, m, s);

            /* Now I need to get them as decimal */
        }

    }
    else if (type == NITF_CORNERS_DECIMAL)
    {
        for (; i < 4; i++)
        {
            char lat[8];
            char lon[9];
            lat[7] = 0;
            lon[8] = 0;
            memcpy(lat, &igeolo[where], 7);
            where += 7;
            if (!nitf_Utils_parseDecimalString(lat, &(corners[i][0]), error))
                return NITF_FAILURE;

            memcpy(lon, &igeolo[where], 8);
            where += 8;

            if (!nitf_Utils_parseDecimalString(lon, &(corners[i][1]), error))
                return NITF_FAILURE;

        }
    }
    else
    {
        nitf_Error_initf(error, NITF_CTXT, NITF_ERR_INVALID_PARAMETER,
                         "Can only support IGEOLO 'D' or 'G' for this operation.  Found %c",
                         subheader->NITF_ICORDS->raw[0]);
        return NITF_FAILURE;
    }
    return NITF_SUCCESS;
}

NITFAPI(void) nitf_ImageSubheader_destruct(nitf_ImageSubheader ** subhdr)
{
    nitf_Error e;
    nitf_Uint32 nbands, i;

    if (!*subhdr)
        return;

    if ((*subhdr)->userDefinedSection)
    {
        nitf_Extensions_destruct(&(*subhdr)->userDefinedSection);
    }
    if ((*subhdr)->extendedSection)
    {
        nitf_Extensions_destruct(&(*subhdr)->extendedSection);
    }
    if ((*subhdr)->securityGroup)
    {
        nitf_FileSecurity_destruct(&(*subhdr)->securityGroup);
        (*subhdr)->securityGroup = NULL;
    }
    if ((*subhdr)->imageComments)
    {
        nitf_ListIterator iter, end;
        iter = nitf_List_begin((*subhdr)->imageComments);
        end = nitf_List_end((*subhdr)->imageComments);
        while (nitf_ListIterator_notEqualTo(&iter, &end))
        {
            nitf_Field* field = (nitf_Field*) nitf_List_remove(
                                    (*subhdr)->imageComments, &iter);
            if (field) nitf_Field_destruct(&field);
        }
        nitf_List_destruct(&(*subhdr)->imageComments);
        (*subhdr)->imageComments = NULL;
    }
    if ((*subhdr)->bandInfo)
    {
        nbands = nitf_ImageSubheader_getBandCount((*subhdr), &e);

        /*nbands = bands + xBands; */
        if (nbands != NITF_INVALID_BAND_COUNT)
            for (i = 0; i < nbands; i++)
                nitf_BandInfo_destruct(&((*subhdr)->bandInfo[i]));
        NITF_FREE((*subhdr)->bandInfo);
    }

    _NITF_DESTRUCT_FIELD(&(*subhdr), NITF_IM);
    _NITF_DESTRUCT_FIELD(&(*subhdr), NITF_IID1);
    _NITF_DESTRUCT_FIELD(&(*subhdr), NITF_IDATIM);
    _NITF_DESTRUCT_FIELD(&(*subhdr), NITF_TGTID);
    _NITF_DESTRUCT_FIELD(&(*subhdr), NITF_IID2);
    _NITF_DESTRUCT_FIELD(&(*subhdr), NITF_ISCLAS);
    _NITF_DESTRUCT_FIELD(&(*subhdr), NITF_ENCRYP);
    _NITF_DESTRUCT_FIELD(&(*subhdr), NITF_ISORCE);
    _NITF_DESTRUCT_FIELD(&(*subhdr), NITF_NROWS);
    _NITF_DESTRUCT_FIELD(&(*subhdr), NITF_NCOLS);
    _NITF_DESTRUCT_FIELD(&(*subhdr), NITF_PVTYPE);
    _NITF_DESTRUCT_FIELD(&(*subhdr), NITF_IREP);
    _NITF_DESTRUCT_FIELD(&(*subhdr), NITF_ICAT);
    _NITF_DESTRUCT_FIELD(&(*subhdr), NITF_ABPP);
    _NITF_DESTRUCT_FIELD(&(*subhdr), NITF_PJUST);
    _NITF_DESTRUCT_FIELD(&(*subhdr), NITF_ICORDS);
    _NITF_DESTRUCT_FIELD(&(*subhdr), NITF_IGEOLO);
    _NITF_DESTRUCT_FIELD(&(*subhdr), NITF_NICOM);
    _NITF_DESTRUCT_FIELD(&(*subhdr), NITF_IC);
    _NITF_DESTRUCT_FIELD(&(*subhdr), NITF_COMRAT);
    _NITF_DESTRUCT_FIELD(&(*subhdr), NITF_NBANDS);
    _NITF_DESTRUCT_FIELD(&(*subhdr), NITF_XBANDS);
    _NITF_DESTRUCT_FIELD(&(*subhdr), NITF_ISYNC);
    _NITF_DESTRUCT_FIELD(&(*subhdr), NITF_IMODE);
    _NITF_DESTRUCT_FIELD(&(*subhdr), NITF_NBPR);
    _NITF_DESTRUCT_FIELD(&(*subhdr), NITF_NBPC);
    _NITF_DESTRUCT_FIELD(&(*subhdr), NITF_NPPBH);
    _NITF_DESTRUCT_FIELD(&(*subhdr), NITF_NPPBV);
    _NITF_DESTRUCT_FIELD(&(*subhdr), NITF_NBPP);
    _NITF_DESTRUCT_FIELD(&(*subhdr), NITF_IDLVL);
    _NITF_DESTRUCT_FIELD(&(*subhdr), NITF_IALVL);
    _NITF_DESTRUCT_FIELD(&(*subhdr), NITF_ILOC);
    _NITF_DESTRUCT_FIELD(&(*subhdr), NITF_IMAG);
    _NITF_DESTRUCT_FIELD(&(*subhdr), NITF_UDIDL);
    _NITF_DESTRUCT_FIELD(&(*subhdr), NITF_UDOFL);
    _NITF_DESTRUCT_FIELD(&(*subhdr), NITF_IXSHDL);
    _NITF_DESTRUCT_FIELD(&(*subhdr), NITF_IXSOFL);

    NITF_FREE(*subhdr);
    *subhdr = NULL;

}


NITFAPI(NITF_BOOL)
nitf_ImageSubheader_setPixelInformation(nitf_ImageSubheader * subhdr,
                                        const char *pvtype, nitf_Uint32 nbpp,
                                        nitf_Uint32 abpp,
                                        const char *justification,
                                        const char *irep,
                                        const char *icat, nitf_Uint32 bandCount,
                                        nitf_BandInfo ** bands,
                                        nitf_Error * error)
{
    nitf_Uint32 nbands;         /* The NBANDS value */
    nitf_Uint32 xbands;         /* The XBANDS value */
    nitf_Uint32 bandCountOld;   /* The current band count */

    /*    Get currrent band count (need to free old bands) */

    bandCountOld = nitf_ImageSubheader_getBandCount(subhdr, error);
    if (bandCountOld == NITF_INVALID_BAND_COUNT)
        return (NITF_FAILURE);

    if (bandCount > 9)
    {
        nbands = 0;
        xbands = bandCount;
    }
    else
    {
        nbands = bandCount;
        xbands = 0;
    }

    /*
     *    Set the fields. There can be errors if the string lengths are too long.
     */

    if (!nitf_Field_setString(subhdr->NITF_PVTYPE, pvtype, error))
        return (NITF_FAILURE);

    if (!nitf_Field_setString(subhdr->NITF_PJUST, justification, error))
        return (NITF_FAILURE);

    if (!nitf_Field_setUint32(subhdr->NITF_NBPP, nbpp, error))
        return (NITF_FAILURE);

    if (!nitf_Field_setUint32(subhdr->NITF_ABPP, abpp, error))
        return (NITF_FAILURE);

    if (!nitf_Field_setUint32(subhdr->NITF_NBANDS, nbands, error))
        return (NITF_FAILURE);

    if (!nitf_Field_setUint32(subhdr->NITF_XBANDS, xbands, error))
        return (NITF_FAILURE);

    if (!nitf_Field_setString(subhdr->NITF_IREP, irep, error))
        return (NITF_FAILURE);

    if (!nitf_Field_setString(subhdr->NITF_ICAT, icat, error))
        return (NITF_FAILURE);

    /*   Free existing bands */

    if (subhdr->bandInfo != NULL)       /* Free existing band data */
    {
        nitf_Uint32 i;

        for (i = 0; i < bandCountOld; i++)
        {
            nitf_BandInfo_destruct(&(subhdr->bandInfo[i]));
        }
        NITF_FREE(subhdr->bandInfo);
    }
    subhdr->bandInfo = bands;

    return (NITF_SUCCESS);
}


NITFAPI(nitf_Uint32) nitf_ImageSubheader_getBandCount(nitf_ImageSubheader *
        subhdr,
        nitf_Error * error)
{
    nitf_Uint32 nbands;         /* The NBANDS field */
    nitf_Uint32 xbands;         /* The XBANDS field */
    nitf_Uint32 bandCount;      /* The result */

    /*    Get the required subheader values */

    if (!nitf_Field_get(subhdr->NITF_NBANDS, (NITF_DATA *) (&nbands),
                        NITF_CONV_INT, NITF_INT32_SZ, error))
        return NITF_INVALID_BAND_COUNT;
    if (!nitf_Field_get(subhdr->NITF_XBANDS, (NITF_DATA *) (&xbands),
                        NITF_CONV_INT, NITF_INT32_SZ, error))
        return NITF_INVALID_BAND_COUNT;

    /*    The count is nbands unless it is 0. */

    if (nbands != 0)
    {
        bandCount = nbands;
        if (xbands != 0)
        {
            nitf_Error_initf(error, NITF_CTXT, NITF_ERR_INVALID_PARAMETER,
                             "NBANDS (%d) and XBANDS (%d) cannot both be non-zero",
                             nbands, xbands);
            return NITF_INVALID_BAND_COUNT;
        }
    }
    else
        bandCount = xbands;

    if (bandCount > NITF_MAX_BAND_COUNT)
    {
        nitf_Error_initf(error, NITF_CTXT, NITF_ERR_INVALID_PARAMETER,
                         "Invalid band count NBANDS is %d and XBANDS is %d",
                         nbands, xbands);
    }

    return bandCount;
}


NITFAPI(nitf_BandInfo *)
nitf_ImageSubheader_getBandInfo(nitf_ImageSubheader * subhdr,
                                nitf_Uint32 band, nitf_Error * error)
{
    nitf_Uint32 bandCount;      /* The band count for the range check */

    /*   Range check */

    bandCount = nitf_ImageSubheader_getBandCount(subhdr, error);
    if (bandCount == NITF_INVALID_BAND_COUNT)
        return (NULL);

    if (bandCount <= band)
    {
        nitf_Error_initf(error, NITF_CTXT, NITF_ERR_INVALID_PARAMETER,
                         "Band index %d is out of range (band count == %d)",
                         band, bandCount);

        return (NULL);
    }

    return (subhdr->bandInfo[band]);
}


NITFAPI(NITF_BOOL) nitf_ImageSubheader_createBands(nitf_ImageSubheader *
        subhdr,
        nitf_Uint32 numBands,
        nitf_Error * error)
{
    nitf_BandInfo *bandInfo = NULL;     /* temp BandInfo object */
    nitf_BandInfo **infos = NULL;       /* new BandInfo array */
    nitf_Uint32 curBandCount;   /* current band count */
    nitf_Uint32 totalBandCount; /* total band count */
    int i;
    char buf[256];              /* temp buf */

    /* first, get the current number of bands */
    curBandCount = nitf_ImageSubheader_getBandCount(subhdr, error);
    /* check to see if this is a NEW subheader with no bands yet */
    if (curBandCount == NITF_INVALID_BAND_COUNT)
        curBandCount = 0;
    totalBandCount = curBandCount + numBands;

    /* check if invalid values */
    if (totalBandCount > NITF_MAX_BAND_COUNT || numBands <= 0)
    {
        /* throw an error */
        nitf_Error_init(error,
                        "Invalid total Band Count, or invalid numBands",
                        NITF_CTXT, NITF_ERR_INVALID_PARAMETER);
        goto CATCH_ERROR;
    }

    /* set the new array */
    infos =
        (nitf_BandInfo **) NITF_MALLOC(sizeof(nitf_BandInfo *) *
                                       (totalBandCount));
    if (!infos)
    {
        nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO),
                        NITF_CTXT, NITF_ERR_MEMORY);
        goto CATCH_ERROR;
    }

    for (i = 0; subhdr->bandInfo && i < curBandCount; ++i)
    {
        /* copy over old pointers -- assuming old array == curBandCount! */
        infos[i] = subhdr->bandInfo[i];
    }

    /* now, create the new infos */
    for (i = 0; i < numBands; ++i)
    {
        bandInfo = nitf_BandInfo_construct(error);
        if (!bandInfo)
        {
            goto CATCH_ERROR;
        }
        /* set it in the new array */
        infos[curBandCount + i] = bandInfo;
    }

    /* set the new values into the ImageSubheader fields */
    NITF_SNPRINTF(buf, 256, "%.*d", NITF_NBANDS_SZ,
            (totalBandCount > 9 ? 0 : totalBandCount));
    nitf_Field_setRawData(subhdr->numImageBands, buf, NITF_NBANDS_SZ,
                          error);

    NITF_SNPRINTF(buf, 256, "%.*d", NITF_XBANDS_SZ,
            (totalBandCount > 9 ? totalBandCount : 0));
    nitf_Field_setRawData(subhdr->numMultispectralImageBands, buf,
                          NITF_XBANDS_SZ, error);

    /* delete old array, and set equal to the new one */
    if (subhdr->bandInfo)
        NITF_FREE(subhdr->bandInfo);
    subhdr->bandInfo = infos;

    /* ok! */
    return NITF_SUCCESS;

CATCH_ERROR:
    if (bandInfo)
        nitf_BandInfo_destruct(&bandInfo);
    if (infos)
        NITF_FREE(infos);
    return NITF_FAILURE;
}



NITFAPI(NITF_BOOL) nitf_ImageSubheader_removeBand(
    nitf_ImageSubheader * subhdr,
    nitf_Uint32 index,
    nitf_Error * error
)
{
    nitf_BandInfo *bandInfo = NULL;     /* temp BandInfo object */
    nitf_BandInfo **infos = NULL;       /* new BandInfo array */
    nitf_Uint32 curBandCount;   /* current band count */
    int i;
    char buf[256];              /* temp buf */

    /* first, get the current number of bands */
    curBandCount = nitf_ImageSubheader_getBandCount(subhdr, error);
    /* check to see if this is a NEW subheader with no bands yet */
    if (curBandCount == NITF_INVALID_BAND_COUNT || index < 0 ||
            index >= curBandCount)
    {
        nitf_Error_init(error, "Invalid band index",
                        NITF_CTXT, NITF_ERR_INVALID_PARAMETER);
        goto CATCH_ERROR;
    }

    /* decrement the band count */
    curBandCount--;

    /* set the new values into the ImageSubheader fields */
    NITF_SNPRINTF(buf, 256, "%.*d", NITF_NBANDS_SZ,
            (curBandCount > 9 ? 0 : curBandCount));
    nitf_Field_setRawData(subhdr->numImageBands, buf, NITF_NBANDS_SZ,
                          error);
    NITF_SNPRINTF(buf, 256, "%.*d", NITF_XBANDS_SZ,
            (curBandCount > 9 ? curBandCount : 0));
    nitf_Field_setRawData(subhdr->numMultispectralImageBands, buf,
                          NITF_XBANDS_SZ, error);

    /* set the new array */
    infos =
        (nitf_BandInfo **) NITF_MALLOC(sizeof(nitf_BandInfo *) *
                                       (curBandCount));
    if (!infos)
    {
        nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO),
                        NITF_CTXT, NITF_ERR_MEMORY);
        goto CATCH_ERROR;
    }

    for (i = 0; subhdr->bandInfo && i < index; ++i)
    {
        infos[i] = subhdr->bandInfo[i];
    }
    for (i = index; subhdr->bandInfo && i < curBandCount; ++i)
    {
        infos[i] = subhdr->bandInfo[i + 1];
    }

    /* delete old array, and set equal to the new one */
    if (subhdr->bandInfo)
    {
        /* delete the removed bandInfo */
        bandInfo = subhdr->bandInfo[index];
        nitf_BandInfo_destruct(&bandInfo);
        NITF_FREE(subhdr->bandInfo);
    }
    subhdr->bandInfo = infos;

    return NITF_SUCCESS;

CATCH_ERROR:
    if (infos)
        NITF_FREE(infos);
    return NITF_FAILURE;
}


/*
 *     Select a block dimension size based on a dimension
 *
 * This function selects a block dimension (row or column length) given the
 * supplied image dimension. It will select the largest length that is a
 * multiple of four and an exact divisor of the block size between
 * _NITF_BLOCK_DEFAULT_MIN and _NITF_BLOCK_DEFAULT_MAX. If none  is found, it
 * selects the one that produces the least pad.
 *
 *  Note: This function is only called if the image dimension is larger than
 *  8192
 */

NITFPRIV(nitf_Uint32) selectBlockSize(nitf_Uint32 size)
{
    nitf_Uint32 blockSize;      /* Current block size */
    nitf_Uint32 blockSizeMin;   /* Block size that give minimum pad */
    nitf_Uint32 padSize;        /* Current pad size */
    nitf_Uint32 padSizeMin;     /* Current minimum pad size */

    nitf_Uint32 remainder;      /* Remainder of size/blockSize */

    /*   Start at MAX and work backwards */

    blockSize = _NITF_BLOCK_DEFAULT_MAX;
    remainder = size % blockSize;
    if (remainder == 0)
        return (blockSize);
    padSizeMin = blockSize - remainder;
    blockSizeMin = blockSize;

    for (blockSize = _NITF_BLOCK_DEFAULT_MAX - 4;
            blockSize >= _NITF_BLOCK_DEFAULT_MIN; blockSize -= 4)
    {
        remainder = size % blockSize;
        if (remainder == 0)
            return (blockSize);

        padSize = blockSize - remainder;
        if (padSize < padSizeMin)
        {
            padSizeMin = padSize;
            blockSizeMin = blockSize;
        }
    }
    return (blockSizeMin);
}



NITFAPI(NITF_BOOL) nitf_ImageSubheader_getDimensions(nitf_ImageSubheader *
        subhdr,
        nitf_Uint32 * numRows,
        nitf_Uint32 * numCols,
        nitf_Error * error)
{

    if (!nitf_Field_get(subhdr->NITF_NROWS,
                        (NITF_DATA *) numRows, NITF_CONV_INT,
                        NITF_INT32_SZ, error))
        return (NITF_FAILURE);

    if (!nitf_Field_get(subhdr->NITF_NCOLS,
                        (NITF_DATA *) numCols, NITF_CONV_INT,
                        NITF_INT32_SZ, error))
        return (NITF_FAILURE);

    return (NITF_SUCCESS);
}


NITFAPI(NITF_BOOL) nitf_ImageSubheader_getBlocking(nitf_ImageSubheader *
        subhdr,
        nitf_Uint32 * numRows,
        nitf_Uint32 * numCols,
        nitf_Uint32 *
        numRowsPerBlock,
        nitf_Uint32 *
        numColsPerBlock,
        nitf_Uint32 *
        numBlocksPerRow,
        nitf_Uint32 *
        numBlocksPerCol,
        char *imode,
        nitf_Error * error)
{

    if (!nitf_Field_get(subhdr->NITF_NROWS,
                        (NITF_DATA *) numRows, NITF_CONV_INT,
                        NITF_INT32_SZ, error))
        return (NITF_FAILURE);

    if (!nitf_Field_get(subhdr->NITF_NCOLS,
                        (NITF_DATA *) numCols, NITF_CONV_INT,
                        NITF_INT32_SZ, error))
        return (NITF_FAILURE);

    if (!nitf_Field_get(subhdr->NITF_NPPBV,
                        (NITF_DATA *) numRowsPerBlock, NITF_CONV_INT,
                        NITF_INT32_SZ, error))
        return (NITF_FAILURE);

    if (!nitf_Field_get(subhdr->NITF_NPPBH,
                        (NITF_DATA *) numColsPerBlock, NITF_CONV_INT,
                        NITF_INT32_SZ, error))
        return (NITF_FAILURE);

    if (!nitf_Field_get(subhdr->NITF_NBPR,
                        (NITF_DATA *) numBlocksPerRow, NITF_CONV_INT,
                        NITF_INT32_SZ, error))
        return (NITF_FAILURE);

    if (!nitf_Field_get(subhdr->NITF_NBPC,
                        (NITF_DATA *) numBlocksPerCol, NITF_CONV_INT,
                        NITF_INT32_SZ, error))
        return (NITF_FAILURE);

    if (!nitf_Field_get(subhdr->NITF_IMODE,
                        (NITF_DATA *) imode, NITF_CONV_STRING,
                        NITF_IMODE_SZ + 1, error))
        return (NITF_FAILURE);
    nitf_Field_trimString(imode);

    return (NITF_SUCCESS);
}


NITFAPI(NITF_BOOL) nitf_ImageSubheader_getCompression(nitf_ImageSubheader *
        subhdr,
        char *imageCompression,
        char *compressionRate,
        nitf_Error * error)
{

    if (!nitf_Field_get(subhdr->NITF_IC,
                        (NITF_DATA *) imageCompression, NITF_CONV_STRING,
                        NITF_IC_SZ + 1, error))
        return (NITF_FAILURE);
    nitf_Field_trimString(imageCompression);

    if (!nitf_Field_get(subhdr->NITF_COMRAT,
                        (NITF_DATA *) compressionRate, NITF_CONV_STRING,
                        NITF_COMRAT_SZ + 1, error))
        return (NITF_FAILURE);
    nitf_Field_trimString(compressionRate);

    return (NITF_SUCCESS);
}


NITFAPI(NITF_BOOL) nitf_ImageSubheader_setDimensions(nitf_ImageSubheader *
        subhdr,
        nitf_Uint32 numRows,
        nitf_Uint32 numCols,
        nitf_Error * error)
{
    nitf_Uint32 numRowsPerBlock;        /* The number of rows/block */
    nitf_Uint32 numColsPerBlock;        /* The number of columns/block */

    if (numRows <= _NITF_BLOCK_DIM_MAX)
        numRowsPerBlock = numRows;
    else
        numRowsPerBlock = selectBlockSize(numRows);

    if (numCols <= _NITF_BLOCK_DIM_MAX)
        numColsPerBlock = numCols;
    else
        numColsPerBlock = selectBlockSize(numCols);

    return (nitf_ImageSubheader_setBlocking(subhdr, numRows, numCols,
                                            numRowsPerBlock,
                                            numColsPerBlock, "B", error));
}


NITFAPI(NITF_BOOL) nitf_ImageSubheader_setBlocking(nitf_ImageSubheader *subhdr,
        nitf_Uint32 numRows,
        nitf_Uint32 numCols,
        nitf_Uint32 numRowsPerBlock,
        nitf_Uint32 numColsPerBlock,
        const char *imode,
        nitf_Error *error)
{
    nitf_Uint32 numBlocksPerRow;        /* Number of blocks/row */
    nitf_Uint32 numBlocksPerCol;        /* Number of blocks/column */

    /* for 2500C - if > 8192, then NROWS/NCOLS specifies NPPBV/NPPBH */
    if (numRowsPerBlock > _NITF_BLOCK_DIM_MAX)
        numRowsPerBlock = 0;
    if (numColsPerBlock > _NITF_BLOCK_DIM_MAX)
        numColsPerBlock = 0;

    if (!nitf_Field_setUint32(subhdr->NITF_NROWS, numRows, error))
        return (NITF_FAILURE);

    if (!nitf_Field_setUint32(subhdr->NITF_NCOLS, numCols, error))
        return (NITF_FAILURE);

    if (!nitf_Field_setUint32(subhdr->NITF_NPPBV, numRowsPerBlock, error))
        return (NITF_FAILURE);

    if (!nitf_Field_setUint32(subhdr->NITF_NPPBH, numColsPerBlock, error))
        return (NITF_FAILURE);

    /*
       The number of blocks per column is a backwards way of saying the number
       of rows of blocks. So the numBlocksPerCol calculation involves row counts
       and numBlocksPerRow calculation involves column counts
     */

    if (numRowsPerBlock != 0)   /* 2500B */
    {
        numBlocksPerCol = numRows / numRowsPerBlock;
        /* Residual, need one more block */
        if (numRows % numRowsPerBlock != 0)
            numBlocksPerCol += 1;
    }
    else
        numBlocksPerCol = 1;    /* 2500C */

    if (numColsPerBlock != 0)   /* 2500B */
    {
        numBlocksPerRow = numCols / numColsPerBlock;
        /* Residual, need one more block */
        if (numCols % numColsPerBlock != 0)
            numBlocksPerRow += 1;
    }
    else
        numBlocksPerRow = 1;    /* 2500C */

    if (!nitf_Field_setUint32(subhdr->NITF_NBPC, numBlocksPerCol, error))
        return (NITF_FAILURE);

    if (!nitf_Field_setUint32(subhdr->NITF_NBPR, numBlocksPerRow, error))
        return (NITF_FAILURE);

    if (!nitf_Field_setString(subhdr->NITF_IMODE, imode, error))
        return (NITF_FAILURE);

    return (NITF_SUCCESS);
}


NITFAPI(NITF_BOOL) nitf_ImageSubheader_setCompression(nitf_ImageSubheader *subhdr,
        const char *imageCompression,
        const char *compressionRate,
        nitf_Error * error)
{
    if (!nitf_Field_setString(subhdr->NITF_IC, imageCompression, error))
        return (NITF_FAILURE);

    if (!nitf_Field_setString(subhdr->NITF_COMRAT, compressionRate, error))
        return (NITF_FAILURE);

    return (NITF_SUCCESS);
}

NITFAPI(int) nitf_ImageSubheader_insertImageComment
(
    nitf_ImageSubheader * subhdr,
    const char *comment,
    int position,
    nitf_Error * error
)
{
    nitf_Uint32 numComments;
    nitf_ListIterator iterPos;
    nitf_Field* field = NULL;
    char numCommentBuf[NITF_NICOM_SZ + 1];
    char commentBuf[NITF_ICOM_SZ + 1];
    int length;

    NITF_TRY_GET_UINT32(subhdr->numImageComments, &numComments, error);
    /* in case there is bad info in numImageComments */
    numComments = numComments < 0 ? 0 : numComments;

    /* see if we can really add another one */
    if (numComments < 9)
    {
        /* check the position */
        if (position < 0 || position > numComments)
            position = numComments;

        /* make the field */
        field = nitf_Field_construct(NITF_ICOM_SZ, NITF_BCS_A, error);
        if (!field) goto CATCH_ERROR;
        memset(commentBuf, 0, NITF_ICOM_SZ + 1);

        length = comment != NULL ? strlen(comment) : 0;

        if (length > 0)
            memcpy(commentBuf, comment, length > NITF_ICOM_SZ ? NITF_ICOM_SZ : length);

        /* Warning: if the comment string is greater than the size of the
         * field,an error will be set -- which is why we copy it here
         * It might be nice to have a size param in nitf_Field_setString */
        if (!nitf_Field_setString(field, commentBuf, error))
            goto CATCH_ERROR;

        /* find the iter where we want to insert */
        iterPos = nitf_List_at(subhdr->imageComments, position);
        if (!nitf_List_insert(subhdr->imageComments, iterPos, field, error))
            goto CATCH_ERROR;

        /* always set the numComments back */
        NITF_SNPRINTF(numCommentBuf, NITF_NICOM_SZ + 1, "%.*d",
                NITF_NICOM_SZ, ++numComments);
        nitf_Field_setRawData(subhdr->numImageComments,
                              numCommentBuf, NITF_NICOM_SZ, error);
    }
    else
    {
        nitf_Error_init(error,
                        "insertComment -> can't add another comment",
                        NITF_CTXT, NITF_ERR_INVALID_PARAMETER);
        goto CATCH_ERROR;
    }
    return position;

CATCH_ERROR:
    return -1;
}

NITFAPI(NITF_BOOL) nitf_ImageSubheader_removeImageComment
(
    nitf_ImageSubheader * subhdr,
    int position,
    nitf_Error * error
)
{
    nitf_Uint32 numComments;    /* number of comments */
    char commentBuf[NITF_NICOM_SZ + 1];
    nitf_ListIterator iterPos;
    nitf_Field* field = NULL;

    NITF_TRY_GET_UINT32(subhdr->numImageComments, &numComments, error);
    numComments = numComments < 0 ? 0 : numComments;

    /* see if we can really remove anything */
    if (numComments > 0 && position >= 0 && position < numComments)
    {

        iterPos = nitf_List_at(subhdr->imageComments, position);
        field = (nitf_Field*)nitf_ListIterator_get(&iterPos);
        if (field) nitf_Field_destruct(&field);
        if (!nitf_List_remove(subhdr->imageComments, &iterPos))
            goto CATCH_ERROR;

        /* always set the numComments back */
        NITF_SNPRINTF(commentBuf, NITF_NICOM_SZ + 1, "%.*d",
                      NITF_NICOM_SZ, --numComments);
        nitf_Field_setRawData(subhdr->numImageComments,
                              commentBuf, NITF_NICOM_SZ, error);
    }
    else
    {
        nitf_Error_init(error,
                        "removeComment -> Invalid index, or nothing to remove",
                        NITF_CTXT, NITF_ERR_INVALID_PARAMETER);
        goto CATCH_ERROR;
    }
    return NITF_SUCCESS;

CATCH_ERROR:
    return NITF_FAILURE;
}


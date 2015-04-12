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

#include "nitf/BandInfo.h"

#define _NITF_CONSTRUCT_FIELD(OWNER, ID, TYPE) \
    OWNER->ID = nitf_Field_construct(ID##_SZ, TYPE, error); \
    if (!OWNER->ID) goto CATCH_ERROR;

#define _NITF_CLONE_FIELD(DEST, SOURCE, ID) \
    DEST->ID = nitf_Field_clone(SOURCE->ID, error); \
    if (!DEST->ID) goto CATCH_ERROR;

#define _NITF_DESTRUCT_FIELD(OWNER, ID) \
    if (OWNER->ID) nitf_Field_destruct(OWNER->ID);

NITFAPI(nitf_BandInfo *) nitf_BandInfo_construct(nitf_Error * error)
{
    /*  Start by allocating the struct */
    nitf_BandInfo *info =
        (nitf_BandInfo *) NITF_MALLOC(sizeof(nitf_BandInfo));

    /*  Return now if we have a problem above */
    if (!info)
    {
        nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO),
                        NITF_CTXT, NITF_ERR_MEMORY);
        return NULL;
    }

    /* This MUST happen before we construct fields */
    /* in case destruction is required */
    info->lut = NULL;

    _NITF_CONSTRUCT_FIELD(info, NITF_IREPBAND, NITF_BCS_A);
    _NITF_CONSTRUCT_FIELD(info, NITF_ISUBCAT, NITF_BCS_A);
    _NITF_CONSTRUCT_FIELD(info, NITF_IFC, NITF_BCS_A);
    _NITF_CONSTRUCT_FIELD(info, NITF_IMFLT, NITF_BCS_A);
    _NITF_CONSTRUCT_FIELD(info, NITF_NLUTS, NITF_BCS_N);
    _NITF_CONSTRUCT_FIELD(info, NITF_NELUT, NITF_BCS_N);

    return info;

CATCH_ERROR:
    /* destruct if it was allocated */
    if (info)
        nitf_BandInfo_destruct(&info);
    return NULL;
}


NITFAPI(void) nitf_BandInfo_destruct(nitf_BandInfo ** info)
{
    if (*info == NULL)
        return;

    _NITF_DESTRUCT_FIELD(&(*info), NITF_IREPBAND);
    _NITF_DESTRUCT_FIELD(&(*info), NITF_ISUBCAT);
    _NITF_DESTRUCT_FIELD(&(*info), NITF_IFC);
    _NITF_DESTRUCT_FIELD(&(*info), NITF_IMFLT);
    _NITF_DESTRUCT_FIELD(&(*info), NITF_NLUTS);
    _NITF_DESTRUCT_FIELD(&(*info), NITF_NELUT);

    if (&(*info)->lut)
    {
        nitf_LookupTable_destruct(&(*info)->lut);
    }

    NITF_FREE(*info);
    *info = NULL;
}


NITFPROT(nitf_BandInfo *) nitf_BandInfo_clone(nitf_BandInfo * source,
        nitf_Error * error)
{
    /*  Start with a NULL pointer  */
    nitf_BandInfo *info = NULL;
    if (source)
    {
        info = (nitf_BandInfo *) NITF_MALLOC(sizeof(nitf_BandInfo));
        if (!info)
        {
            nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO),
                            NITF_CTXT, NITF_ERR_MEMORY);
            return NULL;
        }
        /* This must occur before cloning, in case an error */
        /* occurs and the object must be destructed         */
        info->lut = NULL;

        _NITF_CLONE_FIELD(info, source, NITF_IREPBAND);
        _NITF_CLONE_FIELD(info, source, NITF_ISUBCAT);
        _NITF_CLONE_FIELD(info, source, NITF_IFC);
        _NITF_CLONE_FIELD(info, source, NITF_IMFLT);
        _NITF_CLONE_FIELD(info, source, NITF_NLUTS);
        _NITF_CLONE_FIELD(info, source, NITF_NELUT);

        if (source->lut)
        {
            info->lut = nitf_LookupTable_clone(source->lut, error);
        }

        return info;
    }

CATCH_ERROR:
    return NULL;
}


NITFAPI(NITF_BOOL) nitf_BandInfo_init(nitf_BandInfo * bandInfo,
                                      const char *representation,
                                      const char *subcategory,
                                      const char *imageFilterCondition,
                                      const char *imageFilterCode,
                                      nitf_Uint32 numLUTs,
                                      nitf_Uint32 bandEntriesPerLUT,
                                      nitf_LookupTable * lut,
                                      nitf_Error * error)
{
    if (!nitf_Field_setString
            (bandInfo->NITF_IREPBAND, representation, error))
        return (NITF_FAILURE);

    if (!nitf_Field_setString(bandInfo->NITF_ISUBCAT, subcategory, error))
        return (NITF_FAILURE);

    if (!nitf_Field_setString
            (bandInfo->NITF_IFC, imageFilterCondition, error))
        return (NITF_FAILURE);

    if (!nitf_Field_setString
            (bandInfo->NITF_IMFLT, imageFilterCode, error))
        return (NITF_FAILURE);

    if (!nitf_Field_setUint32(bandInfo->NITF_NLUTS, numLUTs, error))
        return (NITF_FAILURE);

    if (!nitf_Field_setUint32
            (bandInfo->NITF_NELUT, bandEntriesPerLUT, error))
        return (NITF_FAILURE);

    bandInfo->lut = lut;
    return (NITF_SUCCESS);
}

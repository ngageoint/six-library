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

#include "nitf/ComponentInfo.h"

#define _NITF_DESTRUCT_FIELD(OWNER, ID) \
    if (OWNER->ID) nitf_Field_destruct(OWNER->ID);

/*  Create the image info structure */
NITFAPI(nitf_ComponentInfo *) nitf_ComponentInfo_construct(nitf_Uint32
        subheaderFieldWidth,
        nitf_Uint32
        dataFieldWidth,
        nitf_Error *
        error)
{
    nitf_ComponentInfo *info =
        (nitf_ComponentInfo *) NITF_MALLOC(sizeof(nitf_ComponentInfo));
    if (!info)
    {
        nitf_Error_init(error,
                        NITF_STRERROR(NITF_ERRNO),
                        NITF_CTXT, NITF_ERR_MEMORY);
        return NULL;
    }
    /*  Zero-init these fields */
    info->lengthSubheader =
        nitf_Field_construct(subheaderFieldWidth, NITF_BCS_N, error);
    if (!info->lengthSubheader)
        goto CATCH_ERROR;

    info->lengthData =
        nitf_Field_construct(dataFieldWidth, NITF_BCS_N, error);
    if (!info->lengthData)
        goto CATCH_ERROR;
    /*  Return the object */
    return info;

CATCH_ERROR:
    return NULL;
}


NITFAPI(nitf_ComponentInfo *)
nitf_ComponentInfo_clone(nitf_ComponentInfo * source, nitf_Error * error)
{
    nitf_ComponentInfo *info = NULL;
    if (source)
    {
        info =
            nitf_ComponentInfo_construct(source->lengthSubheader->length,
                                         source->lengthData->length,
                                         error);
        if (!info)
            goto CATCH_ERROR;
    }
    else
    {
        nitf_Error_initf(error,
                         NITF_CTXT,
                         NITF_ERR_INVALID_OBJECT,
                         "Trying to clone NULL pointer");
    }
    return info;

CATCH_ERROR:
    return NULL;
}


/*  Destroy the image info structure */
NITFAPI(void) nitf_ComponentInfo_destruct(nitf_ComponentInfo ** info)
{
    if (!*info)
        return;

    _NITF_DESTRUCT_FIELD(&(*info), lengthSubheader);
    _NITF_DESTRUCT_FIELD(&(*info), lengthData);

    NITF_FREE(*info);
    *info = NULL;
}

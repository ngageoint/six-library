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

#include "nitf/RESubheader.h"

#define _NITF_CONSTRUCT_FIELD(OWNER, ID, TYPE) \
    OWNER->ID = nitf_Field_construct(ID##_SZ, TYPE, error); \
    if (!OWNER->ID) goto CATCH_ERROR;

#define _NITF_CLONE_FIELD(DEST, SOURCE, ID) \
    DEST->ID = nitf_Field_clone(SOURCE->ID, error); \
    if (!DEST->ID) goto CATCH_ERROR;

#define _NITF_DESTRUCT_FIELD(OWNER, ID) \
    if (OWNER->ID) nitf_Field_destruct(OWNER->ID);

NITFAPI(nitf_RESubheader *) nitf_RESubheader_construct(nitf_Error * error)
{
    /*  Start by allocating the header */
    nitf_RESubheader *subhdr = (nitf_RESubheader *)
                               NITF_MALLOC(sizeof(nitf_RESubheader));

    /*  Return now if we have a problem above */
    if (!subhdr)
    {
        nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO),
                        NITF_CTXT, NITF_ERR_MEMORY);
        goto CATCH_ERROR;
    }

    subhdr->securityGroup = nitf_FileSecurity_construct(error);
    if (!subhdr->securityGroup)
    {
        nitf_RESubheader_destruct(&subhdr);
        goto CATCH_ERROR;
    }

    _NITF_CONSTRUCT_FIELD(subhdr, NITF_RE, NITF_BCS_A);
    _NITF_CONSTRUCT_FIELD(subhdr, NITF_RESTAG, NITF_BCS_A);
    _NITF_CONSTRUCT_FIELD(subhdr, NITF_RESVER, NITF_BCS_A);
    _NITF_CONSTRUCT_FIELD(subhdr, NITF_RESCLAS, NITF_BCS_A);

    _NITF_CONSTRUCT_FIELD(subhdr, NITF_RESSHL, NITF_BCS_N);
    subhdr->subheaderFields = NULL;
    subhdr->dataLength = 0;

    return subhdr;

CATCH_ERROR:
    /* destruct if it was allocated */
    if (subhdr)
        nitf_RESubheader_destruct(&subhdr);
    return NULL;
}


NITFAPI(nitf_RESubheader *)
nitf_RESubheader_clone(nitf_RESubheader * source, nitf_Error * error)
{
    nitf_Uint32 subLen;
    nitf_RESubheader *subhdr = NULL;
    if (source)
    {
        subhdr =
            (nitf_RESubheader *) NITF_MALLOC(sizeof(nitf_RESubheader));
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

        /*  Copy some fields  */
        _NITF_CLONE_FIELD(subhdr, source, NITF_RE);
        _NITF_CLONE_FIELD(subhdr, source, NITF_RESTAG);
        _NITF_CLONE_FIELD(subhdr, source, NITF_RESVER);
        _NITF_CLONE_FIELD(subhdr, source, NITF_RESCLAS);

        /*  And some ints  */
        _NITF_CLONE_FIELD(subhdr, source, NITF_RESSHL);
        subhdr->dataLength = source->dataLength;
        subhdr->subheaderFields = NULL;

        NITF_TRY_GET_UINT32(source->subheaderFieldsLength, &subLen, error);
        if (source->subheaderFields)
        {
            memcpy(subhdr->subheaderFields,
                   source->subheaderFields, subLen);
        }
        return subhdr;
    }

CATCH_ERROR:
    return NULL;
}


NITFAPI(void) nitf_RESubheader_destruct(nitf_RESubheader ** subhdr)
{
    if (!*subhdr)
        return;

    if ((*subhdr)->securityGroup)
    {
        nitf_FileSecurity_destruct(&(*subhdr)->securityGroup);
        NITF_FREE((*subhdr)->securityGroup);
        (*subhdr)->securityGroup = NULL;
    }
    if ((*subhdr)->subheaderFields)
    {
        NITF_FREE((*subhdr)->subheaderFields);
        (*subhdr)->subheaderFields = NULL;
    }

    _NITF_DESTRUCT_FIELD(&(*subhdr), NITF_RE);
    _NITF_DESTRUCT_FIELD(&(*subhdr), NITF_RESTAG);
    _NITF_DESTRUCT_FIELD(&(*subhdr), NITF_RESVER);
    _NITF_DESTRUCT_FIELD(&(*subhdr), NITF_RESCLAS);
    _NITF_DESTRUCT_FIELD(&(*subhdr), NITF_RESSHL);

    NITF_FREE(*subhdr);
    *subhdr = NULL;
}


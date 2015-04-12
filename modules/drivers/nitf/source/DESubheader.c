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

#include "nitf/DESubheader.h"

#define _NITF_CONSTRUCT_FIELD(OWNER, ID, TYPE) \
    OWNER->ID = nitf_Field_construct(ID##_SZ, TYPE, error); \
    if (!OWNER->ID) goto CATCH_ERROR;

#define _NITF_CLONE_FIELD(DEST, SOURCE, ID) \
    DEST->ID = nitf_Field_clone(SOURCE->ID, error); \
    if (!DEST->ID) goto CATCH_ERROR;

#define _NITF_DESTRUCT_FIELD(OWNER, ID) \
    if (OWNER->ID) nitf_Field_destruct(OWNER->ID);

NITFAPI(nitf_DESubheader *) nitf_DESubheader_construct(nitf_Error * error)
{
    /*  Start by allocating the header */
    nitf_DESubheader *subhdr = (nitf_DESubheader *)
                               NITF_MALLOC(sizeof(nitf_DESubheader));

    /*  Return now if we have a problem above */
    if (!subhdr)
    {
        nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO),
                        NITF_CTXT, NITF_ERR_MEMORY);
        return NULL;
    }
    /* Need to set all of these before destructor could be
       safely called - DP */

    subhdr->securityGroup = NULL;
    subhdr->subheaderFields = NULL;
    subhdr->dataLength = 0;
    subhdr->userDefinedSection = NULL;

    subhdr->securityGroup = nitf_FileSecurity_construct(error);
    if (!subhdr->securityGroup)
    {
        nitf_DESubheader_destruct(&subhdr);
        goto CATCH_ERROR;
    }

    _NITF_CONSTRUCT_FIELD(subhdr, NITF_DE, NITF_BCS_A);
    _NITF_CONSTRUCT_FIELD(subhdr, NITF_DESTAG, NITF_BCS_A);
    _NITF_CONSTRUCT_FIELD(subhdr, NITF_DESVER, NITF_BCS_A);
    _NITF_CONSTRUCT_FIELD(subhdr, NITF_DESCLAS, NITF_BCS_A);

    _NITF_CONSTRUCT_FIELD(subhdr, NITF_DESOFLW, NITF_BCS_A);
    _NITF_CONSTRUCT_FIELD(subhdr, NITF_DESITEM, NITF_BCS_N);

    _NITF_CONSTRUCT_FIELD(subhdr, NITF_DESSHL, NITF_BCS_N);

    subhdr->userDefinedSection = nitf_Extensions_construct(error);
    if (!subhdr->userDefinedSection)
        goto CATCH_ERROR;

    return subhdr;

CATCH_ERROR:
    nitf_DESubheader_destruct(&subhdr);
    return NULL;
}


NITFAPI(nitf_DESubheader *)
nitf_DESubheader_clone(nitf_DESubheader * source, nitf_Error * error)
{
    nitf_Uint32 subLen;
    nitf_DESubheader *subhdr = NULL;
    NITF_BOOL success;
    if (source)
    {
        subhdr =
            (nitf_DESubheader *) NITF_MALLOC(sizeof(nitf_DESubheader));
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
        _NITF_CLONE_FIELD(subhdr, source, NITF_DE);
        _NITF_CLONE_FIELD(subhdr, source, NITF_DESTAG);
        _NITF_CLONE_FIELD(subhdr, source, NITF_DESVER);
        _NITF_CLONE_FIELD(subhdr, source, NITF_DESCLAS);
        _NITF_CLONE_FIELD(subhdr, source, NITF_DESOFLW);
        _NITF_CLONE_FIELD(subhdr, source, NITF_DESITEM);

        /*  And some ints  */
        _NITF_CLONE_FIELD(subhdr, source, NITF_DESSHL);
        subhdr->dataLength = source->dataLength;

        success = nitf_Field_get(source->subheaderFieldsLength, &subLen,
                                 NITF_CONV_INT, sizeof(subLen), error);
        if (!success)
            goto CATCH_ERROR;

        subhdr->subheaderFields = NULL;
        subhdr->userDefinedSection = NULL;

        if (source->subheaderFields)
        {
            memcpy(subhdr->subheaderFields,
                   source->subheaderFields, subLen);
        }
        if (source->userDefinedSection)
        {

            subhdr->userDefinedSection =
                nitf_Extensions_clone(source->userDefinedSection, error);

            if (!subhdr->userDefinedSection)
                goto CATCH_ERROR;
        }

        return subhdr;
    }

CATCH_ERROR:
    nitf_DESubheader_destruct(&subhdr);
    return NULL;
}


NITFAPI(void) nitf_DESubheader_destruct(nitf_DESubheader ** subhdr)
{
    if (!*subhdr)
        return;

    if ((*subhdr)->userDefinedSection)
    {
        nitf_Extensions_destruct(&(*subhdr)->userDefinedSection);
    }
    if ((*subhdr)->securityGroup)
    {
        nitf_FileSecurity_destruct(&(*subhdr)->securityGroup);
        (*subhdr)->securityGroup = NULL;
    }
    if ((*subhdr)->subheaderFields)
    {
        nitf_TRE_destruct(&(*subhdr)->subheaderFields);
    }

    _NITF_DESTRUCT_FIELD(&(*subhdr), NITF_DE);
    _NITF_DESTRUCT_FIELD(&(*subhdr), NITF_DESTAG);
    _NITF_DESTRUCT_FIELD(&(*subhdr), NITF_DESVER);
    _NITF_DESTRUCT_FIELD(&(*subhdr), NITF_DESCLAS);
    _NITF_DESTRUCT_FIELD(&(*subhdr), NITF_DESOFLW);
    _NITF_DESTRUCT_FIELD(&(*subhdr), NITF_DESITEM);
    _NITF_DESTRUCT_FIELD(&(*subhdr), NITF_DESSHL);

    NITF_FREE(*subhdr);
    *subhdr = NULL;
}

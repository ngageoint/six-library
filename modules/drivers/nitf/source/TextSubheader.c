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

#include "nitf/TextSubheader.h"

#define _NITF_CONSTRUCT_FIELD(OWNER, ID, TYPE) \
    OWNER->ID = nitf_Field_construct(ID##_SZ, TYPE, error); \
    if (!OWNER->ID) goto CATCH_ERROR;

#define _NITF_CLONE_FIELD(DEST, SOURCE, ID) \
    DEST->ID = nitf_Field_clone(SOURCE->ID, error); \
    if (!DEST->ID) goto CATCH_ERROR;

#define _NITF_DESTRUCT_FIELD(OWNER, ID) \
    if (OWNER->ID) nitf_Field_destruct(OWNER->ID);

NITFAPI(nitf_TextSubheader *)
nitf_TextSubheader_construct(nitf_Error * error)
{
    /*  Start by allocating the header */
    nitf_TextSubheader *subhdr = (nitf_TextSubheader *)
                                 NITF_MALLOC(sizeof(nitf_TextSubheader));

    /*  Return now if we have a problem above */
    if (!subhdr)
    {
        nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO),
                        NITF_CTXT, NITF_ERR_MEMORY);

        return NULL;
    }

    subhdr->extendedSection = NULL;
    subhdr->securityGroup = nitf_FileSecurity_construct(error);
    if (!subhdr->securityGroup)
    {
        nitf_TextSubheader_destruct(&subhdr);
        goto CATCH_ERROR;
    }

    _NITF_CONSTRUCT_FIELD(subhdr, NITF_TE, NITF_BCS_A);
    _NITF_CONSTRUCT_FIELD(subhdr, NITF_TEXTID, NITF_BCS_A);

    _NITF_CONSTRUCT_FIELD(subhdr, NITF_TXTALVL, NITF_BCS_N);

    _NITF_CONSTRUCT_FIELD(subhdr, NITF_TXTDT, NITF_BCS_A);
    _NITF_CONSTRUCT_FIELD(subhdr, NITF_TXTITL, NITF_BCS_A);
    _NITF_CONSTRUCT_FIELD(subhdr, NITF_TSCLAS, NITF_BCS_A);

    _NITF_CONSTRUCT_FIELD(subhdr, NITF_ENCRYP, NITF_BCS_A);
    _NITF_CONSTRUCT_FIELD(subhdr, NITF_TXTFMT, NITF_BCS_A);

    _NITF_CONSTRUCT_FIELD(subhdr, NITF_TXSHDL, NITF_BCS_N);
    _NITF_CONSTRUCT_FIELD(subhdr, NITF_TXSOFL, NITF_BCS_N);

    subhdr->extendedSection = nitf_Extensions_construct(error);
    if (!subhdr->extendedSection)
        goto CATCH_ERROR;

    return subhdr;

CATCH_ERROR:
    nitf_TextSubheader_destruct(&subhdr);
    return NULL;
}


NITFAPI(nitf_TextSubheader *)
nitf_TextSubheader_clone(nitf_TextSubheader * source, nitf_Error * error)
{
    nitf_TextSubheader *subhdr = NULL;
    if (source)
    {
        subhdr =
            (nitf_TextSubheader *) NITF_MALLOC(sizeof(nitf_TextSubheader));
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
        _NITF_CLONE_FIELD(subhdr, source, NITF_TE);
        _NITF_CLONE_FIELD(subhdr, source, NITF_TEXTID);

        _NITF_CLONE_FIELD(subhdr, source, NITF_TXTALVL);

        _NITF_CLONE_FIELD(subhdr, source, NITF_TXTDT);
        _NITF_CLONE_FIELD(subhdr, source, NITF_TXTITL);
        _NITF_CLONE_FIELD(subhdr, source, NITF_TSCLAS);

        _NITF_CLONE_FIELD(subhdr, source, NITF_ENCRYP);
        _NITF_CLONE_FIELD(subhdr, source, NITF_TXTFMT);

        _NITF_CLONE_FIELD(subhdr, source, NITF_TXSHDL);
        _NITF_CLONE_FIELD(subhdr, source, NITF_TXSOFL);

        /* init to NULL in case */
        subhdr->extendedSection = NULL;

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
    nitf_TextSubheader_destruct(&subhdr);
    return NULL;
}


NITFAPI(void) nitf_TextSubheader_destruct(nitf_TextSubheader ** subhdr)
{
    if (!*subhdr)
        return;

    if ((*subhdr)->extendedSection)
    {
        nitf_Extensions_destruct(&(*subhdr)->extendedSection);
    }
    if ((*subhdr)->securityGroup)
    {
        nitf_FileSecurity_destruct(&(*subhdr)->securityGroup);
        NITF_FREE((*subhdr)->securityGroup);
        (*subhdr)->securityGroup = NULL;
    }

    _NITF_DESTRUCT_FIELD(&(*subhdr), NITF_TE);
    _NITF_DESTRUCT_FIELD(&(*subhdr), NITF_TEXTID);
    _NITF_DESTRUCT_FIELD(&(*subhdr), NITF_TXTALVL);
    _NITF_DESTRUCT_FIELD(&(*subhdr), NITF_TXTDT);
    _NITF_DESTRUCT_FIELD(&(*subhdr), NITF_TXTITL);
    _NITF_DESTRUCT_FIELD(&(*subhdr), NITF_TSCLAS);
    _NITF_DESTRUCT_FIELD(&(*subhdr), NITF_ENCRYP);
    _NITF_DESTRUCT_FIELD(&(*subhdr), NITF_TXTFMT);
    _NITF_DESTRUCT_FIELD(&(*subhdr), NITF_TXSHDL);
    _NITF_DESTRUCT_FIELD(&(*subhdr), NITF_TXSOFL);

    NITF_FREE(*subhdr);
    *subhdr = NULL;
}

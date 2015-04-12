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

#include "nitf/LabelSubheader.h"

#define _NITF_CONSTRUCT_FIELD(OWNER, ID, TYPE) \
    OWNER->ID = nitf_Field_construct(ID##_SZ, TYPE, error); \
    if (!OWNER->ID) goto CATCH_ERROR;

#define _NITF_CLONE_FIELD(DEST, SOURCE, ID) \
    DEST->ID = nitf_Field_clone(SOURCE->ID, error); \
    if (!DEST->ID) goto CATCH_ERROR;

#define _NITF_DESTRUCT_FIELD(OWNER, ID) \
    if (OWNER->ID) nitf_Field_destruct(OWNER->ID);

NITFAPI(nitf_LabelSubheader *)
nitf_LabelSubheader_construct(nitf_Error * error)
{
    /*  Start by allocating the header */
    nitf_LabelSubheader *subhdr = (nitf_LabelSubheader *)
                                  NITF_MALLOC(sizeof(nitf_LabelSubheader));

    /*  Return now if we have a problem above */
    if (!subhdr)
    {
        nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO),
                        NITF_CTXT, NITF_ERR_MEMORY);
        return NULL;
    }

    subhdr->extendedSection = NULL;
    subhdr->securityGroup = NULL;
    subhdr->securityGroup = nitf_FileSecurity_construct(error);
    if (!subhdr->securityGroup)
    {
        nitf_LabelSubheader_destruct(&subhdr);
        goto CATCH_ERROR;
    }
    _NITF_CONSTRUCT_FIELD(subhdr, NITF_LA, NITF_BCS_A);
    _NITF_CONSTRUCT_FIELD(subhdr, NITF_LID, NITF_BCS_A);
    _NITF_CONSTRUCT_FIELD(subhdr, NITF_LSCLAS, NITF_BCS_A);

    _NITF_CONSTRUCT_FIELD(subhdr, NITF_ENCRYP, NITF_BCS_A);
    _NITF_CONSTRUCT_FIELD(subhdr, NITF_LFS, NITF_BCS_A);

    _NITF_CONSTRUCT_FIELD(subhdr, NITF_LCW, NITF_BCS_N);
    _NITF_CONSTRUCT_FIELD(subhdr, NITF_LCH, NITF_BCS_N);
    _NITF_CONSTRUCT_FIELD(subhdr, NITF_LDLVL, NITF_BCS_N);
    _NITF_CONSTRUCT_FIELD(subhdr, NITF_LALVL, NITF_BCS_N);
    _NITF_CONSTRUCT_FIELD(subhdr, NITF_LLOCR, NITF_BCS_N);
    _NITF_CONSTRUCT_FIELD(subhdr, NITF_LLOCC, NITF_BCS_N);

    _NITF_CONSTRUCT_FIELD(subhdr, NITF_LTC, NITF_BINARY);
    _NITF_CONSTRUCT_FIELD(subhdr, NITF_LBC, NITF_BINARY);

    _NITF_CONSTRUCT_FIELD(subhdr, NITF_LXSHDL, NITF_BCS_N);
    _NITF_CONSTRUCT_FIELD(subhdr, NITF_LXSOFL, NITF_BCS_N);

    subhdr->extendedSection = nitf_Extensions_construct(error);
    if (!subhdr->extendedSection)
        goto CATCH_ERROR;

    return subhdr;

CATCH_ERROR:
    /* destruct if it was allocated */
    nitf_LabelSubheader_destruct(&subhdr);
    return NULL;
}


NITFAPI(nitf_LabelSubheader *)
nitf_LabelSubheader_clone(nitf_LabelSubheader * source, nitf_Error * error)
{
    nitf_LabelSubheader *subhdr = NULL;
    if (source)
    {
        subhdr = nitf_LabelSubheader_construct(error);
        if (!subhdr)
            return NULL;

        /* now, destruct the NEW security group, and clone it
         * so that everything is in sync */
        nitf_FileSecurity_destruct(&subhdr->securityGroup);
        subhdr->securityGroup =
            nitf_FileSecurity_clone(source->securityGroup, error);

        /*  Copy some fields  */
        _NITF_CLONE_FIELD(subhdr, source, NITF_LA);
        _NITF_CLONE_FIELD(subhdr, source, NITF_LID);
        _NITF_CLONE_FIELD(subhdr, source, NITF_LSCLAS);

        _NITF_CLONE_FIELD(subhdr, source, NITF_ENCRYP);
        _NITF_CLONE_FIELD(subhdr, source, NITF_LFS);

        _NITF_CLONE_FIELD(subhdr, source, NITF_LCW);
        _NITF_CLONE_FIELD(subhdr, source, NITF_LCH);
        _NITF_CLONE_FIELD(subhdr, source, NITF_LDLVL);
        _NITF_CLONE_FIELD(subhdr, source, NITF_LALVL);
        _NITF_CLONE_FIELD(subhdr, source, NITF_LLOCR);
        _NITF_CLONE_FIELD(subhdr, source, NITF_LLOCC);

        _NITF_CLONE_FIELD(subhdr, source, NITF_LTC);
        _NITF_CLONE_FIELD(subhdr, source, NITF_LBC);

        _NITF_CLONE_FIELD(subhdr, source, NITF_LXSHDL);
        _NITF_CLONE_FIELD(subhdr, source, NITF_LXSOFL);

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
    nitf_LabelSubheader_destruct(&subhdr);
    return NULL;
}


NITFAPI(void) nitf_LabelSubheader_destruct(nitf_LabelSubheader ** subhdr)
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

    _NITF_DESTRUCT_FIELD(&(*subhdr), NITF_LA);
    _NITF_DESTRUCT_FIELD(&(*subhdr), NITF_LID);
    _NITF_DESTRUCT_FIELD(&(*subhdr), NITF_LSCLAS);
    _NITF_DESTRUCT_FIELD(&(*subhdr), NITF_ENCRYP);
    _NITF_DESTRUCT_FIELD(&(*subhdr), NITF_LFS);
    _NITF_DESTRUCT_FIELD(&(*subhdr), NITF_LCW);
    _NITF_DESTRUCT_FIELD(&(*subhdr), NITF_LCH);
    _NITF_DESTRUCT_FIELD(&(*subhdr), NITF_LDLVL);
    _NITF_DESTRUCT_FIELD(&(*subhdr), NITF_LALVL);
    _NITF_DESTRUCT_FIELD(&(*subhdr), NITF_LLOCR);
    _NITF_DESTRUCT_FIELD(&(*subhdr), NITF_LLOCC);
    _NITF_DESTRUCT_FIELD(&(*subhdr), NITF_LTC);
    _NITF_DESTRUCT_FIELD(&(*subhdr), NITF_LBC);
    _NITF_DESTRUCT_FIELD(&(*subhdr), NITF_LXSHDL);
    _NITF_DESTRUCT_FIELD(&(*subhdr), NITF_LXSOFL);

    NITF_FREE(*subhdr);
    *subhdr = NULL;
}


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
 * License along with this program; 
 * if not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "nitf/GraphicSubheader.h"

#define _NITF_CONSTRUCT_FIELD(OWNER, ID, TYPE) \
    OWNER->ID = nitf_Field_construct(ID##_SZ, TYPE, error); \
    if (!OWNER->ID) goto CATCH_ERROR;

#define _NITF_CLONE_FIELD(DEST, SOURCE, ID) \
    DEST->ID = nitf_Field_clone(SOURCE->ID, error); \
    if (!DEST->ID) goto CATCH_ERROR;

#define _NITF_DESTRUCT_FIELD(OWNER, ID) \
    if (OWNER->ID) nitf_Field_destruct(OWNER->ID);

NITFAPI(nitf_GraphicSubheader *)
nitf_GraphicSubheader_construct(nitf_Error * error)
{
    /*  Start by allocating the header */
    nitf_GraphicSubheader *subhdr = (nitf_GraphicSubheader *)
                                    NITF_MALLOC(sizeof(nitf_GraphicSubheader));

    /*  Return now if we have a problem above */
    if (!subhdr)
    {
        nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO),
                        NITF_CTXT, NITF_ERR_MEMORY);
        goto CATCH_ERROR;
    }

    subhdr->extendedSection = NULL;
    subhdr->securityGroup = NULL;
    
    subhdr->securityGroup = nitf_FileSecurity_construct(error);
    if (!subhdr->securityGroup)
        goto CATCH_ERROR;

    _NITF_CONSTRUCT_FIELD(subhdr, NITF_SY, NITF_BCS_A);
    _NITF_CONSTRUCT_FIELD(subhdr, NITF_SID, NITF_BCS_A);
    _NITF_CONSTRUCT_FIELD(subhdr, NITF_SNAME, NITF_BCS_A);
    _NITF_CONSTRUCT_FIELD(subhdr, NITF_SSCLAS, NITF_BCS_A);
    _NITF_CONSTRUCT_FIELD(subhdr, NITF_ENCRYP, NITF_BCS_A);
    _NITF_CONSTRUCT_FIELD(subhdr, NITF_SFMT, NITF_BCS_A);
    _NITF_CONSTRUCT_FIELD(subhdr, NITF_SSTRUCT, NITF_BCS_N);
    _NITF_CONSTRUCT_FIELD(subhdr, NITF_SDLVL, NITF_BCS_N);
    _NITF_CONSTRUCT_FIELD(subhdr, NITF_SALVL, NITF_BCS_N);
    _NITF_CONSTRUCT_FIELD(subhdr, NITF_SLOC, NITF_BCS_N);
    _NITF_CONSTRUCT_FIELD(subhdr, NITF_SBND1, NITF_BCS_N);
    _NITF_CONSTRUCT_FIELD(subhdr, NITF_SCOLOR, NITF_BCS_A);
    _NITF_CONSTRUCT_FIELD(subhdr, NITF_SBND2, NITF_BCS_N);
    _NITF_CONSTRUCT_FIELD(subhdr, NITF_SRES2, NITF_BCS_N);
    _NITF_CONSTRUCT_FIELD(subhdr, NITF_SXSHDL, NITF_BCS_N);
    _NITF_CONSTRUCT_FIELD(subhdr, NITF_SXSOFL, NITF_BCS_N);

    subhdr->extendedSection = nitf_Extensions_construct(error);
    if (!subhdr->extendedSection)
        goto CATCH_ERROR;

    return subhdr;

CATCH_ERROR:
    if (subhdr)
    	nitf_GraphicSubheader_destruct(&subhdr);
    return NULL;
}


NITFAPI(nitf_GraphicSubheader *)
nitf_GraphicSubheader_clone(nitf_GraphicSubheader * source,
                            nitf_Error * error)
{
    nitf_GraphicSubheader *subhdr = NULL;
    if (source)
    {
        subhdr =
            (nitf_GraphicSubheader *)
            NITF_MALLOC(sizeof(nitf_GraphicSubheader));
        if (!subhdr)
        {
            nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO),
                            NITF_CTXT, NITF_ERR_MEMORY);
            return NULL;
        }

        subhdr->securityGroup =
            nitf_FileSecurity_clone(source->securityGroup, error);

        /*  Copy some fields  */
        _NITF_CLONE_FIELD(subhdr, source, NITF_SY);
        _NITF_CLONE_FIELD(subhdr, source, NITF_SID);
        _NITF_CLONE_FIELD(subhdr, source, NITF_SNAME);
        _NITF_CLONE_FIELD(subhdr, source, NITF_SSCLAS);
        _NITF_CLONE_FIELD(subhdr, source, NITF_ENCRYP);
        _NITF_CLONE_FIELD(subhdr, source, NITF_SFMT);
        _NITF_CLONE_FIELD(subhdr, source, NITF_SSTRUCT);
        _NITF_CLONE_FIELD(subhdr, source, NITF_SDLVL);
        _NITF_CLONE_FIELD(subhdr, source, NITF_SALVL);
        _NITF_CLONE_FIELD(subhdr, source, NITF_SLOC);
        _NITF_CLONE_FIELD(subhdr, source, NITF_SBND1);
        _NITF_CLONE_FIELD(subhdr, source, NITF_SCOLOR);
        _NITF_CLONE_FIELD(subhdr, source, NITF_SBND2);
        _NITF_CLONE_FIELD(subhdr, source, NITF_SRES2);
        _NITF_CLONE_FIELD(subhdr, source, NITF_SXSHDL);
        _NITF_CLONE_FIELD(subhdr, source, NITF_SXSOFL);

        /* init to NULL for safety */
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
    nitf_GraphicSubheader_destruct(&subhdr);
    return NULL;
}


NITFAPI(void) nitf_GraphicSubheader_destruct(nitf_GraphicSubheader **
        subhdr)
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

    _NITF_DESTRUCT_FIELD(&(*subhdr), NITF_SY);
    _NITF_DESTRUCT_FIELD(&(*subhdr), NITF_SID);
    _NITF_DESTRUCT_FIELD(&(*subhdr), NITF_SNAME);
    _NITF_DESTRUCT_FIELD(&(*subhdr), NITF_SSCLAS);
    _NITF_DESTRUCT_FIELD(&(*subhdr), NITF_ENCRYP);
    _NITF_DESTRUCT_FIELD(&(*subhdr), NITF_SFMT);
    _NITF_DESTRUCT_FIELD(&(*subhdr), NITF_SSTRUCT);
    _NITF_DESTRUCT_FIELD(&(*subhdr), NITF_SDLVL);
    _NITF_DESTRUCT_FIELD(&(*subhdr), NITF_SALVL);
    _NITF_DESTRUCT_FIELD(&(*subhdr), NITF_SLOC);
    _NITF_DESTRUCT_FIELD(&(*subhdr), NITF_SBND1);
    _NITF_DESTRUCT_FIELD(&(*subhdr), NITF_SCOLOR);
    _NITF_DESTRUCT_FIELD(&(*subhdr), NITF_SBND2);
    _NITF_DESTRUCT_FIELD(&(*subhdr), NITF_SRES2);
    _NITF_DESTRUCT_FIELD(&(*subhdr), NITF_SXSHDL);
    _NITF_DESTRUCT_FIELD(&(*subhdr), NITF_SXSOFL);

    NITF_FREE(*subhdr);
    *subhdr = NULL;
}

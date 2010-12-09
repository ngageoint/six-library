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

#include "nitf/FileSecurity.h"

#define _NITF_CONSTRUCT_FIELD(OWNER, ID, TYPE) \
    OWNER->ID = nitf_Field_construct(ID##_SZ, TYPE, error); \
    if (!OWNER->ID) goto CATCH_ERROR;

#define _NITF_CLONE_FIELD(DEST, SOURCE, ID) \
    DEST->ID = nitf_Field_clone(SOURCE->ID, error); \
    if (!DEST->ID) goto CATCH_ERROR;

#define _NITF_DESTRUCT_FIELD(OWNER, ID) \
    if (OWNER->ID) nitf_Field_destruct(OWNER->ID);

#define _NITF_RESIZE_FIELD(field_, length_) \
    if (!nitf_Field_resetLength(field_, length_, 0, error)) goto CATCH_ERROR;

/*
 *  Create this object and memset components to zero.
 *
 */
NITFAPI(nitf_FileSecurity *) nitf_FileSecurity_construct(nitf_Error *
        error)
{
    nitf_FileSecurity *fs =
        (nitf_FileSecurity *) NITF_MALLOC(sizeof(nitf_FileSecurity));
    if (!fs)
    {
        nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO),
                        NITF_CTXT, NITF_ERR_MEMORY);

        goto CATCH_ERROR;
    }

    _NITF_CONSTRUCT_FIELD(fs, NITF_CLSY, NITF_BCS_A);
    _NITF_CONSTRUCT_FIELD(fs, NITF_CODE, NITF_BCS_A);
    _NITF_CONSTRUCT_FIELD(fs, NITF_CTLH, NITF_BCS_A);
    _NITF_CONSTRUCT_FIELD(fs, NITF_REL, NITF_BCS_A);
    _NITF_CONSTRUCT_FIELD(fs, NITF_DCTP, NITF_BCS_A);
    _NITF_CONSTRUCT_FIELD(fs, NITF_DCDT, NITF_BCS_A);
    _NITF_CONSTRUCT_FIELD(fs, NITF_DCXM, NITF_BCS_A);
    _NITF_CONSTRUCT_FIELD(fs, NITF_DG, NITF_BCS_A);
    _NITF_CONSTRUCT_FIELD(fs, NITF_DGDT, NITF_BCS_A);
    _NITF_CONSTRUCT_FIELD(fs, NITF_CLTX, NITF_BCS_A);
    _NITF_CONSTRUCT_FIELD(fs, NITF_CATP, NITF_BCS_A);
    _NITF_CONSTRUCT_FIELD(fs, NITF_CAUT, NITF_BCS_A);
    _NITF_CONSTRUCT_FIELD(fs, NITF_CRSN, NITF_BCS_A);
    _NITF_CONSTRUCT_FIELD(fs, NITF_RDT, NITF_BCS_A);
    _NITF_CONSTRUCT_FIELD(fs, NITF_CTLN, NITF_BCS_A);

    return fs;

CATCH_ERROR:
    /* destruct if it was allocated */
    if (fs)
        nitf_FileSecurity_destruct(&fs);
    return NULL;
}


NITFAPI(NITF_BOOL) nitf_FileSecurity_resizeForVersion
(nitf_FileSecurity * fs, nitf_Version ver, nitf_Error * error)
{
    if (ver == NITF_VER_20)
    {
        _NITF_RESIZE_FIELD(fs->NITF_CODE, NITF_CODE_20_SZ);
        _NITF_RESIZE_FIELD(fs->NITF_CTLH, NITF_CTLH_20_SZ);
        _NITF_RESIZE_FIELD(fs->NITF_REL, NITF_REL_20_SZ);
        _NITF_RESIZE_FIELD(fs->NITF_CAUT, NITF_CAUT_20_SZ);
        _NITF_RESIZE_FIELD(fs->NITF_CTLN, NITF_CTLN_20_SZ);
        _NITF_RESIZE_FIELD(fs->NITF_DGDT, NITF_DGDT_20_SZ);
        _NITF_RESIZE_FIELD(fs->NITF_CLTX, NITF_CLTX_20_SZ);
    }
    /* otherwise, do 2.1... */
    else
    {
        _NITF_RESIZE_FIELD(fs->NITF_CODE, NITF_CODE_SZ);
        _NITF_RESIZE_FIELD(fs->NITF_CTLH, NITF_CTLH_SZ);
        _NITF_RESIZE_FIELD(fs->NITF_REL, NITF_REL_SZ);
        _NITF_RESIZE_FIELD(fs->NITF_CAUT, NITF_CAUT_SZ);
        _NITF_RESIZE_FIELD(fs->NITF_CTLN, NITF_CTLN_SZ);
        _NITF_RESIZE_FIELD(fs->NITF_DGDT, NITF_DGDT_SZ);
        _NITF_RESIZE_FIELD(fs->NITF_CLTX, NITF_CLTX_SZ);
    }

    return NITF_SUCCESS;

CATCH_ERROR:
    return NITF_FAILURE;
}


NITFAPI(nitf_FileSecurity *) nitf_FileSecurity_clone(nitf_FileSecurity *
        source,
        nitf_Error * error)
{
    /*  In this case, the cheapest is to  */
    /*  duplicate the constructor here...  */
    nitf_FileSecurity *fs = NULL;
    if (source)
    {
        fs = (nitf_FileSecurity *) NITF_MALLOC(sizeof(nitf_FileSecurity));
        if (!fs)
        {
            nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO),
                            NITF_CTXT, NITF_ERR_MEMORY);

            goto CATCH_ERROR;

        }
        /*  ...and copy here instead of memsets  */
        _NITF_CLONE_FIELD(fs, source, NITF_CLSY);
        _NITF_CLONE_FIELD(fs, source, NITF_CODE);
        _NITF_CLONE_FIELD(fs, source, NITF_CTLH);
        _NITF_CLONE_FIELD(fs, source, NITF_REL);
        _NITF_CLONE_FIELD(fs, source, NITF_DCTP);
        _NITF_CLONE_FIELD(fs, source, NITF_DCDT);
        _NITF_CLONE_FIELD(fs, source, NITF_DCXM);
        _NITF_CLONE_FIELD(fs, source, NITF_DG);
        _NITF_CLONE_FIELD(fs, source, NITF_DGDT);
        _NITF_CLONE_FIELD(fs, source, NITF_CLTX);
        _NITF_CLONE_FIELD(fs, source, NITF_CATP);
        _NITF_CLONE_FIELD(fs, source, NITF_CAUT);
        _NITF_CLONE_FIELD(fs, source, NITF_CRSN);
        _NITF_CLONE_FIELD(fs, source, NITF_RDT);
        _NITF_CLONE_FIELD(fs, source, NITF_CTLN);
    }
    else
    {
        nitf_Error_initf(error,
                         NITF_CTXT,
                         NITF_ERR_INVALID_OBJECT,
                         "Trying to clone NULL pointer");
    }
    return fs;

CATCH_ERROR:
    return NULL;
}


/*!
 *  Delete this object and null it.
 *
 *  \param mem The memory object
 *  \return the object
 */
NITFAPI(void) nitf_FileSecurity_destruct(nitf_FileSecurity ** fs)
{
    if (!fs)
        return;

    _NITF_DESTRUCT_FIELD(&(*fs), NITF_CLSY);
    _NITF_DESTRUCT_FIELD(&(*fs), NITF_CODE);
    _NITF_DESTRUCT_FIELD(&(*fs), NITF_CTLH);
    _NITF_DESTRUCT_FIELD(&(*fs), NITF_REL);
    _NITF_DESTRUCT_FIELD(&(*fs), NITF_DCTP);
    _NITF_DESTRUCT_FIELD(&(*fs), NITF_DCDT);
    _NITF_DESTRUCT_FIELD(&(*fs), NITF_DCXM);
    _NITF_DESTRUCT_FIELD(&(*fs), NITF_DG);
    _NITF_DESTRUCT_FIELD(&(*fs), NITF_DGDT);
    _NITF_DESTRUCT_FIELD(&(*fs), NITF_CLTX);
    _NITF_DESTRUCT_FIELD(&(*fs), NITF_CATP);
    _NITF_DESTRUCT_FIELD(&(*fs), NITF_CAUT);
    _NITF_DESTRUCT_FIELD(&(*fs), NITF_CRSN);
    _NITF_DESTRUCT_FIELD(&(*fs), NITF_RDT);
    _NITF_DESTRUCT_FIELD(&(*fs), NITF_CTLN);

    NITF_FREE(*fs);
    *fs = NULL;
}

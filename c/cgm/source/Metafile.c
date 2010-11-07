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
 * If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "cgm/Metafile.h"

NITFAPI(cgm_Metafile*) cgm_Metafile_construct(const char* name,
        const char* description,
        nitf_Error* error)
{
    cgm_Metafile* mf = (cgm_Metafile*) NITF_MALLOC( sizeof(cgm_Metafile) );
    if (!mf)
    {
        nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO),
                NITF_CTXT, NITF_ERR_MEMORY);
        return NULL;
    }

    mf->name = NULL;
    mf->description = NULL;

    mf->version = 1;
    mf->elementList[0] = 1;
    mf->elementList[1] = -1;
    mf->elementList[2] = 1;

    mf->fontList = nitf_List_construct(error);
    if (!mf->fontList)
    {
        NITF_FREE(mf);
        return NULL;
    }
    mf->description = NULL;
    mf->picture = NULL;

    if (name)
    {
        mf->name = (char*)NITF_MALLOC( strlen( name ) + 1 );
        strcpy(mf->name, name);
    }

    if (description)
    {
        mf->description = (char*)NITF_MALLOC( strlen( description ) + 1 );
        strcpy(mf->description, description);
    }

    return mf;
}


NITFAPI(void) cgm_Metafile_destruct(cgm_Metafile** mf)
{
    if (*mf)
    {
        if ( (*mf)->picture )
        {
            cgm_Picture_destruct( & (*mf)->picture );
        }

        if ( (*mf)->fontList )
        {
            /* We actually have to walk this to delete it */

            nitf_List_destruct(& (*mf)->fontList );
        }

        if ( (*mf)->name )
        {
            NITF_FREE( (*mf)->name );
        }

        if ( (*mf)->description )
        {
            NITF_FREE( (*mf)->description );
        }

        NITF_FREE( *mf );
        *mf = NULL;
    }
}

NITFAPI(cgm_Picture*) cgm_Metafile_createPicture(cgm_Metafile* metafile,
        const char* name,
        nitf_Error* error)
{
    cgm_Picture* picture = cgm_Picture_construct(name, error);
    if (!picture)
        goto CATCH_ERROR;
    
    /* also create the PictureBody */
    picture->body = cgm_PictureBody_construct(error);
    if (!picture->body || !picture->body->elements)
        goto CATCH_ERROR;
    
    /* in addition, create the vdcExtent */
    picture->vdcExtent = cgm_Rectangle_construct(error);
    if (!picture->vdcExtent)
        goto CATCH_ERROR;
    
    /* by default, make the extent the maximum size - should we do this? */
    picture->vdcExtent->x1 = 0;
    picture->vdcExtent->y1 = 32767;
    picture->vdcExtent->x2 = 32767;
    picture->vdcExtent->y2 = 0;
    
    metafile->picture = picture;
    return metafile->picture;
  
  CATCH_ERROR:
    if (picture)
        cgm_Picture_destruct(&picture);
    return NULL;
}

NITFAPI(cgm_Metafile*) cgm_Metafile_clone(cgm_Metafile* mf, nitf_Error* error)
{
    return NULL;
}

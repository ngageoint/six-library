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

#ifndef __CGM_METAFILE_H__
#define __CGM_METAFILE_H__

#include <import/nitf.h>
#include "cgm/Picture.h"

NITF_CXX_GUARD

typedef struct _cgm_Metafile
{
    /* The metafile title */
    char* name;
    
    /* NITF only supports CGM version 1 */
    short version;
    
    /* This is required to be 1 -1 1*/
    short elementList[3];
    
    /* This is the description of the metafile */
    char* description;
    
    /* List of fonts */
    nitf_List* fontList;
    
    /* Picture */
    cgm_Picture* picture;
    
} cgm_Metafile;

NITFAPI(cgm_Metafile*) cgm_Metafile_construct(const char* name,
                                              const char* description,
                                              nitf_Error* error);

NITFAPI(void) cgm_Metafile_destruct(cgm_Metafile** mf);

NITFAPI(cgm_Picture*) cgm_Metafile_createPicture(cgm_Metafile* metafile,
                                                 const char* name,
                                                 nitf_Error* error);


NITFAPI(cgm_Metafile*) cgm_Metafile_clone(cgm_Metafile* mf, nitf_Error* error);


NITF_CXX_ENDGUARD

#endif

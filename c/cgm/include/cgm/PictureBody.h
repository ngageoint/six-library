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

#ifndef __CGM_PICTURE_BODY_H__
#define __CGM_PICTURE_BODY_H__

#include <import/nitf.h>
#include "cgm/Element.h"
#include "cgm/Color.h"

NITF_CXX_GUARD


typedef struct _cgm_PictureBody
{

    /* This defaults to 1 */
    NITF_BOOL transparency;
    
    cgm_Color* auxColor;
    
    nitf_List* elements;
    
} cgm_PictureBody;


NITFAPI(cgm_PictureBody*) cgm_PictureBody_construct(nitf_Error* error);

NITFAPI(void) cgm_PictureBody_destruct(cgm_PictureBody** body);


NITF_CXX_ENDGUARD


#endif

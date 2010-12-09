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

#ifndef __CGM_RECTANGLE_H__
#define __CGM_RECTANGLE_H__

#include "cgm/BasicTypes.h"


NITF_CXX_GUARD

typedef struct _cgm_Rectangle
{
    short x1;
    short y1;
    short x2;
    short y2;
    
} cgm_Rectangle;


NITFAPI(cgm_Rectangle*) cgm_Rectangle_construct(nitf_Error* error);

NITFAPI(cgm_Rectangle*) cgm_Rectangle_clone(cgm_Rectangle*, nitf_Error*);

NITFAPI(void) cgm_Rectangle_destruct(cgm_Rectangle** r);

NITFAPI(void) cgm_Rectangle_print(cgm_Rectangle* r);

NITF_CXX_ENDGUARD


#endif

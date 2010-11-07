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

#ifndef __CGM_CIRCLE_ELEMENT_H__
#define __CGM_CIRCLE_ELEMENT_H__

#include "cgm/Element.h"

NITF_CXX_GUARD

typedef struct _cgm_CircleElement
{
    cgm_FillAttributes* attributes;
    short centerX;
    short centerY;
    short radius;
    
} cgm_CircleElement;

NITFAPI(cgm_Element*) cgm_CircleElement_construct(nitf_Error* error);


NITF_CXX_ENDGUARD

#endif

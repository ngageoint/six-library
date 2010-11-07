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

#ifndef __CGM_FILL_ATTRIBUTES_H__
#define __CGM_FILL_ATTRIBUTES_H__

#include <import/nitf.h>
#include "cgm/BasicTypes.h"
#include "cgm/Color.h"

NITF_CXX_GUARD

typedef struct _cgm_FillAttributes
{
    cgm_Color *fillColor;
    cgm_Color *edgeColor;
    cgm_InteriorStyle interiorStyle;
    short edgeVisibility;
    short edgeWidth;
    short edgeType;
} cgm_FillAttributes;


NITFAPI(cgm_FillAttributes*) 
    cgm_FillAttributes_construct(nitf_Error* error);

NITFAPI(cgm_FillAttributes*) 
    cgm_FillAttributes_clone(cgm_FillAttributes* source, nitf_Error* error);

NITFAPI(void) cgm_FillAttributes_destruct(cgm_FillAttributes** atts);

NITFAPI(void) cgm_FillAttributes_print(cgm_FillAttributes* atts);

NITF_CXX_ENDGUARD

#endif

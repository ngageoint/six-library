/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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

#ifndef __CGM_VERTEX_CLOSE_H__
#define __CGM_VERTEX_CLOSE_H__

#include <import/nitf.h>
#include "cgm/BasicTypes.h"
NITF_CXX_GUARD

typedef struct _cgm_VertexClose
{
    short x;
    short y;
    cgm_EdgeCloseType edgeOutFlag;
    
} cgm_VertexClose;



NITFAPI(cgm_VertexClose*) cgm_VertexClose_construct(nitf_Error* error);

NITFAPI(void) cgm_VertexClose_destruct(cgm_VertexClose** v);

NITFAPI(void) cgm_VertexClose_print(cgm_VertexClose* v);

NITF_CXX_ENDGUARD

#endif

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

#ifndef __CGM_VERTEX_H__
#define __CGM_VERTEX_H__

#include <import/nitf.h>

NITF_CXX_GUARD

typedef struct _cgm_Vertex
{
    short x;
    short y;
} cgm_Vertex;


NITFAPI(cgm_Vertex*) cgm_Vertex_construct(short x, short y, nitf_Error* error);

NITFAPI(void) cgm_Vertex_destruct(cgm_Vertex** v);

NITFAPI(void) cgm_Vertex_print(cgm_Vertex* v);

NITF_CXX_ENDGUARD

#endif

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

#include "cgm/Vertex.h"

NITFAPI(cgm_Vertex*) cgm_Vertex_construct(short x, short y, nitf_Error* error)
{
    cgm_Vertex* v = (cgm_Vertex*)NITF_MALLOC(sizeof(cgm_Vertex));
    if (!v)
    {
        nitf_Error_init(error, NITF_STRERROR( NITF_ERRNO ),
                NITF_CTXT, NITF_ERR_MEMORY);
        return NULL;
    }
    v->x = x;
    v->y = y;
    return v;
}

NITFAPI(void) cgm_Vertex_destruct(cgm_Vertex** v)
{
    if (*v)
    {
        NITF_FREE( *v );
        *v = NULL;
    }
}

NITFAPI(void) cgm_Vertex_print(cgm_Vertex* v)
{
    printf("V(%d %d)\n", v->x, v->y );
}

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

#include "cgm/VertexClose.h"


NITFAPI(cgm_VertexClose*) cgm_VertexClose_construct(nitf_Error* error)
{
    cgm_VertexClose* v = (cgm_VertexClose*)NITF_MALLOC(sizeof(cgm_VertexClose));
    if (!v)
    {
	nitf_Error_init(error, NITF_STRERROR( NITF_ERRNO ),
			NITF_CTXT, NITF_ERR_MEMORY);
	return NULL;
    }
    v->x = v->y = -1;
    v->edgeOutFlag = CGM_EDGE_CLOSE_TYPE_NOT_SET;
    return v;
}

NITFAPI(void) cgm_VertexClose_destruct(cgm_VertexClose** v)
{
    if (*v)
    {
	NITF_FREE( *v );
	*v = NULL;
    }
}

NITFAPI(void) cgm_VertexClose_print(cgm_VertexClose* v)
{
    printf("V(%d %d, %d)\n", 
	   v->x, v->y, v->edgeOutFlag );

}

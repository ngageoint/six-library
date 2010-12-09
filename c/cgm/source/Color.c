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

#include "cgm/Color.h"

NITFAPI(cgm_Color*) cgm_Color_construct(short r, short g, short b,
        nitf_Error* error)
{
    cgm_Color* v = (cgm_Color*)NITF_MALLOC(sizeof(cgm_Color));
    if (!v)
    {
        nitf_Error_init(error, NITF_STRERROR( NITF_ERRNO ),
                NITF_CTXT, NITF_ERR_MEMORY);
        return NULL;
    }
    v->r = r;
    v->g = g;
    v->b = b;
    return v;
}

NITFAPI(cgm_Color*) cgm_Color_clone(cgm_Color* source, nitf_Error* error)
{
    return cgm_Color_construct(source->r, source->g, source->b, error);
}

NITFAPI(void) cgm_Color_destruct(cgm_Color** v)
{
    if (*v)
    {
        NITF_FREE( *v );
        *v = NULL;
    }
}

NITFAPI(void) cgm_Color_print(cgm_Color* c)
{
    printf("(%d, %d, %d)\n", c->r, c->g, c->b);
}

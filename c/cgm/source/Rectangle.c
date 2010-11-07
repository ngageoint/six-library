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

#include "cgm/Rectangle.h"

NITFAPI(cgm_Rectangle*) cgm_Rectangle_construct(nitf_Error* error)
{
    cgm_Rectangle* r = (cgm_Rectangle*)NITF_MALLOC(sizeof(cgm_Rectangle));
    if (!r)
    {
        nitf_Error_init(error, NITF_STRERROR( NITF_ERRNO ),
                NITF_CTXT, NITF_ERR_MEMORY);
        return NULL;
    }
    r->x1 = r->x2 = r->y1 = r->y2 = -1;
    return r;
}

NITFAPI(cgm_Rectangle*) cgm_Rectangle_clone(cgm_Rectangle* source,
                                            nitf_Error* error)
{
    cgm_Rectangle* r = cgm_Rectangle_construct(error);
    if (!r)
        return NULL;
    
    r->x1 = source->x1;
    r->x2 = source->x2;
    r->y1 = source->y1;
    r->y2 = source->y2;
    
    return r;
}

NITFAPI(void) cgm_Rectangle_destruct(cgm_Rectangle** r)
{
    if (*r)
    {
        NITF_FREE( *r );
        *r = NULL;
    }
}

NITFAPI(void) cgm_Rectangle_print(cgm_Rectangle* r)
{
    printf("R1(%d %d), R2(%d %d)\n",
           r->x1, r->y1, r->x2, r->y2 );
}

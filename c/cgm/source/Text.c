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

#include "cgm/Text.h"

NITFAPI(cgm_Text*) cgm_Text_construct(const char* text, nitf_Error* error)
{
    cgm_Text* v = (cgm_Text*)NITF_MALLOC(sizeof(cgm_Text));
    if (!v)
    {
        nitf_Error_init(error, NITF_STRERROR( NITF_ERRNO ),
                NITF_CTXT, NITF_ERR_MEMORY);
        return NULL;
    }
    v->x = v->y = -1;
    v->str = NULL;
    if (text)
    {
        v->str = (char*)NITF_MALLOC( strlen( text ) + 1 );
        strcpy(v->str, text);
    }
    return v;
}

NITFAPI(cgm_Text*) cgm_Text_clone(cgm_Text* source, nitf_Error* error)
{
    cgm_Text* text = cgm_Text_construct(source->str, error);
    if (!text)
        return NULL;
    
    text->x = source->x;
    text->y = source->y;
    return text;
}

NITFAPI(void) cgm_Text_destruct(cgm_Text** v)
{
    if (*v)
    {
        if ( (*v)->str )
        {
            NITF_FREE( (*v)->str );
        }
        NITF_FREE( *v );
        *v = NULL;
    }
}

NITFAPI(void) cgm_Text_print(cgm_Text* t)
{
    printf("(%d, %d): %s\n", t->x, t->y, t->str);
}

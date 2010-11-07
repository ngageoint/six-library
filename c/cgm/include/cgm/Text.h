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

#ifndef __CGM_TEXT_H__
#define __CGM_TEXT_H__

#include "cgm/BasicTypes.h"

NITF_CXX_GUARD

typedef struct _cgm_Text
{
    short x;
    short y;
    char* str;

} cgm_Text;

NITFAPI(cgm_Text*) cgm_Text_construct(const char* text,
        nitf_Error* error);

NITFAPI(cgm_Text*) cgm_Text_clone(cgm_Text* source,
        nitf_Error* error);

NITFAPI(void) cgm_Text_destruct(cgm_Text** t);

NITFAPI(void) cgm_Text_print(cgm_Text* t);

NITF_CXX_ENDGUARD

#endif

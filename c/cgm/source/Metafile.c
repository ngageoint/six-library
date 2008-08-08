/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2008, General Dynamics - Advanced Information Systems
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


#include "cgm/Metafile.h"


NITFAPI(cgm_Metafile*) cgm_Metafile_construct(nitf_Error* error)
{
    cgm_Metafile* mf = (cgm_Metafile*) NITF_MALLOC( sizeof(cgm_Metafile) );
    /* Copout */
    return mf;
	
}
NITFAPI(void) cgm_Metafile_destruct(cgm_Metafile** mf)
{
    if (*mf)
    {
	NITF_FREE( *mf );
	*mf = NULL;
    }
}

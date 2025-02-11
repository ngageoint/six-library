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


#ifndef __CGM_METAFILE_READER_H__
#define __CGM_METAFILE_READER_H__

#include "cgm/Metafile.h"
#include "cgm/Color.h"
#include "cgm/TextElement.h"
#include "cgm/PolygonElement.h"
#include "cgm/PolyLineElement.h"
#include "cgm/PolySetElement.h"
#include "cgm/RectangleElement.h"
#include "cgm/CircleElement.h"
#include "cgm/EllipseElement.h"
#include "cgm/CircularArcElement.h"
#include "cgm/CircularArcCloseElement.h"
#include "cgm/EllipticalArcElement.h"
#include "cgm/EllipticalArcCloseElement.h"

NITF_CXX_GUARD


typedef struct _cgm_ParseContext
{
    cgm_Color fillColor;
    cgm_Color color;
    cgm_InteriorStyle style;
    short height;
    short index;
    short orientation[4];
    short visibility;
    short width;
    cgm_HatchType hatchIndex;
    cgm_Type type;
    
} cgm_ParseContext;


typedef NITF_BOOL (*CGM_UNPACK)(cgm_Metafile*,
				cgm_ParseContext*,
				int,
				int,
				char*, 
				int,
				nitf_Error*);

typedef struct _cgm_ElementReader
{
    short code;
    CGM_UNPACK unpack;
} cgm_ElementReader;

typedef struct _cgm_MetafileReader
{
    cgm_ElementReader** unpacker;
} cgm_MetafileReader;

NITFAPI(cgm_MetafileReader*) 
cgm_MetafileReader_construct(nitf_Error* error);

NITFAPI(void) 
cgm_MetafileReader_destruct(cgm_MetafileReader** reader);

NITFAPI(cgm_Metafile*) cgm_MetafileReader_read(cgm_MetafileReader* reader,
					       nitf_IOInterface* in, 
					       nitf_Error* error);


NITF_CXX_ENDGUARD

#endif

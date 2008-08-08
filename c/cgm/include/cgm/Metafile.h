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

#ifndef __CGM_METAFILE_H__
#define __CGM_METAFILE_H__

#include <import/nitf.h>

NITF_CXX_GUARD

typedef enum _cgm_ElementType
{
    CGM_TEXT_ELEMENT = 0,
    CGM_POLYGON_ELEMENT,
    CGM_POLYLINE_ELEMENT,
    CGM_POLYSET_ELEMENT,
    CGM_ELLIPSE_ELEMENT,
    CGM_ELLIPTICAL_ARC_ELEMENT,
    CGM_ELLIPTICAL_CLOSED_ARC_ELEMENT,
    CGM_RECTANGLE_ELEMENT,
    CGM_CIRCLE_ELEMENT,
    CGM_CIRCULAR_ARC_CENTER,
    CGM_CIRCULAR_ARC_CENTER_CLOSE,
} cgm_ElementType;

typedef struct _cgm_Rectangle
{
    short corner1X;
    short corner1Y;
    short corner2X;
    short corner2Y;

} cgm_Rectangle;

typedef struct _cgm_Text
{
    short x;
    short y;
    char* text;
} cgm_Text;

typedef struct _cgm_CharacterOrientation
{
    short y;
    short x;
} cgm_CharacterOrientation;



typedef enum _cgm_Color
{
    CGM_R = 0,
    CGM_G = 1,
    CGM_B = 2,
    CGM_RGB = 3,

} cgm_Color;


typedef struct _cgm_PolygonElement
{
    cgm_ElementType elementType;
    nitf_List* vertices;
} cgm_PolygonElement;

typedef struct _cgm_PolySetElement
{
    cgm_ElementType elementType;
    nitf_List* vertices;
} cgm_PolySetElement;

typedef struct _cgm_PolyLineElement
{
    cgm_ElementType elementType;
    nitf_List* vertices;
} cgm_PolyLineElement;

typedef enum _cgm_ColorSelectionMode
{
    CGM_DIRECT = 1
} cgm_ColorSelectionMode;

typedef enum _cgm_WidthSpecificationMode
{
    CGM_ABSOLUTE = 0
} cgm_WidthSpecificationMode;


typedef enum _cgm_InteriorStyle
{
    CGM_IS_NOT_SET = 0,
    CGM_IS_SOLID,
    CGM_IS_UNKNOWN,
    CGM_IS_HATCH,
    CGM_IS_EMPTY

} cgm_InteriorStyle;

typedef enum _cgm_HatchType
{
    CGM_HATCH_NOT_SET = 0,
    CGM_HATCH_HORIZONTAL,
    CGM_HATCH_VERTICAL,
    CGM_HATCH_POSITIVE_SLOPE,
    CGM_HATCH_NEGATIVE_SLOPE,
    CGM_HATCH_HORIZ_VERT_CROSSHATCH,
    CGM_HATCH_POS_NEG_SLOPE_CROSS
    
} cgm_HatchType;

typedef enum _cgm_Type
{
    CGM_TYPE_NOT_SET = 0,
    CGM_TYPE_SOLID,
    CGM_TYPE_DASHED,
    CGM_TYPE_DOT,
    CGM_TYPE_DASH_DOT,
    CGM_TYPE_DASH_DOT_DOT
} cgm_Type;

typedef struct _cgm_MetafileDescriptor
{
    /* NITF only supports CGM version 1 */
    short version;

    /* This is required to be 1 -1 1*/
    short elementList[3];

    /* This is the description of the metafile */
    char* description;
 
    /* List of fonts */
    nitf_List* fontList;

} cgm_MetafileDescriptor;



typedef struct _cgm_TextElement
{
    cgm_ElementType elementType;
    int color[CGM_RGB];
    short characterHeight;
    short textFontIndex;
    cgm_Rectangle characterOrientation;
    cgm_Text text;
} cgm_TextElement;

typedef struct _cgm_EllipseElement
{
    cgm_ElementType elementType;

    short centerX;
    short centerY;
    short end1X;
    short end1Y;
    short end2X;
    short end2Y;

} cgm_EllipseElement;

/* Serves both elliptical arc and elliptical arc close */
typedef struct _cgm_EllipticalArcElement
{
    cgm_ElementType elementType;
    short centerX;
    short centerY;
    short end1X;
    short end1Y;
    short end2X;
    short end2Y;
    short startVectorX;
    short startVectorY;
    short endVectorX;
    short endVectorY;
    short closeType;
} cgm_EllipticalArcElement;

typedef struct _cgm_CircleElement
{
    cgm_ElementType elementType;
    short centerX;
    short centerY;
    short radius;

} cgm_CircleElement;

/* Serves both circle arc center and circle arc center close */
typedef struct _cgm_CircleArcCenterElement
{
    cgm_ElementType elementType;
    short centerX;
    short centerY;
    short startX;
    short startY;
    short endX;
    short enhdY;
    short radius;
    short closeType;
} cgm_CircleArcCenter;


typedef struct _cgm_PictureBody
{

    NITF_BOOL transparency;
    short auxColor[CGM_RGB];
    nitf_List* elements;

} cgm_PictureBody;

typedef struct _cgm_PictureDescriptor
{
    cgm_ColorSelectionMode colorSelectionMode;
    cgm_WidthSpecificationMode edgeWidthSpec;
    cgm_WidthSpecificationMode lineWidthSpec;
    cgm_Rectangle vdcExtent;

} cgm_PictureDescriptor;

typedef struct _cgm_Picture
{
    char* name;
    cgm_PictureDescriptor descriptor;
    cgm_PictureBody body;

} cgm_Picture;

typedef struct _cgm_Metafile
{
    /* The metafile title */
    char* name;

    /* Description info for metafile */
    cgm_MetafileDescriptor descriptor;

    /* Picture */
    cgm_Picture picture;

} cgm_Metafile;

NITFAPI(cgm_Metafile*) cgm_Metafile_construct(nitf_Error* error);
NITFAPI(void) cgm_Metafile_destruct(cgm_Metafile** mf);
				 
NITF_CXX_ENDGUARD

#endif

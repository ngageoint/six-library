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

#ifndef __CGM_ELEMENTS_H__
#define __CGM_ELEMENTS_H__

#include "cgm/BasicTypes.h"
#include "cgm/Rectangle.h"
#include "cgm/Vertex.h"
#include "cgm/Text.h"

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


typedef void (*CGM_ELEMENT_DESTROY)(NITF_DATA*);
typedef void (*CGM_ELEMENT_PRINT)(NITF_DATA*);
typedef struct _cgm_Element
{

    cgm_ElementType type;
    CGM_ELEMENT_DESTROY destroy;
    CGM_ELEMENT_PRINT print;
    NITF_DATA* data;

} cgm_Element;


typedef struct _cgm_PolySetElement
{
    short fillColor[CGM_RGB];
    cgm_InteriorStyle interiorStyle;
    short edgeVisibility;
    short edgeWidth;
    cgm_Type edgeType;
    short edgeColor[CGM_RGB];
    nitf_List* vertices;
} cgm_PolySetElement;

typedef struct _cgm_PolyLineElement
{
    short lineWidth;
    cgm_Type lineType;
    short lineColor[CGM_RGB];
    nitf_List* vertices;
} cgm_PolyLineElement;



typedef struct _cgm_EllipseElement
{
    short fillColor[CGM_RGB];
    cgm_InteriorStyle interiorStyle;
    short edgeVisibility;
    short edgeWidth;
    cgm_Type edgeType;
    short edgeColor[CGM_RGB];
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
    short lineWidth;
    cgm_Type lineType;
    short lineColor[CGM_RGB];
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
} cgm_EllipticalArcElement;

typedef struct _cgm_EllipticalArcCloseElement
{
    short fillColor[CGM_RGB];
    cgm_InteriorStyle interiorStyle;
    short edgeVisibility;
    short edgeWidth;
    cgm_Type edgeType;
    short edgeColor[CGM_RGB];
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
} cgm_EllipticalArcCloseElement;


typedef struct _cgm_RectangleElement
{
    short fillColor[CGM_RGB];
    cgm_InteriorStyle interiorStyle;
    short edgeVisibility;
    short edgeWidth;
    cgm_Type edgeType;
    short edgeColor[CGM_RGB];
    cgm_Rectangle* rectangle;
} cgm_RectangleElement;

typedef struct _cgm_CircleElement
{
    short fillColor[CGM_RGB];
    cgm_InteriorStyle interiorStyle;
    short edgeVisibility;
    short edgeWidth;
    cgm_Type edgeType;
    short edgeColor[CGM_RGB];
    short centerX;
    short centerY;
    short radius;
    
} cgm_CircleElement;

/* Serves both circle arc center and circle arc center close */
typedef struct _cgm_CircleArcElement
{
    short lineWidth;
    cgm_Type lineType;
    short lineColor[CGM_RGB];
    short centerX;
    short centerY;
    short startX;
    short startY;
    short endX;
    short endY;
    short radius;
} cgm_CircleArcElement;

/* Serves both circle arc center and circle arc center close */
typedef struct _cgm_CircleArcElementClose
{
    short fillColor[CGM_RGB];
    cgm_InteriorStyle interiorStyle;
    short edgeVisibility;
    short edgeWidth;
    cgm_Type edgeType;
    short edgeColor[CGM_RGB];
    short centerX;
    short centerY;
    short startX;
    short startY;
    short endX;
    short endY;
    short radius;
    short closeType;
} cgm_CircleArcElementClose;


/*!
 *  Im thinking, we probably wont instantiate this directly.  Instead,
 *  this class is more like a pure-virtual base.  The important thing,
 *  should be that the unserializers should know how to construct the
 *  object-of-interest, which is contained in the data portion of the
 *  object.  Thus, those objects should have the 'derived' constructors,
 *  and they should also specify destructor functions.
 *
 */
NITFAPI(cgm_Element*) cgm_Element_construct(nitf_Error* error);

/*!
 *  If we follow the approach specified above, we can just destroy
 *  this element by calling the 'sub-class' destroy function pointer
 *  if we are set.  I guess we will handle the NULL data pointer to
 *  make sure they dont have to in their functions (what a nice guy).
 *
 */
NITFAPI(void) cgm_Element_destruct(cgm_Element** element);

/*
 *  We could make all these methods 'factory' like.  It might look a little
 *  more graceful, but the idea is conceptually this:
 *  Use the cgm_Element_construct() to create the base object, and assign
 *  it the destructor for this type (we can make one destructor apply for
 *  all destroy() where the Element type doesnt require deletion of sub-field
 *  data.  Also, we need to malloc our actual element and tie it up to the
 *  'data' field (obviously).
 */

NITFAPI(cgm_Element*) cgm_PolySetElement_construct(nitf_Error* error);
NITFAPI(cgm_Element*) cgm_PolyLineElement_construct(nitf_Error* error);
NITFAPI(cgm_Element*) cgm_RectangleElement_construct(nitf_Error* error);
NITFAPI(cgm_Element*) cgm_EllipseElement_construct(nitf_Error* error);
NITFAPI(cgm_Element*) cgm_EllipticalArcElement_construct(nitf_Error* error);
NITFAPI(cgm_Element*) cgm_CircleElement_construct(nitf_Error* error);
NITFAPI(cgm_Element*) cgm_CircleArcElement_construct(nitf_Error* error);

NITFAPI(void) cgm_Element_print(cgm_Element* elem);


NITF_CXX_ENDGUARD

#endif

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

#ifndef __CGM_BASIC_TYPES_H__
#define __CGM_BASIC_TYPES_H__

#include <import/nitf.h>

NITF_CXX_GUARD

typedef struct _cgm_CharacterOrientation
{
    short y;
    short x;
} cgm_CharacterOrientation;


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

typedef enum _cgm_EdgeCloseType
{
    CGM_EDGE_CLOSE_TYPE_NOT_SET = -1,
    CGM_EDGE_CLOSE_TYPE_INVISIBLE,
    CGM_EDGE_CLOSE_TYPE_VISIBLE,
    CGM_EDGE_CLOSE_TYPE_CLOSE_INVISIBLE,
    CGM_EDGE_CLOSE_TYPE_CLOSE_VISIBLE
    
} cgm_EdgeCloseType;

typedef enum _cgm_CloseType
{
    CGM_CLOSE_TYPE_PIE = 0,
    CGM_CLOSE_TYPE_CHORD
    
} cgm_CloseType;

NITF_CXX_ENDGUARD

#endif

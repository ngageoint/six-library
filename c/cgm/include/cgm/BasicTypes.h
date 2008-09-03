#ifndef __CGM_BASIC_TYPES_H__
#define __CGM_BASIC_TYPES_H__

#include <import/nitf.h>

NITF_CXX_GUARD

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

typedef enum _cgm_EllipticalCloseType
{
    CGM_ELLIPTICAL_CLOSE_TYPE_PIE = 0,
    CGM_ELLIPTICAL_CLOSE_TYPE_CHORD
    
} cgm_EllipticalCloseType;

NITF_CXX_ENDGUARD

#endif

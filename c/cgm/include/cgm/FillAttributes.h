#ifndef __CGM_FILL_ATTRIBUTES_H__
#define __CGM_FILL_ATTRIBUTES_H__

#include <import/nitf.h>
#include "cgm/BasicTypes.h"

NITF_CXX_GUARD

typedef struct _cgm_FillAttributes
{
    short *fillColor;
    short *edgeColor;
    cgm_InteriorStyle interiorStyle;
    short edgeVisibility;
    short edgeWidth;
    short edgeType;
} cgm_FillAttributes;


NITFAPI(cgm_FillAttributes*) 
    cgm_FillAttributes_construct(nitf_Error* error);

NITFAPI(void) cgm_FillAttributes_destruct(cgm_FillAttributes** atts);

NITFAPI(void) cgm_FillAttributes_print(cgm_FillAttributes* atts);

NITF_CXX_ENDGUARD

#endif

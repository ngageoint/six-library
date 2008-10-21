#ifndef __CGM_LINE_ATTRIBUTES_H__
#define __CGM_LINE_ATTRIBUTES_H__

#include <import/nitf.h>
#include "cgm/BasicTypes.h"

NITF_CXX_GUARD

typedef struct _cgm_LineAttributes
{
    short *lineColor;
    short lineWidth;
    cgm_Type lineType;
} cgm_LineAttributes;


NITFAPI(cgm_LineAttributes*) 
    cgm_LineAttributes_construct(nitf_Error* error);

NITFAPI(void) cgm_LineAttributes_destruct(cgm_LineAttributes** atts);

NITFAPI(void) cgm_LineAttributes_print(cgm_LineAttributes* atts);

NITF_CXX_ENDGUARD

#endif

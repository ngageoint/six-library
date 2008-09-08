#ifndef __CGM_TEXT_ATTRIBUTES_H__
#define __CGM_TEXT_ATTRIBUTES_H__

#include <import/nitf.h>
#include "cgm/BasicTypes.h"
#include "cgm/Rectangle.h"

NITF_CXX_GUARD

typedef struct _cgm_TextAttributes
{
    short textColor[CGM_RGB];
    short characterHeight;
    short textFontIndex;
    cgm_Rectangle* characterOrientation;
} cgm_TextAttributes;


NITFAPI(cgm_TextAttributes*) 
    cgm_TextAttributes_construct(nitf_Error* error);

NITFAPI(void) cgm_TextAttributes_destruct(cgm_TextAttributes** atts);

NITFAPI(void) cgm_TextAttributes_print(cgm_TextAttributes* atts);

NITF_CXX_ENDGUARD

#endif

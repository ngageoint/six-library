#ifndef __CGM_TEXT_ELEMENT_H__
#define __CGM_TEXT_ELEMENT_H__

#include "cgm/Elements.h"

NITF_CXX_GUARD

typedef struct _cgm_TextElement
{
    short color[CGM_RGB];
    short characterHeight;
    short textFontIndex;
    cgm_Rectangle* characterOrientation;
    cgm_Text* text;
} cgm_TextElement;

NITFAPI(cgm_Element*) cgm_TextElement_construct(nitf_Error* error);

NITF_CXX_ENDGUARD

#endif

#ifndef __CGM_TEXT_H__
#define __CGM_TEXT_H__

#include "cgm/BasicTypes.h"


NITF_CXX_GUARD

typedef struct _cgm_Text
{
    short x;
    short y;
    char* str;

} cgm_Text;

NITF_CXX_ENDGUARD

NITFAPI(cgm_Text*) cgm_Text_construct(char* text, 
				      nitf_Error* error);

NITFAPI(void) cgm_Text_destruct(cgm_Text** t);

#endif

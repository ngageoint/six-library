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


NITFAPI(cgm_Text*) cgm_Text_construct(char* text, 
				      nitf_Error* error);

NITFAPI(void) cgm_Text_destruct(cgm_Text** t);

NITFAPI(void) cgm_Text_print(cgm_Text* t);

NITF_CXX_ENDGUARD

#endif

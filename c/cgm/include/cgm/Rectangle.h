#ifndef __CGM_RECTANGLE_H__
#define __CGM_RECTANGLE_H__

#include "cgm/BasicTypes.h"


NITF_CXX_GUARD

typedef struct _cgm_Rectangle
{
    short x1;
    short y1;
    short x2;
    short y2;
    
} cgm_Rectangle;


NITFAPI(cgm_Rectangle*) cgm_Rectangle_construct(nitf_Error* error);

NITFAPI(void) cgm_Rectangle_destruct(cgm_Rectangle** r);

NITFAPI(void) cgm_Rectangle_print(cgm_Rectangle* r);

NITF_CXX_ENDGUARD


#endif

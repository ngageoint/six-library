#ifndef __CGM_VERTEX_CLOSE_H__
#define __CGM_VERTEX_CLOSE_H__

#include <import/nitf.h>
#include "cgm/BasicTypes.h"
NITF_CXX_GUARD

typedef struct _cgm_VertexClose
{
    short x;
    short y;
    cgm_EdgeCloseType edgeOutFlag;
    
} cgm_VertexClose;



NITFAPI(cgm_VertexClose*) cgm_VertexClose_construct(nitf_Error* error);

NITFAPI(void) cgm_VertexClose_destruct(cgm_VertexClose** v);

NITFAPI(void) cgm_VertexClose_print(cgm_VertexClose* v);

NITF_CXX_ENDGUARD

#endif

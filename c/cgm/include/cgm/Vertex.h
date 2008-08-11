#ifndef __CGM_VERTEX_H__
#define __CGM_VERTEX_H__

#include <import/nitf.h>

NITF_CXX_GUARD


typedef struct _cgm_Vertex
{
    short x;
    short y;
} cgm_Vertex;

NITF_CXX_ENDGUARD

NITFAPI(cgm_Vertex*) cgm_Vertex_construct(nitf_Error* error);

NITFAPI(void) cgm_Vertex_destruct(cgm_Vertex** v);

NITFAPI(void) cgm_Vertex_print(cgm_Vertex* v);

#endif

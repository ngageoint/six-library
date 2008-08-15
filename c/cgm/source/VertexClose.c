#include "cgm/VertexClose.h"


NITFAPI(cgm_VertexClose*) cgm_VertexClose_construct(nitf_Error* error)
{
    cgm_VertexClose* v = (cgm_VertexClose*)NITF_MALLOC(sizeof(cgm_VertexClose));
    if (!v)
    {
	nitf_Error_init(error, NITF_STRERROR( NITF_ERRNO ),
			NITF_CTXT, NITF_ERR_MEMORY);
	return NULL;
    }
    v->x = v->y = -1;
    v->edgeOutFlag = CGM_EDGE_CLOSE_TYPE_NOT_SET;
    return v;
}

NITFAPI(void) cgm_VertexClose_destruct(cgm_VertexClose** v)
{
    if (*v)
    {
	NITF_FREE( *v );
	*v = NULL;
    }
}

NITFAPI(void) cgm_VertexClose_print(cgm_VertexClose* v)
{
    printf("V(%d %d, %d)\n", 
	   v->x, v->y, v->edgeOutFlag );

}

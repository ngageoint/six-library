#include "cgm/Vertex.h"


NITFAPI(cgm_Vertex*) cgm_Vertex_construct(nitf_Error* error)
{
    cgm_Vertex* v = (cgm_Vertex*)NITF_MALLOC(sizeof(cgm_Vertex));
    if (!v)
    {
	nitf_Error_init(error, NITF_STRERROR( NITF_ERRNO ),
			NITF_CTXT, NITF_ERR_MEMORY);
	return NULL;
    }
    v->x = v->y = -1;;
    return v;
}

NITFAPI(void) cgm_Vertex_destruct(cgm_Vertex** v)
{
    if (*v)
    {
	NITF_FREE( *v );
	*v = NULL;
    }
}

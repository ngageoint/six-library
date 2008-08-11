#include "cgm/Rectangle.h"


NITFAPI(cgm_Rectangle*) cgm_Rectangle_construct(nitf_Error* error)
{
    cgm_Rectangle* r = (cgm_Rectangle*)NITF_MALLOC(sizeof(cgm_Rectangle));
    if (!r)
    {
	nitf_Error_init(error, NITF_STRERROR( NITF_ERRNO ),
			NITF_CTXT, NITF_ERR_MEMORY);
	return NULL;
    }
    r->x1 = r->x2 = r->y1 = r->y2 = -1;
    return r;
}

NITFAPI(void) cgm_Rectangle_destruct(cgm_Rectangle** r)
{
    if (*r)
    {
	NITF_FREE( *r );
	*r = NULL;
    }
}

NITFAPI(void) cgm_Rectangle_print(cgm_Rectangle* r)
{
    printf("R1(%d %d), R2(%d %d)\n", 
	   r->x1, r->y1, r->x2, r->y2 );
}

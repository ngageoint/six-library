#include "cgm/Text.h"


NITFAPI(cgm_Text*) cgm_Text_construct(char* text, nitf_Error* error)
{
    cgm_Text* v = (cgm_Text*)NITF_MALLOC(sizeof(cgm_Text));
    if (!v)
    {
	nitf_Error_init(error, NITF_STRERROR( NITF_ERRNO ),
			NITF_CTXT, NITF_ERR_MEMORY);
	return NULL;
    }
    v->x = v->y = -1;
    v->str = NULL;
    if (text)
    {
	v->str = (char*)NITF_MALLOC( strlen( text ) + 1 );
	strcpy(v->str, text);
    }
    return v;
}

NITFAPI(void) cgm_Text_destruct(cgm_Text** v)
{
    if (*v)
    {

	if ( (*v)->str )
	{
	    NITF_FREE( (*v)->str );
	}
	NITF_FREE( *v );
	*v = NULL;
    }
}

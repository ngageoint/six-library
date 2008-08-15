#include "cgm/Elements.h"


NITFAPI(cgm_Element*) cgm_Element_construct(nitf_Error* error)
{
    cgm_Element* element = (cgm_Element*)NITF_MALLOC(sizeof(cgm_Element));
    if (!element)
    {
	nitf_Error_init(error,
			NITF_STRERROR( NITF_ERRNO ),
			NITF_CTXT,
			NITF_ERR_MEMORY);
	return NULL;
    }
    element->print = NULL;
    element->destroy = NULL;
    element->data = NULL;
    return element;
    
}

NITFAPI(void) cgm_Element_destruct(cgm_Element** element)
{
    if (*element)
    {
	if ( (*element)->data )
	    (*element)->destroy( (*element)->data );
	NITF_FREE( (*element) );
    }
    *element = NULL;
}

NITFAPI(void) cgm_Element_print(cgm_Element* element)
{
    if (element->print && element->data)
    {
	printf("Element:\n");
	(* element->print )(element->data);
    }
}
/*!
 *  Beauty is, we dont even need to know what type it is.  Lets count
 *  how many times we can get away with this (I guess probably a lot).
 */
NITFPRIV(void) basicDestroy(NITF_DATA* data)
{
    NITF_FREE( data );
}



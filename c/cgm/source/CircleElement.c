#include "cgm/CircleElement.h"

NITFPRIV(void) destroy(NITF_DATA* data)
{
    if ( ((cgm_CircleElement*)data)->attributes)
    {
        cgm_FillAttributes_destruct( & ((cgm_CircleElement*)data)->attributes);
    }
    NITF_FREE( data );
}

NITFPRIV(void) print(NITF_DATA* data)
{
    cgm_CircleElement* circle = (cgm_CircleElement*)data;
    if (circle->attributes)
        cgm_FillAttributes_print(circle->attributes);
    printf("\tOrigin: (%d, %d), Radius: [%d]\n",
	   circle->centerX, circle->centerY, circle->radius);
    
}
NITFAPI(cgm_Element*) cgm_CircleElement_construct(nitf_Error* error)
{
    cgm_Element* element = cgm_Element_construct(CGM_CIRCLE_ELEMENT, error);
    if (element)
    {
	cgm_CircleElement* circle = (cgm_CircleElement*)
	    NITF_MALLOC(sizeof(cgm_CircleElement));
	if (!circle)
	{
	    NITF_FREE(element);
	    return NULL;
	    
	}
        circle->attributes = NULL;        
	circle->centerX = -1;
	circle->centerY = -1;
	circle->radius = -1;
	element->data = (NITF_DATA*)circle;
    }
    element->print = &print;
    element->destroy = &destroy;
    
    return element;
    
}

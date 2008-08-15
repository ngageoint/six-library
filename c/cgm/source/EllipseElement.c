#include "cgm/EllipseElement.h"

NITFPRIV(void) destroy(NITF_DATA* data)
{
    if ( ((cgm_EllipseElement*)data)->attributes)
    {
        cgm_FillAttributes_destruct( & ((cgm_EllipseElement*)data)->attributes);
    }
    NITF_FREE( data );

}

NITFPRIV(void) print(NITF_DATA* data)
{
    cgm_EllipseElement* ellipse = (cgm_EllipseElement*)data;
    if (ellipse->attributes)
        cgm_FillAttributes_print( ellipse->attributes );

    printf("\tOrigin (%d, %d)\n",
	   ellipse->centerX, ellipse->centerY);
    printf("\tEnd Point 1 (%d, %d)\n",
	   ellipse->end1X, ellipse->end1Y);
    printf("\tEnd Point 2 (%d, %d)\n",
	   ellipse->end2X, ellipse->end2Y);
    
    
}

NITFAPI(cgm_Element*) cgm_EllipseElement_construct(nitf_Error* error)
{
    cgm_Element* element = cgm_Element_construct(CGM_ELLIPSE_ELEMENT, error);
    if (element)
    {
	cgm_EllipseElement* ellipse = (cgm_EllipseElement*)
	    NITF_MALLOC(sizeof(cgm_EllipseElement));
	if (!ellipse)
	{
	    NITF_FREE(element);
	    return NULL;
	    
	}
        ellipse->attributes = NULL;	
	element->data = (NITF_DATA*)ellipse;
        
    }
    element->print = &print;
    element->destroy = &destroy;
    
    return element;
    
}

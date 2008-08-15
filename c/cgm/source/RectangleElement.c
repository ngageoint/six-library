#include "cgm/RectangleElement.h"



NITFPRIV(void) rectangleDestroy(NITF_DATA* data)
{
    cgm_RectangleElement* rect = (cgm_RectangleElement*)data;
    
    if (rect->attributes)
    {
        cgm_FillAttributes_destruct( &(rect->attributes) );
    }
    if (rect->rectangle)
    {
	cgm_Rectangle_destruct( & (rect->rectangle) );
    }
    NITF_FREE(data);
}

NITFPRIV(void) rectanglePrint(NITF_DATA* data)
{
    cgm_RectangleElement* rect = (cgm_RectangleElement*)data;
    if (rect->attributes)
        cgm_FillAttributes_print( rect->attributes );
    printf("\t");
    cgm_Rectangle_print(rect->rectangle);
    
}



NITFAPI(cgm_Element*) cgm_RectangleElement_construct(nitf_Error* error)
{
    cgm_Element* element = cgm_Element_construct(CGM_RECTANGLE_ELEMENT, error);
    if (element)
    {
	cgm_RectangleElement* rect = (cgm_RectangleElement*)
	    NITF_MALLOC(sizeof(cgm_RectangleElement));
	if (!rect)
	{
	    NITF_FREE(element);
	    return NULL;
	    
	}

        rect->attributes = NULL;
	/* TODO: This default arg in constructor doesnt appear to be 
	   that helpful.  Remove?  Otherwise, might want to make consistent
	 */
	rect->rectangle = cgm_Rectangle_construct(error);
	if (!rect->rectangle)
	{
	    NITF_FREE( rect );
	    NITF_FREE( element );
	    return NULL;
	      
	}
	
	element->data = (NITF_DATA*)rect;

    }

    element->print = &rectanglePrint;
    element->destroy = &rectangleDestroy;
	    
    return element;

}

#include "cgm/RectangleElement.h"



NITFPRIV(void) rectangleDestroy(NITF_DATA* data)
{
    cgm_RectangleElement* rect = (cgm_RectangleElement*)data;
    
    if (rect->rectangle)
    {
	cgm_Rectangle_destruct( & (rect->rectangle) );
    }
    NITF_FREE(data);
}

NITFPRIV(void) rectanglePrint(NITF_DATA* data)
{
    cgm_RectangleElement* rect = (cgm_RectangleElement*)data;
    printf("\tFill Color: (%d, %d, %d)\n", 
	   rect->fillColor[CGM_R],
	   rect->fillColor[CGM_G],
	   rect->fillColor[CGM_B]);
    printf("\tInterior Style [%d]\n", rect->interiorStyle);
    printf("\tEdge Visibility [%d]\n", rect->edgeVisibility);
    printf("\tEdge Width [%d]\n", rect->edgeWidth);
    printf("\tEdge Color: (%d, %d, %d)\n", 
	   rect->edgeColor[CGM_R],
	   rect->edgeColor[CGM_G],
	   rect->edgeColor[CGM_B]);

    cgm_Rectangle_print(rect->rectangle);
    
}



NITFAPI(cgm_Element*) cgm_RectangleElement_construct(nitf_Error* error)
{
    cgm_Element* element = cgm_Element_construct(error);
    if (element)
    {
	cgm_RectangleElement* rect = (cgm_RectangleElement*)
	    NITF_MALLOC(sizeof(cgm_RectangleElement));
	if (!rect)
	{
	    NITF_FREE(element);
	    return NULL;
	    
	}
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

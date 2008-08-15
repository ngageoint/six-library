#include "cgm/CircularArcElement.h"


NITFPRIV(void) destroy(NITF_DATA* data)
{
    if ( ((cgm_CircularArcElement*)data)->attributes)
    {
        cgm_LineAttributes_destruct( & ((cgm_CircularArcElement*)data)->attributes);
    }
    NITF_FREE( data );

}

NITFPRIV(void) print(NITF_DATA* data)
{
    cgm_CircularArcElement* arc = (cgm_CircularArcElement*)data;

    if (arc->attributes)
        cgm_LineAttributes_print( arc->attributes );    

    printf("\tOrigin (%d, %d)\n",
	   arc->centerX, arc->centerY);
    printf("\tPoint 1(%d, %d)\n",
	   arc->startX, arc->startY);
    printf("\tPoint 2(%d, %d)\n",
	   arc->endX, arc->endY);
    printf("\tRadius [%d]\n",
	   arc->radius);
    
}

NITFAPI(cgm_Element*) cgm_CircularArcElement_construct(nitf_Error* error)
{
    cgm_Element* element = 
        cgm_Element_construct(CGM_CIRCULAR_ARC_CENTER_ELEMENT, 
                              error);
    if (element)
    {
	cgm_CircularArcElement* arc = (cgm_CircularArcElement*)
	    NITF_MALLOC(sizeof(cgm_CircularArcElement));
	if (!arc)
	{
	    NITF_FREE(element);
	    return NULL;
	    
	}
        arc->attributes = NULL;
        
	arc->centerX = -1;
	arc->centerY = -1;
	arc->startX = -1;
	arc->startY = -1;
	arc->endX = -1;
	arc->endY = -1;
	arc->radius = -1;
	element->data = (NITF_DATA*)arc;
        
    }
    element->print = &print;
    element->destroy = &destroy;
    
    return element;
    
}

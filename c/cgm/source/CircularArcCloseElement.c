#include "cgm/CircularArcCloseElement.h"


NITFPRIV(void) destroy(NITF_DATA* data)
{
    if ( ((cgm_CircularArcCloseElement*)data)->attributes)
    {
        cgm_FillAttributes_destruct( & ((cgm_CircularArcCloseElement*)data)->attributes);
    }
    NITF_FREE( data );
}

NITFPRIV(void) print(NITF_DATA* data)
{
    cgm_CircularArcCloseElement* arc = 
        (cgm_CircularArcCloseElement*)data;

    if (arc->attributes)
        cgm_FillAttributes_print( arc->attributes );
    printf("\tOrigin (%d, %d)\n",
	   arc->centerX, arc->centerY);
    printf("\tStart (%d, %d)\n",
	   arc->startX, arc->startY);
    printf("\tEnd (%d, %d)\n",
	   arc->endX, arc->endY);
    printf("\tRadius [%d]\n",
	   arc->radius);
    printf("\tClose Type (%s)\n",
	   arc->closeType == CGM_CLOSE_TYPE_PIE ?
           "pie": "chord");
    
    

}

NITFAPI(cgm_Element*) cgm_CircularArcCloseElement_construct(nitf_Error* error)
{
    cgm_Element* element = 
        cgm_Element_construct(CGM_CIRCULAR_ARC_CENTER_CLOSE_ELEMENT,
                              error);
    if (element)
    {
	cgm_CircularArcCloseElement* arc = (cgm_CircularArcCloseElement*)
	    NITF_MALLOC(sizeof(cgm_CircularArcCloseElement));
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
	arc->radius = CGM_CLOSE_TYPE_PIE;
	arc->closeType = -1;
	element->data = (NITF_DATA*)arc;
        
    }
    element->print = &print;
    element->destroy = &destroy;
    
    return element;
    
}

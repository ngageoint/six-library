#include "cgm/EllipticalArcCloseElement.h"

NITFPRIV(void) destroy(NITF_DATA* data)
{
    if ( ((cgm_EllipticalArcCloseElement*)data)->attributes)
    {
        cgm_FillAttributes_destruct( & ((cgm_EllipticalArcCloseElement*)data)->attributes);
    }
    NITF_FREE( data );

}

NITFPRIV(void) print(NITF_DATA* data)
{
    cgm_EllipticalArcCloseElement* arc = (cgm_EllipticalArcCloseElement*)data;
    if (arc->attributes)
        cgm_FillAttributes_print(arc->attributes);

    printf("\tOrigin (%d, %d)\n",
	   arc->centerX, arc->centerY);
    printf("\tEnd Point 1 (%d, %d)\n",
	   arc->end1X, arc->end1Y);
    printf("\tEnd Point 2 (%d, %d)\n",
	   arc->end2X, arc->end2Y);
    printf("\tStart Vector (%d, %d)\n",
	   arc->startVectorX, arc->startVectorY);
    printf("\tEnd Vector (%d, %d)\n",
	   arc->endVectorX, arc->endVectorY);
    

}

NITFAPI(cgm_Element*) cgm_EllipticalArcCloseElement_construct(nitf_Error* error)
{
    cgm_Element* element = 
        cgm_Element_construct(CGM_ELLIPTICAL_ARC_CENTER_CLOSE_ELEMENT,
                              error);
    if (element)
    {
	cgm_EllipticalArcCloseElement* arc = (cgm_EllipticalArcCloseElement*)
	    NITF_MALLOC(sizeof(cgm_EllipticalArcCloseElement));
	if (!arc)
	{
	    NITF_FREE(element);
	    return NULL;
	    
	}

        arc->attributes = NULL;
        
	arc->centerX = -1;
	arc->centerY = -1;
	arc->end1X = -1;
	arc->end1Y = -1;
	arc->end2X = -1;
	arc->end2Y = -1;
	arc->startVectorX = -1;
	arc->endVectorY = -1;
	element->data = (NITF_DATA*)arc;
        
    }
    element->print = &print;
    element->destroy = &destroy;
    
    return element;
    
}

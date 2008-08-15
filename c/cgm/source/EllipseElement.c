#include "cgm/EllipseElement.h"

NITFPRIV(void) destroy(NITF_DATA* data)
{
    NITF_FREE( data );
}

NITFPRIV(void) print(NITF_DATA* data)
{
    cgm_EllipseElement* ellipse = (cgm_EllipseElement*)data;
    printf("\tFill Color: (%d, %d, %d)\n", 
	   ellipse->fillColor[CGM_R],
	   ellipse->fillColor[CGM_G],
	   ellipse->fillColor[CGM_B]);
    printf("\tInterior Style [%d]\n", ellipse->interiorStyle);
    printf("\tEdge Visibility [%d]\n", ellipse->edgeVisibility);
    printf("\tEdge Width [%d]\n", ellipse->edgeWidth);
    printf("\tEdge Color: (%d, %d, %d)\n", 
	   ellipse->edgeColor[CGM_R],
	   ellipse->edgeColor[CGM_G],
	   ellipse->edgeColor[CGM_B]);
    printf("\tE O(%d, %d)\n",
	   ellipse->centerX, ellipse->centerY);
    printf("\tE P1(%d, %d)\n",
	   ellipse->end1X, ellipse->end1Y);
    printf("\tE P2(%d, %d)\n",
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
	ellipse->centerX = -1;
	ellipse->centerY = -1;
	ellipse->end1X = -1;
	ellipse->end1Y = -1;
	ellipse->end2X = -1;
	ellipse->end2Y = -1;
	
	element->data = (NITF_DATA*)ellipse;
        
    }
    element->print = &print;
    element->destroy = &destroy;
    
    return element;
    
}

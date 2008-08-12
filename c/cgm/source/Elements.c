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


NITFAPI(cgm_Element*) cgm_PolySetElement_construct(nitf_Error* error)
{
    return NULL;
}
NITFAPI(cgm_Element*) cgm_PolyLineElement_construct(nitf_Error* error)
{
    return NULL;
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

NITFAPI(cgm_Element*) cgm_EllipseElement_construct(nitf_Error* error)
{
    cgm_Element* element = cgm_Element_construct(error);
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

    element->destroy = &basicDestroy;
	    
    return element;

}
NITFAPI(cgm_Element*) cgm_EllipticalArcElement_construct(nitf_Error* error)
{
    cgm_Element* element = cgm_Element_construct(error);
    if (element)
    {
	cgm_EllipticalArcElement* ellipse = (cgm_EllipticalArcElement*)
	    NITF_MALLOC(sizeof(cgm_EllipticalArcElement));
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
	ellipse->startVectorX = -1;
	ellipse->endVectorY = -1;
	//ellipse->closeType = -1;
	element->data = (NITF_DATA*)ellipse;

    }

    element->destroy = &basicDestroy;
	    
    return element;

}
NITFAPI(cgm_Element*) cgm_CircleElement_construct(nitf_Error* error)
{
    cgm_Element* element = cgm_Element_construct(error);
    if (element)
    {
	cgm_CircleElement* circle = (cgm_CircleElement*)
	    NITF_MALLOC(sizeof(cgm_CircleElement));
	if (!circle)
	{
	    NITF_FREE(element);
	    return NULL;
	    
	}
	circle->centerX = -1;
	circle->centerY = -1;
	circle->radius = -1;
	element->data = (NITF_DATA*)circle;
    }

    element->destroy = &basicDestroy;
	    
    return element;

}
NITFAPI(cgm_Element*) cgm_CircleArcElement_construct(nitf_Error* error)
{
    cgm_Element* element = cgm_Element_construct(error);
    if (element)
    {
	cgm_CircleArcElement* circle = (cgm_CircleArcElement*)
	    NITF_MALLOC(sizeof(cgm_CircleArcElement));
	if (!circle)
	{
	    NITF_FREE(element);
	    return NULL;
	    
	}
	circle->centerX = -1;
	circle->centerY = -1;

	circle->startX = -1;
	circle->startY = -1;

	circle->endX = -1;
	circle->endY = -1;

	circle->radius = -1;

	//circle->closeType = -1;
	element->data = (NITF_DATA*)circle;

    }

    element->destroy = &basicDestroy;
	    
    return element;
    
}


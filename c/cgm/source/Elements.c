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

/*!
 *  Beauty is, we dont even need to know what type it is.  Lets count
 *  how many times we can get away with this (I guess probably a lot).
 */
NITFPRIV(void) basicDestroy(NITF_DATA* data)
{
    NITF_FREE( data );
}


NITFPRIV(void) polygonDestroy(NITF_DATA* data)
{
    /* TODO!! */

    NITF_FREE(data);
}

NITFPRIV(void) polySetDestroy(NITF_DATA* data)
{
    /* TODO!! */

    NITF_FREE(data);

}

NITFPRIV(void) polyLineDestroy(NITF_DATA* data)
{
    /* TODO!! */

    NITF_FREE(data);

}


NITFAPI(cgm_Element*) cgm_PolygonElement_construct(nitf_Error* error)
{
    
}
NITFAPI(cgm_Element*) cgm_PolySetElement_construct(nitf_Error* error)
{

}
NITFAPI(cgm_Element*) cgm_PolyLineElement_construct(nitf_Error* error)
{

}
NITFAPI(cgm_Element*) cgm_TextElement_construct(nitf_Error* error)
{

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
	ellipse->startVectorX = -1;
	ellipse->endVectorY = -1;
	ellipse->closeType = -1;
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
	circle->radius;

    }

    element->destroy = &basicDestroy;
	    
    return element;

}
NITFAPI(cgm_Element*) cgm_CircleArcElement_construct(nitf_Error* error)
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

	circle->startX = -1;
	circle->startY = -1;

	circle->endX = -1;
	circle->endY = -1;

	circle->radius = -1;

	circle->closeType = -1;

    }

    element->destroy = &basicDestroy;
	    
    return element;
    
}


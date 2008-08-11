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



NITFPRIV(void) polyDestroy(NITF_DATA* data)
{
    
    
    /* TODO!! */
    cgm_PolygonElement* poly = (cgm_PolygonElement*)data;
    nitf_ListIterator it, end;
    
    nitf_List* list = (nitf_List*)poly->vertices;
    it = nitf_List_begin(list);
    end = nitf_List_end(list);

    while (nitf_ListIterator_notEqualTo(&it, &end))
    {
	cgm_Vertex* v = (cgm_Vertex*)nitf_ListIterator_get(&it);
	cgm_Vertex_destruct(&v);
	nitf_ListIterator_increment(&it);
    }
    
    nitf_List_destruct(&list);
    
    NITF_FREE(data);

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

NITFPRIV(void) textDestroy(NITF_DATA* data)
{
    
    
    /* TODO!! */
    cgm_TextElement* text = (cgm_TextElement*)data;

    if (text->characterOrientation)
	cgm_Rectangle_destruct( &(text->characterOrientation) );
    
    if (text->text)
    {
	cgm_Text_destruct( &(text->text) );
    }

    NITF_FREE(data);

}

NITFAPI(cgm_Element*) cgm_PolygonElement_construct(nitf_Error* error)
{
    cgm_Element* element = cgm_Element_construct(error);
    if (element)
    {
	cgm_PolygonElement* poly = (cgm_PolygonElement*)
	    NITF_MALLOC(sizeof(cgm_PolygonElement));

	if (!poly)
	{
	    NITF_FREE(element);
	    return NULL;
	    
	}
	poly->vertices = NULL;
	element->data = (NITF_DATA*)poly;

    }

    element->destroy = &polyDestroy;
	    
    return element;
    
}

NITFAPI(cgm_Element*) cgm_PolySetElement_construct(nitf_Error* error)
{
    return NULL;
}
NITFAPI(cgm_Element*) cgm_PolyLineElement_construct(nitf_Error* error)
{
    return NULL;
}

NITFAPI(cgm_Element*) cgm_TextElement_construct(nitf_Error* error)
{
    cgm_Element* element = cgm_Element_construct(error);
    if (element)
    {
	cgm_TextElement* text = (cgm_TextElement*)
	    NITF_MALLOC(sizeof(cgm_TextElement));
	if (!text)
	{
	    NITF_FREE(element);
	    return NULL;
	    
	}
	/* TODO: This default arg in constructor doesnt appear to be 
	   that helpful.  Remove?  Otherwise, might want to make consistent
	 */
	text->text = cgm_Text_construct(NULL, error);
	if (!text->text)
	{
	    NITF_FREE( text );
	    NITF_FREE( element );
	    return NULL;
	      
	}

	text->characterOrientation = cgm_Rectangle_construct(error);
	if (!text->characterOrientation)
	{
	    cgm_Text_destruct( &(text->text) );
	    NITF_FREE( text );
	    NITF_FREE( element );
	    return NULL;
	      
	}

	text->color[CGM_R] = -1;
	text->color[CGM_G] = -1;
	text->color[CGM_B] = -1;

	text->characterHeight = -1;
	text->textFontIndex = -1;
	
	element->data = (NITF_DATA*)text;

    }


    element->destroy = &textDestroy;
	    
    return element;

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

	circle->closeType = -1;
	element->data = (NITF_DATA*)circle;

    }

    element->destroy = &basicDestroy;
	    
    return element;
    
}


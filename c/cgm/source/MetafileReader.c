/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2008, General Dynamics - Advanced Information Systems
 *
 * NITRO is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public 
 * License along with this program;
 * If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "cgm/MetafileReader.h"

#define DEBUG_CGM 1

#ifdef DEBUG_CGM
#    define DBG_TRACE() \
printf("%s(%d, %d, %d)\n", __PRETTY_FUNCTION__, classType, shortCode, len)

#else
/* Do nothing */
#    define DBG_TRACE() 
#endif

#define FILL_EDGE_ATTS(ELEM, PC) \
{ \
    memcpy(ELEM->fillColor, PC->fillColor, 6); \
    ELEM->interiorStyle = PC->style; \
    ELEM->edgeVisibility = PC->visibility; \
    ELEM->edgeWidth = PC->width; \
    ELEM->edgeType = PC->type; \
    memcpy(ELEM->edgeColor, PC->color, 6); \
}

#define FILL_LINE_ATTS(ELEM, PC) \
{ \
    ELEM->lineWidth = pc->width; \
    ELEM->lineType = pc->type; \
    memcpy( ELEM->lineColor, PC->color, 6); \
}

NITFPRIV(void) resetParseContext(cgm_ParseContext* pc)
{
    printf("=======Reset Parse=========\n");
    pc->fillColor[CGM_R] = -1;
    pc->fillColor[CGM_G] = -1;
    pc->fillColor[CGM_B] = -1;

    pc->color[CGM_R] = -1;
    pc->color[CGM_G] = -1;
    pc->color[CGM_B] = -1;

    pc->style = CGM_IS_NOT_SET;
    pc->height = pc->width = pc->index = -1;

    pc->orientation[0] = -1;
    pc->orientation[1] = -1;
    pc->orientation[2] = -1;
    pc->orientation[3] = -1;
    pc->visibility = -1;
    pc->type = CGM_TYPE_NOT_SET;
    pc->hatchIndex = CGM_HATCH_NOT_SET;

}

NITFPRIV(void) printParseContext(cgm_ParseContext* pc)
{
    printf("=== Parse Context ===\n");
    if (pc->fillColor[CGM_R] != -1)
    {
	printf("\tFill Color (%d, %d, %d)\n",
	       pc->fillColor[CGM_R],
	       pc->fillColor[CGM_G],
	       pc->fillColor[CGM_B]);
    }
    if (pc->color[CGM_R] != -1)
    {
	printf("\tEdge/Line Color (%d, %d, %d)\n",
	       pc->color[CGM_R],
	       pc->color[CGM_G],
	       pc->color[CGM_B]);
    }

    if (pc->style != CGM_IS_NOT_SET)
	printf("\tInterior Style: [%d]\n", pc->style);

    if (pc->height != -1)
	printf("\tCharacter Height: [%d]\n", pc->height);

    if (pc->width != -1)
	printf("\tEdge/Line Width: [%d]\n", pc->width);

    if (pc->index != -1)
	printf("\tFont index: [%d]\n", pc->index);

    if (pc->orientation[0] != -1)
    {
	printf("\tCharacter Orientation (%d, %d, %d, %d)\n",
	       pc->orientation[0],
	       pc->orientation[1],
	       pc->orientation[2],
	       pc->orientation[3]);
    }

    if (pc->visibility != -1)
	printf("\tEdge Visibility: [%d]\n", pc->visibility);

    if (pc->type != CGM_TYPE_NOT_SET)
	printf("\tEdge/Line Type: [%d]\n", pc->type);
    
    if (pc->hatchIndex != CGM_HATCH_NOT_SET)
	printf("\tHatch Index: [%d]\n", pc->hatchIndex);
}


NITFPRIV(void) readRGB(char* b, int length, short *rgb3)
{
    rgb3[CGM_R] = 0x00FF & b[CGM_R];
    rgb3[CGM_G] = 0x00FF & b[CGM_G];
    rgb3[CGM_B] = 0x00FF & b[CGM_B];
}

NITFPRIV(cgm_Rectangle*) readRectangle(char* b, int len, nitf_Error* error)
{
    short x1, x2, y1, y2;
    cgm_Rectangle* rectangle = cgm_Rectangle_construct(error);
    if (!rectangle)
	return NULL;

    memcpy(&x1, &b[0], 2);
    rectangle->x1 = NITF_NTOHS(x1);

    memcpy(&y1, &b[2], 2);
    rectangle->y1 = NITF_NTOHS(y1);

    memcpy(&x2, &b[4], 2);
    rectangle->x2 = NITF_NTOHS(x2);

    memcpy(&y2, &b[6], 2);
    rectangle->y2 = NITF_NTOHS(x2);
    return rectangle;
}

NITFPRIV(cgm_Vertex*) readVertex(char* b, nitf_Error* error)
{
    cgm_Vertex* v = cgm_Vertex_construct(error);
    short s;
    if (!v) return NULL;
    
    memcpy(&s, &b[0], 2);
    v->x = NITF_NTOHS(s);
    memcpy(&s, &b[2], 2);
    v->y = NITF_NTOHS(s);
    return v;
}
NITFPRIV(nitf_List*) readVertices(char* b, int length, nitf_Error* error)
{

    int i = 0;
    assert(length % 4 == 0);
    nitf_List* list;
    list = nitf_List_construct(error);
    
    if (!list)
	return NITF_FAILURE;

    for (i = 0; i < length; i+=4)
    {

	cgm_Vertex* v = readVertex(&b[i], error);
	if (!v)
	{
	    /*  TODO: We dont get out so easy, but for now... */
	    nitf_List_destruct(&list);
	    return NULL;
	}
	if (!nitf_List_pushBack(list, v, error))
	{
	    nitf_List_destruct(&list);
	    return NULL;
	}
	
    }
    return list;
}

NITFPRIV(short) readShort(char* b)
{
    short s;
    memcpy(&s, b, 2);
    return NITF_NTOHS(s);
}



NITFPRIV(char*) readString(char* b, int length)
{
    int slen = (int) b[0];
    char* str = (char*)NITF_MALLOC(slen + 1);
    str[slen] = 0;
    memcpy(str, &b[1], slen);
    return str;

}

NITF_BOOL beginMetafile(cgm_Metafile* mf, cgm_ParseContext* pc, int classType, int shortCode, char* b, int len, nitf_Error* error)
{

    DBG_TRACE();
    mf->name = readString(b, len);
    return NITF_SUCCESS;
}

NITF_BOOL endMetafile(cgm_Metafile* mf, cgm_ParseContext* pc, int classType, int shortCode, char* b, int len, nitf_Error* error)
{
    DBG_TRACE();
    return NITF_SUCCESS;
}
NITF_BOOL endPicture(cgm_Metafile* mf, cgm_ParseContext* pc, int classType, int shortCode, char* b, int len, nitf_Error* error)
{
    DBG_TRACE();
    /* We really do nothing here yet */
    return NITF_SUCCESS;
}

NITF_BOOL beginPicture(cgm_Metafile* mf, cgm_ParseContext* pc, int classType, int shortCode, char* b, int len, nitf_Error* error)
{
    DBG_TRACE();
    
    if (mf->picture != NULL)
    {
	nitf_Error_initf(error,
			 NITF_CTXT,
			 NITF_ERR_INVALID_OBJECT,
			 "Picture Body was already set");
	return NITF_FAILURE;

    }
    mf->picture = cgm_Picture_construct(error);
    if (!mf->picture)
	return NITF_FAILURE;

    mf->picture->name = readString(b, len);


    return NITF_SUCCESS;
}

NITF_BOOL beginPictureBody(cgm_Metafile* mf, cgm_ParseContext* pc, int classType, int shortCode, char* b, int len, nitf_Error* error)
{
    DBG_TRACE();
    /* We really do nothing here yet */
    assert(mf->picture);

    if (mf->picture->body != NULL)
    {
	nitf_Error_initf(error,
			 NITF_CTXT,
			 NITF_ERR_INVALID_OBJECT,
			 "Picture Body was already set");
	return NITF_FAILURE;

    }

    mf->picture->body = cgm_PictureBody_construct(error);

    return NITF_SUCCESS;
}

NITF_BOOL metafileVersion(cgm_Metafile* mf, cgm_ParseContext* pc, int classType, int shortCode, char* b, int len, nitf_Error* error)
{
    DBG_TRACE();
    mf->version = readShort(b);
    return NITF_SUCCESS;
}

NITF_BOOL metafileDesc(cgm_Metafile* mf, cgm_ParseContext* pc, int classType, int shortCode, char* b, int len, nitf_Error* error)
{
    DBG_TRACE();
    mf->description = readString(b, len);
    return NITF_SUCCESS;
}

NITF_BOOL metafileList(cgm_Metafile* mf, cgm_ParseContext* pc, int classType, int shortCode, char* b, int len, nitf_Error* error)
{
    DBG_TRACE();
    assert(len == 6);
    mf->elementList[0] = readShort(&b[0]);
    mf->elementList[1] = readShort(&b[2]);
    mf->elementList[2] = readShort(&b[4]);

    return NITF_SUCCESS;
}

NITF_BOOL fontList(cgm_Metafile* mf, cgm_ParseContext* pc, int classType, int shortCode, char* b, int len, nitf_Error* error)
{
    int i = 0;
    int j = 1;
    int total = 0;
    char* p;

    DBG_TRACE();
    
    if (mf->fontList != NULL)
    {
	nitf_Error_initf(error,
			 NITF_CTXT,
			 NITF_ERR_INVALID_OBJECT,
			 "Font list was already set");
	return NITF_FAILURE;

    }
    mf->fontList = nitf_List_construct( error );
    if (!mf->fontList ) return NITF_FAILURE;

    while (1)
    {
	assert ( i <= len);
	unsigned char bu = b[i];
	int slen = 0x000000FF & bu;
	
	p = (char*)NITF_MALLOC( slen + 1);
	p[slen] = 0;
	memcpy(p, &b[++i], slen);
	total += slen + 1;
	
	if(total >= len)
	    break;
	
	i += slen;	
	j++;
	/* Actually, this could get complicated, need to destruct then */
	if (!nitf_List_pushBack(mf->fontList, p, error))
	    return NITF_FAILURE;

    }

    return NITF_SUCCESS;
}

NITF_BOOL colorSelectionMode(cgm_Metafile* mf, cgm_ParseContext* pc, int classType, int shortCode, char* b, int len, nitf_Error* error)
{
    DBG_TRACE();
    mf->picture->colorSelectionMode = readShort(b);
    return NITF_SUCCESS;
}

NITF_BOOL lineWidthSpecMode(cgm_Metafile* mf, cgm_ParseContext* pc, int classType, int shortCode, char* b, int len, nitf_Error* error)
{
    DBG_TRACE();
    mf->picture->lineWidthSpec = readShort(b);
    return NITF_SUCCESS;
}

NITF_BOOL edgeWidthSpecMode(cgm_Metafile* mf, cgm_ParseContext* pc, int classType, int shortCode, char* b, int len, nitf_Error* error)
{
    DBG_TRACE();
    mf->picture->edgeWidthSpec = readShort(b);
    return NITF_SUCCESS;
}

NITF_BOOL vdcExtent(cgm_Metafile* mf, cgm_ParseContext* pc, int classType, int shortCode, char* b, int len, nitf_Error* error)
{
    
    DBG_TRACE();
    assert(len == 8);
    assert(mf->picture);
    
    if (mf->picture->vdcExtent)
    {
	nitf_Error_initf(error,
			 NITF_CTXT,
			 NITF_ERR_INVALID_OBJECT,
			 "VDC was already set");
	return NITF_FAILURE;
	
    }

    mf->picture->vdcExtent = readRectangle(b, len, error);

    return NITF_SUCCESS;
}

NITF_BOOL characterHeight(cgm_Metafile* mf, cgm_ParseContext* pc, int classType, int shortCode, char* b, int len, nitf_Error* error)
{
    if (pc->height != -1)
    {
	nitf_Error_initf(error,
			 NITF_CTXT,
			 NITF_ERR_INVALID_OBJECT,
			 "Height was already set: [%d]",
			 pc->height);
	return NITF_FAILURE;
    }

    pc->height = readShort(b);
    return NITF_SUCCESS;
}

NITF_BOOL interiorStyle(cgm_Metafile* mf, cgm_ParseContext* pc, int classType, int shortCode, char* b, int len, nitf_Error* error)
{
    short s;
	    
    DBG_TRACE();
    s = readShort(b);
    /*  Make sure its not already set  */
    if (pc->style != CGM_IS_NOT_SET)
    {
	nitf_Error_initf(error,
			 NITF_CTXT,
			 NITF_ERR_INVALID_OBJECT,
			 "Interior style was already set: [%d]",
			 pc->style);
	return NITF_FAILURE;
    }

    pc->style = (cgm_InteriorStyle)s;

    return NITF_SUCCESS;
}

NITF_BOOL auxColor(cgm_Metafile* mf, cgm_ParseContext* pc, int classType, int shortCode, char* b, int len, nitf_Error* error)
{
    DBG_TRACE();
    /* TODO: handle multiple pix */
    readRGB(b, len, mf->picture->body->auxColor);

    return NITF_SUCCESS;
}
NITF_BOOL transparency(cgm_Metafile* mf, cgm_ParseContext* pc, int classType, int shortCode, char* b, int len, nitf_Error* error)
{
    DBG_TRACE();
    mf->picture->body->transparency = readShort(b);

    return NITF_SUCCESS;
}
NITF_BOOL polyLine(cgm_Metafile* mf, cgm_ParseContext* pc, int classType, int shortCode, char* b, int len, nitf_Error* error)
{

    cgm_PolyLineElement* poly;
    cgm_Element* elem = cgm_PolyLineElement_construct(error);
    if (!elem) return NITF_FAILURE;
    poly = (cgm_PolyLineElement*) elem->data;

    DBG_TRACE();
    FILL_LINE_ATTS(poly, pc);
    resetParseContext(pc);

    poly->vertices = readVertices(b, len, error);
    if (! poly->vertices )
	return NITF_FAILURE;

    if (!nitf_List_pushBack(mf->picture->body->elements, elem, error))
	return NITF_FAILURE;

    cgm_Element_print(elem);

    return NITF_SUCCESS;
}
NITF_BOOL textElement(cgm_Metafile* mf, cgm_ParseContext* pc, int classType, int shortCode, char* b, int len, nitf_Error* error)
{
    short _1;
    cgm_TextElement* te;
    cgm_Element* elem = cgm_TextElement_construct(error);
    if (!elem) return NITF_FAILURE;
    te = (cgm_TextElement*) elem->data;
    te->text->x = readShort(b);
    te->text->y = readShort(&b[2]);
    _1 = readShort(&b[4]);
    te->text->str = readString(&b[6], len - 6);

    DBG_TRACE();
    //printParseContext(pc);
    memcpy(te->color, pc->color, 6);
    te->characterHeight = pc->height;
    te->textFontIndex = pc->index;
    te->characterOrientation->x1 = pc->orientation[0];
    te->characterOrientation->y1 = pc->orientation[1];
    te->characterOrientation->x2 = pc->orientation[2];
    te->characterOrientation->y2 = pc->orientation[3];

    cgm_Element_print(elem);
    resetParseContext(pc);

    if (!nitf_List_pushBack(mf->picture->body->elements, elem, error))
	return NITF_FAILURE;

    return NITF_SUCCESS;
}
NITF_BOOL polygon(cgm_Metafile* mf, cgm_ParseContext* pc, int classType, int shortCode, char* b, int len, nitf_Error* error)
{
    cgm_PolygonElement* poly;
    cgm_Element* elem = cgm_PolygonElement_construct(error);
    if (!elem) return NITF_FAILURE;
    poly = (cgm_PolygonElement*) elem->data;

    DBG_TRACE();
    FILL_EDGE_ATTS(poly, pc);
    resetParseContext(pc);

    

    poly->vertices = readVertices(b, len, error);
    if (! poly->vertices )
	return NITF_FAILURE;

    if (!nitf_List_pushBack(mf->picture->body->elements, elem, error))
	return NITF_FAILURE;

    cgm_Element_print(elem);

    return NITF_SUCCESS;
}


NITF_BOOL polySet(cgm_Metafile* mf, cgm_ParseContext* pc, int classType, int shortCode, char* b, int len, nitf_Error* error)
{
    DBG_TRACE();
    printParseContext(pc);
    resetParseContext(pc);
    return NITF_SUCCESS;
}
NITF_BOOL rectangleElement(cgm_Metafile* mf, cgm_ParseContext* pc, int classType, int shortCode, char* b, int len, nitf_Error* error)
{
    cgm_RectangleElement* rect;
    cgm_Element* elem = cgm_RectangleElement_construct(error);
    if (!elem) return NITF_FAILURE;
    rect = (cgm_RectangleElement*)elem->data;


    DBG_TRACE();
    printParseContext(pc);
    
    rect->rectangle = readRectangle(b, len, error);
    if (!rect->rectangle)
	return NITF_FAILURE;
    FILL_EDGE_ATTS(rect, pc);

    resetParseContext(pc);
    
    if (!nitf_List_pushBack(mf->picture->body->elements, elem, error))
	return NITF_FAILURE;

    cgm_Element_print(elem);

    return NITF_SUCCESS;
}
NITF_BOOL circle(cgm_Metafile* mf, cgm_ParseContext* pc, int classType, int shortCode, char* b, int len, nitf_Error* error)
{
    cgm_CircleElement* circle;
    cgm_Element* elem = cgm_CircleElement_construct(error);
    if (!elem) return NITF_FAILURE;
    circle = (cgm_CircleElement*)elem->data;

    DBG_TRACE();
    FILL_EDGE_ATTS(circle, pc);
    circle->centerX = readShort(b);
    circle->centerX = readShort(&b[2]);
    circle->radius = readShort(&b[4]);
    cgm_Element_print(elem);

    //printParseContext(pc);
    if (!nitf_List_pushBack(mf->picture->body->elements, elem, error))
	return NITF_FAILURE;


    resetParseContext(pc);
    return NITF_SUCCESS;
}
NITF_BOOL circularArcCenter(cgm_Metafile* mf, cgm_ParseContext* pc, int classType, int shortCode, char* b, int len, nitf_Error* error)
{
    DBG_TRACE();
    //printParseContext(pc);
    resetParseContext(pc);
    return NITF_SUCCESS;
}
NITF_BOOL circularArcCenterClose(cgm_Metafile* mf, cgm_ParseContext* pc, int classType, int shortCode, char* b, int len, nitf_Error* error)
{
    DBG_TRACE();
    //printParseContext(pc);
    resetParseContext(pc);

    return NITF_SUCCESS;
}
NITF_BOOL ellipse(cgm_Metafile* mf, cgm_ParseContext* pc, int classType, int shortCode, char* b, int len, nitf_Error* error)
{
    DBG_TRACE();
    //printParseContext(pc);
    resetParseContext(pc);

    return NITF_SUCCESS;
}
NITF_BOOL ellipticalArcCenter(cgm_Metafile* mf, cgm_ParseContext* pc, int classType, int shortCode, char* b, int len, nitf_Error* error)
{
    DBG_TRACE();
    //printParseContext(pc);
    resetParseContext(pc);

    return NITF_SUCCESS;
}
NITF_BOOL ellipticalArcCenterClose(cgm_Metafile* mf, cgm_ParseContext* pc, int classType, int shortCode, char* b, int len, nitf_Error* error)
{
    DBG_TRACE();

    //printParseContext(pc);
    resetParseContext(pc);

    return NITF_SUCCESS;
}


NITF_BOOL textFontIndex(cgm_Metafile* mf, cgm_ParseContext* pc, int classType, int shortCode, char* b, int len, nitf_Error* error)
{
    DBG_TRACE();
    pc->index = readShort(b);
    return NITF_SUCCESS;
}

NITF_BOOL characterOrient(cgm_Metafile* mf, cgm_ParseContext* pc, int classType, int shortCode, char* b, int len, nitf_Error* error)
{
    DBG_TRACE();
    assert(len == 8);
    pc->orientation[0] = readShort(&b[0]); /* x1, y1, x2, y2 */
    pc->orientation[1] = readShort(&b[2]);
    pc->orientation[2] = readShort(&b[4]);
    pc->orientation[3] = readShort(&b[6]);
   
    return NITF_SUCCESS;
}

NITF_BOOL fillColor(cgm_Metafile* mf, cgm_ParseContext* pc, int classType, int shortCode, char* b, int len, nitf_Error* error)
{
    DBG_TRACE();
    readRGB(b, len, pc->fillColor);
    return NITF_SUCCESS;
}


NITF_BOOL hatchIndex(cgm_Metafile* mf, cgm_ParseContext* pc, int classType, int shortCode, char* b, int len, nitf_Error* error)
{
    short s;
	    
    DBG_TRACE();
    s = readShort(b);
    /*  Make sure its not already set  */
    if (pc->hatchIndex != CGM_HATCH_NOT_SET)
    {
	nitf_Error_initf(error,
			 NITF_CTXT,
			 NITF_ERR_INVALID_OBJECT,
			 "Hatch Index was already set: [%d]",
			 pc->hatchIndex);
	return NITF_FAILURE;
    }

    pc->style= (cgm_HatchType)s;

    return NITF_SUCCESS;
}

NITF_BOOL edgeVisibility(cgm_Metafile* mf, cgm_ParseContext* pc, int classType, int shortCode, char* b, int len, nitf_Error* error)
{
    DBG_TRACE();
    pc->visibility = readShort(b);

    return NITF_SUCCESS;
}

NITF_BOOL readWidth(cgm_Metafile* mf, cgm_ParseContext* pc, int classType, int shortCode, char* b, int len, nitf_Error* error)
{
    DBG_TRACE();
    /*
    if (pc->width != -1)
    {
	nitf_Error_initf(error,
			 NITF_CTXT,
			 NITF_ERR_INVALID_OBJECT,
			 "Width was already set: [%d]",
			 pc->width);
	return NITF_FAILURE;
	}*/

    pc->width = readShort(b);
    return NITF_SUCCESS;
}
NITF_BOOL readType(cgm_Metafile* mf, cgm_ParseContext* pc, int classType, int shortCode, char* b, int len, nitf_Error* error)
{
    DBG_TRACE();
    /*
    if (pc->type != CGM_TYPE_NOT_SET)
    {
	nitf_Error_initf(error,
			 NITF_CTXT,
			 NITF_ERR_INVALID_OBJECT,
			 "Type was already set: [%d]",
			 pc->type);
	return NITF_FAILURE;
	}*/

    pc->type = readShort(b);
    return NITF_SUCCESS;
}
NITF_BOOL readShapeColor(cgm_Metafile* mf, cgm_ParseContext* pc, int classType, int shortCode, char* b, int len, nitf_Error* error)
{
    DBG_TRACE();
    /*
    if (pc->color[CGM_R] != -1)
    {
	nitf_Error_initf(error,
			 NITF_CTXT,
			 NITF_ERR_INVALID_OBJECT,
			 "Color was already set: (%d, %d, %d)",
			 pc->color[CGM_R],
			 pc->color[CGM_G],
			 pc->color[CGM_B]
	    );
	return NITF_FAILURE;
	}*/


    readRGB(b, len, pc->color);
    return NITF_SUCCESS;
}

static cgm_ElementHandler zero[] =
{ 
    { 1, (CGM_UNPACK)&beginMetafile },
    { 2, (CGM_UNPACK)&endMetafile },
    { 3, (CGM_UNPACK)&beginPicture  },
    { 4, (CGM_UNPACK)&beginPictureBody },
    { 5, (CGM_UNPACK)&endPicture },
    { 0, NULL }
};
    
static cgm_ElementHandler one[] =
{
    { 1, (CGM_UNPACK)&metafileVersion },
    { 2, (CGM_UNPACK)&metafileDesc },
    { 11, (CGM_UNPACK)&metafileList },
    { 13, (CGM_UNPACK)&fontList },
    { 0, NULL }
    
};

static cgm_ElementHandler two[] =
{
    { 2, (CGM_UNPACK)&colorSelectionMode },
    { 3, (CGM_UNPACK)&lineWidthSpecMode },
    { 5, (CGM_UNPACK)&edgeWidthSpecMode },
    { 6, (CGM_UNPACK)&vdcExtent },
    { 0, NULL }
    
};

static cgm_ElementHandler three[] =
{
    { 3, (CGM_UNPACK)&auxColor },
    { 3, (CGM_UNPACK)&transparency },
    { 0, NULL }
    
};

static cgm_ElementHandler four[] =
{
    { 1, (CGM_UNPACK)&polyLine },
    { 4, (CGM_UNPACK)&textElement },
    { 7, (CGM_UNPACK)&polygon },
    { 8, (CGM_UNPACK)&polySet },
    { 11, (CGM_UNPACK)&rectangleElement },
    { 12, (CGM_UNPACK)&circle },
    { 15, (CGM_UNPACK)&circularArcCenter },
    { 16, (CGM_UNPACK)&circularArcCenterClose },
    { 17, (CGM_UNPACK)&ellipse },
    { 18, (CGM_UNPACK)&ellipticalArcCenter },
    { 19, (CGM_UNPACK)&ellipticalArcCenterClose },
    { 0, NULL }
    
};

static cgm_ElementHandler five[] =
{
    { 2, (CGM_UNPACK)&readType },
    { 3, (CGM_UNPACK)&readWidth },
    { 4, (CGM_UNPACK)&readShapeColor },
    { 10, (CGM_UNPACK)&textFontIndex },
    { 14, (CGM_UNPACK)&readShapeColor },
    { 15, (CGM_UNPACK)&characterHeight },
    { 16, (CGM_UNPACK)&characterOrient },
    { 22, (CGM_UNPACK)&interiorStyle },
    { 23, (CGM_UNPACK)&fillColor },
    { 24, (CGM_UNPACK)&hatchIndex },
    { 27, (CGM_UNPACK)&readType },
    { 28, (CGM_UNPACK)&readWidth },
    { 29, (CGM_UNPACK)&readShapeColor },
    { 30, (CGM_UNPACK)&edgeVisibility },

    { 0, NULL }
    
};


static cgm_ElementHandler* classes[] = 
{
    zero,
    one,
    two,
    three,
    four,
    five
};

NITFAPI(cgm_MetafileReader*) 
    cgm_MetafileReader_construct(nitf_Error* error)
{
    error->level = NITF_NO_ERR;
    cgm_MetafileReader* reader = 
	(cgm_MetafileReader*)NITF_MALLOC(sizeof(cgm_MetafileReader));

    if (!reader)
    {
        nitf_Error_init(error,
                        NITF_STRERROR(NITF_ERRNO),
                        NITF_CTXT, NITF_ERR_MEMORY);
	return NULL;
    }
    
    reader->unpacker = classes;
    
    return reader;
    
}

NITFPRIV(int) readCGM(cgm_MetafileReader* reader,
		      cgm_Metafile* mf, 
		      cgm_ParseContext* pc,
		      nitf_IOHandle in,
		      nitf_Error* error)
{

    short s;
    int params, i;
    short classType;
    short code = -1;
    char* bytes = NULL;
    cgm_ElementHandler* handlers;
    if (!nitf_IOHandle_read(in, (char*)&s, 2, error))
	goto END_OF_FUNCTION;


    s = NITF_NTOHS(s);
    
    if (s == 0x0040)
	goto END_OF_FUNCTION;
    
    params = 0x0000001F & s;
    classType = 0x000F & (s >> 12);
    code = (0x0FF0 & s) >> 5;
    
    if (params == 31)
    {
	/* We have too many params to fit, so we are overflowing */
	/* into the next short */
	short x;
	if (!nitf_IOHandle_read(in, (char*)&x, 2, error))
	    goto END_OF_FUNCTION;

	x = NITF_NTOHS(x);
	/* This looks like a mistake */
	params = 0x0000FFFFF & x;
	
    }
    if (params % 2)
    {
	/* We need to pad to fall on 2-byte boundaries */
	params++;
    }
    if ( params )
    {
	bytes = (char*)NITF_MALLOC( params );
	if (!bytes)
	{
	    nitf_Error_init(error, 
			    NITF_STRERROR( NITF_ERRNO ),
			    NITF_CTXT,
			    NITF_ERR_MEMORY);
	    goto END_OF_FUNCTION;
	}

	if (!nitf_IOHandle_read(in, bytes, params, error))
	    goto END_OF_FUNCTION;

    }
    
    /*  This will be an ElementHandler[] */
    handlers = reader->unpacker[ classType ];
    for (i = 0; ; i++)
    {
	/* op[i] */
	cgm_ElementHandler handler  = handlers[i];
	/* If this happened, we ran out of codes in class */
	if (handler.code == 0)
	{
	    nitf_Error_initf(error, 
			     NITF_CTXT,
			     NITF_ERR_INVALID_OBJECT,
			     "Couldnt find [%d %d %d]\n", 
			     classType, 
			     code, 
			     params);
	    /*  Now error is set */
	    break;
	}
	if (handler.code == code)
	{
	    if (!(handler.unpack)(mf, pc,
				  classType, code, 
				  bytes, params, error))
		goto END_OF_FUNCTION;

	    //printParseContext(&parseContext);

	    if (bytes)
		free(bytes);
	    return 1;
	}
    }

	    

END_OF_FUNCTION:
    if (bytes)
	free(bytes);
    return 0;

}

NITFAPI(void) 
    cgm_MetafileReader_destruct(cgm_MetafileReader** reader)
{
    /*  If we have a reader, lets blow it away */
    if (*reader)
    {
	NITF_FREE(*reader);
	*reader = NULL;
    }
}

NITFAPI(cgm_Metafile*) cgm_MetafileReader_read(cgm_MetafileReader* reader,
					       nitf_IOHandle in, 
					       nitf_Error* error)
{
    int success;
    /*  Try and alloc our model */
    cgm_Metafile* mf = cgm_Metafile_construct(error);
    if (!mf)
	return NULL;

    cgm_ParseContext parseContext;
    resetParseContext(&parseContext);

    /* Keep going while there is more to read */
    while ( (success = readCGM(reader, mf, &parseContext, in, error)) > 0 );
    /* If we return 0, check if there is an error */
    if ( error->level != NITF_NO_ERR )
    {
	/* If so, lets get rid of the Metafile */
	cgm_Metafile_destruct(&mf);

    }
    /* This works in both cases, mf is NULL if error */
    return mf;
}

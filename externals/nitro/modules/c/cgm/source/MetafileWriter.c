/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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

#include "cgm/MetafileWriter.h"

NITF_BOOL writeRectangle(cgm_Rectangle* r, nitf_IOInterface* io, nitf_Error* error)
{
    short s = NITF_HTONS(r->x1);
    if (!nitf_IOInterface_write(io, (const char*)&s, 2, error))
        return NITF_FAILURE;
    s = NITF_HTONS(r->y1);
    if (!nitf_IOInterface_write(io, (const char*)&s, 2, error))
        return NITF_FAILURE;
    s = NITF_HTONS(r->x2);
    if (!nitf_IOInterface_write(io, (const char*)&s, 2, error))
        return NITF_FAILURE;
    s = NITF_HTONS(r->y2);
    if (!nitf_IOInterface_write(io, (const char*)&s, 2, error))
        return NITF_FAILURE;
    return NITF_SUCCESS;
}

NITFPRIV(NITF_BOOL) writeHeader(short classType, short code, short size,
        nitf_IOInterface* io, short* actual, nitf_Error* error)
{
    short params = size;
    short extendedParams = 0;
    short header = 0;

    *actual = size;

    if (size % 2 != 0)
    {
        /* We have to do padding */
        (*actual)++;
    }

    if (*actual >= 31)
    {
        extendedParams = *actual;
        params = 31;
    }

    header = params & 0x001F;
    header |= classType << 12;
    header |= code << 5;
    header = NITF_HTONS(header);

    if (!nitf_IOInterface_write(io, (const char*)&header, 2, error))
        goto CATCH_ERROR;

    if (extendedParams != 0)
    {
        extendedParams = NITF_HTONS(extendedParams);
        if (!nitf_IOInterface_write(io, (const char*)&extendedParams, 2, error))
            goto CATCH_ERROR;
    }
    return NITF_SUCCESS;

  CATCH_ERROR:
    return NITF_FAILURE;
}

NITFPRIV(NITF_BOOL) writeField(short classType, short code, const char* data,
        short size, nitf_IOInterface* io, nitf_Error* error)
{
    short actual = 0;
    if (!writeHeader(classType, code, size, io, &actual, error))
        return NITF_FAILURE;

    if (size)
    {
        /* Now we want to write the real data */
        if (!nitf_IOInterface_write(io, data, size, error))
            goto CATCH_ERROR;
    }
    if (actual != size)
    {
        char zero = 0;
        if (!nitf_IOInterface_write(io, (const char*)&zero, 1, error))
            goto CATCH_ERROR;
    }

    return NITF_SUCCESS;
  CATCH_ERROR:
    return NITF_FAILURE;

}
NITF_BOOL writeVDC(cgm_Rectangle* r, nitf_IOInterface* io, nitf_Error* error)
{
    short actual;
    if (!writeHeader(2, 6, 8, io, &actual, error))
        return NITF_FAILURE;
    return writeRectangle(r, io, error);
}

static NITF_BOOL nitf_IOInterface_writeRGB(nitf_IOInterface* io, short rgb, nitf_Error* error)
{
    unsigned char c = (unsigned char)rgb;
    return nitf_IOInterface_write(io, (const char*)&c, 1, error);
}
NITF_BOOL writeRGB(cgm_Color* color3, nitf_IOInterface* io, nitf_Error* error)
{
    if (!nitf_IOInterface_writeRGB(io, color3->r, error))
        return NITF_FAILURE;

    if (!nitf_IOInterface_writeRGB(io, color3->g, error))
        return NITF_FAILURE;

    return nitf_IOInterface_writeRGB(io, color3->b, error);
}

NITF_BOOL writeRGBPadded(cgm_Color *color3, nitf_IOInterface* io, nitf_Error* error)
{
    char zero = 0;

    if (!writeRGB(color3, io, error))
        return NITF_FAILURE;

    if (!nitf_IOInterface_write(io, &zero, 1, error))
        return NITF_FAILURE;
    return NITF_SUCCESS;
}

NITF_BOOL colorIsSet(cgm_Color* color3)
{
    return (color3->r != -1 && color3->g != -1 && color3->b != -1);
}

NITF_BOOL rectangleIsSet(cgm_Rectangle* r)
{
    return (r->x1 != -1 || r->x2 != -1 || r->y1 != -1 || r->y2 != -1);
}

NITF_BOOL writeFillAttributes(cgm_FillAttributes* atts, nitf_IOInterface* io,
        nitf_Error* error)
{
    short actual, tmpShort;
    /* Write the fill color if its set */
    if (colorIsSet(atts->fillColor))
    {
        if (!writeHeader(5, 23, 3, io, &actual, error))
            return NITF_FAILURE;

        if (!writeRGBPadded(atts->fillColor, io, error))
            return NITF_FAILURE;
    }
    /* Write the edge color if its set */
    if (colorIsSet(atts->edgeColor))
    {
        if (!writeHeader(5, 29, 3, io, &actual, error))
            return NITF_FAILURE;

        if (!writeRGBPadded(atts->edgeColor, io, error))
            return NITF_FAILURE;
    }

    /* Write the interior style */
    if (atts->interiorStyle != CGM_IS_NOT_SET)
    {
        tmpShort = NITF_HTONS(atts->interiorStyle);
        if (!writeField(5, 22, (const char*)&tmpShort, 2, io, error))
            return NITF_FAILURE;
    }
    /* Write the edge vis */
    if (atts->edgeVisibility != -1)
    {
        tmpShort = NITF_HTONS(atts->edgeVisibility);
        if (!writeField(5, 30, (const char*)&tmpShort, 2, io, error))
            return NITF_FAILURE;
    }
    /* Write the edge width */
    if (atts->edgeWidth != -1)
    {
        tmpShort = NITF_HTONS(atts->edgeWidth);
        if (!writeField(5, 28, (const char*)&tmpShort, 2, io, error))
            return NITF_FAILURE;
    }
    /* Write the edge type */
    if (atts->edgeType != CGM_TYPE_NOT_SET)
    {
        tmpShort = NITF_HTONS(atts->edgeType);
        if (!writeField(5, 27, (const char*)&tmpShort, 2, io, error))
            return NITF_FAILURE;
    }
    return NITF_SUCCESS;
}

NITF_BOOL writeLineAttributes(cgm_LineAttributes* atts, nitf_IOInterface* io,
        nitf_Error* error)
{
    short actual, tmpShort;
    /* Write the fill color if its set */
    if (colorIsSet(atts->lineColor))
    {
        if (!writeHeader(5, 4, 3, io, &actual, error))
            return NITF_FAILURE;

        if (!writeRGBPadded(atts->lineColor, io, error))
            return NITF_FAILURE;
    }
    /* Write the edge width */
    if (atts->lineWidth != -1)
    {
        tmpShort = NITF_HTONS(atts->lineWidth);
        if (!writeField(5, 3, (const char*)&tmpShort, 2, io, error))
            return NITF_FAILURE;
    }
    /* Write the edge type */
    if (atts->lineType != CGM_TYPE_NOT_SET)
    {
        tmpShort = NITF_HTONS(atts->lineType);
        if (!writeField(5, 2, (const char*)&tmpShort, 2, io, error))
            return NITF_FAILURE;
    }
    return NITF_SUCCESS;
}

NITF_BOOL writeTextAttributes(cgm_TextAttributes* atts, nitf_IOInterface* io,
        nitf_Error* error)
{
    short actual, tmpShort;
    /* Write the fill color if its set */
    if (colorIsSet(atts->textColor))
    {
        if (!writeHeader(5, 14, 3, io, &actual, error))
            return NITF_FAILURE;

        if (!writeRGBPadded(atts->textColor, io, error))
            return NITF_FAILURE;
    }
    /* Write the edge width */
    if (atts->characterHeight != -1)
    {
        tmpShort = NITF_HTONS(atts->characterHeight);
        if (!writeField(5, 15, (const char*)&tmpShort, 2, io, error))
            return NITF_FAILURE;
    }
    /* Write the edge width */
    if (atts->textFontIndex != -1)
    {
        tmpShort = NITF_HTONS(atts->textFontIndex);
        if (!writeField(5, 10, (const char*)&tmpShort, 2, io, error))
            return NITF_FAILURE;
    }

    /* Write the edge type */
    if (rectangleIsSet(atts->characterOrientation))
    {
        if (!writeHeader(5, 16, 8, io, &actual, error))
            return NITF_FAILURE;

        if (!writeRectangle(atts->characterOrientation, io, error))
            return NITF_FAILURE;
    }
    return NITF_SUCCESS;
}

NITF_BOOL writeElements(cgm_MetafileWriter* writer, nitf_List* elements,
        nitf_IOInterface* io, nitf_Error* error)
{
    nitf_ListIterator it = nitf_List_begin(elements);
    nitf_ListIterator end = nitf_List_end(elements);
    /*    short dataLen = 0;
     short actual = 0;*/
    while (nitf_ListIterator_notEqualTo(&it, &end))
    {
        cgm_Element *elem = (cgm_Element*)nitf_ListIterator_get(&it);
        if (elem->type >= CGM_ELEMENT_RANGE || elem->type < 0)
        {
            nitf_Error_initf(error, NITF_CTXT, NITF_ERR_INVALID_OBJECT,
                    "Invalid element type [%d] encountered", elem->type);
            return NITF_FAILURE;
        }
        
        if (!(*(writer->packers[ elem->type ]))(elem, io, error))
            return NITF_FAILURE;

        nitf_ListIterator_increment(&it);
    }

    return NITF_SUCCESS;
}

NITF_BOOL writeBody(cgm_MetafileWriter* writer, cgm_PictureBody* body,
        nitf_IOInterface* io, nitf_Error* error)
{
    short actual;

    /* Write picture body */
    if (!writeHeader(0, 4, 0, io, &actual, error))
        return NITF_FAILURE;

    /* Transparency */
    short tmpShort = NITF_HTONS((uint16_t)body->transparency);
    if (!writeField(3, 4, (const char*)&tmpShort, 2, io, error))
        return NITF_FAILURE;

    if (colorIsSet(body->auxColor))
    {
        if (!writeHeader(3, 3, 3, io, &actual, error))
            return NITF_FAILURE;
        if (!writeRGBPadded(body->auxColor, io, error))
            return NITF_FAILURE;
    }

    return writeElements(writer, body->elements, io, error);
}

NITF_BOOL writePicture(cgm_MetafileWriter* writer, cgm_Picture* picture,
        nitf_IOInterface* io, nitf_Error* error)
{
    short actual, tmpShort;
    /* Begin picture */
    if (!writeField(0, 3, picture->name, nrt_strlen16(picture->name), io, error))
        return NITF_FAILURE;

    /* Color selection mode */
    tmpShort = NITF_HTONS(picture->colorSelectionMode);
    if (!writeField(2, 2, (const char*)&tmpShort, 2, io, error))
        return NITF_FAILURE;

    /* [Edge width spec mode] */
    tmpShort = NITF_HTONS(picture->edgeWidthSpec);
    if (!writeField(2, 5, (const char*)&tmpShort, 2, io, error))
        return NITF_FAILURE;

    /* [Line width spec mode] */
    tmpShort = NITF_HTONS(picture->lineWidthSpec);
    if (!writeField(2, 3, (const char*)&tmpShort, 2, io, error))
        return NITF_FAILURE;

    /* VDC */
    if (!writeVDC(picture->vdcExtent, io, error))
        return NITF_FAILURE;

    /* Write picture body */
    if (!writeBody(writer, picture->body, io, error))
        return NITF_FAILURE;

    /* End picture */
    return writeHeader(0, 5, 0, io, &actual, error);
}

NITF_BOOL writeFontList(nitf_List* fontList, nitf_IOInterface* io, nitf_Error* error)
{
    nitf_ListIterator it = nitf_List_begin(fontList);
    nitf_ListIterator end = nitf_List_end(fontList);
    short dataLen = 0;
    short actual = 0;
    
    while (nitf_ListIterator_notEqualTo(&it, &end))
    {
        char *data = (char*)nitf_ListIterator_get(&it);
        /* The extra byte is for the length each time */
        dataLen += nrt_strlen16(data) + 1;
        nitf_ListIterator_increment(&it);
    }
    
    if (!writeHeader(1, 13, dataLen, io, &actual, error))
        return NITF_FAILURE;

    it = nitf_List_begin(fontList);
    dataLen = 0;
    while (nitf_ListIterator_notEqualTo(&it, &end))
    {
        char *data = (char*)nitf_ListIterator_get(&it);
        unsigned char len = nrt_strlen8(data);
        if (!nitf_IOInterface_write(io, (const char*)&len, 1, error))
            return NITF_FAILURE;

        if (!nitf_IOInterface_write(io, (const char*)data, len, error))
            return NITF_FAILURE;

        dataLen += (1 + len);
        nitf_ListIterator_increment(&it);
    }

    if (actual != dataLen)
    {
        char zero = 0;
        if (!nitf_IOInterface_write(io, (const char*)&zero, 1, error))
            return NITF_FAILURE;

    }
    return NITF_SUCCESS;
}

NITFPRIV(NITF_BOOL) writeMetafileInfo(cgm_MetafileWriter* writer,
        cgm_Metafile* mf,
        nitf_IOInterface* io,
        nitf_Error* error)
{
    short reversed[3];
    short actual;
    
    if (!mf->name || strlen(mf->name) <= 0)
    {
        nitf_Error_init(error,
                        "Metafile name must exist and have length > 0",
                        NITF_CTXT, NITF_ERR_INVALID_OBJECT);
        return NITF_FAILURE;
    }
        
    /* Begin Metafile */
    if (!writeField(0, 1, mf->name, nrt_strlen16(mf->name), io, error))
        return NITF_FAILURE;

    /* Write version */
    actual = NITF_HTONS(mf->version);
    if (!writeField(1, 1, (const char*)&actual, 2, io, error))
        return NITF_FAILURE;
    
    /* Metafile element list */
    reversed[0] = NITF_HTONS(mf->elementList[0]);
    reversed[1] = NITF_HTONS(mf->elementList[1]);
    reversed[2] = NITF_HTONS(mf->elementList[2]);

    /* Element list */
    if (!writeField(1, 11, (const char*)reversed, 6, io, error))
        return NITF_FAILURE;

    /* Metafile description */
    if (!writeField(1, 2, mf->description, nrt_strlen16(mf->description), io, error))
        return NITF_FAILURE;

    if (mf->fontList != NULL)
    {
        if (!writeFontList(mf->fontList, io, error))
            return NITF_FAILURE;
    }
    if (!writePicture(writer, mf->picture, io, error))
        return NITF_FAILURE;

    /* Write metafile end */
    return writeHeader(0, 2, 0, io, &actual, error);
}

NITFAPI(NITF_BOOL) cgm_MetafileWriter_write(cgm_MetafileWriter* writer,
					    cgm_Metafile* mf,
					    nitf_IOInterface* io,
					    nitf_Error* error)
{
    return writeMetafileInfo(writer, mf, io, error);
}


NITF_BOOL writeText(cgm_Element* element, nitf_IOInterface* io, nitf_Error* error)
{
    short s;
    short actual;
    uint8_t slen;
    cgm_TextElement* textElement = (cgm_TextElement*)element->data;
    cgm_Text* text = textElement->text;
    
    if (textElement->attributes)
    {
        if (!writeTextAttributes(textElement->attributes, io, error))
            return NITF_FAILURE;
    }
    
    /* the text must exist! */
    if (!text || !text->str)
    {
        nitf_Error_initf(error, NITF_CTXT, NITF_ERR_INVALID_OBJECT,
                         "Text element must contain text data");
        return NITF_FAILURE;
    }

    /* Text length + x + y */
    s = nrt_strlen16(text->str) + 1 + 6;
    if (!writeHeader(4, 4, s, io, &actual, error))
        return NITF_FAILURE;

    s = NITF_HTONS(text->x);
    if (!nitf_IOInterface_write(io, (const char*)&s, 2, error))
        return NITF_FAILURE;

    s = NITF_HTONS(text->y);
    if (!nitf_IOInterface_write(io, (const char*)&s, 2, error))
        return NITF_FAILURE;

    s = NITF_HTONS(1);
    if (!nitf_IOInterface_write(io, (const char*)&s, 2, error))
        return NITF_FAILURE;

    slen = nrt_strlen8(text->str);
    if (!nitf_IOInterface_write(io, (const char*)&slen, 1, error))
        return NITF_FAILURE;

    /* Trick since we know it zero padded in memory */
    const size_t actual_ = actual >= 7 ? actual - 7 : 0;
    if (!nitf_IOInterface_write(io, (const char*)text->str, actual_, error))
        return NITF_FAILURE;

    return NITF_SUCCESS;
}
NITF_BOOL writePolygon(cgm_Element* element, nitf_IOInterface* io, nitf_Error* error)
{
    cgm_PolygonElement* polyElement = (cgm_PolygonElement*)element->data;
    nitf_ListIterator it = nitf_List_begin(polyElement->vertices);
    nitf_ListIterator end = nitf_List_end(polyElement->vertices);
    short actual;
    uint16_t size = nitf_List_size16(polyElement->vertices);
    size *= 4;

    if (polyElement->attributes)
    {
        if (!writeFillAttributes(polyElement->attributes, io, error))
            return NITF_FAILURE;
    }

    if (!writeHeader(4, 7, size, io, &actual, error))
        return NITF_FAILURE;

    while (nitf_ListIterator_notEqualTo(&it, &end))
    {
        short s;
        cgm_Vertex* v = (cgm_Vertex*)nitf_ListIterator_get(&it);
        s = NITF_HTONS(v->x);

        if (!nitf_IOInterface_write(io, (const char*)&s, 2, error))
            return NITF_FAILURE;

        s = NITF_HTONS(v->y);

        if (!nitf_IOInterface_write(io, (const char*)&s, 2, error))
            return NITF_FAILURE;

        nitf_ListIterator_increment(&it);
    }

    return NITF_SUCCESS;
}

NITF_BOOL writePolySet(cgm_Element* element, nitf_IOInterface* io, nitf_Error* error)
{
    cgm_PolySetElement* polyElement = (cgm_PolySetElement*)element->data;
    nitf_ListIterator it = nitf_List_begin(polyElement->vertices);
    nitf_ListIterator end = nitf_List_end(polyElement->vertices);
    short actual;
    uint16_t size = nitf_List_size16(polyElement->vertices);
    size *= 6;

    if (polyElement->attributes)
    {
        if (!writeFillAttributes(polyElement->attributes, io, error))
            return NITF_FAILURE;
    }

    if (!writeHeader(4, 8, size, io, &actual, error))
        return NITF_FAILURE;

    while (nitf_ListIterator_notEqualTo(&it, &end))
    {
        short s;
        cgm_VertexClose* v = (cgm_VertexClose*)nitf_ListIterator_get(&it);
        s = NITF_HTONS(v->x);

        if (!nitf_IOInterface_write(io, (const char*)&s, 2, error))
            return NITF_FAILURE;

        s = NITF_HTONS(v->y);

        if (!nitf_IOInterface_write(io, (const char*)&s, 2, error))
            return NITF_FAILURE;

        if (v->edgeOutFlag == CGM_EDGE_CLOSE_TYPE_NOT_SET)
        {
            nitf_Error_initf(error, NITF_CTXT, NITF_ERR_INVALID_OBJECT,
                    "Edge out flags not set in polygon set");
            return NITF_FAILURE;

        }
        s = NITF_HTONS(v->edgeOutFlag);

        if (!nitf_IOInterface_write(io, (const char*)&s, 2, error))
            return NITF_FAILURE;

        nitf_ListIterator_increment(&it);
    }

    return NITF_SUCCESS;
}
NITF_BOOL writePolyLine(cgm_Element* element, nitf_IOInterface* io,
        nitf_Error* error)
{
    cgm_PolyLineElement* polyElement = (cgm_PolyLineElement*)element->data;
    nitf_ListIterator it = nitf_List_begin(polyElement->vertices);
    nitf_ListIterator end = nitf_List_end(polyElement->vertices);
    short actual;
    uint16_t size = nitf_List_size16(polyElement->vertices);
    size *= 4;

    if (polyElement->attributes)
    {
        if (!writeLineAttributes(polyElement->attributes, io, error))
            return NITF_FAILURE;
    }

    if (!writeHeader(4, 1, size, io, &actual, error))
        return NITF_FAILURE;

    while (nitf_ListIterator_notEqualTo(&it, &end))
    {
        short s;
        cgm_Vertex* v = (cgm_Vertex*)nitf_ListIterator_get(&it);
        s = NITF_HTONS(v->x);

        if (!nitf_IOInterface_write(io, (const char*)&s, 2, error))
            return NITF_FAILURE;

        s = NITF_HTONS(v->y);

        if (!nitf_IOInterface_write(io, (const char*)&s, 2, error))
            return NITF_FAILURE;

        nitf_ListIterator_increment(&it);
    }

    return NITF_SUCCESS;
}

NITF_BOOL writeEllipse(cgm_Element* element, nitf_IOInterface* io, nitf_Error* error)
{
    short s;
    short actual;
    cgm_EllipseElement* ellipse = (cgm_EllipseElement*)element->data;

    if (ellipse->attributes)
    {
        if (!writeFillAttributes(ellipse->attributes, io, error))
            return NITF_FAILURE;
    }

    if (!writeHeader(4, 17, 12, io, &actual, error))
        return NITF_FAILURE;

    s = NITF_HTONS(ellipse->centerX);
    if (!nitf_IOInterface_write(io, (const char*)&s, 2, error))
        return NITF_FAILURE;

    s = NITF_HTONS(ellipse->centerY);
    if (!nitf_IOInterface_write(io, (const char*)&s, 2, error))
        return NITF_FAILURE;

    s = NITF_HTONS(ellipse->end1X);
    if (!nitf_IOInterface_write(io, (const char*)&s, 2, error))
        return NITF_FAILURE;
    s = NITF_HTONS(ellipse->end1Y);
    if (!nitf_IOInterface_write(io, (const char*)&s, 2, error))
        return NITF_FAILURE;
    s = NITF_HTONS(ellipse->end2X);
    if (!nitf_IOInterface_write(io, (const char*)&s, 2, error))
        return NITF_FAILURE;
    s = NITF_HTONS(ellipse->end2Y);
    if (!nitf_IOInterface_write(io, (const char*)&s, 2, error))
        return NITF_FAILURE;

    return NITF_SUCCESS;
}

NITF_BOOL writeEllipticalArcCenter(cgm_Element* element, nitf_IOInterface* io,
        nitf_Error* error)
{
    short s;
    short actual;
    cgm_EllipticalArcElement* arc = (cgm_EllipticalArcElement*)element->data;

    if (arc->attributes)
    {
        if (!writeLineAttributes(arc->attributes, io, error))
            return NITF_FAILURE;
    }

    if (!writeHeader(4, 18, 20, io, &actual, error))
        return NITF_FAILURE;

    s = NITF_HTONS(arc->centerX);
    if (!nitf_IOInterface_write(io, (const char*)&s, 2, error))
        return NITF_FAILURE;

    s = NITF_HTONS(arc->centerY);
    if (!nitf_IOInterface_write(io, (const char*)&s, 2, error))
        return NITF_FAILURE;

    s = NITF_HTONS(arc->end1X);
    if (!nitf_IOInterface_write(io, (const char*)&s, 2, error))
        return NITF_FAILURE;
    s = NITF_HTONS(arc->end1Y);
    if (!nitf_IOInterface_write(io, (const char*)&s, 2, error))
        return NITF_FAILURE;
    s = NITF_HTONS(arc->end2X);
    if (!nitf_IOInterface_write(io, (const char*)&s, 2, error))
        return NITF_FAILURE;
    s = NITF_HTONS(arc->end2Y);
    if (!nitf_IOInterface_write(io, (const char*)&s, 2, error))
        return NITF_FAILURE;

    s = NITF_HTONS(arc->startVectorY);
    if (!nitf_IOInterface_write(io, (const char*)&s, 2, error))
        return NITF_FAILURE;
    s = NITF_HTONS(arc->startVectorY);
    if (!nitf_IOInterface_write(io, (const char*)&s, 2, error))
        return NITF_FAILURE;
    s = NITF_HTONS(arc->endVectorX);
    if (!nitf_IOInterface_write(io, (const char*)&s, 2, error))
        return NITF_FAILURE;
    s = NITF_HTONS(arc->endVectorY);
    if (!nitf_IOInterface_write(io, (const char*)&s, 2, error))
        return NITF_FAILURE;

    return NITF_SUCCESS;
}
NITF_BOOL writeEllipticalArcCenterClose(cgm_Element* element, nitf_IOInterface* io,
        nitf_Error* error)
{
    short s;
    short actual;
    cgm_EllipticalArcCloseElement* arc =
            (cgm_EllipticalArcCloseElement*)element->data;

    if (arc->attributes)
    {
        if (!writeFillAttributes(arc->attributes, io, error))
            return NITF_FAILURE;
    }

    if (!writeHeader(4, 19, 22, io, &actual, error))
        return NITF_FAILURE;

    s = NITF_HTONS(arc->centerX);
    if (!nitf_IOInterface_write(io, (const char*)&s, 2, error))
        return NITF_FAILURE;

    s = NITF_HTONS(arc->centerY);
    if (!nitf_IOInterface_write(io, (const char*)&s, 2, error))
        return NITF_FAILURE;

    s = NITF_HTONS(arc->end1X);
    if (!nitf_IOInterface_write(io, (const char*)&s, 2, error))
        return NITF_FAILURE;
    s = NITF_HTONS(arc->end1Y);
    if (!nitf_IOInterface_write(io, (const char*)&s, 2, error))
        return NITF_FAILURE;
    s = NITF_HTONS(arc->end2X);
    if (!nitf_IOInterface_write(io, (const char*)&s, 2, error))
        return NITF_FAILURE;
    s = NITF_HTONS(arc->end2Y);
    if (!nitf_IOInterface_write(io, (const char*)&s, 2, error))
        return NITF_FAILURE;

    s = NITF_HTONS(arc->startVectorY);
    if (!nitf_IOInterface_write(io, (const char*)&s, 2, error))
        return NITF_FAILURE;
    s = NITF_HTONS(arc->startVectorY);
    if (!nitf_IOInterface_write(io, (const char*)&s, 2, error))
        return NITF_FAILURE;
    s = NITF_HTONS(arc->endVectorX);
    if (!nitf_IOInterface_write(io, (const char*)&s, 2, error))
        return NITF_FAILURE;
    s = NITF_HTONS(arc->endVectorY);
    if (!nitf_IOInterface_write(io, (const char*)&s, 2, error))
        return NITF_FAILURE;

    s = NITF_HTONS(arc->closeType);
    if (!nitf_IOInterface_write(io, (const char*)&s, 2, error))
        return NITF_FAILURE;

    return NITF_SUCCESS;

}
NITF_BOOL writeRectangleElement(cgm_Element* element, nitf_IOInterface* io,
        nitf_Error* error)
{
    short actual;
    cgm_RectangleElement* r = (cgm_RectangleElement*)element->data;

    if (r->attributes)
    {
        if (!writeFillAttributes(r->attributes, io, error))
            return NITF_FAILURE;
    }

    if (!writeHeader(4, 11, 8, io, &actual, error))
        return NITF_FAILURE;

    if (!r->rectangle)
    {
        nitf_Error_initf(error, NITF_CTXT, NITF_ERR_INVALID_OBJECT,
                         "Rectangle element must contain rectangle data");
        return NITF_FAILURE;
    }
    
    return writeRectangle(r->rectangle, io, error);
}
NITF_BOOL writeCircle(cgm_Element* element, nitf_IOInterface* io, nitf_Error* error)
{
    short s;
    short actual;
    cgm_CircleElement* circle = (cgm_CircleElement*)element->data;

    if (circle->attributes)
    {
        if (!writeFillAttributes(circle->attributes, io, error))
            return NITF_FAILURE;
    }

    if (!writeHeader(4, 12, 6, io, &actual, error))
        return NITF_FAILURE;

    s = NITF_HTONS(circle->centerX);
    if (!nitf_IOInterface_write(io, (const char*)&s, 2, error))
        return NITF_FAILURE;

    s = NITF_HTONS(circle->centerY);
    if (!nitf_IOInterface_write(io, (const char*)&s, 2, error))
        return NITF_FAILURE;

    s = NITF_HTONS(circle->radius);
    if (!nitf_IOInterface_write(io, (const char*)&s, 2, error))
        return NITF_FAILURE;

    return NITF_SUCCESS;
}
NITF_BOOL writeCircularArcCenter(cgm_Element* element, nitf_IOInterface* io,
        nitf_Error* error)
{
    short s;
    short actual;
    cgm_CircularArcElement* arc = (cgm_CircularArcElement*)element->data;

    if (arc->attributes)
    {
        if (!writeLineAttributes(arc->attributes, io, error))
            return NITF_FAILURE;
    }

    if (!writeHeader(4, 18, 20, io, &actual, error))
        return NITF_FAILURE;

    s = NITF_HTONS(arc->centerX);
    if (!nitf_IOInterface_write(io, (const char*)&s, 2, error))
        return NITF_FAILURE;

    s = NITF_HTONS(arc->centerY);
    if (!nitf_IOInterface_write(io, (const char*)&s, 2, error))
        return NITF_FAILURE;

    s = NITF_HTONS(arc->startX);
    if (!nitf_IOInterface_write(io, (const char*)&s, 2, error))
        return NITF_FAILURE;
    s = NITF_HTONS(arc->startY);
    if (!nitf_IOInterface_write(io, (const char*)&s, 2, error))
        return NITF_FAILURE;
    s = NITF_HTONS(arc->endX);
    if (!nitf_IOInterface_write(io, (const char*)&s, 2, error))
        return NITF_FAILURE;
    s = NITF_HTONS(arc->endY);
    if (!nitf_IOInterface_write(io, (const char*)&s, 2, error))
        return NITF_FAILURE;

    s = NITF_HTONS(arc->radius);
    if (!nitf_IOInterface_write(io, (const char*)&s, 2, error))
        return NITF_FAILURE;

    return NITF_SUCCESS;
}
NITF_BOOL writeCircularArcCenterClose(cgm_Element* element, nitf_IOInterface* io,
        nitf_Error* error)
{
    short s;
    cgm_CircularArcCloseElement* arc =
            (cgm_CircularArcCloseElement*)element->data;
    
    if (arc->attributes)
    {
        if (!writeFillAttributes(arc->attributes, io, error))
            return NITF_FAILURE;
    }

    s = NITF_HTONS(arc->centerX);
    if (!nitf_IOInterface_write(io, (const char*)&s, 2, error))
        return NITF_FAILURE;

    s = NITF_HTONS(arc->centerY);
    if (!nitf_IOInterface_write(io, (const char*)&s, 2, error))
        return NITF_FAILURE;

    s = NITF_HTONS(arc->startX);
    if (!nitf_IOInterface_write(io, (const char*)&s, 2, error))
        return NITF_FAILURE;

    s = NITF_HTONS(arc->startY);
    if (!nitf_IOInterface_write(io, (const char*)&s, 2, error))
        return NITF_FAILURE;

    s = NITF_HTONS(arc->endX);
    if (!nitf_IOInterface_write(io, (const char*)&s, 2, error))
        return NITF_FAILURE;

    s = NITF_HTONS(arc->endY);
    if (!nitf_IOInterface_write(io, (const char*)&s, 2, error))
        return NITF_FAILURE;

    s = NITF_HTONS(arc->radius);
    if (!nitf_IOInterface_write(io, (const char*)&s, 2, error))
        return NITF_FAILURE;

    s = NITF_HTONS(arc->closeType);
    if (!nitf_IOInterface_write(io, (const char*)&s, 2, error))
        return NITF_FAILURE;

    return NITF_SUCCESS;
}

static CGM_PACK elements[] =
{
    &writeText,
    &writePolygon,
    &writePolyLine,
    &writePolySet,
    &writeEllipse,
    &writeEllipticalArcCenter,
    &writeEllipticalArcCenterClose,
    &writeRectangleElement,
    &writeCircle,
    &writeCircularArcCenter,
    &writeCircularArcCenterClose
};

NITFAPI(cgm_MetafileWriter*) cgm_MetafileWriter_construct(nitf_Error* error)
{
    cgm_MetafileWriter* writer =
    (cgm_MetafileWriter*)NITF_MALLOC(sizeof(cgm_MetafileWriter));

    if (!writer)
    {
        nitf_Error_init(error,
                NITF_STRERROR(NITF_ERRNO),
                NITF_CTXT, NITF_ERR_MEMORY);
        return NULL;
    }

    writer->packers = elements;

    return writer;
}

NITFAPI(void) cgm_MetafileWriter_destruct(cgm_MetafileWriter** writer)
{
    /*  If we have a reader, lets blow it away */
    if (*writer)
    {
        NITF_FREE(*writer);
        *writer = NULL;
    }
}

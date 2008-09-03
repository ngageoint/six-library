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

#include "cgm/MetafileWriter.h"

static cgm_ElementWriter elements[] =
{
/*
    { CGM_TEXT_ELEMENT, (CGM_PACK)&writeText },
    { CGM_POLYGON_ELEMENT, (CGM_PACK)&writePolygon },
    { CGM_POLYSET_ELEMENT, (CGM_PACK)&writePolySet },
    { CGM_POLYLINE_ELEMENT, (CGM_PACK)&writePolyLine },
    { CGM_ELLIPSE_ELEMENT, (CGM_PACK)&writeEllipse },
    { CGM_ELLIPTICAL_ARC_CENTER_ELEMENT, (CGM_PACK)&writeEllipticalArcCenter },
    { CGM_ELLIPTICAL_ARC_CENTER_CLOSE_ELEMENT, (CGM_PACK)&writeEllipticalArcCenterClose },
    { CGM_RECTANGLE_ELEMENT, (CGM_PACK)&writeRectangle },
    { CGM_CIRCLE_ELEMENT, (CGM_PACK)&writeCircle },
    { CGM_CIRCULAR_ARC_CENTER_ELEMENT, (CGM_PACK)&writeCircularArcCenter },
    { CGM_CIRCULAR_ARC_CENTER_CLOSE_ELEMENT, (CGM_PACK)&writeCircularArcCenterClose },
    
*/
    { 0, NULL }
};

NITFAPI(cgm_MetafileWriter*) 
    cgm_MetafileWriter_construct(nitf_Error* error)
{
    cgm_MetafileWriter* reader = 
	(cgm_MetafileWriter*)NITF_MALLOC(sizeof(cgm_MetafileWriter));
    
    if (!reader)
    {
        nitf_Error_init(error,
                        NITF_STRERROR(NITF_ERRNO),
                        NITF_CTXT, NITF_ERR_MEMORY);
	return NULL;
    }
    
    reader->packer = elements;
    
    return reader;
    
}

NITFAPI(void) 
    cgm_MetafileWriter_destruct(cgm_MetafileWriter** writer)
{
    /*  If we have a reader, lets blow it away */
    if (*writer)
    {
	NITF_FREE(*writer);
	*writer = NULL;
    }
}

NITF_BOOL writeRectangle(cgm_Rectangle* r, nitf_IOHandle io, nitf_Error* error)
{
    NITF_BOOL rv;
    short s = NITF_HTONS(r->x1);
    rv = nitf_IOHandle_write(io, (const char*)&s, 2, error);
    if (!rv) return NITF_FAILURE;
    s = NITF_HTONS(r->y1);
    rv = nitf_IOHandle_write(io, (const char*)&s, 2, error);
    if (!rv) return NITF_FAILURE;
    s = NITF_HTONS(r->x2);
    rv = nitf_IOHandle_write(io, (const char*)&s, 2, error);
    if (!rv) return NITF_FAILURE;
    s = NITF_HTONS(r->y2);
    rv = nitf_IOHandle_write(io, (const char*)&s, 2, error);
    return rv;
}



NITFPRIV(NITF_BOOL) writeHeader(short classType, short code, short size, nitf_IOHandle io, short* actual, nitf_Error* error)
{
    *actual = size;
    short extendedParams = 0;
    short params = 0;
    short header = 0;
    if (size % 2 != 0)
        /* We have to do padding */
        (*actual)++;

    params = *actual;


    if (*actual >= 31)
    {
        extendedParams = *actual;
        params = 31;
    }
    
    header = params & 0x001F;
    header |= classType << 12;
    header |= code << 5;
    header = NITF_HTONS(header);
    
    if (!nitf_IOHandle_write(io, (const char*)&header, 2, error))
        goto CATCH_ERROR;

    if (extendedParams != 0)
    {
        extendedParams = NITF_HTONS(extendedParams);
        if (!nitf_IOHandle_write(io, (const char*)&extendedParams, 2, error))
            goto CATCH_ERROR;
    }
    return NITF_SUCCESS;

CATCH_ERROR:
    return NITF_FAILURE;
    
}


NITFPRIV(NITF_BOOL) writeField(short classType, short code, const char* data, short size, nitf_IOHandle io, nitf_Error* error)
{
    short actual = 0;
    if (!writeHeader(classType, code, size, io, &actual, error))
        return NITF_FAILURE;

    if (size)
    {
        /* Now we want to write the real data */
        if (!nitf_IOHandle_write(io, data, size, error))
            goto CATCH_ERROR;
    }
    if (actual != size)
    {
        char zero = 0;
        if (!nitf_IOHandle_write(io, (const char*)&zero, 1, error))
            goto CATCH_ERROR;
    }

    return NITF_SUCCESS;
CATCH_ERROR:
    return NITF_FAILURE;
    
}
NITF_BOOL writeVDC(cgm_Rectangle* r, nitf_IOHandle io, nitf_Error* error)
{
    short actual;
    NITF_BOOL rv;
    rv = writeHeader(2, 6, 8, io, &actual, error);
    if (!rv) return NITF_FAILURE;
    return writeRectangle(r, io, error);
}

NITF_BOOL writeRGB(short *color3, nitf_IOHandle io, nitf_Error* error)
{
    NITF_BOOL rv;
    short s = NITF_HTONS(color3[0]);
    rv = nitf_IOHandle_write(io, (const char*)&s, 2, error);
    if (!rv) return NITF_FAILURE;

    s = NITF_NTOHS(color3[1]);
    rv = nitf_IOHandle_write(io, (const char*)&s, 2, error);
    if (!rv) return NITF_FAILURE;

    s = NITF_NTOHS(color3[2]);
    return nitf_IOHandle_write(io, (const char*)&s, 2, error);

}

NITF_BOOL writeRGBPadded(short *color3, nitf_IOHandle io, nitf_Error* error)
{
    char zero = 0;

    if (!writeRGB(color3, io, error))
        return NITF_FAILURE;

    if (!nitf_IOHandle_write(io, &zero, 2, error))
        return NITF_FAILURE;
    return NITF_SUCCESS;
}

NITF_BOOL colorIsSet(short* color3)
{
    return (color3[0] != -1 &&
            color3[1] != -1 &&
            color3[2] != -1);
}


NITF_BOOL rectangleIsSet(cgm_Rectangle* r)
{
    return (r->x1 != -1 && r->x2 != -1 && r->y1 != -1 && r->y2 != -1);
    
}

NITF_BOOL writeFillAttributes(cgm_FillAttributes* atts, nitf_IOHandle io, nitf_Error* error)
{
    short actual;
    NITF_BOOL rv;
    /* Write the fill color if its set */
    if (colorIsSet(atts->fillColor))
    {
        rv = writeHeader(5, 23, 3, io,&actual, error);
        if (!rv) return NITF_FAILURE;

        rv = writeRGBPadded(atts->fillColor, io, error);
        if (!rv) return NITF_FAILURE;
    }
    /* Write the edge color if its set */
    if (colorIsSet(atts->edgeColor))
    {
        rv = writeHeader(5, 29, 3, io,&actual, error);
        if (!rv) return NITF_FAILURE;
        
        rv = writeRGBPadded(atts->edgeColor, io, error);
        if (!rv) return NITF_FAILURE;
    }

    /* Write the interior style */
    if (atts->interiorStyle != CGM_IS_NOT_SET)
    {
        rv = writeField(5, 22, (const char*)&(atts->interiorStyle), 2, io, error);
        if (!rv) return NITF_FAILURE;
    }
    /* Write the edge vis */
    if (atts->edgeVisibility != -1)
    {
        rv = writeField(5, 30, (const char*)&(atts->edgeVisibility), 2, io, error);
        if (!rv) return NITF_FAILURE;
    }
    /* Write the edge width */
    if (atts->edgeWidth != -1)
    {
        rv = writeField(5, 28, (const char*)&(atts->edgeWidth), 2, io, error);
        if (!rv) return NITF_FAILURE;
    }
    /* Write the edge type */
    if (atts->edgeType != CGM_TYPE_NOT_SET)
    {
        rv = writeField(5, 27, (const char*)&(atts->edgeType), 2, io, error);
        if (!rv) return NITF_FAILURE;
    }
    return NITF_SUCCESS;
}



NITF_BOOL writeLineAttributes(cgm_LineAttributes* atts, nitf_IOHandle io, nitf_Error* error)
{
    short actual;
    NITF_BOOL rv;
    /* Write the fill color if its set */
    if (colorIsSet(atts->lineColor))
    {
        rv = writeHeader(5, 48, 3, io,&actual, error);
        if (!rv) return NITF_FAILURE;

        rv = writeRGBPadded(atts->lineColor, io, error);
        if (!rv) return NITF_FAILURE;
    }
    /* Write the edge width */
    if (atts->lineWidth != -1)
    {
        rv = writeField(5, 3, (const char*)&(atts->lineWidth), 2, io, error);
        if (!rv) return NITF_FAILURE;
    }
    /* Write the edge type */
    if (atts->lineType != CGM_TYPE_NOT_SET)
    {
        rv = writeField(5, 2, (const char*)&(atts->lineType), 2, io, error);
        if (!rv) return NITF_FAILURE;
    }
    return NITF_SUCCESS;
}


NITF_BOOL writeTextAttributes(cgm_TextAttributes* atts, nitf_IOHandle io, nitf_Error* error)
{
    short actual;
    NITF_BOOL rv;
    /* Write the fill color if its set */
    if (colorIsSet(atts->textColor))
    {
        rv = writeHeader(5, 14, 3, io,&actual, error);
        if (!rv) return NITF_FAILURE;

        rv = writeRGBPadded(atts->textColor, io, error);
        if (!rv) return NITF_FAILURE;
    }
    /* Write the edge width */
    if (atts->characterHeight != -1)
    {
        rv = writeField(5, 15, (const char*)&(atts->characterHeight), 2, io, error);
        if (!rv) return NITF_FAILURE;
    }
    /* Write the edge width */
    if (atts->textFontIndex != -1)
    {
        rv = writeField(5, 10, (const char*)&(atts->textFontIndex), 2, io, error);
        if (!rv) return NITF_FAILURE;
    }

    
    /* Write the edge type */
    if (rectangleIsSet(atts->characterOrientation))
    {
        rv = writeHeader(5, 16, 8, io, &actual, error);
        if (!rv) return NITF_FAILURE;

        rv = writeRectangle(atts->characterOrientation, io, error);
        if (!rv) return NITF_FAILURE;
            
    }
    return NITF_SUCCESS;
}


NITF_BOOL writeElements(nitf_List* elements, nitf_IOHandle io, nitf_Error* error)
{
    return NITF_SUCCESS;
}


NITF_BOOL writeBody(cgm_PictureBody* body, nitf_IOHandle io, nitf_Error* error)
{
    NITF_BOOL rv;
    short actual;
    
    /* Write picture body */    
    rv = writeHeader(0, 4, 0, io, &actual, error);
    if (!rv) return NITF_FAILURE;
    
    /* Transparency */
    rv = writeField(3, 4,  (const char*)&(body->transparency), 2, io, error);
    if (!rv) return NITF_FAILURE;

    if (colorIsSet(body->auxColor))
    {
        rv = writeHeader(3, 3, 3, io, &actual, error);
        if (!rv) return NITF_FAILURE;
        rv = writeRGBPadded(body->auxColor, io, error);
        if (!rv) return NITF_FAILURE;
    }
    
    return writeElements(body->elements, io, error);
}

NITF_BOOL writePicture(cgm_Picture* picture, nitf_IOHandle io, nitf_Error* error)
{
    short actual;
    NITF_BOOL rv;
    /* Begin picture */
    rv = writeField(0, 3, picture->name, strlen(picture->name), io, error);
    if (!rv) return NITF_FAILURE;

    /* Color selection mode */
    rv = writeField(2, 2, (const char*)&(picture->colorSelectionMode), 2, io, error);
    if (!rv) return NITF_FAILURE;

    /* [Edge width spec mode] */
    rv = writeField(2, 5, (const char*)&picture->edgeWidthSpec, 2, io, error);
    if (!rv) return NITF_FAILURE;

    /* [Line width spec mode] */
    rv = writeField(2, 3, (const char*)&picture->lineWidthSpec, 2, io, error);
    if (!rv) return NITF_FAILURE;

    /* VDC */
    rv = writeVDC(picture->vdcExtent, io, error);
    if (!rv) return NITF_FAILURE;

    /* Write picture body */
    rv = writeBody(picture->body, io, error);
    if (!rv) return NITF_FAILURE;

    /* End picture */
    return writeHeader(0, 5, 0, io, &actual, error);
}

NITF_BOOL writeFontList(nitf_List* fontList, nitf_IOHandle io, nitf_Error* error)
{
    nitf_ListIterator it = nitf_List_begin(fontList);
    nitf_ListIterator end = nitf_List_end(fontList);
    short dataLen = 0;
    short actual = 0;
    NITF_BOOL rv;
    /* So lazy... */
    while (nitf_ListIterator_notEqualTo(&it, &end))
    {
        char *data = (char*)nitf_ListIterator_get(&it);
        /* The extra byte is for the length each time */
        dataLen += (strlen(data) + 1);
        nitf_ListIterator_increment(&it);
    }
    rv = writeHeader(1, 13, dataLen, io, &actual, error);
    if (!rv) return NITF_FAILURE;

    it = nitf_List_begin(fontList);
    dataLen = 0;
    while (nitf_ListIterator_notEqualTo(&it, &end))
    {
        char *data = (char*)nitf_ListIterator_get(&it);
        unsigned char len = strlen(data);
        rv = nitf_IOHandle_write(io, (const char*)&len, 1, error);
        if (!rv) return NITF_FAILURE;
            
        rv = nitf_IOHandle_write(io, (const char*)data, len, error);
        if (!rv) return NITF_FAILURE;
    
        dataLen += (1 + len);
        nitf_ListIterator_increment(&it);
    }
    
    if (actual != dataLen)
    {
        char zero = 0;
        rv = nitf_IOHandle_write(io, (const char*)&zero, 1, error);
        if (!rv) return NITF_FAILURE;

    }
    return NITF_SUCCESS;
}


NITFPRIV(NITF_BOOL) writeMetafileInfo(cgm_Metafile* mf,
                                      nitf_IOHandle io,
                                      nitf_Error* error)
{
    short reversed[3];
    NITF_BOOL rv;
    short actual;
    /* Begin Metafile */
    rv = writeField(0, 1, mf->name, strlen(mf->name), io, error);
    if (!rv) return NITF_FAILURE;

    /* Write version */
    rv = writeField(1, 1, (const char*)&(mf->version), 2, io, error);
    if (!rv) return NITF_FAILURE;
    /* Metafile element list */
    reversed[0] = NITF_HTONS(mf->elementList[0]);
    reversed[1] = NITF_HTONS(mf->elementList[1]);
    reversed[2] = NITF_HTONS(mf->elementList[2]);

    /* Element list */
    rv = writeField(1, 11, (const char*)reversed, 6, io, error);
    if (!rv) return NITF_FAILURE;
    
    /* Metafile description */
    rv = writeField(1, 2, mf->description, strlen(mf->description), io, error);
    if (!rv) return NITF_FAILURE;
    
    if (mf->fontList != NULL)
    {
        rv = writeFontList(mf->fontList, io, error);
        if (!rv) return NITF_FAILURE;
    }
    rv = writePicture(mf->picture, io, error);
    if (!rv) return NITF_FAILURE;

    /* Write metafile end */
    return writeHeader(0, 2, 0, io, &actual, error);
    
}

NITFAPI(NITF_BOOL) cgm_MetafileWriter_write(cgm_MetafileWriter* writer,
                                            cgm_Metafile* mf,
                                            nitf_IOHandle io, 
                                            nitf_Error* error)
{
/*
    NITF_BOOL ok;
    size_t memoryLength;
    char *memory = 
        cgm_MetafileWriter_writeMemory(writer, mf, &memoryLength, error);
    if (!memory)
        return NITF_FAILURE;

    ok = nitf_IOHandle_write(io, memory, memoryLength, error);
    
    NITF_FREE(memory);

    return ok;
*/
    return writeMetafileInfo(mf, io, error);

}

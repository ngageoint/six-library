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

NITF_BOOL writePicture(cgm_Picture* picture, nitf_IOHandle io, nitf_Error* error)
{
    return NITF_SUCCESS;
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

CATCH_ERROR:
    return NITF_FAILURE;
    
}

NITF_BOOL writeFontList(nitf_List* fontList, nitf_IOHandle io, nitf_Error* error)
{
    nitf_ListIterator it = nitf_List_begin(fontList);
    nitf_ListIterator end = nitf_List_end(fontList);
    short dataLen = 0;
    short actual = 0;
    
    /* So lazy... */
    while (nitf_ListIterator_notEqualTo(&it, &end))
    {
        char *data = (char*)nitf_ListIterator_get(&it);
        /* The extra byte is for the length each time */
        dataLen += (strlen(data) + 1);
        nitf_ListIterator_increment(&it);
    }
    writeHeader(1, 13, dataLen, io, &actual, error);
    it = nitf_List_begin(fontList);
    dataLen = 0;
    while (nitf_ListIterator_notEqualTo(&it, &end))
    {
        char *data = (char*)nitf_ListIterator_get(&it);
        unsigned char len = strlen(data);
        assert(nitf_IOHandle_write(io, (const char*)&len, 1, error));
        assert(nitf_IOHandle_write(io, (const char*)data, len, error));

        dataLen += (1 + len);
        nitf_ListIterator_increment(&it);
    }
    
    if (actual != dataLen)
    {
        char zero = 0;
        nitf_IOHandle_write(io, (const char*)&zero, 1, error);
    }
    return NITF_SUCCESS;
}


NITFPRIV(NITF_BOOL) writeMetafileInfo(cgm_Metafile* mf,
                                      nitf_IOHandle io,
                                      nitf_Error* error)
{
    size_t actual;
    /* Begin Metafile */
    writeField(0, 1, mf->name, strlen(mf->name), io, error);
    /* Write version */
    writeField(1, 1, (const char*)&(mf->version), 2, io, error);
    /* Metafile element list */
    short reversed[3];
    
    reversed[0] = NITF_HTONS(mf->elementList[0]);
    reversed[1] = NITF_HTONS(mf->elementList[1]);
    reversed[2] = NITF_HTONS(mf->elementList[2]);

    /* Element list */
    writeField(1, 11, (const char*)reversed, 6, io, error);

    /* Metafile description */
    writeField(1, 2, mf->description, strlen(mf->description), io, error);

    if (mf->fontList != NULL)
    {
        writeFontList(mf->fontList, io, error);
    }
    writePicture(mf->picture, io, error);

    /* Write metafile end */
    writeHeader(0, 2, 0, io, &actual, error);
    return NITF_SUCCESS;
    
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

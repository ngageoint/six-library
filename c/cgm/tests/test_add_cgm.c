/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2010, General Dynamics - Advanced Information Systems
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
 * License along with this program; if not, If not, 
 * see <http://www.gnu.org/licenses/>.
 *
 */

#include <import/nitf.h>
#include <import/cgm.h>


NITF_BOOL addGraphicSegment(nitf_Record *record, nitf_Error *error)
{
    /* add a new graphics segment */
    nitf_GraphicSegment *graphic = nitf_Record_newGraphicSegment(record, error);
    
    if (!nitf_Field_setString(graphic->subheader->filePartType, "SY", error))
        goto CATCH_ERROR;
    if (!nitf_Field_setString(graphic->subheader->graphicID, "TEST", error))
        goto CATCH_ERROR;
    if (!nitf_Field_setString(graphic->subheader->name, " ", error))
        goto CATCH_ERROR;
    if (!nitf_Field_setString(graphic->subheader->securityClass, "U", error))
        goto CATCH_ERROR;
    if (!nitf_Field_setString(graphic->subheader->encrypted, "0", error))
        goto CATCH_ERROR;
    if (!nitf_Field_setString(graphic->subheader->stype, "C", error))
        goto CATCH_ERROR;
    if (!nitf_Field_setUint32(graphic->subheader->res1, 0, error))
        goto CATCH_ERROR;
    /* might want to calculate this... for now, hard code it */
    if (!nitf_Field_setUint32(graphic->subheader->displayLevel, 2, error))
        goto CATCH_ERROR;
    if (!nitf_Field_setUint32(graphic->subheader->attachmentLevel, 1, error))
        goto CATCH_ERROR;
    if (!nitf_Field_setUint32(graphic->subheader->location, 0, error))
        goto CATCH_ERROR;
    if (!nitf_Field_setString(graphic->subheader->bound2Loc, "0000000000", error))
        goto CATCH_ERROR;
    if (!nitf_Field_setString(graphic->subheader->color, "C", error))
        goto CATCH_ERROR;
    if (!nitf_Field_setString(graphic->subheader->bound2Loc, "0007000500", error))
        goto CATCH_ERROR;
    if (!nitf_Field_setUint32(graphic->subheader->res2, 0, error))
        goto CATCH_ERROR;
    
    return NITF_SUCCESS;
    
  CATCH_ERROR:
    return NITF_FAILURE;
}


cgm_Metafile* createCGM(nitf_Error *error)
{
    cgm_Metafile *mf = NULL;
    cgm_Element *element = NULL, *dolly = NULL;
    cgm_TextElement *textElement = NULL;
    cgm_PolyLineElement *lineElement = NULL, *lineClone = NULL;
    cgm_Vertex *vertex = NULL;
    
    /* first, we create a metafile object */
    if (!(mf = cgm_Metafile_construct("TEXT", "TEXT", error)))
        goto CATCH_ERROR;
    
    /* next, create a picture */
    if (!cgm_Metafile_createPicture(mf, "Text", error))
        goto CATCH_ERROR;
    
    /* add some fonts */
    if (!nitf_List_pushBack(mf->fontList, (NITF_DATA*)"Helvetica", error))
        goto CATCH_ERROR;
    if (!nitf_List_pushBack(mf->fontList, (NITF_DATA*)"TIMES_ROMAN", error))
        goto CATCH_ERROR;
    if (!nitf_List_pushBack(mf->fontList, (NITF_DATA*)"TIMES_ITALIC", error))
        goto CATCH_ERROR;
    if (!nitf_List_pushBack(mf->fontList, (NITF_DATA*)"Courier", error))
        goto CATCH_ERROR;
    
    /* let's create a text element */
    if (!(element = cgm_TextElement_construct(error)))
        goto CATCH_ERROR;
    
    /* because of how the Elements are derived, get the text-specific part */
    textElement = (cgm_TextElement*)element->data;
    
    /* create text attributes */
    if (!(textElement->attributes = cgm_TextAttributes_construct(error)))
        goto CATCH_ERROR;
    
    /* create the text that goes in the text element */
    if (!(textElement->text = cgm_Text_construct("NITRO rocks!", error)))
        goto CATCH_ERROR;
    
    /* set some attributes */
    textElement->attributes->characterHeight = 21;
    textElement->attributes->textFontIndex = 2; /* courier */
    textElement->attributes->characterOrientation->y1 = -1;
    
    /* set the color of the text */
    textElement->attributes->textColor->r = 0;
    textElement->attributes->textColor->g = 255;
    textElement->attributes->textColor->b = 0;
    
    /* set the text coordinates */
    textElement->text->x = 50;
    textElement->text->y = 50;
    
    /* add the element to the picture body */
    if (!nitf_List_pushBack(mf->picture->body->elements,
            (NITF_DATA*)element, error))
        goto CATCH_ERROR;
    
    
    /* let's create some polyline elements to make a cross */
    if (!(element = cgm_PolyLineElement_construct(error)))
        goto CATCH_ERROR;
    
    lineElement = (cgm_PolyLineElement*)element->data;
    
    /* create attributes */
    if (!(lineElement->attributes = cgm_LineAttributes_construct(error)))
        goto CATCH_ERROR;
    
    /* set some attributes */
    lineElement->attributes->lineWidth = 2;
    lineElement->attributes->lineType = CGM_TYPE_SOLID;
    
    /* set the color of the line */
    lineElement->attributes->lineColor->r = 255;
    lineElement->attributes->lineColor->g = 0;
    lineElement->attributes->lineColor->b = 0;
    
    /* before we add vertices, let's clone it as it is now */
    if (!(dolly = cgm_Element_clone(element, error)))
        goto CATCH_ERROR;
    lineClone = (cgm_PolyLineElement*)dolly->data;
    
    /* add some vertices */
    if (!(vertex = cgm_Vertex_construct(25, 50, error)))
        goto CATCH_ERROR;
    if (!(nitf_List_pushBack(lineElement->vertices, (NITF_DATA*)vertex, error)))
        goto CATCH_ERROR;
    
    if (!(vertex = cgm_Vertex_construct(75, 50, error)))
        goto CATCH_ERROR;
    if (!(nitf_List_pushBack(lineElement->vertices, (NITF_DATA*)vertex, error)))
        goto CATCH_ERROR;
    
    if (!(vertex = cgm_Vertex_construct(50, 25, error)))
        goto CATCH_ERROR;
    if (!(nitf_List_pushBack(lineClone->vertices, (NITF_DATA*)vertex, error)))
        goto CATCH_ERROR;
    
    if (!(vertex = cgm_Vertex_construct(50, 75, error)))
        goto CATCH_ERROR;
    if (!(nitf_List_pushBack(lineClone->vertices, (NITF_DATA*)vertex, error)))
        goto CATCH_ERROR;
    
    /* add the line elements to the picture body */
    if (!nitf_List_pushBack(mf->picture->body->elements,
            (NITF_DATA*)element, error))
        goto CATCH_ERROR;
    if (!nitf_List_pushBack(mf->picture->body->elements,
            (NITF_DATA*)dolly, error))
        goto CATCH_ERROR;
    
    return mf;
    
  CATCH_ERROR:
    if (mf)
        cgm_Metafile_destruct(&mf);
    return NULL;
}

NITF_BOOL writeNITF(nitf_Record * record, nitf_IOInterface *input,
		    char *outFile, cgm_Metafile *lastCGM, nitf_Error *error)
{
    nitf_ListIterator iter;
    nitf_ListIterator end;
    int num;
    int numSegments;
    nitf_Writer *writer = NULL;
   
    /* open the output IO Handle */
    nitf_IOHandle output = nitf_IOHandle_create(outFile,
            NITF_ACCESS_WRITEONLY, NITF_CREATE, error);

    if (NITF_INVALID_HANDLE(output))
    {
        goto CATCH_ERROR;
    }

    writer = nitf_Writer_construct(error);
    if (!writer)
    {
        goto CATCH_ERROR;
    }
    
    /* prepare the writer with this record */
    if (!nitf_Writer_prepare(writer, record, output, error))
    {
        goto CATCH_ERROR;
    }

    /**************************************************************************/
    /* IMAGES                                                                 */
    /**************************************************************************/
    /* get the # of images from the field */
    if (!nitf_Field_get(record->header->numImages,
            &numSegments, NITF_CONV_INT, NITF_INT32_SZ, error))
    {
        nitf_Error_print(error, stderr, "nitf::Value::get() failed");
        numSegments = 0;
    }

    if (record->images)
    {
        iter = nitf_List_begin(record->images);
        end = nitf_List_end(record->images);
        
        num = 0;
        while(nitf_ListIterator_notEqualTo(&iter, &end))
        {
            nitf_WriteHandler *writeHandler = NULL;
            nitf_ImageSegment *segment =
                    (nitf_ImageSegment *) nitf_ListIterator_get(&iter);
            
            writeHandler = nitf_StreamIOWriteHandler_construct(input,
                    segment->imageOffset, segment->imageEnd - segment->imageOffset,
                    error);
            if (!writeHandler)
                goto CATCH_ERROR;
            
            /* attach the write handler */
            if (!nitf_Writer_setImageWriteHandler(writer, num,
                    writeHandler, error))
            {
                nitf_WriteHandler_destruct(&writeHandler);
                goto CATCH_ERROR;
            }
            
            nitf_ListIterator_increment(&iter);
            ++num;
        }
    }
    
    /**************************************************************************/
    /* GRAPHICS                                                               */
    /**************************************************************************/
    /* get the # of images from the field */
    if (!nitf_Field_get(record->header->numGraphics,
            &numSegments, NITF_CONV_INT, NITF_INT32_SZ, error))
    {
        nitf_Error_print(error, stderr, "nitf::Value::get() failed");
        numSegments = 0;
    }

    if (record->graphics)
    {
        iter = nitf_List_begin(record->graphics);
        end = nitf_List_end(record->graphics);
        
        num = 0;
        while(nitf_ListIterator_notEqualTo(&iter, &end))
        {
            nitf_WriteHandler *writeHandler = NULL;
            
            /* if this is the last graphic segment - this is the
             * graphic segment we created by hand, so we want provide our
             * own WriteHandler
             */
            if (num == numSegments - 1)
            {
                writeHandler = cgm_NITFWriteHandler_construct(lastCGM, error);
            }
            else
            {
                /* otherwise, this graphic existed previously */
                nitf_GraphicSegment *segment =
                        (nitf_GraphicSegment *) nitf_ListIterator_get(&iter);
                
                writeHandler = nitf_StreamIOWriteHandler_construct(input,
                        segment->offset, segment->end - segment->offset,
                        error);
            }
            
            if (!writeHandler)
                goto CATCH_ERROR;
            
            /* attach the write handler */
            if (!nitf_Writer_setGraphicWriteHandler(writer, num,
                    writeHandler, error))
            {
                nitf_WriteHandler_destruct(&writeHandler);
                goto CATCH_ERROR;
            }
            
            nitf_ListIterator_increment(&iter);
            ++num;
        }
    }
    
    /**************************************************************************/
    /* TEXTS                                                                  */
    /**************************************************************************/
    /* get the # of images from the field */
    if (!nitf_Field_get(record->header->numTexts,
            &numSegments, NITF_CONV_INT, NITF_INT32_SZ, error))
    {
        nitf_Error_print(error, stderr, "nitf::Value::get() failed");
        numSegments = 0;
    }

    if (record->texts)
    {
        iter = nitf_List_begin(record->texts);
        end = nitf_List_end(record->texts);
        
        num = 0;
        while(nitf_ListIterator_notEqualTo(&iter, &end))
        {
            nitf_WriteHandler *writeHandler = NULL;
            nitf_TextSegment *segment =
                    (nitf_TextSegment *) nitf_ListIterator_get(&iter);
            
            writeHandler = nitf_StreamIOWriteHandler_construct(input,
                    segment->offset, segment->end - segment->offset,
                    error);
            if (!writeHandler)
                goto CATCH_ERROR;
            
            /* attach the write handler */
            if (!nitf_Writer_setTextWriteHandler(writer, num,
                    writeHandler, error))
            {
                nitf_WriteHandler_destruct(&writeHandler);
                goto CATCH_ERROR;
            }
            
            nitf_ListIterator_increment(&iter);
            ++num;
        }
    }
    
    /**************************************************************************/
    /* DES                                                                    */
    /**************************************************************************/
    /* get the # of images from the field */
    if (!nitf_Field_get(record->header->numDataExtensions,
            &numSegments, NITF_CONV_INT, NITF_INT32_SZ, error))
    {
        nitf_Error_print(error, stderr, "nitf::Value::get() failed");
        numSegments = 0;
    }

    if (record->dataExtensions)
    {
        iter = nitf_List_begin(record->dataExtensions);
        end = nitf_List_end(record->dataExtensions);
        
        num = 0;
        while(nitf_ListIterator_notEqualTo(&iter, &end))
        {
            nitf_WriteHandler *writeHandler = NULL;
            nitf_DESegment *segment =
                    (nitf_DESegment *) nitf_ListIterator_get(&iter);
            
            writeHandler = nitf_StreamIOWriteHandler_construct(input,
                    segment->offset, segment->end - segment->offset,
                    error);
            if (!writeHandler)
                goto CATCH_ERROR;
            
            /* attach the write handler */
            if (!nitf_Writer_setDEWriteHandler(writer, num,
                    writeHandler, error))
            {
                nitf_WriteHandler_destruct(&writeHandler);
                goto CATCH_ERROR;
            }
            
            nitf_ListIterator_increment(&iter);
            ++num;
        }
    }
    
    if (!nitf_Writer_write(writer, error))
    {
        goto CATCH_ERROR;
    }
    
    nitf_IOHandle_close(output);
    nitf_Writer_destruct(&writer);
    return NITF_SUCCESS;
    
  CATCH_ERROR:    
    if (!NITF_INVALID_HANDLE(output))
        nitf_IOHandle_close(output);
    if (writer)
        nitf_Writer_destruct(&writer);
    return NITF_FAILURE;
}

int main(int argc, char **argv)
{
    nitf_Record *record = NULL;
    nitf_Reader *reader = NULL;
    cgm_Metafile *metafile = NULL;
    nitf_Error error;
    nitf_IOInterface *io = NULL;
    
    if (argc != 3)
    {
        printf("Usage: %s <input-file> <output-file> \n", argv[0]);
        exit(EXIT_FAILURE);
    }
    
    reader = nitf_Reader_construct(&error);
    if (!reader)
    {
        nitf_Error_print(&error, stderr, "Reader construction failed");
        exit(EXIT_FAILURE);
    }

    io = nitf_IOHandleAdapter_open(argv[1], NITF_ACCESS_READONLY,
                                   NITF_OPEN_EXISTING, &error);

    /*  read the file  */
    record = nitf_Reader_readIO(reader, io, &error);
    if (!record)
    {
        nitf_Error_print(&error, stderr, "Read failed");
        exit(EXIT_FAILURE);
    }
    
    /* add a graphic segment */
    addGraphicSegment(record, &error);
    
    metafile = createCGM(&error);
    
    if (!writeNITF(record, io, argv[2], metafile, &error))
    {
        nitf_Error_print(&error, stderr, "Write failed");
    }
    
    if (io)
        nitf_IOInterface_destruct(&io);
    nitf_Reader_destruct(&reader);
    nitf_Record_destruct(&record);
    
    return 0;
}

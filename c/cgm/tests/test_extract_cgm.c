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


void extractGraphics(nitf_Record *record, nitf_Reader *reader,
        nitf_Error *error)
{
    nitf_ListIterator iter;
    nitf_ListIterator end;
    int num;
    int numSegments;
    
    /**************************************************************************/
    /* GRAPHICS                                                               */
    /**************************************************************************/
    /* get the # of images from the field */
    if (!nitf_Field_get(record->header->numGraphics,
            &numSegments, NITF_CONV_INT, NITF_INT32_SZ, error))
    {
        nitf_Error_print(error, stderr, "Field retrieval failed");
        numSegments = 0;
    }

    if (record->graphics)
    {
        iter = nitf_List_begin(record->graphics);
        end = nitf_List_end(record->graphics);
        
        num = 0;
        while(nitf_ListIterator_notEqualTo(&iter, &end))
        {
            nitf_SegmentReader *segmentReader = NULL;
            nitf_Uint8 *buf = NULL;
            size_t bytes;
            nitf_GraphicSegment *segment =
                    (nitf_GraphicSegment *) nitf_ListIterator_get(&iter);
            
            segmentReader = nitf_Reader_newGraphicReader(reader, num, error);
            
            bytes = (size_t)(segment->end - segment->offset);
            buf = (nitf_Uint8*)NITF_MALLOC(bytes);
            
            if (nitf_SegmentReader_read(segmentReader, buf, bytes, error))
            {
                nitf_IOHandle output;
                char *file = (char *) NITF_MALLOC(NITF_MAX_PATH);
                sprintf(file, "graphic_%d.cgm", num);
                
                /* write to a file */
                output = nitf_IOHandle_create(file,
                        NITF_ACCESS_WRITEONLY, NITF_CREATE, error);
                
                nitf_IOHandle_write(output, buf, bytes, error);
                nitf_IOHandle_close(output);
                
                printf("Extracted graphic %d to %s\n", num, file);
                
                NITF_FREE(file);
            }
            
            NITF_FREE(buf);
            nitf_SegmentReader_destruct(&segmentReader);
            
            nitf_ListIterator_increment(&iter);
            ++num;
        }
    }
}

int main(int argc, char **argv)
{
    nitf_Record *record = NULL;
    nitf_Reader *reader = NULL;
    cgm_Metafile *metafile = NULL;
    nitf_Error error;
    nitf_IOHandle io;
    
    if (argc != 2)
    {
        printf("Usage: %s <input-file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    
    reader = nitf_Reader_construct(&error);
    if (!reader)
    {
        nitf_Error_print(&error, stderr, "nitf::Reader::construct() failed");
        exit(EXIT_FAILURE);
    }

    /* open the input handle */
    io = nitf_IOHandle_create(argv[1],
                              NITF_ACCESS_READONLY,
                              NITF_OPEN_EXISTING, &error);

    /* check to make sure it is valid */
    if (NITF_INVALID_HANDLE(io))
    {
        nitf_Error_print(&error, stderr, "nitf::IOHandle::create() failed");
        exit(EXIT_FAILURE);
    }

    /*  read the file  */
    record = nitf_Reader_read(reader, io, &error);
    if (!record)
    {
        nitf_Error_print(&error, stderr, "nitf::Reader::read() failed");
        exit(EXIT_FAILURE);
    }
    
    extractGraphics(record, reader, &error);
    
    nitf_IOHandle_close(io);
    nitf_Reader_destruct(&reader);
    nitf_Record_destruct(&record);
    
    return 0;
}

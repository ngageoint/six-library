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
 * License along with this program; If not, 
 * see <http://www.gnu.org/licenses/>.
 *
 */

#include <import/nitf.h>
/*
 * Reads in a NITF record, and generates a KML file for each segment within
 * the NITF outlining the boundaries of the footprint.  If the image does
 * not have an ICORD of 'D' or 'G' (translation, the IGEOLO is not listed
 * in geographic or decimal degress), we do nothing
 *
 */


/*
 * We could make this a polygon instead.
 *
 */
#define KML_TEMPLATE \
"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\
<kml xmlns=\"http://www.opengis.net/kml/2.2\">\
<Document><name>%s</name><open>1</open>\
<Style id=\"s%d\"><LineStyle><color>ffff00cc</color><width>2</width>\
</LineStyle></Style>\
<Placemark><name>%s</name><styleUrl>#s%d</styleUrl><LinearRing>\
<coordinates>%f,%f,0 %f,%f,0 %f,%f,0 %f,%f,0 %f,%f,0</coordinates>\
</LinearRing></Placemark></Document></kml>"

/*
 * For each image segment, generate a KML file with a bounding
 * path.
 *
 */
void writeKML(nitf_ImageSubheader* header, 
              int i, 
              const char* file)
{

    nitf_CornersType type = nitf_ImageSubheader_getCornersType(header);
    double corners[4][2];
    char buf[1024];
    char outfile[NITF_MAX_PATH];
    nitf_IOHandle out;
    nitf_Error error;

    if (type < NITF_CORNERS_GEO)
    {
        printf("Image subheader has icords [%c].  Ignoring.\n",
               nitf_Utils_cornersTypeAsCoordRep(type));
        return;
    }

    /* We're in luck!  So lets print the footprint to KML! */
    if (!nitf_ImageSubheader_getCornersAsLatLons(header, corners, &error))
    {
        nitf_Error_print(&error, stdout, "Bad corners.  Ignoring");
        return;
    }

    NITF_SNPRINTF(outfile, NITF_MAX_PATH, "%s-%d.kml", file, i+1);

    out = nitf_IOHandle_create(outfile, 
                               NITF_ACCESS_WRITEONLY,
                               NITF_CREATE, 
                               &error);

    /* KML is lon first! */
    NITF_SNPRINTF(buf, NITF_MAX_PATH, KML_TEMPLATE, outfile, i+1, outfile, i+1,
                  corners[0][1], corners[0][0],
                  corners[1][1], corners[1][0],
                  corners[2][1], corners[2][0],
                  corners[3][1], corners[3][0],
                  corners[0][1], corners[0][0]);

    if (!nitf_IOHandle_write(out, buf, strlen(buf), &error))
    {
        nitf_Error_print(&error, stdout, "Write failed");
        return;

    }
    nitf_IOHandle_close(out);


}

int main(int argc, char** argv)
{
    nitf_Record* record;
    nitf_Reader* reader;
    nitf_IOHandle io;
    nitf_Error error;
    char file[NITF_MAX_PATH];
    int i;
    nitf_Uint32 num;

    if ( argc != 2 )
    {
        fprintf(stdout, "Usage: %s <nitf-file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* Check if its a NITF */
    if (nitf_Reader_getNITFVersion(argv[1]) == NITF_VER_UNKNOWN)
    {
        fprintf(stdout, "Invalid NITF: %s\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    /* Open the file handle */
    io = nitf_IOHandle_create(argv[1], NITF_ACCESS_READONLY,
                              NITF_OPEN_EXISTING, &error);

    if (NITF_INVALID_HANDLE(io))
    {
        nitf_Error_print(&error, stdout, "Exiting...");
        exit(EXIT_FAILURE);
    }

    /* Make a reader */
    reader = nitf_Reader_construct(&error);
    if (!reader)
    {
        nitf_Error_print(&error, stdout, "Exiting (1) ...");
        exit(EXIT_FAILURE);
    }

    /*  This parses all header data within the NITF  */
    record = nitf_Reader_read(reader, io, &error);
    if (!record) goto CLEANUP;


    /* Figure out if we have a nitf */
    num = nitf_Record_getNumImages(record, &error);
    if (NITF_INVALID_NUM_SEGMENTS( num ))
    {
        nitf_Error_print(&error, stdout, "Invalid num image segments");
        goto CLEANUP;
    }
    if (num == 0)
    {
        printf("No images found in file.  Ignoring.\n");
        goto CLEANUP;
    }
    
    nitf_Utils_baseName(file, argv[1], ".");

    for (i = 0; i < num; i++)
    {
        nitf_ImageSegment* segment = nitf_List_get(record->images, i, &error);
        if (!segment)
        {
            nitf_Error_print(&error, stdout, "Image list retrieval");
            goto CLEANUP;
        }
        writeKML(segment->subheader, i, file);
    } 

CLEANUP:
    nitf_IOHandle_close(io);
    if (record) nitf_Record_destruct(&record);
    if (reader) nitf_Reader_destruct(&reader);

    return 0;
}


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
 * License along with this program;
 * If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <import/nitf.h>
#include <import/cgm.h>

/*!
 *  This program reads only a CGM metafile subset that pertains to the
 *  NITF MIL-STD (2301A).
 */


void printCGM(cgm_Metafile *mf, nitf_Error *error)
{
    nitf_ListIterator iter;
    nitf_ListIterator end;
    int i;
    
    printf("metafile:\n");
    printf("    name        : %s\n", mf->name);
    printf("    version     : %d\n", mf->version);
    printf("    description : %s\n", mf->description);
    printf("    fonts       : { ");
    
    iter = nitf_List_begin(mf->fontList);
    end = nitf_List_end(mf->fontList);
    while(nitf_ListIterator_notEqualTo(&iter, &end))
    {
        printf("%s, ", (char*)nitf_ListIterator_get(&iter));
        nitf_ListIterator_increment(&iter);
    }
    printf("}\n");
    
    printf("    picture     : \n");
    printf("        name               : %s\n", mf->picture->name);
    printf("        colorSelectionMode : %d\n", mf->picture->colorSelectionMode);
    printf("        edgeWidthSpec      : %d\n", mf->picture->edgeWidthSpec);
    printf("        lineWidthSpec      : %d\n", mf->picture->lineWidthSpec);
    printf("        vdcExtent          : \n");
    printf("            x1 : %d\n", mf->picture->vdcExtent->x1);
    printf("            y1 : %d\n", mf->picture->vdcExtent->y1);
    printf("            x2 : %d\n", mf->picture->vdcExtent->x2);
    printf("            y2 : %d\n", mf->picture->vdcExtent->y2);
    printf("        body               :\n");
    printf("            transparency : %d\n", mf->picture->body->transparency);
    
    iter = nitf_List_begin(mf->picture->body->elements);
    end = nitf_List_end(mf->picture->body->elements);
    while(nitf_ListIterator_notEqualTo(&iter, &end))
    {
        cgm_Element_print((cgm_Element*)nitf_ListIterator_get(&iter));
        nitf_ListIterator_increment(&iter);
    }
}

int main(int argc, char** argv)
{
    nitf_Error error;
    cgm_Metafile* mf = NULL;
    cgm_MetafileReader* reader = NULL;
    cgm_MetafileWriter* writer = NULL;
    nitf_IOInterface* io = NULL;

    if (argc != 2)
    {
        printf("Usage: %s <Version 1 CGM>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    reader = cgm_MetafileReader_construct(&error);
    assert(reader);

    io = nitf_IOHandleAdapter_open(argv[1], NITF_ACCESS_READONLY,
                                   NITF_OPEN_EXISTING, &error);

    mf = cgm_MetafileReader_read(reader, io, &error);
    if (!mf)
    {
        nitf_Error_print(&error, stdout, "Exiting...");
        goto END_OF_FILE;

    }
    
    printCGM(mf, &error);
    
    cgm_Metafile_destruct(&mf);

    END_OF_FILE:
    {
        if (io)
            nitf_IOInterface_destruct(&io);
        if (reader)
            cgm_MetafileReader_destruct(&reader);
        assert(!reader);
        return 0;
    }
}

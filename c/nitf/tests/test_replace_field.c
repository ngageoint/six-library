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

/*
 *  This test case does a search and replace for all field values that match.
 *  It tests the find functionality along with the replace for a node that
 *  already exists.  This is not a good test case for determining full
 *  correctness of nitf_TRE_setField, since it doesnt try and create a
 *  new field.
 *
 */

void lookForTREField(nitf_Extensions* ext, 
		     const char* tag, 
		     const char* pattern,
		     const char* replace)
{
    nitf_ListIterator current, last;

    nitf_List* list = nitf_Extensions_getTREsByName(ext, tag);
    if (list == NULL) return;
    
    current = nitf_List_begin(list);
    last = nitf_List_end(list);

    while (nitf_ListIterator_notEqualTo(&current, &last))
    {
	nitf_Error error;
	nitf_TRE* tre = nitf_ListIterator_get(&current);
	nitf_ListIterator currentInst;
	nitf_ListIterator lastInst;
	
	nitf_List* found = nitf_TRE_find(tre, pattern, &error);
	if (!found) return;
	

	currentInst = nitf_List_begin(found);
	lastInst = nitf_List_end(found);
	
	while (nitf_ListIterator_notEqualTo(&currentInst, &lastInst))
	{
	    nitf_Pair* pair = nitf_ListIterator_get(&currentInst);
	    nitf_Field* field = (nitf_Field*)pair->data;
	    printf("Found: %s [%.*s]\n", pair->key, (int)field->length, field->raw);
	    printf("Replacing with [%s]\n", replace);
	    if (!nitf_TRE_setField(tre, 
				   pair->key, 
				   (NITF_DATA*)replace, 
				   strlen(replace), 
				   &error))
	    {
		nitf_Error_print(&error, stdout, "Replace failed");
		return;
	    }
	    
	    field = nitf_TRE_getField(tre, pair->key);
	    printf("Round Trip Value: %s [%.*s]\n", pair->key, (int)field->length, field->raw);
	    nitf_ListIterator_increment(&currentInst);

	}
	nitf_ListIterator_increment(&current);
    }


}

int main(int argc, char **argv)
{
    /*  Get the error object       */
    nitf_Error error;

    /* so I can remember what Im doing with args */
    const char* treName;
    const char* fieldName;
    const char* replace;
    /*  This is the reader object  */
    nitf_Reader *reader;
    nitf_Record *record;
	
    /*  The IO handle  */
    nitf_IOHandle io;
    int num;

    /*  Check argv and make sure we are happy  */
    if (argc != 5)
    {
        printf("Usage: %s <nitf-file> <TRE> <field> <replacement value>\n", 
	       argv[0]);
        exit(EXIT_FAILURE);
    }

    if (nitf_Reader_getNITFVersion(argv[1]) == NITF_VER_UNKNOWN)
    {
	printf("File: %s is not a NITF\n", argv[1]);
	exit(EXIT_FAILURE);
    }
	
    treName = argv[2];
    fieldName = argv[3];
    replace = argv[4];
    io = nitf_IOHandle_create(argv[1], NITF_ACCESS_READONLY,
                              NITF_OPEN_EXISTING, &error);

    if (NITF_INVALID_HANDLE(io))
    {
        nitf_Error_print(&error, stdout, "Exiting...");
        exit(EXIT_FAILURE);
    }

    reader = nitf_Reader_construct(&error);
    if (!reader)
    {
        nitf_Error_print(&error, stdout, "Exiting (1) ...");
        exit(EXIT_FAILURE);
    }
   
#if NITF_VERBOSE_READER
    printf("Here are the loaded handlers\n");
    printf("* * * * * * * * * * * * * * * *\n");
    nitf_HashTable_print(reader->reg->treHandlers);
    printf("* * * * * * * * * * * * * * * *\n");
#endif
    record = nitf_Reader_read(reader, io, &error);


    lookForTREField(record->header->extendedSection, treName, fieldName, replace);
    lookForTREField(record->header->userDefinedSection, treName, fieldName, replace);

    if (!nitf_Field_get(record->header->numImages,
                        &num, NITF_CONV_INT, NITF_INT32_SZ, &error))
        goto CATCH_ERROR;

    /* And now show the image information */
    if (num > 0)
    {

        /*  Walk each image and show  */
        nitf_ListIterator iter = nitf_List_begin(record->images);
        nitf_ListIterator end = nitf_List_end(record->images);

        while (nitf_ListIterator_notEqualTo(&iter, &end))
        {
            nitf_ImageSegment *segment =
                (nitf_ImageSegment *) nitf_ListIterator_get(&iter);

	    lookForTREField(segment->subheader->extendedSection, treName, fieldName, replace);
	    lookForTREField(segment->subheader->userDefinedSection, treName, fieldName, replace);
            nitf_ListIterator_increment(&iter);
        }
    }
    else
    {
        printf("No image in file!\n");
    }

    nitf_IOHandle_close(io);
    nitf_Record_destruct(&record);

    nitf_Reader_destruct(&reader);

    return 0;

CATCH_ERROR:
    printf("!!! we had a problem reading the file !!!\n");
    nitf_Error_print(&error, stdout, "Exiting...");
    exit(EXIT_FAILURE);
}


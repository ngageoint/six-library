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



#define SHOW(X) printf("%s=[%s]\n", #X, ((X==0)?"(nul)":X))
#define SHOWI(X) printf("%s=[%ld]\n", #X, X)
#define SHOWLL(X) printf("%s=[%lld]\n", #X, X)
#define SHOWRGB(X) printf("%s(R,G,B)=[%02x,%02x,%02x]\n", #X, (unsigned char) X[0], (unsigned char) X[1], (unsigned char) X[2])
#define SHOW_VAL(X) printf("%s=[%.*s]\n", #X, ((X==0)?8:((X->raw==0)?5:X->length)), ((X==0)?"(nulptr)":((X->raw==0)?"(nul)":X->raw)))

void showTRE(nitf_TRE* tre)
{
    nitf_Error error;
    if (! nitf_TRE_print(tre, &error))
        nitf_Error_print(&error, stdout, "Ignoring...");
}

void showExtSection(nitf_Extensions* ext)
{
    nitf_Error lerror;
    if (ext)
    {
        nitf_ExtensionsIterator extIter;
        nitf_ExtensionsIterator endIter;

        if ( nitf_Extensions_exists( ext, "AIMIDB" ) )
        {
            printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
            printf("DETECTED AIMIDB, REMOVING...\n");
            nitf_Extensions_removeTREsByName(ext, "AIMIDB");
            printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
        }

        extIter = nitf_Extensions_begin(ext);
        endIter = nitf_Extensions_end(ext);

        while (nitf_ExtensionsIterator_notEqualTo(&extIter, &endIter) )
        {
            nitf_TRE* tre = nitf_ExtensionsIterator_get(&extIter);
            if ( strcmp( tre->tag, "BLOCKA") == 0 )
            {
                printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
                printf("DETECTED SINGLE BLOCKA, REMOVING...\n");
                if ( (tre = nitf_Extensions_remove(ext, &extIter, &lerror)) == NULL)
                {
                    nitf_Error_print(&lerror, stdout, "Couldnt blow away blocka!\n");
                }

                printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
                printf("NOW RE-INSERTING BLOCK...\n");
                if (! nitf_Extensions_insert(ext, &extIter, tre, &lerror) )
                {
                    nitf_Error_print(&lerror, stdout, "Couldnt blow away blocka!\n");
                }
                printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");


            }
            /*printf("TRE: [%s]\n", tre->tag);
            showTRE(tre);*/
            nitf_ExtensionsIterator_increment(&extIter);
        }

        extIter = nitf_Extensions_begin(ext);
        endIter = nitf_Extensions_end(ext);

        while (nitf_ExtensionsIterator_notEqualTo(&extIter, &endIter) )
        {
            nitf_TRE* tre = nitf_ExtensionsIterator_get(&extIter);
            printf("TRE: [%s]\n", tre->tag);
            showTRE(tre);
            nitf_ExtensionsIterator_increment(&extIter);
        }

    }
}





int main(int argc, char **argv)
{
    /*  Get the error object       */
    nitf_Error     error;

    /*  This is the reader object  */
    nitf_Reader* reader;
    nitf_Record* record;

    /*  The IO handle  */
    nitf_IOHandle io;
    int num;

    /*  Check argv and make sure we are happy  */
    if ( argc != 2 )
    {
        printf("Usage: %s <nitf-file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    io = nitf_IOHandle_create(argv[1], NITF_ACCESS_READONLY, NITF_OPEN_EXISTING, &error);
    if ( NITF_INVALID_HANDLE( io ) )
    {
        nitf_Error_print(&error, stdout, "Exiting...");
        exit( EXIT_FAILURE );
    }

    reader = nitf_Reader_construct(&error);
    if (!reader)
    {
        nitf_Error_print(&error, stdout, "Exiting (1) ...");
        exit( EXIT_FAILURE );
    }

    /*    record = nitf_Record_construct(&error);
    if (!record)
    {
    nitf_Error_print(&error, stdout, "Exiting (2) ...");
    nitf_Reader_destruct(&reader);
    exit( EXIT_FAILURE );
    }*/


#if NITF_VERBOSE_READER
    printf("Here are the loaded handlers\n");
    printf("* * * * * * * * * * * * * * * *\n");
    nitf_HashTable_print(reader->reg->treHandlers);
    printf("* * * * * * * * * * * * * * * *\n");
#endif

    if ( ! (record = nitf_Reader_read(reader, io, &error  )) )
    {
        nitf_Error_print(&error, stdout, "Exiting ...");
        exit(EXIT_FAILURE);
    }

    printf("User defined: in file header\n");
    showExtSection(record->header->userDefinedSection);
    printf("Extended defined: in file header\n");
    showExtSection(record->header->extendedSection);



    if (!nitf_Field_get(record->header->numImages,
                        &num,
                        NITF_CONV_INT,
                        NITF_INT32_SZ,
                        &error))
        nitf_Error_print(&error, stdout, "Skipping b/c Invalid numImages");
    /* And now show the image information */
    else if (num > 0)
    {

        /*  Walk each image and show  */
        nitf_ListIterator iter = nitf_List_begin(record->images);
        nitf_ListIterator end  = nitf_List_end(record->images);

        while ( nitf_ListIterator_notEqualTo(&iter, &end) )
        {
            nitf_ImageSegment* segment =
                (nitf_ImageSegment*)nitf_ListIterator_get(&iter);

            printf("User defined: in image segment\n");
            showExtSection(segment->subheader->userDefinedSection);
            printf("Extended defined: in image segment\n");
            showExtSection(segment->subheader->extendedSection);


            nitf_ListIterator_increment(&iter);

        }
    }
    else
    {
        printf("No image in file!\n");
    }




    nitf_IOHandle_close(io);
    nitf_Record_destruct(&record);

    if ( !nitf_List_isEmpty(reader->warningList))
    {
        /*  Iterator to a list  */
        nitf_ListIterator it;

        /*  Iterator to the end of list  */
        nitf_ListIterator endList;

        it      = nitf_List_begin(reader->warningList);

        /*  End of list pointer  */
        endList = nitf_List_end(reader->warningList);

        printf("WARNINGS: ");
        printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

        /*  While we are not at the end  */
        while ( nitf_ListIterator_notEqualTo( &it, &endList ) )
        {
            /*  Get the last data  */
            char* p = (char*)nitf_ListIterator_get(&it);
            /*  Make sure  */
            assert(p != NULL);

            /*  Show the data  */
            printf("\tFound problem: [%s]\n\n", p);

            /*  Increment the list iterator  */
            nitf_ListIterator_increment(&it);
        }
        printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    }

    nitf_Reader_destruct(&reader);

    return 0;
}

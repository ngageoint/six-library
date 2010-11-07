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


void findInExtensions(nitf_Extensions* ext)
{
    /*  These iterators are for going through the image segments  */
    nitf_ListIterator  iter;
    nitf_ListIterator  end;
    nitf_List* list;
    assert( ext );

    list = nitf_Extensions_get(ext, "ACFTB");
    if (list)
    {
        /*  Set the iterator to traverse the list of image segments  */
        iter = nitf_List_begin(list);
        /*  And set this one to the end, so we'll know when we're done!  */
        end  = nitf_List_end(list);

        /*  While we are not done...  */
        while ( nitf_ListIterator_notEqualTo(&iter, &end) )
        {
            nitf_TRE* tre;

            printf("Found ACFTB instance\n");

            /*  Get the image segment as its proper object  */
            tre = (nitf_TRE*)nitf_ListIterator_get(&iter);
            if ( nitf_HashTable_exists( tre->hash, "raw_data" ) )
            {
                printf("Your plugin for ACFTB was not loaded so the data is contained in the RAW section\n");
            }
            else
            {
                nitf_Pair* mission = nitf_HashTable_find( tre->hash,
                                     "ACMSNID" );
                if (! mission )
                {
                    printf("Error: no Mission ID available\n");
                    nitf_HashTable_print( tre->hash );

                }


                else
                {
                    nitf_Field* field = (nitf_Field*)mission->data;

                    printf("Mission ID: [%.*s]\n", field->length, field->raw);
                }
            }

            /*  Increment the iterator so we can continue  */
            nitf_ListIterator_increment(&iter);
        }
    }
    else
    {
        printf("No ACFTB\n");
    }

}

int main(int argc, char **argv)
{

    /*  This is the error we hopefully wont receive  */
    nitf_Error         e;

    /*  This is the reader  */
    nitf_Reader*     reader;

    /*  This is the record of the file we are reading  */
    nitf_Record*       record;


    /*  This is the io handle we will give the reader to parse  */
    nitf_IOHandle      io;


    /*  These iterators are for going through the image segments  */
    nitf_ListIterator  iter;
    nitf_ListIterator  end;

    /*  If you didnt give us a nitf file, we're croaking  */
    if (argc != 2)
    {
        printf("Usage: %s <nitf-file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }


    reader = nitf_Reader_construct(&e);
    if (!reader)
    {
        nitf_Error_print(&e, stderr, "nitf::Reader::construct() failed");
        exit(EXIT_FAILURE);
    }

    /*  If you did, though, we'll be nice and open it for you  */
    io = nitf_IOHandle_create(argv[1],
                              NITF_ACCESS_READONLY,
                              NITF_OPEN_EXISTING,
                              &e);

    /*  But, oh boy, if you gave us a bad location...!  */
    if ( NITF_INVALID_HANDLE( io ) )
    {
        /*  You had this coming!  */
        nitf_Error_print(&e, stderr, "nitf::IOHandle::create() failed");
        exit(EXIT_FAILURE);
    }

    /*  Read the file  */
    record = nitf_Reader_read(reader,
                              io,
                              &e);
    if (!record)
    {
        nitf_Error_print(&e, stderr, "nitf::Reader::read() failed");
        exit(EXIT_FAILURE);
    }

    /*  Set the iterator to traverse the list of image segments  */
    iter = nitf_List_begin(record->images);
    /*  And set this one to the end, so we'll know when we're done!  */
    end  = nitf_List_end(record->images);

    /*  While we are not done...  */
    while ( nitf_ListIterator_notEqualTo(&iter, &end) )
    {

        /*  Get the image segment as its proper object  */
        nitf_ImageSegment* imageSegment =
            (nitf_ImageSegment*)nitf_ListIterator_get(&iter);

        assert( imageSegment->subheader);
        if ( imageSegment->subheader->userDefinedSection )
            findInExtensions( imageSegment->subheader->userDefinedSection );
        else
            printf("Nothing found in user defined section!\n");

        if ( imageSegment->subheader->extendedSection )
            findInExtensions( imageSegment->subheader->extendedSection );
        else
            printf("Nothing found in extended section!\n");


        /*  Increment the iterator so we can continue  */
        nitf_ListIterator_increment(&iter);
    }

    nitf_Record_destruct(&record);
    nitf_Reader_destruct(&reader);
    nitf_IOHandle_close(io);

    return 0;
}

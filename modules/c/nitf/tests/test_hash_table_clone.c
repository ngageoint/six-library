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

/*  The error is just to fulfill the interface  */
/*  I hand it NULL!  */
char* clone_string(char* data, nitf_Error* error)
{

    int data_len = strlen(data);
    char* new_data = (char*)NITF_MALLOC( data_len + 1);
    printf("Cloning '%s'...\n", data);
    new_data[ data_len ] = 0;
    assert(new_data);
    strcpy(new_data, data);
    return new_data;
}


/*  For this first test case, the DATA is a simple char*   */
/*  This case should be trivial since a data pointer is    */
/*  always the same size in C                              */

int main(int argc, char** argv)
{
    /*  An error to populate on failure  */
    nitf_Error      error;
    /*  A hash table  */
    nitf_HashTable* hashTable;
    /*  A clone  */
    nitf_HashTable* dolly = NULL;


    /*  Construct the hash table  */
    hashTable = nitf_HashTable_construct(4, &error);

    /*  Make sure we got it  */
    if (!hashTable)
    {
        /*  This call is like perror()  */
        nitf_Error_print(&error, stderr, "Exiting...");
    }

    /*  We didnt allocate the valueBuf -- its static          */
    /*  As a result, we need to notify our hash table NOT to  */
    /*  destroy it on failure                                 */
    nitf_HashTable_setPolicy(hashTable, NITF_DATA_ADOPT);

    /*  Make sure our key value file and search file exist  */
    if ( (argc % 2 ) == 0 )
    {
        printf("Usage: %s <key1> <value1> .. <keyn> <valuen>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    {
        int i;
        for (i = 1; i < argc; i += 2)
        {
            if (!nitf_HashTable_insert(hashTable,
                                       argv[i],
                                       clone_string(argv[i+1],
                                                    NULL), &error))
            {
                nitf_Error_print(&error, stderr, "Insert!");
            }
        }
    }


    dolly = nitf_HashTable_clone(hashTable,
                                 (NITF_DATA_ITEM_CLONE)clone_string,
                                 &error);
    if (!dolly)
    {
        nitf_Error_print(&error, stderr, "Dolly died...");
    }

    assert(nitf_HashTable_insert(dolly, "Cracked", clone_string("Hash", NULL),
                                 &error));


    printf("[hashTable]:\n");
    /*  Now, lets be expansive and print the hash  */
    nitf_HashTable_print(hashTable);


    printf("[dolly]:\n");
    /*  And lets print the clone  */
    nitf_HashTable_print(dolly);


    /*  Destroy our hash table  */
    nitf_HashTable_destruct(&hashTable);
    /*  Destroy our hash table  */
    nitf_HashTable_destruct(&dolly);

    return 0;
}

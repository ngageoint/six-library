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


/*  For this first test case, the DATA is a simple char*   */
/*  This case should be trivial since a data pointer is    */
/*  always the same size in C                              */

int main(int argc, char** argv)
{
    /*  An error to populate on failure  */
    nitf_Error      error;
    /*  A hash table  */
    nitf_HashTable* hashTable;
    /*  The hash table configurator  */
    FILE* config;

    /*  The search file -- look for this text  */
    FILE* search;

    /*  A key buffer  */
    char keyBuf[512] = "";

    /*  A value buffer  */
    char valueBuf[512] = "";

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
    nitf_HashTable_setPolicy(hashTable, NITF_DATA_RETAIN_OWNER);

    /*  Make sure our key value file and search file exist  */
    if (argc != 3)
    {
        printf("Usage: %s <key[tab]value file> <search-key file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    /*  Open our config file, this contains the key-value pairs we  */
    /*  will insert into our hash, in the order key[tab]value       */
    config = fopen( argv[1], "r" );

    if ( !config )
    {
        /*  If we didnt open, freak out at the user  */
        printf("Could not find file [%s]\n", argv[1]);
        exit(EXIT_FAILURE);
    }
    /*  Scan the configuration file, and read into the buffers  */
    while ( fscanf( config, "%s\t%s\n", keyBuf, valueBuf) != EOF )
    {

        printf("Read Key: %s\n", keyBuf);

        /*  Now comes the important part -- insert  */
        if (!nitf_HashTable_insert(hashTable, keyBuf, valueBuf, &error))
        {
            nitf_Error_fprintf(&error, stderr, "While trying to insert [%s]\n",
                               keyBuf);
            /*  And here is the REAL test, can it delete properly!!  */
            nitf_HashTable_destruct(&hashTable);
            exit(EXIT_FAILURE);
        }

        /*  Be nice -- reset buffers  */
        memset(keyBuf, 0, 512);
        memset(valueBuf, 0, 512);

    }
    /*  Close the configurator  */
    fclose( config );

    /*  Now, lets be expansive and print the list  */
    nitf_HashTable_print(hashTable);

    /*  Open the search file -- this contains keys to search for  */
    search = fopen( argv[2], "r" );

    /*  If we couldnt open  */
    if ( !search )
    {
        /*  Die a sad death  */
        printf("Could not find file [%s]\n", argv[1]);

        /*  We've already created the hash table, now we have to dump it  */
        nitf_HashTable_destruct(&hashTable);
        exit(EXIT_FAILURE);

    }
    /*  Reset the key again, we need it one more time  */
    memset(keyBuf, 0, 512);
    while ( fscanf( search, "%s\n", keyBuf) != EOF )
    {
        /*  A find gives us back a pair, just like in a std::map  */
        nitf_Pair* where = NULL;

        printf("Searching for key [%s] in hash table\n",
               keyBuf);

        /*  Find the key/value pair  */
        where = nitf_HashTable_find(hashTable, keyBuf);

        /*  We didnt get it  */
        if ( where == NULL )
        {
            printf("\t[%s] Search unsuccessful.  No such key\n",
                   keyBuf);
        }
        /*  Its there in the hash!!  */
        else
        {
            printf("\t[%s] Located the value! Value: [%s]\n",
                   keyBuf,
                   (char*)where->data);
        }

        /*  Reset the key buffer  */
        memset(keyBuf, 0, 512);

    }
    /*  Close the search file  */
    fclose(search);

    /*  Destroy our hash table  */
    nitf_HashTable_destruct(&hashTable);
    return 0;
}

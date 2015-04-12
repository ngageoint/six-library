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

typedef struct _TestData
{
    char  raw[512];
    int   len;
}
TestData;
/*  For this first test case, the DATA is a simple char*   */
/*  This case should be trivial since a data pointer is    */
/*  always the same size in C                              */

int main(int argc, char** argv)
{
    nitf_HashTable* hashTable;
    int i;
    FILE* config;
    FILE* search;
    char keyBuf[512] = "";
    char valueBuf[512] = "";
    TestData* allocVal;

    hashTable = nitf_HashTable_construct(50);

    nitf_HashTable_initDefaults(hashTable);
    if (argc != 3)
    {
        printf("Usage: %s <key[tab]value file> <search-key file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    config = fopen( argv[1], "r" );
    if ( !config )
    {
        printf("Could not find file [%s]\n", argv[1]);
        exit(EXIT_FAILURE);
    }
    while ( fscanf( config, "%s\t%s\n", keyBuf, valueBuf) != EOF )
    {

        printf("While reading file:\n");
        printf("----------------------------\n");
        printf("Read Key: %s\n", keyBuf);

        allocVal = (TestData*)malloc(sizeof(TestData));

        /*  Initialize it with garbage  */

        memset(allocVal->raw, '?', 512);
        allocVal->len = strlen(valueBuf);
        /*  This string is NOT NULL terminated  */
        memcpy(allocVal->raw, valueBuf, allocVal->len);


        nitf_HashTable_insert(hashTable, keyBuf, allocVal);
        /* Reset buffers */
        memset(keyBuf, 0, 512);
        memset(valueBuf, 0, 512);

    }
    fclose( config );


    search = fopen( argv[2], "r" );
    if ( !search )
    {
        printf("Could not find file [%s]\n", argv[1]);
        exit(EXIT_FAILURE);
    }
    memset(keyBuf, 0, 512);
    while ( fscanf( search, "%s\n", keyBuf) != EOF )
    {
        nitf_Pair* where = NULL;
        printf("Searching for key [%s] in hash table\n",
               keyBuf);

        if ( !nitf_HashTable_exists(hashTable, keyBuf) )
        {
            printf("Warning: no key found [%s] in exists() call\n",
                   keyBuf);
        }

        where = nitf_HashTable_find(hashTable, keyBuf);
        if ( where == NULL )
        {
            printf("\t[%s] Search unsuccessful.  No such key\n",
                   keyBuf);
        }
        else
        {
            TestData* testData = (TestData*)where->data;
            char buf[512];
            memset(buf, 0, 512);
            memcpy(buf, testData->raw, testData->len);
            printf("\t[%s] Located the value! Value: [%s]\n",
                   keyBuf,
                   buf);
        }


        memset(keyBuf, 0, 512);

    }
    fclose(search);
    nitf_HashTable_destruct(&hashTable);
    return 0;
}

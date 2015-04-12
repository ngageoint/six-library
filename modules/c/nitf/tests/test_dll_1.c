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
 *  Read a file of the form
 *  <file>\t<retrieve>\n
 *  Retrieval function should have the signature
 *  int func(void)
 *
 */

int main(int argc, char **argv)
{
    FILE* config;
    char file[NITF_MAX_PATH] = "";
    char func[NITF_MAX_PATH] = "";

    if (argc != 2)
    {
        printf("Usage: %s <dll name/function file>\n",
               argv[0]);
        exit(EXIT_FAILURE);
    }
    config = fopen( argv[1], "r");
    if ( !config )
    {
        printf("Could not find file [%s]\n", argv[1]);
        exit(EXIT_FAILURE);
    }


    while ( fscanf( config, "%s\t%s\n", file, func) != EOF )
    {
        int ok;
        nitf_Error error;
        nitf_DLL* dll;
        void (*ptr)(char**);
        printf("Serving file [%s] at entry point [%s]...\n",
               file, func );
        dll = nitf_DLL_construct(&error);
        if (! dll )
        {
            nitf_Error_print(&error, stdout, "");
            exit(EXIT_FAILURE);
        }

        ok = nitf_DLL_load( dll, file, &error );
        if (! ok )
        {
            nitf_Error_print(&error, stdout, "");
            exit(EXIT_FAILURE);
        }


        printf("\t>Successfully loaded dso!\n");

        printf("Retrieving [%s]\n", func );

        if ( nitf_DLL_isValid( dll ))
        {
            ptr = (void (*)(char**) )
                  nitf_DLL_retrieve(dll, func, &error);
            /*sleep(20);*/
            if ( ptr == (void (*)(char**))NULL )
            {
                printf("Here!!!\n");
                nitf_Error_print(&error, stdout, "");
                exit(EXIT_FAILURE);
            }
            printf("Calling [%s]\n", func);

            (*ptr)(&argv[1]);

            printf("Argv call [%s]\n", argv[1]);
        }
        if (!nitf_DLL_unload(dll, &error))
        {
            nitf_Error_print(&error, stdout, "");
        }
        printf("\t>Successfully unloaded!\n");
        nitf_DLL_destruct(&dll);

    }
    fclose( config );
    return 0;
}


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

int main(int argc, char**argv)
{
    nitf_Error error;
    nitf_PluginRegistry* reg;
    NITF_PLUGIN_TRE_HANDLER_FUNCTION test_main;
    int bad = 0;
    if (argc != 2)
    {
        printf("Usage: %s <TRE>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    reg = nitf_PluginRegistry_getInstance(&error);

    if (!reg)
    {
        nitf_Error_print(&error, stdout, "Exiting...");
        exit(EXIT_FAILURE);
    }
    /*  Don't need this now that it is a singleton
     *
     * if (! nitf_PluginRegistry_load(reg, &error) )
     * {
     *   nitf_Error_print(&error, stdout, "Exiting...");
     *   exit(EXIT_FAILURE);
     * }
     */

    nitf_HashTable_print(reg->treHandlers);

    test_main =
        nitf_PluginRegistry_retrieveTREHandler(reg,
                                               argv[1],
                                               &bad,
                                               &error);


    if (bad)
    {
        nitf_Error_print(&error, stderr, "Error!");
    }
    else if (test_main == (NITF_PLUGIN_TRE_HANDLER_FUNCTION)NULL)
    {
        printf("No such plugin could be found\n");
    }
    else
    {
        int ok;
        printf("Found DLL and main!!!\n");
        ok = (*test_main)(0, NULL, NULL, &error);
        if (!ok)
        {
            nitf_Error_print(&error, stderr , "");
        }

    }


    /*  Don't need this now that the registry is a singleton
     *  if (! nitf_PluginRegistry_unload(reg, &error) )
     *  {
     *    nitf_Error_print(&error, stdout, "Exiting...");
     *    exit(EXIT_FAILURE);
     *  }
     */

    /*  Don't need this now that the registry is a singleton
     *
     *  nitf_PluginRegistry_destruct(&reg);
     */
    return 0;

}

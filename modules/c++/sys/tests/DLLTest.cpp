/* =========================================================================
 * This file is part of sys-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
 *
 * sys-c++ is free software; you can redistribute it and/or modify
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

#include <import/except.h>
#include <import/str.h>
#include <import/sys.h>
#include <string>
using namespace except;
using namespace sys;

typedef bool (*HOOK_FN)(void);

int main(int argc, char** argv)
{
    if (argc != 3)
    {
        die_printf(
                   "Usage: %s <dso-path> <hook>\n\twhere hook=bool (void*)(void)\n",
                   argv[0]);
    }
    // Ok, we got past the initial hurdle, now lets 
    // try and load the thing
    try
    {
        std::string dsoPath = argv[1];
        std::string hook = argv[2];

        DLL dso(dsoPath);

        HOOK_FN theHook = (HOOK_FN) dso.retrieve(hook);

        // Call the hook
        if (!theHook())
        {
            throw Exception(Ctxt("The hook failed!"));
        }

    }
    catch (Throwable& ex)
    {
        std::cout << ex.toString() << std::endl;
    }

}


/* =========================================================================
 * This file is part of sys-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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

int main(int, char**)
{
    try
    {
        #ifdef _MSC_VER
        #pragma warning(push)
        #pragma warning(disable: 4996) // '...': This function or variable may be unsafe. Consider using fopen_s instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details.
        #endif

        // open non-existant file
        FILE* f = fopen("supercalifragilisticexpialidocious.tmpl", "r");

        #ifdef _MSC_VER
        #pragma warning(pop)
        #endif

        
        if (!f) throw except::Exception(Ctxt("File not Found! That's weird!?"));
    }
    catch (...)
    {
        sys::Err err;
        sys::Err copyErr (err);
        sys::Err assignErr = err;
        
        std::cout << "Default Constructed Error        : " << err.toString() << std::endl;
        std::cout << "Copy Constructed Error           : " << copyErr.toString() << std::endl;
        std::cout << "Assignment Constructed Error     : " << assignErr.toString() << std::endl;

        sys::SocketErr socErr;
        std::cout << "Default Constructed Socket Error : " << socErr.toString() << std::endl;
    }
}


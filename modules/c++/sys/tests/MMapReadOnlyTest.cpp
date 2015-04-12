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

#include <import/sys.h>
#include <import/except.h>

int main(int argc, char** argv)
{

    if (argc != 2)
        die_printf("Usage: %s <file-to-map>\n", argv[0]);

    try
    {
        std::string fname = argv[1];
        sys::OS os;

        sys::Handle_T handle;
        size_t size = os.getSize(fname);
        FILE* file = fopen(fname.c_str(), "r+b");
        handle = fileno(file);

        char* memLoc = (char*) os.mapFile(handle, size, PROT_READ, MAP_SHARED,
                                          0);

        for (int i = 0; i < size; i++)
            std::cout << memLoc[i];

        os.unmapFile(memLoc, size);

        fclose(file);
        return 0;
    }
    catch (except::Throwable& t)
    {
        std::cout << t.toString() << std::endl;
    }

}

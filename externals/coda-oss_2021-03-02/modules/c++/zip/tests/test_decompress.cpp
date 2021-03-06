/* =========================================================================
 * This file is part of zip-c++
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2016, MDA Information Systems LLC
 *
 * zip-c++ is free software; you can redistribute it and/or modify
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
#include <import/io.h>
#include <import/zip.h>

int main(int argc, char** argv)
{
    if (argc != 3)
        die_printf("Usage: %s <gz-file> <out-file>\n", argv[0]);

    try
    {
        std::string inputName(argv[1]);
        std::string outputName(argv[2]);

        std::cout << "Attempting to unzip: " 
                  << std::endl << "\tInput: " << inputName << std::endl
                  << "\tTarget: " << outputName << std::endl;;
        
        zip::GZipInputStream input(inputName);
        io::FileOutputStream output(outputName);
        while ( input.streamTo(output, 8192) );

        input.close();
        output.close();
    }
    catch (except::Exception& ex)
    {
        std::cout << ex.toString() << std::endl;
        exit(EXIT_FAILURE);
    }
    return 0;
}


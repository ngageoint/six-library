/* =========================================================================
 * This file is part of tiff-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * tiff-c++ is free software; you can redistribute it and/or modify
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
#include <import/io.h>
#include <import/sys.h>
#include <import/tiff.h>
#include <fstream>

int main(int argc, char **argv)
{
    try
    {
        if (argc < 2)
            throw except::Exception(str::Format("usage: %s <tiff file>", argv[0]));

        sys::OS os;
        std::string path = sys::Path::absolutePath(argv[1]);
        if (!os.exists(path))
            throw except::FileNotFoundException(path);

        io::StandardOutStream outStream;
        tiff::FileReader reader(path);
        reader.print(outStream);

        for (sys::Uint32_T i = 0, n = reader.getImageCount(); i < n; ++i)
        {
            if (tiff::Utils::hasGeoTiffIFD(reader[i]->getIFD()))
            {
                outStream.writeln("===========================");
                outStream.writeln(str::Format("GeoTIFF detected: Image %d\n", (i + 1)));
                tiff::IFD *geoIFD =
                        tiff::Utils::createGeoTiffIFD(reader[i]->getIFD());
                geoIFD->print(outStream);
                outStream.writeln("===========================");
                delete geoIFD;
            }
        }
    }
    catch (except::Throwable& t)
    {
        std::cerr << t.toString() << std::endl;
        exit( EXIT_FAILURE);
    }
    catch (...)
    {
        std::cerr << "Caught unnamed exception" << std::endl;
        exit( EXIT_FAILURE);
    }
    return 0;
}

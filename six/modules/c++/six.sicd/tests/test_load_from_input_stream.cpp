/* =========================================================================
* This file is part of six.sicd-c++
* =========================================================================
*
* (C) Copyright 2004 - 2016, MDA Information Systems LLC
*
* six.sicd-c++ is free software; you can redistribute it and/or modify
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

#include <iostream>
#include <stdexcept>
#include <memory>
#include <complex>

#include <sys/Path.h>
#include <except/Exception.h>
#include <io/StandardStreams.h>
#include <six/Utilities.h>
#include <six/sicd/ComplexXMLControl.h>
#include <six/sicd/Utilities.h>
#include <six/sicd/ComplexData.h>
#include <nitf/IOStreamReader.hpp>

#include <sys/Filesystem.h>
namespace fs = std::filesystem;

int main(int argc, char** argv)
{
    try
    {
        // Parse the command line
        const std::string progname(argv[0]);
        if (argc != 2 && argc != 3)
        {
            std::cerr << "Usage: " << fs::path(progname).filename().string()
                << " <SICD pathname> [<schema dirname>]\n\n";
            return 1;
        }

        const std::string sicdPathname(argv[1]);

        std::vector<std::string> schemaPaths;
        if (argc == 3)
        {
            schemaPaths.push_back(argv[2]);
        }
        else
        {
            schemaPaths.push_back(six::findSchemaPath(progname));
        }

        std::unique_ptr<six::sicd::ComplexData> fileComplexData;
        std::vector<std::complex<float> > fileWidebandData;
        six::sicd::Utilities::readSicd(sicdPathname, schemaPaths,
            fileComplexData, fileWidebandData);

        std::unique_ptr<six::sicd::ComplexData> streamComplexData;
        std::vector<std::complex<float> > streamWidebandData;
        io::FileInputStream stream(sicdPathname);

        six::XMLControlRegistry xmlRegistry;
        xmlRegistry.addCreator(six::DataType::COMPLEX,
            new six::XMLControlCreatorT<
            six::sicd::ComplexXMLControl>());

        six::NITFReadControl reader;
        reader.setXMLControlRegistry(&xmlRegistry);
        reader.load(stream, schemaPaths);

        streamComplexData = six::sicd::Utilities::getComplexData(reader);
        six::sicd::Utilities::getWidebandData(reader, *streamComplexData,
                streamWidebandData);

        if (*streamComplexData != *fileComplexData)
        {
           std::cerr << "Loaded complex data differs\n";
           return 1;
        }
        if (streamWidebandData != fileWidebandData)
        {
            std::cerr << "Loaded wideband data differs\n";
            return 1;
        }
        std::cout << "Test passed!" << std::endl;
        return 0;
    }
    catch (const except::Exception& e)
    {
        std::cerr << e.getMessage() << std::endl;
        return 1;
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    catch (...)
    {
        std::cerr << "Unknown exception" << std::endl;
        return 1;
    }
}


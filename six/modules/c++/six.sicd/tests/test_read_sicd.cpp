/* =========================================================================
 * This file is part of six-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * six-c++ is free software; you can redistribute it and/or modify
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

#include <sys/Filesystem.h>
namespace fs = sys::Filesystem;

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

        std::unique_ptr<six::sicd::ComplexData> complexData;
        std::vector<std::complex<float> > widebandData;
        six::sicd::Utilities::readSicd(sicdPathname, schemaPaths,
                complexData, widebandData);

        std::cout << "Loaded!" << std::endl;

        // We're just going to take the average of some data
        // to show that we've loaded things...
        size_t nl = complexData->getNumRows();
        size_t ne = complexData->getNumCols();

        double sumMag   = 0.0;
        double sumPhase = 0.0;
        for(size_t ii = 0; ii < nl*ne; ++ii)
        {
            sumMag   += std::abs(widebandData[ii]);
            sumPhase += std::arg(widebandData[ii]);
        }

        std::cout << "Average pixel magnitude: " << sumMag / (nl*ne) << std::endl;
        std::cout << "Average pixel phase: " << sumPhase / (nl*ne) << std::endl;

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


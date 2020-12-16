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

#include <stdexcept>
#include <sstream>

#include <sys/Path.h>
#include <except/Exception.h>
#include <six/NITFReadControl.h>

#include <sys/Filesystem.h>
namespace fs = std::filesystem;

namespace
{
void testDataType(const std::string& pathname, six::DataType expectedDataType)
{
    six::NITFReadControl readControl;

    const six::DataType dataType = readControl.getDataType(pathname);

    if (dataType != expectedDataType)
    {
        std::ostringstream ostr;
        ostr << "Expected " << pathname << " to be "
             << expectedDataType.toString() << " but got "
             << dataType.toString();

        throw except::Exception(Ctxt(ostr.str()));
    }
}
}

int main(int argc, char** argv)
{
    try
    {
        // Parse the command line
        if (argc != 2)
        {
            std::cerr << "Usage: " << fs::path(argv[0]).filename().string()
                      << " <samples/nitf directory>\n\n";
            return 1;
        }
        const fs::path samplesDir(argv[1]);

        testDataType(samplesDir / "sicd_0.4.1.nitf", six::DataType::COMPLEX);
        testDataType(samplesDir / "sicd_1.1.0.nitf", six::DataType::COMPLEX);
        testDataType(samplesDir / "sidd_1.0.0.nitf", six::DataType::DERIVED);
        testDataType(samplesDir / "empty_file", six::DataType::NOT_SET);

        std::cout << "All data types matched expected values" << std::endl;

        return 0;
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
        return 1;
    }
    catch (const except::Exception& ex)
    {
        std::cerr << ex.toString() << std::endl;
        return 1;
    }
    catch (...)
    {
        std::cerr << "Unknown exception\n";
        return 1;
    }
}

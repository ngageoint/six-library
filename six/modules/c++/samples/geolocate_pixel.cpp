/* =========================================================================
 * This file is part of six-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2017, MDA Information Systems LLC
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

#include <exception>
#include <iostream>

#include <cli/ArgumentParser.h>
#include <cli/Results.h>
#include <import/except.h>
#include <import/six/sicd.h>
#include "utils.h"


int main(int argc, char** argv)
{
    try
    {
        cli::ArgumentParser parser;
        parser.setDescription("Given a SICD, and a pixel in the corresponding "
                              "output plane, display the lat/lon/alt");
        parser.addArgument("-s --schemaPath",
                           "Specify a schema or directory of schemata",
                           cli::STORE, "schema", "FILE");
        parser.addArgument("-r --rotateToShadowsDown",
                           "Specify whether to rotate to shadows-down",
                           cli::STORE, "shadowsDown", "SHADOWS")->setDefault(true);
        parser.addArgument("input", "Input SICD/SIDD file", cli::STORE, "input",
                           "INPUT", 1, 1);
        parser.addArgument("row", "Pixel row location", cli::STORE, "row",
                           "ROW", 1, 1);
        parser.addArgument("col", "Pixel col location", cli::STORE, "col",
                           "COL", 1, 1);
        const std::auto_ptr<cli::Results>
            options(parser.parse(argc, (const char**) argv));

        sys::OS os;
        const std::string sicdPathname(options->get<std::string>("input"));
        if (!os.exists(sicdPathname))
        {
            std::cerr << sicdPathname << " does not exist.\n";
            return 1;
        }

        std::vector<std::string> schemaPaths;
        getSchemaPaths(*options, "--schemaPath", "schema", schemaPaths);

        const double row = options->get<double>("row");
        const double col = options->get<double>("col");
        const six::RowColDouble rowCol(row, col);

        six::XMLControlRegistry xmlRegistry;
        xmlRegistry.addCreator(six::DataType::COMPLEX,
                new six::XMLControlCreatorT<six::sicd::ComplexXMLControl>());

        std::auto_ptr<six::sicd::ComplexData> complexData;
        std::vector<std::complex<float> > widebandData;
        six::sicd::Utilities::readSicd(sicdPathname, schemaPaths, complexData,
                widebandData);

        const bool shadowsDown(options->get<bool>("shadowsDown"));
        if (!six::sicd::AreaPlaneUtility::hasAreaPlane(*complexData))
        {
            six::sicd::AreaPlaneUtility::setAreaPlane(*complexData);
        }

        six::sicd::AreaPlane& plane = *complexData->radarCollection->area->plane;
        if (shadowsDown)
        {
            plane.rotateCCW();
        }

        const six::RowColDouble spacing(plane.xDirection->spacing,
                plane.yDirection->spacing);
        const scene::PlanarGridECEFTransform gridTransform(
                spacing,
                plane.getAdjustedReferencePoint(),
                plane.xDirection->unitVector,
                plane.yDirection->unitVector,
                plane.referencePoint.ecef);
        const six::Vector3 ecef = gridTransform.rowColToECEF(rowCol);
        const scene::ECEFToLLATransform transformer;
        const six::LatLonAlt lla = transformer.transform(ecef);
        std::cout << "Latitude: " <<  lla.getLat() << "\n"
            << "Longitude: " << lla.getLon() << "\n"
            << "Altitude: " << lla.getAlt() << "\n";
        return 0;
    }
    catch(const except::Exception& ex)
    {
        std::cerr << ex.toString() << std::endl;
    }
    catch(const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
    }
    catch(...)
    {
        std::cerr << "An unknown exception occured\n";
    }
    return 1;
}

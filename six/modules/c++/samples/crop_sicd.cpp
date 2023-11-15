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
#include <six/sicd/CropUtils.h>

#include <memory>
#include <iostream>
#include <stdexcept>

#include <std/filesystem>

#include <six/sicd/CropUtils.h>

#include <cli/ArgumentParser.h>
#include <except/Exception.h>
#include <str/Convert.h>
#include <str/Manip.h>
#include <six/sicd/ComplexXMLControl.h>
#include <six/XMLControlFactory.h>
#include "utils.h"

namespace fs = std::filesystem;

namespace
{
void parseECEF(cli::Results& options, std::vector<scene::Vector3>& ecef)
{
    const cli::Value* const value = options.getValue("ecef");
    ecef.resize(value->size());

    for (size_t ii = 0; ii < value->size(); ++ii)
    {
        const std::vector<std::string> values =
                str::split(value->get<std::string>(ii), ",");
        if (values.size() != 3)
        {
            throw except::Exception(Ctxt("Expected 3 values but got " +
                    std::to_string(values.size())));
        }

        scene::Vector3& ecefCur(ecef[ii]);
        for (size_t jj = 0; jj < 3; ++jj)
        {
            ecefCur[jj] = str::toType<double>(values[jj]);
        }
    }
}

void parseLatLon(cli::Results& options, std::vector<scene::LatLonAlt>& latLon)
{
    const cli::Value* const value = options.getValue("latlon");
    latLon.resize(value->size());

    for (size_t ii = 0; ii < value->size(); ++ii)
    {
        const std::vector<std::string> values =
                str::split(value->get<std::string>(ii), ",");
        if (values.size() != 2 && values.size() != 3)
        {
            throw except::Exception(Ctxt("Expected 2 or 3 values but got " +
                    std::to_string(values.size())));
        }

        scene::LatLonAlt& latLonCur(latLon[ii]);
        latLonCur.setLat(str::toType<double>(values[0]));
        latLonCur.setLon(str::toType<double>(values[1]));
        if (values.size() == 3)
        {
            latLonCur.setAlt(str::toType<double>(values[2]));
        }
    }
}
}

int main(int argc, char** argv)
{
    try
    {
        const fs::path argv0(argv[0]);
        const auto progDirname = argv0.parent_path();
        const auto installPath = fs::absolute(progDirname.parent_path());
        const auto nitroDir = installPath / "share" / "nitf" / "plugins";
        const auto schemaDir = installPath / "conf" / "schema" / "six";

        // Parse the command line
        cli::ArgumentParser parser;
        parser.setDescription("This program reads a SICD and crops it based "
                              "on a specified AOI which may be specified in "
                              "pixels, ECEF, or lat/lon");
        parser.addArgument("--start-row", "Start row", cli::STORE,
                           "sRow", "#");
        parser.addArgument("--start-col", "Start column", cli::STORE,
                           "sCol", "#");
        parser.addArgument("--num-rows", "Number of rows", cli::STORE,
                           "numRows", "#");
        parser.addArgument("--num-cols", "Number of columns", cli::STORE,
                           "numCols", "#");
        parser.addArgument("--ecef",
                           "Four ECEF coordinates specified as x,y,z tuples",
                           cli::STORE,
                           "ecef", "#,#,#", 4, 4);
        parser.addArgument("--latlon",
                           "Four lat/lon coordinates specified as "
                           "latDegrees,lonDegrees[,altMeters] tuples",
                           cli::STORE,
                           "latlon", "#,#,#", 4, 4);
        parser.addArgument("--schema",
                           "Specify a schema or directory of schemas "
                           "(or set SIX_SCHEMA_PATH). Use \"\" as value to "
                           "skip schema validation.",
                           cli::STORE)->setDefault(schemaDir.string());
        parser.addArgument("--require-aoi-in-bounds",
                           "When the AOI is specified in ECEF or lat/lon, by "
                           "default it will be trimmed to be within the "
                           "pixel boundaries of the image if necessary.  If "
                           "this flag is specified, the program will instead "
                           "exit with an error if the AOI is not completely "
                           "in bounds.",
                           cli::STORE_TRUE,
                           "requireAoiInBounds")->setDefault(false);
        parser.addArgument("input", "Input SICD pathname", cli::STORE,
                           "input", "<input SICD pathname>", 1, 1);
        parser.addArgument("output", "Output SICD pathname", cli::STORE,
                           "output", "<output SICD pathname>", 1, 1);

        const std::unique_ptr<cli::Results> options(parser.parse(argc, argv));
        const std::string inPathname(options->get<std::string> ("input"));
        const std::string outPathname(options->get<std::string> ("output"));
        const bool trimCornersIfNeeded =
                !options->get<bool>("requireAoiInBounds");

        std::vector<std::string> schemaPaths;
        const std::string schemaPath(options->get<std::string>("schema"));
        if (!schemaPath.empty())
        {
            schemaPaths.push_back(schemaPath);
        }

        // Crop it
        six::XMLControlFactory::getInstance().addCreator<six::sicd::ComplexXMLControl>();

        if (options->hasValue("ecef"))
        {
            std::vector<scene::Vector3> corners;
            parseECEF(*options, corners);
            six::sicd::cropSICD(inPathname, schemaPaths, corners,
                                outPathname, trimCornersIfNeeded);
        }
        else if (options->hasValue("latlon"))
        {
            std::vector<scene::LatLonAlt> corners;
            parseLatLon(*options, corners);
            six::sicd::cropSICD(inPathname, schemaPaths, corners,
                                outPathname, trimCornersIfNeeded);
        }
        else
        {
            const types::RowCol<size_t> aoiOffset(
                    options->get<size_t>("sRow"),
                    options->get<size_t>("sCol"));

            const types::RowCol<size_t> aoiDims(
                    options->get<size_t>("numRows"),
                    options->get<size_t>("numCols"));

            six::sicd::cropSICD(inPathname,
                                schemaPaths,
                                aoiOffset,
                                aoiDims,
                                outPathname);
        }

        return 0;
    }
    catch (const except::Exception& ex)
    {
        std::cerr << ex.toString() << std::endl;
        return 1;
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
        return 1;
    }
    catch (...)
    {
        std::cerr << "Unknown exception\n";
        return 1;
    }
}

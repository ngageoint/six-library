/* =========================================================================
 * This file is part of six.sicd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2019, MDA Information Systems LLC
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
#include <six/Utilities.h>
#include <six/sicd/Utilities.h>
#include <six/sicd/ComplexData.h>
#include <import/cli.h>

int main(int argc, char** argv)
{
    try
    {
        // Set up command line arguments.
        cli::ArgumentParser parser;
        parser.addArgument("--vdp", "Use the valid data polygon",
            cli::STORE_TRUE, "useVDP", "useVDP")->setDefault(false);
        parser.addArgument("input", "SICD path", cli::STORE, "sicdPath",
            "sicdPath")->setDefault(""); 

        // Parse the command line.
        std::auto_ptr<cli::Results> options(parser.parse(argc, argv));
        const std::string sicdPath = options->get<std::string>("sicdPath");
        const bool useVDP = options->get<bool>("useVDP");
        
        const std::string progname(argv[0]);
        if (sicdPath.empty())
        {
            std::cerr << "Usage: " << sys::Path::basename(progname)
                      << " <SICD pathname> [--vdp]\n\n";
            return 1;
        }

        std::vector<std::string> schemaPaths; 
        std::auto_ptr<six::sicd::ComplexData> complexData(
            six::sicd::Utilities::getComplexData(sicdPath, schemaPaths));

        const size_t numPoints1D =
            scene::ProjectionPolynomialFitter::DEFAULTS_POINTS_1D;
        std::auto_ptr<scene::ProjectionPolynomialFitter> polyfitter(
            six::sicd::Utilities::getPolynomialFitter(
                *complexData,
                numPoints1D,
                useVDP));

        const types::RowCol<size_t> spPixelStart(0, 0);
        const types::RowCol<double> spSceneCenter(
            complexData->imageData->scpPixel);
        const types::RowCol<double> spSampleSpacing(
            complexData->grid->row->sampleSpacing,
            complexData->grid->col->sampleSpacing);
        const size_t polyOrder = 4;
        six::Poly2D outputRowColToSlantRow;
        six::Poly2D outputRowColToSlantCol;
        double meanResidualErrorRow = 0.0;
        double meanResidualErrorCol = 0.0;

        polyfitter->fitOutputToSlantPolynomials(
                   spPixelStart,
                   spSceneCenter,   
                   spSceneCenter,
                   spSampleSpacing,
                   polyOrder,
                   polyOrder,
                   outputRowColToSlantRow,
                   outputRowColToSlantCol,
                   &meanResidualErrorRow,
                   &meanResidualErrorCol);

        std::cout << "outputRowColToSlantRow:" << outputRowColToSlantRow <<
            std::endl;
        std::cout << "outputRowColToSlantCol:" << outputRowColToSlantCol <<
            std::endl;
        std::cout << "meanResidualErrorRow:" << meanResidualErrorRow <<
            std::endl;
        std::cout << "meanResidualErrorCol:" << meanResidualErrorCol <<
            std::endl;
    }
    catch (const except::Exception& e)
    {
        std::cerr << "Caught exception: " << e.getMessage() << std::endl;
        return 1;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Caught exception: " << e.what() << std::endl;
        return 1;
    }
    catch (...)
    {
        std::cerr << "Caught exception: " << "Unknown exception" << std::endl;
        return 1;
    }

    return 0;
}


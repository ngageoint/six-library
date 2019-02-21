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
//#include <except/Exception.h>
//#include <io/StandardStreams.h>
#include <six/Utilities.h>
//#include <six/sicd/ComplexXMLControl.h>
#include <six/sicd/Utilities.h>
#include <six/sicd/ComplexData.h>

int main(int argc, char** argv)
{
    try
    {
        // Parse the command line
        const std::string progname(argv[0]);
        if (argc != 2 && argc != 3)
        {
            std::cerr << "Usage: " << sys::Path::basename(progname)
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

        std::auto_ptr<six::sicd::ComplexData> complexData(
            six::sicd::Utilities::getComplexData(sicdPathname, schemaPaths));

        std::auto_ptr<scene::ProjectionPolynomialFitter> polyfitter(
            six::sicd::Utilities::getPolynomialFitterVDP(
                *complexData).release());

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
}


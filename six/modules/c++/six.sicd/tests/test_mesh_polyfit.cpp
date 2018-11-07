/* =========================================================================
 * This file is part of six-c++
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2016, MDA Information Systems LLC
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

#include <cstdlib>
#include <ctime>
#include <memory>
#include <iostream>
#include <iomanip>

#include <six/sicd/SICDMesh.h>
#include <six/sicd/Utilities.h>

namespace
{
void populatePlanarCoordinateMeshVectors(const types::RowCol<size_t>& meshDims,
                                         double xMin, double xMax,
                                         double yMin, double yMax,
                                         std::vector<double>& x,
                                         std::vector<double>& y)
{
    x.clear();
    y.clear();

    double xDelta = (xMax - xMin) / ((double) (meshDims.row - 1));
    double yDelta = (yMax - yMin) / ((double) (meshDims.col - 1));
    double xv = xMin;
    for (size_t ri = 0; ri < meshDims.row; ++ri, xv += xDelta)
    {
        double yv = yMin;
        for (size_t ci = 0; ci < meshDims.col; ++ci, yv += yDelta)
        {
            x.push_back(xv);
            y.push_back(yv);
        }
    }
}
}

int main(int argc, char** argv)
{
    const std::string RTNID = "[test_mesh_polyfit] ";
    
    double epsilon = 1e-9;
    double xMax =  10000.0;
    double yMax =  20000.0;
    size_t orderX = 4;
    size_t orderY = 4;
    types::RowCol<size_t> slantMeshDims(7, 9);
    types::RowCol<double> slantSampleSpacing(1.0, 1.0);
    types::RowCol<double> slantCenter(0.0, 0.0);
    types::RowCol<size_t> outputMeshDims(7, 9);
    types::RowCol<double> outputSampleSpacing(1.0, 1.0);
    types::RowCol<double> outputCenter(0.0, 0.0);
    bool printPolys = false;

    for (int ii = 1; ii < argc; ++ii)
    {
        std::string arg(argv[ii]);
        if (arg == "-slant-rows")
        {
            if (ii == argc - 1)
            {
                std::cout << RTNID << "Error: -slant-rows requires a value"
                    << std::endl;
                return 1;
            }

            ++ii;
            slantMeshDims.row = std::atoi(argv[ii]);
        }
        else if (arg == "-slant-cols")
        {
            if (ii == argc - 1)
            {
                std::cout << RTNID << "Error: -slant-cols requires a value"
                    << std::endl;
                return 1;
            }

            ++ii;
            slantMeshDims.row = std::atoi(argv[ii]);
        }
        else if (arg == "-output-rows")
        {
            if (ii == argc - 1)
            {
                std::cout << RTNID << "Error: -output-rows requires a value"
                    << std::endl;
                return 1;
            }

            ++ii;
            outputMeshDims.row = std::atoi(argv[ii]);
        }
        else if (arg == "-output-cols")
        {
            if (ii == argc - 1)
            {
                std::cout << RTNID << "Error: -output-cols requires a value"
                    << std::endl;
                return 1;
            }

            ++ii;
            outputMeshDims.row = std::atoi(argv[ii]);
        }
        else if (arg == "-print-polys")
        {
            printPolys = true;
        }
        else if (arg == "-order-x")
        {
            if (ii == argc - 1)
            {
                std::cout << RTNID << "Error: -order-x requires a value"
                    << std::endl;
                return 1;
            }

            ++ii;
            orderX = std::atoi(argv[ii]);
        }
        else if (arg == "-order-y")
        {
            if (ii == argc - 1)
            {
                std::cout << RTNID << "Error: -order-y requires a value"
                    << std::endl;
                return 1;
            }

            ++ii;
            orderY = std::atoi(argv[ii]);
        }
        else if (arg == "-epsilon")
        {
            if (ii == argc - 1)
            {
                std::cout << RTNID << "Error: -epsilon requires a value"
                    << std::endl;
                return 1;
            }

            ++ii;
            epsilon = std::atof(argv[ii]);
        }
        else if (arg == "-slant-x-max")
        {
            if (ii == argc - 1)
            {
                std::cout << RTNID << "Error: -slant-x-max requires a value"
                    << std::endl;
                return 1;
            }

            ++ii;
            xMax = std::atof(argv[ii]);
        }
        else if (arg == "-slant-y-max")
        {
            if (ii == argc - 1)
            {
                std::cout << RTNID << "Error: -slant-y-max requires a value"
                    << std::endl;
                return 1;
            }

            ++ii;
            yMax = std::atof(argv[ii]);
        }
        else if (arg == "-slant-sample-spacing-row")
        {
            if (ii == argc - 1)
            {
                std::cout << RTNID << "Error: -slant-sample-spacing-row requires a value"
                    << std::endl;
                return 1;
            }

            ++ii;
            slantSampleSpacing.row = std::atof(argv[ii]);
        }
        else if (arg == "-slant-sample-spacing-col")
        {
            if (ii == argc - 1)
            {
                std::cout << RTNID << "Error: -slant-sample-spacing-col requires a value"
                    << std::endl;
                return 1;
            }

            ++ii;
            slantSampleSpacing.col = std::atof(argv[ii]);
        }
        else if (arg == "-output-sample-spacing-row")
        {
            if (ii == argc - 1)
            {
                std::cout << RTNID << "Error: -output-sample-spacing-row requires a value"
                    << std::endl;
                return 1;
            }

            ++ii;
            outputSampleSpacing.row = std::atof(argv[ii]);
        }
        else if (arg == "-output-sample-spacing-col")
        {
            if (ii == argc - 1)
            {
                std::cout << RTNID << "Error: -output-sample-spacing-col requires a value"
                    << std::endl;
                return 1;
            }

            ++ii;
            outputSampleSpacing.col = std::atof(argv[ii]);
        }
        else if (arg == "-slant-center-row")
        {
            if (ii == argc - 1)
            {
                std::cout << RTNID << "Error: -slant-center-row requires a value"
                    << std::endl;
                return 1;
            }

            ++ii;
            slantCenter.row = std::atof(argv[ii]);
        }
        else if (arg == "-slant-center-col")
        {
            if (ii == argc - 1)
            {
                std::cout << RTNID << "Error: -slant-center-col requires a value"
                    << std::endl;
                return 1;
            }

            ++ii;
            slantCenter.col = std::atof(argv[ii]);
        }
        else if (arg == "-output-center-row")
        {
            if (ii == argc - 1)
            {
                std::cout << RTNID << "Error: -output-center-row requires a value"
                    << std::endl;
                return 1;
            }

            ++ii;
            outputCenter.row = std::atof(argv[ii]);
        }
        else if (arg == "-output-center-col")
        {
            if (ii == argc - 1)
            {
                std::cout << RTNID << "Error: -output-center-col requires a value"
                    << std::endl;
                return 1;
            }

            ++ii;
            outputCenter.col = std::atof(argv[ii]);
        }
        else
        {
            std::cout << RTNID << "Error: Unknown argument: " << 
                arg << std::endl;
            return 1;
        }
    }

    std::cout << "xMax: " << xMax << std::endl;
    std::cout << "yMax: " << yMax << std::endl;
    std::cout << "slantSampleSpacing: " << slantSampleSpacing.row << ":" <<
        slantSampleSpacing.col << std::endl;
    std::cout << "slantCenter: " << slantCenter.row << ":" <<
        slantCenter.col << std::endl;
    std::cout << "outputSampleSpacing: " << outputSampleSpacing.row << ":" <<
        outputSampleSpacing.col << std::endl;
    std::cout << "outputCenter: " << outputCenter.row << ":" <<
        outputCenter.col << std::endl;

    // Extent in meters.
    double xMin = -xMax;
    double yMin = -yMax;

    // Populate the slant plane mesh.
    std::vector<double> x;
    std::vector<double> y;

    populatePlanarCoordinateMeshVectors(slantMeshDims, xMin, xMax, yMin, yMax, x, y);
    six::sicd::PlanarCoordinateMesh slantMesh("Slant plane coordinate mesh",
        slantMeshDims, x, y);

    // Populate the output plane mesh - make it the same as the slant mesh.
    populatePlanarCoordinateMeshVectors(outputMeshDims, xMin, xMax, yMin, yMax, x, y);
    six::sicd::PlanarCoordinateMesh outputMesh("Output plane coordinate mesh",
        outputMeshDims, x, y);

    six::Poly2D outputXYToSlantX;               
    six::Poly2D outputXYToSlantY;             
    six::Poly2D slantXYToOutputX;                 
    six::Poly2D slantXYToOutputY;       

    try
    {
        // Compute (x,y) to (x,y) projection polynomials
        six::sicd::Utilities::fitXYProjectionPolys(
                outputMesh,
                slantMesh,
                orderX,
                orderY,
                outputXYToSlantX,                  
                outputXYToSlantY,                  
                slantXYToOutputX,                  
                slantXYToOutputY);
    }
    catch(except::Exception& e)
    {
        std::cout << "Caught exception: " << e.getMessage() << std::endl;
        return 1;
    }

    double val = std::abs(outputXYToSlantX(0,0));
    std::string pf = val <= epsilon ? "passed" : "failed";
    std::cout << "outputXYToSlantX(0,0): " << pf << " : " << val << std::endl;

    val = std::abs(outputXYToSlantX[0][1]);
    pf = val <= epsilon ? "passed" : "failed";
    std::cout << "outputXYToSlantX[0][1]: " << pf << " : " << val << std::endl;

    val = std::abs(outputXYToSlantX[1][0] - 1.0);
    pf = val <= epsilon ? "passed" : "failed";
    std::cout << "outputXYToSlantX[1][0] - 1.0: " << pf << " : " << val << std::endl;

    val = std::abs(outputXYToSlantY(0,0));
    pf = val <= epsilon ? "passed" : "failed";
    std::cout << "outputXYToSlantY(0,0): " << pf << " : " << val << std::endl;

    val = std::abs(outputXYToSlantY[1][0]);
    pf = val <= epsilon ? "passed" : "failed";
    std::cout << "outputXYToSlantY[1][0]: " << pf << " : " << val << std::endl;

    val = std::abs(outputXYToSlantY[0][1] - 1.0);
    pf = val <= epsilon ? "passed" : "failed";
    std::cout << "outputXYToSlantY[0][1] - 1.0: " << pf << " : " << val << std::endl;

    val = std::abs(slantXYToOutputX(0,0));
    pf = val <= epsilon ? "passed" : "failed";
    std::cout << "slantXYToOutputX(0,0): " << pf << " : " << val << std::endl;

    val = std::abs(slantXYToOutputX[0][1]);
    pf = val <= epsilon ? "passed" : "failed";
    std::cout << "slantXYToOutputX[0][1]: " << pf << " : " << val << std::endl;

    val = std::abs(slantXYToOutputX[1][0] - 1.0);
    pf = val <= epsilon ? "passed" : "failed";
    std::cout << "slantXYToOutputX[1][0] - 1.0: " << pf << " : " << val << std::endl;

    val = std::abs(slantXYToOutputY(0,0));
    pf = val <= epsilon ? "passed" : "failed";
    std::cout << "slantXYToOutputY(0,0): " << pf << " : " << val << std::endl;

    val = std::abs(slantXYToOutputY[1][0]);
    pf = val <= epsilon ? "passed" : "failed";
    std::cout << "slantXYToOutputY[1][0]: " << pf << " : " << val << std::endl;

    val = std::abs(slantXYToOutputY[0][1] - 1.0);
    pf = val <= epsilon ? "passed" : "failed";
    std::cout << "slantXYToOutputY[0][1] - 1.0: " << pf << " : " << val << std::endl;

    // Compute the output (row, col) to slant (row, col) projection polynomials.
    six::Poly2D outputRowColToSlantRow;
    six::Poly2D outputRowColToSlantCol;               
    six::sicd::Utilities::transformXYProjectionPolys(
            outputXYToSlantX,
            outputXYToSlantY,
            slantSampleSpacing,
            outputSampleSpacing,
            slantCenter,
            outputCenter,
            outputRowColToSlantRow,
            outputRowColToSlantCol);

    val = std::abs(outputRowColToSlantRow(outputCenter.row, outputCenter.col) -
        slantCenter.row);
    pf = val <= epsilon ? "passed" : "failed";
    std::cout << "outputRowColToSlantRow(outputCenter) - slantCenter.row: " << 
        pf << " : " << val << std::endl;

    val = std::abs(outputRowColToSlantCol(outputCenter.row, outputCenter.col) -
        slantCenter.col);
    pf = val <= epsilon ? "passed" : "failed";
    std::cout << "outputRowColToSlantCol(outputCenter) - slantCenter.col: " << 
        pf << " : " << val << std::endl;

    // Compute the slant (row, col) to output (row, col) projection polynomials.
    six::Poly2D slantRowColToOutputRow;                 
    six::Poly2D slantRowColToOutputCol;                 
    six::sicd::Utilities::transformXYProjectionPolys(
            slantXYToOutputX,
            slantXYToOutputY,
            outputSampleSpacing,
            slantSampleSpacing,
            outputCenter,
            slantCenter,
            slantRowColToOutputRow,
            slantRowColToOutputCol);

    val = std::abs(slantRowColToOutputRow(slantCenter.row, slantCenter.col) -
        outputCenter.row);
    pf = val <= epsilon ? "passed" : "failed";
    std::cout << "slantRowColToOutputRow(slantCenter) - outputCenter.row: " << 
        pf << " : " << val << std::endl;

    val = std::abs(slantRowColToOutputCol(slantCenter.row, slantCenter.col) -
        outputCenter.col);
    pf = val <= epsilon ? "passed" : "failed";
    std::cout << "slantRowColToOutputCol(slantCenter) - outputCenter.col: " << 
        pf << " : " << val << std::endl;

    if (printPolys)
    {
        std::cout << std::endl;
        std::cout << "Polynomials:" << std::endl;
        std::cout << "outputXYToSlantX: " << outputXYToSlantX << std::endl;
        std::cout << "outputXYToSlantY: " << outputXYToSlantY << std::endl;
        std::cout << "slantXYToOutputX: " << slantXYToOutputX << std::endl;
        std::cout << "slantXYToOutputY: " << slantXYToOutputY << std::endl;
        std::cout << "outputRowColToSlantRow: " << outputRowColToSlantRow << std::endl;
        std::cout << "outputRowColToSlantCol: " << outputRowColToSlantCol << std::endl;
        std::cout << "slantRowColToOutputRow: " << slantRowColToOutputRow << std::endl;
        std::cout << "slantRowColToOutputCol: " << slantRowColToOutputCol << std::endl;
    }

    return 0;
}


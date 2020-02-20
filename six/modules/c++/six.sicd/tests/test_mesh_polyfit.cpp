/* =========================================================================
 * This file is part of six.sicd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2018, MDA Information Systems LLC
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

#include <cstdlib>
#include <ctime>
#include <memory>
#include <iostream>
#include <iomanip>

#include <import/cli.h>
#include <sys/Path.h>
#include <six/sicd/SICDMesh.h>
#include <six/sicd/Utilities.h>

namespace
{
bool evaluate(const std::string& key,
              double value,
              double target,
              double tolerance,
              std::ostringstream& report)
{
    const double diff = std::abs(value - target);
    const bool success = diff <= tolerance;
    const std::string status(success ? "PASS" : "FAIL");
    report << key << " - " << target << " = "
           << diff << " <= " << tolerance << ": "
           << status << std::endl;
    return success;
}

six::sicd::PlanarCoordinateMesh populatePlanarCoordinateMeshVectors(
    const std::string& meshName,
    const types::RowCol<size_t>& meshDims,
    const types::RowCol<double>& extent)
{
    std::vector<double> x;
    std::vector<double> y;
    const types::RowCol<double> delta =
        extent / (meshDims - 1);
    double xv = -0.5 * extent.row;
    for (size_t ri = 0; ri < meshDims.row; ++ri, xv += delta.row)
    {
        double yv = -0.5 * extent.col;
        for (size_t ci = 0; ci < meshDims.col; ++ci, yv += delta.col)
        {
            x.push_back(xv);
            y.push_back(yv);
        }
    }

    return six::sicd::PlanarCoordinateMesh(
        meshName, meshDims, x, y);
}
}

int main(int argc, char** argv)
{
    try
    {
        cli::ArgumentParser parser;
        parser.setDescription(
            "This program sets up slant and output plane meshes over "
            "the same scene extent, fits projection polynomials, "
            "and transforms the fitted polynomials to a "
            "(row, column) coordinate system");
        parser.addArgument(
            "--slant-rows", "Number of rows in slant plane mesh",
            cli::STORE, "slantMeshRows")->setDefault(7);
        parser.addArgument(
            "--slant-cols", "Number of columns in slant plane mesh",
            cli::STORE, "slantMeshCols")->setDefault(9);
        parser.addArgument(
            "--output-rows", "Number of rows in output plane mesh",
            cli::STORE, "outputMeshRows")->setDefault(7);
        parser.addArgument(
            "--output-cols", "Number of columns in output plane mesh",
            cli::STORE, "outputMeshCols")->setDefault(9);
        parser.addArgument(
            "--order-x", "X-order of fitted projection polynomials",
            cli::STORE, "orderX", "orderX", 0, 10)->setDefault(4);
        parser.addArgument(
            "--order-y", "Y-order of fitted projection polynomials",
            cli::STORE, "orderY", "orderY", 0, 10)->setDefault(4);
        parser.addArgument(
            "--extent-x", "Scene extent in meters in row direction",
            cli::STORE, "extentX", "extentX", 1)->setDefault(10000.0);
        parser.addArgument(
            "--extent-y", "Scene extent in meters in column direction",
            cli::STORE, "extentY", "extentY", 1)->setDefault(20000.0);
        parser.addArgument(
            "--slant-row-ss",
            "Slant plane image sample spacing in row direction (meters/pixel)",
            cli::STORE, "slantRowSS", "slantRowSS", 0)->setDefault(1.0);
        parser.addArgument(
            "--slant-col-ss",
            "Slant plane image sample spacing in column direction (meters/pixel)",
            cli::STORE, "slantColSS", "slantColSS", 0)->setDefault(1.0);
        parser.addArgument(
            "--output-row-ss",
            "Output plane image sample spacing in row direction (meters/pixel)",
            cli::STORE, "outputRowSS", "outputRowSS", 0)->setDefault(1.0);
        parser.addArgument(
            "--output-col-ss",
            "Output plane image sample spacing in column direction (meters/pixel)",
            cli::STORE, "outputColSS", "outputColSS", 0)->setDefault(1.0);
        parser.addArgument("--tol", "Floating point tolerance in diff",
                           cli::STORE, "tol", "tol", 0)->setDefault(1.0e-9);
        parser.addArgument("-v", "Enable verbose output",
                           cli::STORE_TRUE, "verbose")->setDefault(false);

        std::auto_ptr<cli::Results> options(parser.parse(argc, argv));
        const double tol = options->get<double>("tol");
        const types::RowCol<size_t> slantMeshDims(
            options->get<size_t>("slantMeshRows"),
            options->get<size_t>("slantMeshCols"));
        const types::RowCol<size_t> outputMeshDims(
            options->get<size_t>("outputMeshRows"),
            options->get<size_t>("outputMeshCols"));
        const types::RowCol<double> sceneExtent(
            options->get<double>("extentX"),
            options->get<double>("extentY"));
        const types::RowCol<double> slantSampleSpacing(
            options->get<double>("slantRowSS"),
            options->get<double>("slantColSS"));
        const types::RowCol<double> outputSampleSpacing(
            options->get<double>("outputRowSS"),
            options->get<double>("outputColSS"));
        const size_t orderX = options->get<size_t>("orderX");
        const size_t orderY = options->get<size_t>("orderY");
        const bool verbose = options->get<bool>("verbose");

        // Center pixel of the slant and output plane images
        const types::RowCol<double> slantCenter =
            sceneExtent / slantSampleSpacing * 0.5;
        const types::RowCol<double> outputCenter =
            sceneExtent / outputSampleSpacing * 0.5;

        if (verbose)
        {
            std::cout << "sceneExtent: (" << sceneExtent.row
                      << "," << sceneExtent.col << ")" << std::endl;
            std::cout << "slantSampleSpacing: (" << slantSampleSpacing.row
                      << "," << slantSampleSpacing.col << ")" << std::endl;
            std::cout << "slantCenter: (" << slantCenter.row
                      << "," << slantCenter.col << ")" << std::endl;
            std::cout << "outputSampleSpacing: (" << outputSampleSpacing.row
                      << "," << outputSampleSpacing.col << ")" << std::endl;
            std::cout << "outputCenter: (" << outputCenter.row
                      << "," << outputCenter.col << ")" << std::endl;
        }

        // Populate the slant plane mesh
        six::sicd::PlanarCoordinateMesh slantMesh =
            populatePlanarCoordinateMeshVectors(
                "Slant plane coordinate mesh",
                slantMeshDims,
                sceneExtent);

        // Populate the output plane mesh
        six::sicd::PlanarCoordinateMesh outputMesh =
            populatePlanarCoordinateMeshVectors(
                "Output plane coordinate mesh",
                outputMeshDims,
                sceneExtent);

        // Compute (x,y) to (x,y) projection polynomials
        six::Poly2D outputXYToSlantX;
        six::Poly2D outputXYToSlantY;
        six::Poly2D slantXYToOutputX;
        six::Poly2D slantXYToOutputY;
        six::sicd::Utilities::fitXYProjectionPolys(
            outputMesh,
            slantMesh,
            orderX,
            orderY,
            outputXYToSlantX,
            outputXYToSlantY,
            slantXYToOutputX,
            slantXYToOutputY);

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

        std::ostringstream oss;
        bool pass = true;

        // The scene extents are identical for the slant and output
        // plane meshes. Verify coefficients are simply linear ramps
        // between the two planes
        pass &= evaluate("outputXYToSlantX[0][1]", outputXYToSlantX[0][1], 0.0, tol, oss);
        pass &= evaluate("outputXYToSlantX[1][0]", outputXYToSlantX[1][0], 1.0, tol, oss);
        pass &= evaluate("outputXYToSlantY[1][0]", outputXYToSlantY[1][0], 0.0, tol, oss);
        pass &= evaluate("outputXYToSlantY[0][1]", outputXYToSlantY[0][1], 1.0, tol, oss);
        pass &= evaluate("slantXYToOutputX[0][1]", slantXYToOutputX[0][1], 0.0, tol, oss);
        pass &= evaluate("slantXYToOutputX[1][0]", slantXYToOutputX[1][0], 1.0, tol, oss);
        pass &= evaluate("slantXYToOutputY[1][0]", slantXYToOutputY[1][0], 0.0, tol, oss);
        pass &= evaluate("slantXYToOutputY[0][1]", slantXYToOutputY[0][1], 1.0, tol, oss);

        // Verify fitted XY polynomials map (0,0) to itself
        pass &= evaluate("outputXYToSlantX(0,0)", outputXYToSlantX(0,0), 0.0, tol, oss);
        pass &= evaluate("outputXYToSlantY(0,0)", outputXYToSlantY(0,0), 0.0, tol, oss);
        pass &= evaluate("slantXYToOutputX(0,0)", slantXYToOutputX(0,0), 0.0, tol, oss);
        pass &= evaluate("slantXYToOutputY(0,0)", slantXYToOutputY(0,0), 0.0, tol, oss);

        // Verify transformed (row,col) polynomials map the center
        // pixels in the slant and output planes to each other
        const double pixelTol = 0.5;
        pass &= evaluate("outputRowColToSlantRow(outputCenter)",
                         outputRowColToSlantRow(outputCenter.row, outputCenter.col),
                         slantCenter.row, pixelTol, oss);
        pass &= evaluate("outputRowColToSlantCol(outputCenter)",
                         outputRowColToSlantCol(outputCenter.row, outputCenter.col),
                         slantCenter.col, pixelTol, oss);
        pass &= evaluate("slantRowColToOutputRow(slantCenter)",
                         slantRowColToOutputRow(slantCenter.row, slantCenter.col),
                         outputCenter.row, pixelTol, oss);
        pass &= evaluate("slantRowColToOutputCol(slantCenter)",
                         slantRowColToOutputCol(slantCenter.row, slantCenter.col),
                         outputCenter.col, pixelTol, oss);

        if (!pass || verbose)
        {
            std::cerr << "Test results:" << std::endl << oss.str() << std::endl;

            std::cout << "Fitted Polynomials:" << std::endl;
            std::cout << "outputXYToSlantX:" << std::endl
                      << outputXYToSlantX << std::endl;
            std::cout << "outputXYToSlantY:" << std::endl
                      << outputXYToSlantY << std::endl;
            std::cout << "slantXYToOutputX:" << std::endl
                      << slantXYToOutputX << std::endl;
            std::cout << "slantXYToOutputY:" << std::endl
                      << slantXYToOutputY << std::endl;

            std::cout << "Transformed Polynomials:" << std::endl;
            std::cout << "outputRowColToSlantRow:" << std::endl
                      << outputRowColToSlantRow << std::endl;
            std::cout << "outputRowColToSlantCol:" << std::endl
                      << outputRowColToSlantCol << std::endl;
            std::cout << "slantRowColToOutputRow:" << std::endl
                      << slantRowColToOutputRow << std::endl;
            std::cout << "slantRowColToOutputCol:" << std::endl
                      << slantRowColToOutputCol << std::endl;
        }

        return (pass ? 0 : 1);
    }
    catch(except::Exception& e)
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


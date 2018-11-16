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

#include <cmath>
#include <cstdlib>
#include <ctime>
#include <memory>
#include <iostream>
#include <iomanip>

#include <import/cli.h>
#include <sys/Path.h>
#include <six/sicd/SICDMesh.h>

namespace
{
void populatePlanarCoordinateMeshVectors(const types::RowCol<size_t>& meshDims,
                                         std::vector<double>& x,
                                         std::vector<double>& y)
{
    x.clear();
    y.clear();

    std::srand(static_cast<unsigned long>(std::time(0)));
    for (size_t ri = 0; ri < meshDims.row; ++ri)
    {
        for (size_t ci = 0; ci < meshDims.col; ++ci)
        {
            x.push_back((double) std::rand() / (double) RAND_MAX);
            y.push_back((double) std::rand() / (double) RAND_MAX);
        }
    }
}

void populateNoiseMeshVectors(const types::RowCol<size_t>& meshDims,
                              std::vector<double>& x,
                              std::vector<double>& y,
                              std::vector<double>& mainBeamNoise,
                              std::vector<double>& azimuthAmbiguityNoise,
                              std::vector<double>& combinedNoise)
{
    populatePlanarCoordinateMeshVectors(meshDims, x, y);

    mainBeamNoise.clear();
    azimuthAmbiguityNoise.clear();
    combinedNoise.clear();

    std::srand(static_cast<unsigned long>(std::time(0)));
    for (size_t ri = 0; ri < meshDims.row; ++ri)
    {
        for (size_t ci = 0; ci < meshDims.col; ++ci)
        {
            mainBeamNoise.push_back((double) std::rand() / (double) RAND_MAX);
            azimuthAmbiguityNoise.push_back((double) std::rand() /
                                            (double) RAND_MAX);
            combinedNoise.push_back((double) std::rand() / (double) RAND_MAX);
        }
    }
}

/*! Compare PlanarCoordinate meshes.
 * \param meshA The first mesh
 * \param meshB The second mesh
 * \param verbose Verbose results output
 * \param[out] diffRows Difference in number of rows.
 * \param[out] diffCols Difference in number of cols.
 * \param[out] maxDiffX Difference in x values.
 * \param[out] maxDiffY Difference in y values.
 */
bool comparePlanarCoordinateMesh(
    const six::sicd::PlanarCoordinateMesh* meshA,
    const six::sicd::PlanarCoordinateMesh* meshB,
    bool verbose,
    size_t& diffRows,
    size_t& diffCols,
    double& maxDiffX,
    double& maxDiffY)
{
    const types::RowCol<size_t>& meshDimsA = meshA->getMeshDims();
    const types::RowCol<size_t>& meshDimsB = meshB->getMeshDims();
    diffRows = meshDimsA.row > meshDimsB.row ?
        meshDimsA.row - meshDimsB.row :
        meshDimsB.row - meshDimsA.row;
    diffCols = meshDimsA.col > meshDimsB.col ?
        meshDimsA.col - meshDimsB.col :
        meshDimsB.col - meshDimsA.col;
    maxDiffX = 0.0;
    maxDiffY = 0.0;

    if (diffRows > 0 || diffCols > 0)
    {
        std::cerr << "PlanarCoordinateMesh comparison failed. "
                  << "Mesh dimensions do not agree:" << std::endl
                  << "    diffRows: " << diffRows << std::endl
                  << "    diffCols: " << diffCols << std::endl;
        return false;
    }

    for (size_t ii = 0; ii < meshA->getMeshDims().area(); ++ii)
    {
        maxDiffX = std::max(maxDiffX,
                            std::abs(meshA->getX()[ii] - meshB->getX()[ii]));
        maxDiffY = std::max(maxDiffY,
                            std::abs(meshA->getY()[ii] - meshB->getY()[ii]));
    }

    if (verbose)
    {
        std::cout << "PlanarCoordinateMesh compare: " << std::endl;
        std::cout << "    diffRows: " << diffRows << std::endl;
        std::cout << "    diffCols: " << diffCols << std::endl;
        std::cout << "    maxDiffX: " << maxDiffX << std::endl;
        std::cout << "    maxDiffY: " << maxDiffY << std::endl;
    }

    return (diffRows == 0 && maxDiffX == 0.0 &&
            diffCols == 0 && maxDiffY == 0.0);
}

/*! Compare Noise meshes.
 * \param meshA The first mesh.
 * \param meshB The second mesh.
 * \param verbose Verbose results output
 */
bool compareNoiseMesh(
    const six::sicd::NoiseMesh* meshA,
    const six::sicd::NoiseMesh* meshB,
    bool verbose)
{
    size_t diffRows;
    size_t diffCols;
    double maxDiffX;
    double maxDiffY;
    const bool coordsMatch = comparePlanarCoordinateMesh(
        meshA, meshB, false, diffRows, diffCols, maxDiffX, maxDiffY);

    if (!coordsMatch)
    {
        std::cerr << "NoiseMesh comparison failed. "
                  << "Coordinates do not agree:" << std::endl
                  << "    diffRows: " << diffRows << std::endl
                  << "    diffCols: " << diffCols << std::endl
                  << "    maxDiffX: " << maxDiffX << std::endl
                  << "    maxDiffY: " << maxDiffY << std::endl;
        return false;
    }

    double maxDiffMainBeamNoise = 0.0;
    double maxDiffAzimuthAmbiguityNoise = 0.0;
    double maxDiffCombinedNoise = 0.0;
    for (size_t ii = 0; ii < meshA->getMeshDims().area(); ++ii)
    {
        maxDiffMainBeamNoise = std::max(
            maxDiffMainBeamNoise,
            std::abs(
                meshA->getMainBeamNoise()[ii] -
                meshB->getMainBeamNoise()[ii]));

        maxDiffAzimuthAmbiguityNoise = std::max(
            maxDiffAzimuthAmbiguityNoise,
            std::abs(
                meshA->getAzimuthAmbiguityNoise()[ii] -
                meshB->getAzimuthAmbiguityNoise()[ii]));

        maxDiffCombinedNoise = std::max(
            maxDiffCombinedNoise,
            std::abs(
                meshA->getCombinedNoise()[ii] -
                meshB->getCombinedNoise()[ii]));
    }

    if (verbose)
    {
        std::cout << "NoiseMesh roundtrip compare: " << std::endl;
        std::cout << "    diffRows: " << diffRows << std::endl;
        std::cout << "    diffCols: " << diffCols << std::endl;
        std::cout << "    maxDiffX: " << maxDiffX << std::endl;
        std::cout << "    maxDiffY: " << maxDiffY << std::endl;
        std::cout << "    maxDiffMainBeamNoise: "
                  << maxDiffMainBeamNoise << std::endl;
        std::cout << "    maxDiffAzimuthAmbiguityNoise: "
                  << maxDiffAzimuthAmbiguityNoise << std::endl;
        std::cout << "    maxDiffCombinedNoise: "
                  << maxDiffCombinedNoise << std::endl;
    }

    return (coordsMatch &&
            maxDiffMainBeamNoise == 0.0 &&
            maxDiffAzimuthAmbiguityNoise == 0.0 &&
            maxDiffCombinedNoise == 0.0);
}

bool roundTripPlanarMesh(const types::RowCol<size_t>& meshDims,
                         bool verbose)
{
    std::vector<double> x;
    std::vector<double> y;
    populatePlanarCoordinateMeshVectors(meshDims, x, y);

    const std::string name("Test plane coordinate mesh");
    six::sicd::PlanarCoordinateMesh serializedMesh(
        name, meshDims, x, y);

    // Serialize the mesh.
    std::vector<sys::byte> serializedValues;
    serializedMesh.serialize(serializedValues);

    // Deserialize the mesh.
    x.clear();
    y.clear();

    six::sicd::PlanarCoordinateMesh deserializedMesh(name);
    const sys::byte* serializedValuesBuffer = &serializedValues[0];
    deserializedMesh.deserialize(serializedValuesBuffer);

    // Compare meshes.
    size_t maxDiffRows;
    size_t maxDiffCols;
    double maxDiffX;
    double maxDiffY;
    return comparePlanarCoordinateMesh(
        &serializedMesh,
        &deserializedMesh,
        verbose,
        maxDiffRows,
        maxDiffCols,
        maxDiffX,
        maxDiffY);
}

bool roundTripNoiseMesh(const types::RowCol<size_t>& meshDims,
                        bool verbose)
{
    // Make a NoiseMesh.
    std::vector<double> x;
    std::vector<double> y;
    std::vector<double> mainBeamNoise;
    std::vector<double> azimuthAmbiguityNoise;
    std::vector<double> combinedNoise;
    populateNoiseMeshVectors(meshDims, x, y, mainBeamNoise,
                             azimuthAmbiguityNoise, combinedNoise);

    const std::string name("Test noise mesh");
    six::sicd::NoiseMesh serializedNoiseMesh(
        name, meshDims, x, y, mainBeamNoise,
        azimuthAmbiguityNoise, combinedNoise);

    // Serialize the mesh.
    std::vector<sys::byte> serializedValues;
    serializedNoiseMesh.serialize(serializedValues);

    // Deserialize the mesh.
    six::sicd::NoiseMesh deserializedNoiseMesh(name);

    const sys::byte* serializedValuesBuffer = &serializedValues[0];
    deserializedNoiseMesh.deserialize(serializedValuesBuffer);

    // Compare meshes.
    return compareNoiseMesh(&serializedNoiseMesh,
                            &deserializedNoiseMesh,
                            verbose);
}
}

int main(int argc, char** argv)
{
    try
    {
        cli::ArgumentParser parser;
        parser.setDescription(
            "This program creates randomized SICD mesh objects and "
            "round-trips them via serialize-deserialize operations.");
        parser.addArgument("--rows", "Number of mesh rows",
                           cli::STORE, "numRows", "numRows",
                           1, 2048, false)->setDefault(7);
        parser.addArgument("--cols", "Number of mesh columns",
                           cli::STORE, "numCols", "numCols",
                           1, 2048, false)->setDefault(9);
        parser.addArgument("-v", "Enable verbose output",
                           cli::STORE_TRUE, "verbose",
                           "verbose")->setDefault(false);

        std::auto_ptr<cli::Results> options(parser.parse(argc, argv));
        const types::RowCol<size_t> meshDims(
            options->get<size_t>("numRows"),
            options->get<size_t>("numCols"));
        const bool verbose = options->get<bool>("verbose");

        if (verbose)
        {
            std::cout << "numRows = " << options->get<size_t>("numRows") << std::endl;
            std::cout << "numCols = " << options->get<size_t>("numCols") << std::endl;
            std::cout << "MeshDims: (" << meshDims.row << "," << meshDims.col << ")" << std::endl;
        }

        bool success = true;
        success &= roundTripPlanarMesh(meshDims, verbose);
        success &= roundTripNoiseMesh(meshDims, verbose);

        return (success ? 0 : 1);
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


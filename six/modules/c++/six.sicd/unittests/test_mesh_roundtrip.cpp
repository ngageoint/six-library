/* =========================================================================
 * This file is part of six.side-c++
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

    std::srand(std::time(0));
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

    std::srand(std::time(0));
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
 * \param[out] diffRows Difference in number of rows.
 * \param[out] diffCols Difference in number of cols.
 * \param[out] diffX Difference in x values.
 * \param[out] diffY Difference in y values.
 */
void comparePlanarCoordinateMesh(
    const six::sicd::PlanarCoordinateMesh* meshA,
    const six::sicd::PlanarCoordinateMesh* meshB,
    size_t& diffRows,
    size_t& diffCols,
    double& diffX,
    double& diffY)
{
    diffRows = std::abs(meshA->getMeshDims().row - meshB->getMeshDims().row);
    diffCols = std::abs(meshA->getMeshDims().col - meshB->getMeshDims().col);
    diffX = 0.0;
    diffY = 0.0;

    if (diffRows > 0 || diffCols > 0)
    {
        return;
    }

    for (size_t ri = 0; ri < meshA->getMeshDims().row; ++ri)
    {
        for (size_t ci = 0; ci < meshA->getMeshDims().col; ++ci)
        {
            size_t idx = ri * meshA->getMeshDims().col + ci;
            diffX = std::max(diffX,
                             std::abs(meshA->getX()[idx] - meshB->getX()[idx]));
            diffY = std::max(diffY,
                             std::abs(meshA->getY()[idx] - meshB->getY()[idx]));
        }
    }
}

/*! Compare Noise meshes.
 * \param meshA The first mesh.
 * \param meshB The second mesh.
 * \param[out] diffRows Difference in number of rows.
 * \param[out] diffCols Difference in number of cols.
 * \param[out] diffX Difference in x values.
 * \param[out] diffY Difference in y values.
 * \param[out] diffMainBeamNoise Difference in MainBeamNoise values.
 * \param[out] diffAzimuthAmbiguityNoise Difference in AzimuthAmbiguityNoise values.
 * \param[out] diffCombinedNoise Difference in CombinedNoise values.
 */
void compareNoiseMesh(
    const six::sicd::NoiseMesh* meshA,
    const six::sicd::NoiseMesh* meshB,
    size_t& diffRows,
    size_t& diffCols,
    double& diffX,
    double& diffY,
    double& diffMainBeamNoise,
    double& diffAzimuthAmbiguityNoise,
    double& diffCombinedNoise)
{
    diffMainBeamNoise = 0.0;
    diffAzimuthAmbiguityNoise = 0.0;
    diffCombinedNoise = 0.0;

    comparePlanarCoordinateMesh(meshA, meshB,
        diffRows, diffCols, diffX, diffY);

    if (diffRows > 0 || diffCols > 0)
    {
        return;
    }

    for (size_t ri = 0; ri < meshA->getMeshDims().row; ++ri)
    {
        for (size_t ci = 0; ci < meshA->getMeshDims().col; ++ci)
        {
            size_t idx = ri * meshA->getMeshDims().col + ci;

            diffMainBeamNoise = std::max(diffMainBeamNoise,
                std::abs(
                    meshA->getMainBeamNoise()[idx] - 
                    meshB->getMainBeamNoise()[idx]));

            diffAzimuthAmbiguityNoise = std::max(diffAzimuthAmbiguityNoise,
                std::abs(
                    meshA->getAzimuthAmbiguityNoise()[idx] - 
                    meshB->getAzimuthAmbiguityNoise()[idx]));

            diffCombinedNoise = std::max(diffCombinedNoise,
                std::abs(
                    meshA->getCombinedNoise()[idx] - 
                    meshB->getCombinedNoise()[idx]));
        }
    }
}
}

int main(int argc, char** argv)
{
    try
    {
        const std::string RTNID = sys::Path::basename(argv[0]);
    
        size_t numRows = 7;
        size_t numCols = 9;

        for (int ii = 1; ii < argc; ++ii)
        {
            std::string arg(argv[ii]);
            if (arg == "-rows")
            {
                if (ii == argc - 1)
                {
                    std::cerr << RTNID << "Error: -rows requires a value"
                              << std::endl;
                    return 1;
                }

                ++ii;
                numRows = std::atoi(argv[ii]);
            }
            else if (arg == "-cols")
            {
                if (ii == argc - 1)
                {
                    std::cerr << RTNID << "Error: -cols requires a value"
                              << std::endl;
                    return 1;
                }

                ++ii;
                numCols = std::atoi(argv[ii]);
            }
            else
            {
                std::cerr << RTNID << "Error: Unknown argument: " << 
                    arg << std::endl;
            }
        }

        // Make a PlanarCoordinateMesh.
        types::RowCol<size_t> meshDims(numRows, numCols);
        std::vector<double> x;
        std::vector<double> y;

        populatePlanarCoordinateMeshVectors(meshDims, x, y);

        std::string name = "Test plane coordinate mesh";
        six::sicd::PlanarCoordinateMesh serializedMesh(name, meshDims, x, y);

        // Serialize the mesh.
        std::vector<sys::byte> serializedValues;
        serializedMesh.serialize(serializedValues);

        // Deserialize the mesh. 
        x.clear();
        y.clear();

        six::sicd::PlanarCoordinateMesh deserializedMesh(name, meshDims, x, y);
        const sys::byte* serializedValuesBuffer = &serializedValues[0];
        deserializedMesh.deserialize(serializedValuesBuffer);

        // Compare meshes.
        size_t maxDiffRows;
        size_t maxDiffCols;
        double maxDiffX;
        double maxDiffY;

        comparePlanarCoordinateMesh(&serializedMesh,
                                    &deserializedMesh,
                                    maxDiffRows,
                                    maxDiffCols,
                                    maxDiffX,
                                    maxDiffY);

        std::cout << "PlanarCoordinateMesh roundtrip compare: " << std::endl;
        std::cout << "    maxDiffRows: " << maxDiffRows << std::endl;
        std::cout << "    maxDiffCols: " << maxDiffCols << std::endl;
        std::cout << "    maxDiffX: " << maxDiffX << std::endl;
        std::cout << "    maxDiffY: " << maxDiffY << std::endl;

        // Make a NoiseMesh.
        std::vector<double> mainBeamNoise;
        std::vector<double> azimuthAmbiguityNoise;
        std::vector<double> combinedNoise;
        populateNoiseMeshVectors(meshDims, x, y, mainBeamNoise,
                                 azimuthAmbiguityNoise, combinedNoise);

        name = "Test noise mesh";
        six::sicd::NoiseMesh serializedNoiseMesh(name, meshDims, x, y,
                                                 mainBeamNoise, azimuthAmbiguityNoise, combinedNoise);

        // Serialize the mesh.
        serializedValues.clear();
        serializedNoiseMesh.serialize(serializedValues);

        // Deserialize the mesh. 
        x.clear();
        y.clear();
        mainBeamNoise.clear();
        azimuthAmbiguityNoise.clear();
        combinedNoise.clear();

        six::sicd::NoiseMesh deserializedNoiseMesh(name, meshDims, x, y,
                                                   mainBeamNoise, azimuthAmbiguityNoise, combinedNoise);

        serializedValuesBuffer = &serializedValues[0];
        deserializedNoiseMesh.deserialize(serializedValuesBuffer);

        // Compare meshes.
        double maxDiffMainBeamNoise;
        double maxDiffAzimuthAmbiguityNoise;
        double maxDiffCombinedNoise;

        compareNoiseMesh(&serializedNoiseMesh,
                         &deserializedNoiseMesh,
                         maxDiffRows,
                         maxDiffCols,
                         maxDiffX,
                         maxDiffY,
                         maxDiffMainBeamNoise,
                         maxDiffAzimuthAmbiguityNoise,
                         maxDiffCombinedNoise);

        std::cout << "NoiseMesh roundtrip compare: " << std::endl;
        std::cout << "    maxDiffRows: " << maxDiffRows << std::endl;
        std::cout << "    maxDiffCols: " << maxDiffCols << std::endl;
        std::cout << "    maxDiffX: " << maxDiffX << std::endl;
        std::cout << "    maxDiffY: " << maxDiffY << std::endl;
        std::cout << "    maxDiffMainBeamNoise: " << maxDiffMainBeamNoise << std::endl;
        std::cout << "    maxDiffAzimuthAmbiguityNoise: " << 
            maxDiffAzimuthAmbiguityNoise << std::endl;
        std::cout << "    maxDiffCombinedNoise: " << maxDiffCombinedNoise << std::endl;

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


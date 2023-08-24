/* =========================================================================
 * This file is part of six.sicd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2019, MDA Information Systems LLC
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
#include <iostream>
#include <vector>

#include <nitf/coda-oss.hpp>
#include <cli/ArgumentParser.h>
#include <six/sicd/Utilities.h>

namespace
{
typedef std::map<std::string, std::vector<double> > ScalarDataMap;

bool compareMeshDims(const std::string& baselineSicdPathname,
                     const six::Mesh& baselineMesh,
                     const std::string& testSicdPathname,
                     const six::Mesh& testMesh)
{
    const types::RowCol<size_t> baselineDims = baselineMesh.getMeshDims();
    const types::RowCol<size_t> testDims = testMesh.getMeshDims();
    if (baselineDims != testDims)
    {
        std::cerr << "Failure - Mesh dimensions do not match: "
                  << baselineSicdPathname << " had dimensions (row = "
                  << baselineDims.row << ", col = " << baselineDims.col << "), "
                  << testSicdPathname << " had dimensions (row = "
                  << testDims.row << ", col = " << testDims.col << ")"
                  << std::endl;

        return false;
    }

    return true;
}

bool compareMeshData(const std::string& baselineSicdPathname,
                     const std::vector<double>& baselineData,
                     const std::string& testSicdPathname,
                     const std::vector<double>& testData,
                     const types::RowCol<size_t>& dims)
{
    if (baselineData.size() != testData.size())
    {
        std::cerr << "Failure - Mesh data sizes do not match: "
                  << baselineSicdPathname << " size = " << baselineData.size()
                  << ", " << testSicdPathname << " size = " << testData.size()
                  << std::endl;

        return false;
    }

    if (baselineData.size() != dims.area())
    {
        std::cerr << "Failure - Dimensions (row = " << dims.row << ", col = "
                  << dims.col << ") do not match num elements in mesh ("
                  << baselineData.size() << " elements)." << std::endl;

        return false;
    }

    bool success = true;
    for (size_t row = 0; row < dims.row; ++row)
    {
        for (size_t col = 0; col < dims.col; ++col)
        {
            const size_t idx = row * dims.col + col;
            if (baselineData[idx] != testData[idx])
            {
                std::cerr << "Failure - Values do not match at row = "
                          << row << ", col = " << col << ": "
                          << baselineSicdPathname << " value = "
                          << baselineData[idx] << ", " << testSicdPathname
                          << " value = " << testData[idx] << std::endl;

                success = false;
            }
        }
    }

    return success;
}

bool comparePlanarCoordinateMeshes(
        const std::string& baselineSicdPathname,
        const six::sicd::PlanarCoordinateMesh& baselineMesh,
        const std::string& testSicdPathname,
        const six::sicd::PlanarCoordinateMesh& testMesh)
{
    std::cout << "Comparing X coordinates..." << std::endl;
    bool success = compareMeshData(
            baselineSicdPathname,
            baselineMesh.getX(),
            testSicdPathname,
            testMesh.getX(),
            testMesh.getMeshDims());

    std::cout << "Comparing Y coordinates..." << std::endl;
    success &= compareMeshData(
            baselineSicdPathname,
            baselineMesh.getY(),
            testSicdPathname,
            testMesh.getY(),
            testMesh.getMeshDims());

    return success;
}

bool compareNoiseMeshes(const std::string& baselineSicdPathname,
                        const six::sicd::NoiseMesh& baselineMesh,
                        const std::string& testSicdPathname,
                        const six::sicd::NoiseMesh& testMesh)
{
    if (!compareMeshDims(baselineSicdPathname,
                         baselineMesh,
                         testSicdPathname,
                         testMesh))
    {
        return false;
    }

    bool success = comparePlanarCoordinateMeshes(
            baselineSicdPathname,
            baselineMesh,
            testSicdPathname,
            testMesh);

    std::cout << "Comparing azimuth ambiguity noise data..." << std::endl;
    success &= compareMeshData(
            baselineSicdPathname,
            baselineMesh.getAzimuthAmbiguityNoise(),
            testSicdPathname,
            testMesh.getAzimuthAmbiguityNoise(),
            testMesh.getMeshDims());

    std::cout << "Comparing combined noise data..." << std::endl;
    success &= compareMeshData(
            baselineSicdPathname,
            baselineMesh.getCombinedNoise(),
            testSicdPathname,
            testMesh.getCombinedNoise(),
            testMesh.getMeshDims());

    std::cout << "Comparing main beam noise data..." << std::endl;
    success &= compareMeshData(
            baselineSicdPathname,
            baselineMesh.getMainBeamNoise(),
            testSicdPathname,
            testMesh.getMainBeamNoise(),
            testMesh.getMeshDims());

    return success;
}

bool compareScalarMeshes(const std::string& baselineSicdPathname,
                         const six::sicd::ScalarMesh& baselineMesh,
                         const std::string& testSicdPathname,
                         const six::sicd::ScalarMesh& testMesh)
{
    if (!compareMeshDims(baselineSicdPathname,
                         baselineMesh,
                         testSicdPathname,
                         testMesh))
    {
        return false;
    }

    bool success = comparePlanarCoordinateMeshes(
            baselineSicdPathname,
            baselineMesh,
            testSicdPathname,
            testMesh);

    const ScalarDataMap& baselineScalars = baselineMesh.getScalars();
    const ScalarDataMap& testScalars = baselineMesh.getScalars();
    if (baselineScalars.size() != testScalars.size())
    {
        std::cerr << "Failure - SICDs do not have the same number of "
                  << "scalar meshes: " << baselineSicdPathname << " has "
                  << baselineScalars.size() << " meshes, " << testSicdPathname
                  << " has " << testScalars.size() << " meshes" << std::endl;

        success = false;
    }

    ScalarDataMap::const_iterator it = baselineScalars.begin();
    for (; it != baselineScalars.end(); ++it)
    {
        std::cout << "Comparing " << it->first << " meshes." << std::endl;
        ScalarDataMap::const_iterator testIt = testScalars.find(it->first);
        if (testIt == testScalars.end())
        {
            std::cerr << "Failure - Mesh not found in " << testSicdPathname
                      << std::endl;
            success = false;
            continue;
        }

        const std::vector<double>& baselineScalarData = it->second;
        const std::vector<double>& testScalarData = testIt->second;
        success &= compareMeshData(
                baselineSicdPathname,
                baselineScalarData,
                testSicdPathname,
                testScalarData,
                testMesh.getMeshDims());
    }

    return success;
}

void readMeshes(const std::string& sicdPathname,
                std::unique_ptr<six::sicd::NoiseMesh>& noiseMesh,
                std::unique_ptr<six::sicd::ScalarMesh>& scalarMesh)
{
    const size_t orderX = 3;
    const size_t orderY = 3;

    std::unique_ptr<six::sicd::ComplexData> complexData;
    std::vector<six::zfloat > widebandData;

    six::Poly2D outputRowColToSlantRow;
    six::Poly2D outputRowColToSlantCol;

    six::sicd::Utilities::readSicd(sicdPathname,
                                   std::vector<std::string>(),
                                   orderX,
                                   orderY,
                                   complexData,
                                   widebandData,
                                   outputRowColToSlantRow,
                                   outputRowColToSlantCol,
                                   noiseMesh,
                                   scalarMesh);
}

std::unique_ptr<cli::Results> parseCommandLine(int argc, char** argv)
{
    cli::ArgumentParser parser;
    parser.setDescription("Reads the full meshes from two SICDs and compares"
                          " them, failing if any of the fields"
                          " differ, succeeding if all fields match.");

    parser.addArgument("baselineSicdPathname",
                       "Specify a baseline input SICD",
                       cli::STORE,
                       "baselineSicdPathname",
                       "SICD",
                       1,
                       1);

    parser.addArgument("testSicdPathname",
                       "Specify a test SICD to compare against the baseline SICD",
                       cli::STORE,
                       "testSicdPathname",
                       "SICD",
                       1,
                       1);

    return std::unique_ptr<cli::Results>(parser.parse(argc, argv));
}
}

int main(int argc, char** argv)
{
    try
    {
        std::unique_ptr<cli::Results> options = parseCommandLine(argc, argv);
        const std::string baselineSicdPathname =
                options->get<std::string>("baselineSicdPathname");

        const std::string testSicdPathname =
                options->get<std::string>("testSicdPathname");

        std::unique_ptr<six::sicd::NoiseMesh> baselineNoiseMesh;
        std::unique_ptr<six::sicd::ScalarMesh> baselineScalarMesh;
        readMeshes(baselineSicdPathname,
                   baselineNoiseMesh,
                   baselineScalarMesh);

        std::unique_ptr<six::sicd::NoiseMesh> testNoiseMesh;
        std::unique_ptr<six::sicd::ScalarMesh> testScalarMesh;
        readMeshes(testSicdPathname,
                   testNoiseMesh,
                   testScalarMesh);

        std::cout << "Comparing Noise Meshes..." << std::endl;
        bool success = compareNoiseMeshes(
                baselineSicdPathname,
                *baselineNoiseMesh,
                testSicdPathname,
                *testNoiseMesh);

        if (baselineScalarMesh.get() && testScalarMesh.get())
        {
            std::cout << "Comparing Scalar Meshes..." << std::endl;
            success &= compareScalarMeshes(
                    baselineSicdPathname,
                    *baselineScalarMesh,
                    testSicdPathname,
                    *testScalarMesh);
        }
        else if (baselineScalarMesh.get() || testScalarMesh.get())
        {
            const std::string missingScalarMeshPathname =
                    baselineScalarMesh.get() ?
                            testSicdPathname : baselineSicdPathname;

            std::cerr << "Failure - scalar mesh not found in "
                      << missingScalarMeshPathname << std::endl;
            success = false;
        }

        if (!success)
        {
            return 1;
        }

        std::cout << "Success - mesh data matches." << std::endl;
        return 0;
    }
    catch (const except::Exception& e)
    {
        std::cerr << e.getMessage() << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
    catch (...)
    {
        std::cerr << "Unknown exception" << std::endl;
    }

    return 1;
}

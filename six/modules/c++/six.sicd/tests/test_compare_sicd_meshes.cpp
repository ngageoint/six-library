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

#include <sys/Conf.h>
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
                return false;
            }
        }
    }

    return true;
}

bool comparePlanarCoordinateMeshes(
        const std::string& baselineSicdPathname,
        const six::sicd::PlanarCoordinateMesh& baselineMesh,
        const std::string& testSicdPathname,
        const six::sicd::PlanarCoordinateMesh& testMesh)
{
    if (!compareMeshDims(baselineSicdPathname,
                         baselineMesh,
                         testSicdPathname,
                         testMesh))
    {
        return false;
    }

    std::cout << "Comparing X coordinates..." << std::endl;
    if (!compareMeshData(baselineSicdPathname,
                         baselineMesh.getX(),
                         testSicdPathname,
                         testMesh.getX(),
                         testMesh.getMeshDims()))
    {
        return false;
    }

    std::cout << "Comparing Y coordinates..." << std::endl;
    if (!compareMeshData(baselineSicdPathname,
                         baselineMesh.getY(),
                         testSicdPathname,
                         testMesh.getY(),
                         testMesh.getMeshDims()))
    {
        return false;
    }

    return true;
}

bool compareNoiseMeshes(const std::string& baselineSicdPathname,
                        const six::sicd::NoiseMesh& baselineMesh,
                        const std::string& testSicdPathname,
                        const six::sicd::NoiseMesh& testMesh)
{

    if (!comparePlanarCoordinateMeshes(baselineSicdPathname,
                                       baselineMesh,
                                       testSicdPathname,
                                       testMesh))
    {
        return false;
    }

    std::cout << "Comparing azimuth ambiguity noise data..." << std::endl;
    if (!compareMeshData(baselineSicdPathname,
                         baselineMesh.getAzimuthAmbiguityNoise(),
                         testSicdPathname,
                         testMesh.getAzimuthAmbiguityNoise(),
                         testMesh.getMeshDims()))
    {
        return false;
    }

    std::cout << "Comparing combined noise data..." << std::endl;
    if (!compareMeshData(baselineSicdPathname,
                         baselineMesh.getCombinedNoise(),
                         testSicdPathname,
                         testMesh.getCombinedNoise(),
                         testMesh.getMeshDims()))
    {
        return false;
    }

    std::cout << "Comparing main beam noise data..." << std::endl;
    if (!compareMeshData(baselineSicdPathname,
                         baselineMesh.getMainBeamNoise(),
                         testSicdPathname,
                         testMesh.getMainBeamNoise(),
                         testMesh.getMeshDims()))
    {
        return false;
    }

    return true;
}

bool compareScalarMeshes(const std::string& baselineSicdPathname,
                         const six::sicd::ScalarMesh& baselineMesh,
                         const std::string& testSicdPathname,
                         const six::sicd::ScalarMesh& testMesh)
{
    if (!comparePlanarCoordinateMeshes(baselineSicdPathname,
                                       baselineMesh,
                                       testSicdPathname,
                                       testMesh))
    {
        return false;
    }

    const ScalarDataMap& baselineScalars = baselineMesh.getScalars();
    const ScalarDataMap& testScalars = baselineMesh.getScalars();
    if (baselineScalars.size() != testScalars.size())
    {
        std::cerr << "Failure - SICDs do not have the same number of "
                  << "scalar meshes: " << baselineSicdPathname << " has "
                  << baselineScalars.size() << " meshes, " << testSicdPathname
                  << " has " << testScalars.size() << " meshes" << std::endl;

        return false;
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
            return false;
        }

        const std::vector<double>& baselineScalarData = it->second;
        const std::vector<double>& testScalarData = testIt->second;
        if (!compareMeshData(baselineSicdPathname,
                             baselineScalarData,
                             testSicdPathname,
                             testScalarData,
                             testMesh.getMeshDims()))
        {
            return false;
        }
    }

    return true;
}

void readMeshes(const std::string& sicdPathname,
                std::auto_ptr<six::sicd::NoiseMesh>& noiseMesh,
                std::auto_ptr<six::sicd::ScalarMesh>& scalarMesh)
{
    const size_t orderX = 4;
    const size_t orderY = 4;

    std::auto_ptr<six::sicd::ComplexData> complexData;
    std::vector<std::complex<float> > widebandData;

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

std::auto_ptr<cli::Results> parseCommandLine(int argc, char** argv)
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
                       1 ,
                       1);

    parser.addArgument("testSicdPathname",
                       "Specify a test SICD to compare against the baseline SICD",
                       cli::STORE,
                       "testSicdPathname",
                       "SICD",
                       1 ,
                       1);

    return std::auto_ptr<cli::Results>(parser.parse(argc, argv));
}
}

int main(int argc, char** argv)
{
    try
    {
        std::auto_ptr<cli::Results> options = parseCommandLine(argc, argv);
        const std::string baselineSicdPathname =
                options->get<std::string>("baselineSicdPathname");

        const std::string testSicdPathname =
                options->get<std::string>("testSicdPathname");

        std::auto_ptr<six::sicd::NoiseMesh> baselineNoiseMesh;
        std::auto_ptr<six::sicd::ScalarMesh> baselineScalarMesh;
        readMeshes(baselineSicdPathname,
                   baselineNoiseMesh,
                   baselineScalarMesh);

        std::auto_ptr<six::sicd::NoiseMesh> testNoiseMesh;
        std::auto_ptr<six::sicd::ScalarMesh> testScalarMesh;
        readMeshes(testSicdPathname,
                   testNoiseMesh,
                   testScalarMesh);

        std::cout << "Comparing Noise Meshes..." << std::endl;
        if (!compareNoiseMeshes(baselineSicdPathname,
                                *baselineNoiseMesh,
                                testSicdPathname,
                                *testNoiseMesh))
        {
            return 1;
        }

        if (baselineScalarMesh.get() && testScalarMesh.get())
        {
            std::cout << "Comparing Scalar Meshes..." << std::endl;
            if (!compareScalarMeshes(baselineSicdPathname,
                                     *baselineScalarMesh,
                                     testSicdPathname,
                                     *testScalarMesh))
            {
                return 1;
            }
        }
        else if (baselineScalarMesh.get() || testScalarMesh.get())
        {
            const std::string missingScalarMeshPathname =
                    baselineScalarMesh.get() ?
                            testSicdPathname : baselineSicdPathname;

            std::cerr << "Failure - scalar mesh not found in "
                      << missingScalarMeshPathname << std::endl;
            return 1;
        }

        std::cout << "Success - mesh data matches." << std::endl;
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

    return 0;
}

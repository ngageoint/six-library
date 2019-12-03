/* =========================================================================
 * This file is part of cphd03-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2016, MDA Information Systems LLC
 *
 * cphd03-c++ is free software; you can redistribute it and/or modify
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

#include <cstdio>
#include <stdlib.h>
#include <iostream>
#include <stdexcept>
#include <string>
#include <memory>

#include <cphd03/CPHDReader.h>
#include <cphd03/CPHDWriter.h>
#include <cphd/Wideband.h>
#include <types/RowCol.h>
#include <cli/ArgumentParser.h>
#include <io/TempFile.h>

#include "TestCase.h"

namespace
{

template<typename T>
std::vector<std::complex<T> > generateData(size_t length)
{
    std::vector<std::complex<T> > data(length);
    srand(0);
    for (size_t ii = 0; ii < data.size(); ++ii)
    {
        float real = static_cast<T>(rand() / 100);
        float imag = static_cast<T>(rand() / 100);
        data[ii] = std::complex<T>(real, imag);
    }
    return data;
}

std::vector<double> generateScaleFactors(size_t length, bool scale)
{
    std::vector<double> scaleFactors(length, 1);

    if (scale)
    {
        for (size_t ii = 0; ii < scaleFactors.size(); ++ii)
        {
            scaleFactors[ii] *= 2;
        }
    }

    return scaleFactors;
}

template<typename T>
void writeCPHD(const std::string& outPathname, size_t numThreads,
        const types::RowCol<size_t> dims,
        const std::vector<std::complex<T> >& writeData)
{
    const size_t numChannels = 1;
    const std::vector<size_t> numVectors(numChannels, dims.row);

    cphd03::Metadata metadata;

    metadata.data.numCPHDChannels = numChannels;
    for (size_t ii = 0; ii < metadata.data.numCPHDChannels; ++ii)
    {
        metadata.data.arraySize.push_back(
                cphd03::ArraySize(dims.row, dims.col));
    }

    //! Must set the sample type
    if (sizeof(writeData[0]) == 2)
    {
        metadata.data.sampleType = cphd::SampleType::RE08I_IM08I;
    }
    else if (sizeof(writeData[0]) == 4)
    {
        metadata.data.sampleType = cphd::SampleType::RE16I_IM16I;
    }
    else if (sizeof(writeData[0]) == 8)
    {
        metadata.data.sampleType = cphd::SampleType::RE32F_IM32F;
    }

    //! We must have a radar mode set
    metadata.collectionInformation.radarMode = cphd::RadarModeType::SPOTLIGHT;

    //! We must have corners set
    for (size_t ii = 0; ii < six::LatLonAltCorners::NUM_CORNERS; ++ii)
    {
        metadata.global.imageArea.acpCorners.getCorner(ii).setLat(0.0);
        metadata.global.imageArea.acpCorners.getCorner(ii).setLon(0.0);
        metadata.global.imageArea.acpCorners.getCorner(ii).setAlt(0.0);
    }

    //! Add a channel parameter. This will write without this but it will
    //  not read.
    cphd03::ChannelParameters param;
    metadata.channel.parameters.push_back(param);

    //! We must set SRP Type
    metadata.srp.srpType = cphd::SRPType::STEPPED;

    //! We must set domain parameters
    metadata.global.domainType = cphd::DomainType::FX;
    metadata.vectorParameters.fxParameters.reset(new cphd03::FxParameters());

    cphd03::VBM vbm(numChannels,
                  numVectors,
                  false,
                  false,
                  false,
                  metadata.global.domainType);

    cphd03::CPHDWriter writer(metadata, outPathname, numThreads);
    writer.writeMetadata(vbm);
    for (size_t ii = 0; ii < numChannels; ++ii)
    {
        writer.writeCPHDData(&writeData[0], dims.area());
    }
    writer.close();
}

std::vector<std::complex<float> > checkData(const std::string& pathname,
        size_t numThreads,
        const std::vector<double>& scaleFactors,
        bool scale,
        const types::RowCol<size_t>& dims)
{
    cphd03::CPHDReader reader(pathname, numThreads);
    cphd::Wideband& wideband = reader.getWideband();
    std::vector<std::complex<float> > readData(dims.area());

    size_t sizeInBytes = readData.size() * sizeof(readData[0]);
    mem::ScopedArray<sys::ubyte> scratchData(new sys::ubyte[sizeInBytes]);
    mem::BufferView<sys::ubyte> scratch(scratchData.get(), sizeInBytes);
    mem::BufferView<std::complex<float> > data(&readData[0], readData.size());

    wideband.read(0, 0, cphd::Wideband::ALL, 0, cphd::Wideband::ALL,
            scaleFactors, numThreads, scratch, data);

    return readData;
}

template<typename T>
bool compareVectors(const std::vector<std::complex<float> >& readData,
                    const std::vector<std::complex<T> >& writeData,
                    const std::vector<double>& scaleFactors,
                    bool scale)
{
    size_t pointsPerScale = readData.size() / scaleFactors.size();
    for (size_t ii = 0; ii < readData.size(); ++ii)
    {
        std::complex<float> val(writeData[ii].real(), writeData[ii].imag());
        if (scale)
        {
            val *= scaleFactors[ii / pointsPerScale];
        }

        if (val != readData[ii])
        {
            std::cerr << "Value mismatch at index " << ii << std::endl;
            return false;
        }
    }
    return true;
}

template<typename T>
bool runTest(bool scale, const std::vector<std::complex<T> >& writeData)
{
    io::TempFile tempfile;
    const size_t numThreads = sys::OS().getNumCPUs();
    const types::RowCol<size_t> dims(128, 128);
    const std::vector<double> scaleFactors =
            generateScaleFactors(dims.row, scale);
    writeCPHD(tempfile.pathname(), numThreads, dims, writeData);
    const std::vector<std::complex<float> > readData =
            checkData(tempfile.pathname(), numThreads, scaleFactors,
            scale, dims);
    return compareVectors(readData, writeData, scaleFactors, scale);
}

TEST_CASE(testUnscaledInt8)
{
    const types::RowCol<size_t> dims(128, 128);
    const std::vector<std::complex<sys::Int8_T> > writeData =
            generateData<sys::Int8_T>(dims.area());
    const bool scale = false;
    TEST_ASSERT(runTest(scale, writeData))
}

TEST_CASE(testScaledInt8)
{
    const types::RowCol<size_t> dims(128, 128);
    const std::vector<std::complex<sys::Int8_T> > writeData =
            generateData<sys::Int8_T>(dims.area());
    const bool scale = true;
    TEST_ASSERT(runTest(scale, writeData));
}
TEST_CASE(testUnscaledInt16)
{
    const types::RowCol<size_t> dims(128, 128);
    const std::vector<std::complex<sys::Int16_T> > writeData =
            generateData<sys::Int16_T>(dims.area());
    const bool scale = false;
    TEST_ASSERT(runTest(scale, writeData))
}

TEST_CASE(testScaledInt16)
{
    const types::RowCol<size_t> dims(128, 128);
    const std::vector<std::complex<sys::Int16_T> > writeData =
            generateData<sys::Int16_T>(dims.area());
    const bool scale = true;
    TEST_ASSERT(runTest(scale, writeData));
}

TEST_CASE(testUnscaledFloat)
{
    const types::RowCol<size_t> dims(128, 128);
    const std::vector<std::complex<float> > writeData =
            generateData<float>(dims.area());
    const bool scale = false;
    TEST_ASSERT(runTest(scale, writeData))
}

TEST_CASE(testScaledFloat)
{
    const types::RowCol<size_t> dims(128, 128);
    const std::vector<std::complex<float> > writeData =
            generateData<float>(dims.area());
    const bool scale = true;
    TEST_ASSERT(runTest(scale, writeData));
}
}

int main(int argc, char** argv)
{
    try
    {
        TEST_CHECK(testUnscaledInt8);
        TEST_CHECK(testScaledInt8);
        TEST_CHECK(testUnscaledInt16);
        TEST_CHECK(testScaledInt16);
        TEST_CHECK(testUnscaledFloat);
        TEST_CHECK(testScaledFloat);
        return 0;
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
        return 1;
    }
    catch (const except::Exception& ex)
    {
        std::cerr << ex.toString() << std::endl;
        return 1;
    }
    catch (...)
    {
        std::cerr << "Unknown exception\n";
        return 1;
    }
}


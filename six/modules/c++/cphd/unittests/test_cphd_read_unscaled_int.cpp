/* =========================================================================
 * This file is part of cphd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2016, MDA Information Systems LLC
 *
 * cphd-c++ is free software; you can redistribute it and/or modify
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

#include <cphd/CPHDReader.h>
#include <cphd/CPHDWriter.h>
#include <types/RowCol.h>
#include <cli/ArgumentParser.h>

#include "TestCase.h"

namespace
{
class TempFile
{
public:
    TempFile();
    ~TempFile();
    std::string pathname() const;
private:
    std::string mName;
};

TempFile::TempFile()
{
    char nameBuffer[L_tmpnam];
    std::tmpnam(nameBuffer);
    mName = nameBuffer;
}

TempFile::~TempFile()
{
    std::remove(mName.c_str());
}

std::string TempFile::pathname() const
{
    return mName;
}


std::vector<std::complex<sys::Int16_T> > generateData(size_t length)
{
    std::vector<std::complex<sys::Int16_T> > data(length);
    srand(0);
    for (size_t ii = 0; ii < data.size(); ++ii)
    {
        float real = static_cast<sys::Int16_T>(rand() / 100);
        float imag = static_cast<sys::Int16_T>(rand() / 100);
        data[ii] = std::complex<sys::Int16_T>(real, imag);
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

void writeCPHD(const std::string& outPathname, size_t numThreads,
        const types::RowCol<size_t> dims,
        const std::vector<std::complex<sys::Int16_T> >& writeData)
{
    const size_t numChannels = 1;
    const std::vector<size_t> numVectors(numChannels, dims.row);

    cphd::Metadata metadata;

    metadata.data.numCPHDChannels = numChannels;
    for (size_t ii = 0; ii < metadata.data.numCPHDChannels; ++ii)
    {
        metadata.data.arraySize.push_back(
                cphd::ArraySize(dims.row, dims.col));
    }

    //! Must set the sample type (complex<int>)
    metadata.data.sampleType = cphd::SampleType::RE16I_IM16I;

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
    cphd::ChannelParameters param;
    metadata.channel.parameters.push_back(param);

    //! We must set SRP Type
    metadata.srp.srpType = cphd::SRPType::STEPPED;

    //! We must set domain parameters
    metadata.global.domainType = cphd::DomainType::FX;
    metadata.vectorParameters.fxParameters.reset(new cphd::FxParameters());

    cphd::VBM vbm(numChannels,
                  numVectors,
                  false,
                  false,
                  false,
                  metadata.global.domainType);

    cphd::CPHDWriter writer(metadata, numThreads);
    writer.writeMetadata(outPathname, vbm);
    for (size_t ii = 0; ii < numChannels; ++ii)
    {
        writer.writeCPHDData(&writeData[0], dims.area());
    }
}

std::vector<std::complex<float> >checkData(const std::string& pathname,
        size_t numThreads,
        const std::vector<double>& scaleFactors,
        bool scale,
        const types::RowCol<size_t>& dims,
        const std::vector<std::complex<sys::Int16_T> >& writtenData)
{
    cphd::CPHDReader reader(pathname, numThreads);
    cphd::Wideband& wideband = reader.getWideband();
    std::vector<std::complex<float> > readData(writtenData.size());

    size_t sizeInBytes = readData.size() * sizeof(readData[0]);
    sys::ubyte scratchData[sizeInBytes];
    mem::BufferView<sys::ubyte> scratch(scratchData, sizeInBytes);
    mem::BufferView<std::complex<float> > data(&readData[0], readData.size());

    wideband.read(0, 0, cphd::Wideband::ALL, 0, cphd::Wideband::ALL,
            scaleFactors, numThreads, scratch, data);

    return readData;
}

bool compareVectors(const std::vector<std::complex<float> >& readData,
                    const std::vector<std::complex<sys::Int16_T> >& writeData,
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

bool runTest(bool scale)
{
    TempFile tempfile;
    const size_t numThreads = sys::OS().getNumCPUs();
    const types::RowCol<size_t> dims(128, 128);
    const std::vector<std::complex<sys::Int16_T> > writeData =
            generateData(dims.area());
    const std::vector<double> scaleFactors =
            generateScaleFactors(dims.row, scale);
    writeCPHD(tempfile.pathname(), numThreads, dims, writeData);
    const std::vector<std::complex<float> > readData =
            checkData(tempfile.pathname(), numThreads, scaleFactors,
            scale, dims, writeData);
    return compareVectors(readData, writeData, scaleFactors, scale);
}

TEST_CASE(testUnscaledInt)
{
    TempFile tempfile;
    const bool scale = false;
    TEST_ASSERT(runTest(scale))
}

TEST_CASE(testScaledInt)
{
    TempFile tempfile;
    const bool scale = false;
    TEST_ASSERT(runTest(scale));
}
}

int main(int argc, char** argv)
{
    try
    {
        TEST_CHECK(testUnscaledInt);
        TEST_CHECK(testScaledInt);
        TempFile tempfile;
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


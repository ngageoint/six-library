/* =========================================================================
 * This file is part of cphd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2019, MDA Information Systems LLC
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
#include <thread>

#include <nitf/coda-oss.hpp>
#include <types/RowCol.h>
#include <io/TempFile.h>
#include <cphd/CPHDWriter.h>
#include <cphd/CPHDReader.h>
#include <cphd/Wideband.h>
#include <cphd/Metadata.h>
#include <cphd/PVP.h>
#include <cphd/PVPBlock.h>
#include <cphd/ReferenceGeometry.h>
#include <cphd/TestDataGenerator.h>
#include <TestCase.h>

/*!
 * Tests write and read of Signal Block
 * Fails if values don't match
 */

template<typename TComplex>
auto generateData(size_t length)
{
    using value_type = typename TComplex::value_type;
    std::vector<TComplex> data(length);
    srand(0);
    for (size_t ii = 0; ii < data.size(); ++ii)
    {
        auto real = static_cast<value_type>(rand() / 100);
        auto imag = static_cast<value_type>(rand() / 100);
        data[ii] = TComplex(real, imag);
    }
    return data;
}

inline std::vector<double> generateScaleFactors(size_t length, bool scale)
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

template<typename TCxVector>
void writeCPHD(const std::string& outPathname, size_t /*numThreads*/,
        const types::RowCol<size_t> dims,
        const TCxVector& writeData,
        cphd::Metadata& metadata,
        cphd::PVPBlock& pvpBlock)
{
    const size_t numChannels = 1;
    const std::vector<size_t> numVectors(numChannels, dims.row);

    for (size_t ii = 0; ii < numChannels; ++ii)
    {
        for (size_t jj = 0; jj < numVectors[ii]; ++jj)
        {
            cphd::setVectorParameters(ii, jj, pvpBlock);
        }
    }

    cphd::CPHDWriter writer(metadata, outPathname);
    writer.writeMetadata(pvpBlock);
    writer.writePVPData(pvpBlock);
    for (size_t ii = 0; ii < numChannels; ++ii)
    {
        writer.writeCPHDData(writeData.data(),dims.area());
    }
}

std::vector<cphd::zfloat > checkData(const std::string& pathname,
        size_t numThreads,
        const std::vector<double>& scaleFactors,
        const types::RowCol<size_t> dims)
{
    cphd::CPHDReader reader(pathname, numThreads);
    const cphd::Wideband& wideband = reader.getWideband();
    std::vector<cphd::zfloat > readData(dims.area());

    size_t sizeInBytes = readData.size() * sizeof(readData[0]);
    std::vector<std::byte> scratchData(sizeInBytes);
    std::span<std::byte> scratch(scratchData.data(), scratchData.size());
    std::span<cphd::zfloat> data(readData.data(), readData.size());

    wideband.read(0, 0, cphd::Wideband::ALL, 0, cphd::Wideband::ALL,
                  scaleFactors, numThreads, scratch, data);

    return readData;
}

template<typename TCxVector>
bool compareVectors(const std::vector<cphd::zfloat >& readData,
                    const TCxVector& writeData,
                    const std::vector<double>& scaleFactors,
                    bool scale)
{
    size_t pointsPerScale = readData.size() / scaleFactors.size();
    for (size_t ii = 0; ii < readData.size(); ++ii)
    {
        cphd::zfloat val(writeData[ii].real(), writeData[ii].imag());
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

template<typename TCxVector>
bool runTest(bool scale, const TCxVector& writeData)
{
    io::TempFile tempfile;
    const size_t numThreads = std::thread::hardware_concurrency();
    const types::RowCol<size_t> dims(128, 128);
    const std::vector<double> scaleFactors =
            generateScaleFactors(dims.row, scale);
    cphd::Metadata meta = cphd::Metadata();
    setUpData(meta, dims, writeData);
    cphd::setPVPXML(meta.pvp);
    cphd::PVPBlock pvpBlock(meta.pvp, meta.data);

    writeCPHD(tempfile.pathname(), numThreads, dims, writeData, meta, pvpBlock);
    const std::vector<cphd::zfloat > readData =
            checkData(tempfile.pathname(), numThreads,
                      scaleFactors, dims);
    return compareVectors(readData, writeData, scaleFactors, scale);
    return true;
}

TEST_CASE(testUnscaledInt8)
{
    const types::RowCol<size_t> dims(128, 128);
    const auto writeData = generateData<cphd::zint8_t>(dims.area());
    const bool scale = false;
    TEST_ASSERT_TRUE(runTest(scale, writeData));
}

TEST_CASE(testScaledInt8)
{
    const types::RowCol<size_t> dims(128, 128);
    const auto writeData = generateData<cphd::zint8_t>(dims.area());
    const bool scale = true;
    TEST_ASSERT_TRUE(runTest(scale, writeData));
}

TEST_CASE(testUnscaledInt16)
{
    const types::RowCol<size_t> dims(128, 128);
    const auto writeData = generateData<cphd::zint16_t>(dims.area());
    const bool scale = false;
    TEST_ASSERT_TRUE(runTest(scale, writeData));
}

TEST_CASE(testScaledInt16)
{
    const types::RowCol<size_t> dims(128, 128);
    const auto writeData =  generateData<cphd::zint16_t>(dims.area());
    const bool scale = true;
    TEST_ASSERT_TRUE(runTest(scale, writeData));
}

TEST_CASE(testUnscaledFloat)
{
    const types::RowCol<size_t> dims(128, 128);
    const auto writeData = generateData<cphd::zfloat>(dims.area());
    const bool scale = false;
    TEST_ASSERT_TRUE(runTest(scale, writeData));
}

TEST_CASE(testScaledFloat)
{
    const types::RowCol<size_t> dims(128, 128);
    const auto writeData = generateData<cphd::zfloat>(dims.area());
    const bool scale = true;
    TEST_ASSERT_TRUE(runTest(scale, writeData));
}

TEST_MAIN(
        TEST_CHECK(testUnscaledInt8);
        TEST_CHECK(testScaledInt8);
        TEST_CHECK(testUnscaledInt16);
        TEST_CHECK(testScaledInt16);
        TEST_CHECK(testUnscaledFloat);
        TEST_CHECK(testScaledFloat);
        )

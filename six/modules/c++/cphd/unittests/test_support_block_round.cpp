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

#include <nitf/coda-oss.hpp>
#include <types/RowCol.h>
#include <io/TempFile.h>
#include <io/FileInputStream.h>
#include <io/FileOutputStream.h>
#include <cphd/CPHDWriter.h>
#include <cphd/CPHDReader.h>
#include <cphd/Metadata.h>
#include <cphd/Data.h>
#include <cphd/PVP.h>
#include <cphd/PVPBlock.h>
#include <cphd/SupportBlock.h>
#include <cphd/ReferenceGeometry.h>
#include <cphd/TestDataGenerator.h>
#include <TestCase.h>

static constexpr size_t NUM_SUPPORT = 3;
static constexpr size_t NUM_ROWS = 3;
static constexpr size_t NUM_COLS = 4;

template<typename T>
std::vector<T> generateSupportData(size_t length)
{
    std::vector<T> data(length);
    srand(0);
    for (size_t ii = 0; ii < data.size(); ++ii)
    {
        data[ii] = rand() % 16;
    }
    return data;
}

template <typename T>
void setSupport(cphd::Data& d)
{
    d.setSupportArray("1.0", NUM_ROWS, NUM_COLS, sizeof(T), 0);
    d.setSupportArray("2.0", NUM_ROWS, NUM_COLS, sizeof(T), NUM_ROWS*NUM_COLS*sizeof(T));
    d.setSupportArray("AddedSupport", NUM_ROWS, NUM_COLS, sizeof(T), 2*NUM_ROWS*NUM_COLS*sizeof(T));
}

template<typename T>
void writeSupportData(const std::string& outPathname, size_t numThreads,
        const std::vector<T>& writeData,
        cphd::Metadata& metadata,
        cphd::PVPBlock& pvpBlock)
{
    const size_t numChannels = 1;
    // Required but doesn't matter
    const std::vector<size_t> numVectors(numChannels, 128);

    for (size_t ii = 0; ii < numChannels; ++ii)
    {
        for (size_t jj = 0; jj < numVectors[ii]; ++jj)
        {
            cphd::setVectorParameters(ii, jj, pvpBlock);
        }
    }
    cphd::CPHDWriter writer(metadata, outPathname, std::vector<std::string>(), numThreads);
    writer.writeMetadata(pvpBlock);
    writer.writeSupportData(writeData.data());
    writer.writePVPData(pvpBlock);
}

std::vector<std::byte> checkSupportData(
        const std::string& pathname,
        size_t /*size*/,
        size_t numThreads)
{
    cphd::CPHDReader reader(pathname, numThreads);
    const cphd::SupportBlock& supportBlock = reader.getSupportBlock();

    std::unique_ptr<std::byte[]> readPtr;
    supportBlock.readAll(numThreads, readPtr);

    std::vector<std::byte> readData(readPtr.get(), readPtr.get() + reader.getMetadata().data.getAllSupportSize());
    return readData;
}

template<typename T>
bool compareVectors(const std::vector<std::byte>& readData,
                    const T* writeData,
                    size_t writeDataSize)
{
    if (writeDataSize * sizeof(T) != readData.size())
    {
        std::cerr << "Size mismatch. Writedata size: "<< writeDataSize * sizeof(T)
                  << "ReadData size: " << readData.size() << "\n";
        return false;
    }
    const std::byte* ptr = reinterpret_cast<const std::byte*>(writeData);
    for (size_t ii = 0; ii < readData.size(); ++ii, ++ptr)
    {
        if (*ptr != readData[ii])
        {
            std::cerr << "Value mismatch at index " << ii << std::endl;
            std::cerr << "readData: " << static_cast<char>(readData[ii]) << " " << "writeData: " << static_cast<char>(*ptr) << "\n";
            return false;
        }
    }
    return true;
}

template<typename T>
bool runTest(const std::vector<T>& writeData)
{
    io::TempFile tempfile;
    const size_t numThreads = 1;
    cphd::Metadata meta = cphd::Metadata();
    cphd::setUpData(meta, types::RowCol<size_t>(128,256), std::vector<cphd::zfloat >());
    setSupport<T>(meta.data);
    cphd::setPVPXML(meta.pvp);
    cphd::PVPBlock pvpBlock(meta.pvp, meta.data);
    writeSupportData(tempfile.pathname(), numThreads, writeData, meta, pvpBlock);
    const std::vector<std::byte> readData =
            checkSupportData(tempfile.pathname(), NUM_SUPPORT*NUM_ROWS*NUM_COLS*sizeof(T), numThreads);

    return compareVectors(readData, writeData.data(), writeData.size());
}

TEST_CASE(testSupportsInt)
{
    const types::RowCol<size_t> dims(NUM_ROWS, NUM_COLS);
    const std::vector<int> writeData =
            generateSupportData<int>(NUM_SUPPORT*dims.area());
    TEST_ASSERT_TRUE(runTest(writeData));
}

TEST_CASE(testSupportsDouble)
{
    const types::RowCol<size_t> dims(NUM_ROWS, NUM_COLS);
    const std::vector<double> writeData =
            generateSupportData<double>(NUM_SUPPORT*dims.area());
    TEST_ASSERT_TRUE(runTest(writeData));
}

TEST_MAIN(
        TEST_CHECK(testSupportsInt);
        TEST_CHECK(testSupportsDouble);
        )

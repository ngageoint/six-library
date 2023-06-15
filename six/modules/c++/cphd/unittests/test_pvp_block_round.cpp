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

#include <thread>

#include <TestCase.h>
#include <cphd/CPHDReader.h>
#include <cphd/CPHDWriter.h>
#include <cphd/Metadata.h>
#include <cphd/PVP.h>
#include <cphd/PVPBlock.h>
#include <cphd/ReferenceGeometry.h>
#include <cphd/TestDataGenerator.h>
#include <cphd/Wideband.h>
#include <io/FileInputStream.h>
#include <io/FileOutputStream.h>
#include <io/TempFile.h>
#include <stdlib.h>
#include <types/RowCol.h>
#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

template <typename TComplex>
auto generateComplexData(size_t length)
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

void setPVPBlock(const types::RowCol<size_t> dims,
                 cphd::PVPBlock& pvpBlock,
                 bool isAmpSF,
                 bool isFxN1,
                 bool isFxN2,
                 bool isTOAE1,
                 bool isTOAE2,
                 bool isSignal,
                 const std::vector<std::string>& addedParams)
{
    const size_t numChannels = 1;
    const std::vector<size_t> numVectors(numChannels, dims.row);

    for (size_t ii = 0; ii < numChannels; ++ii)
    {
        for (size_t jj = 0; jj < numVectors[ii]; ++jj)
        {
            setVectorParameters(ii, jj, pvpBlock);

            if (isAmpSF)
            {
                const double ampSF = cphd::getRandom();
                pvpBlock.setAmpSF(ampSF, ii, jj);
            }
            if (isFxN1)
            {
                const double fxN1 = cphd::getRandom();
                pvpBlock.setFxN1(fxN1, ii, jj);
            }
            if (isFxN2)
            {
                const double fxN2 = cphd::getRandom();
                pvpBlock.setFxN2(fxN2, ii, jj);
            }
            if (isTOAE1)
            {
                const double toaE1 = cphd::getRandom();
                pvpBlock.setTOAE1(toaE1, ii, jj);
            }
            if (isTOAE2)
            {
                const double toaE2 = cphd::getRandom();
                pvpBlock.setTOAE2(toaE2, ii, jj);
            }
            if (isSignal)
            {
                const double signal = cphd::getRandom();
                pvpBlock.setTOAE2(signal, ii, jj);
            }

            for (size_t idx = 0; idx < addedParams.size(); ++idx)
            {
                const double val = cphd::getRandom();
                pvpBlock.setAddedPVP(val, ii, jj, addedParams[idx]);
            }
        }
    }
}

template <typename T>
void writeCPHD(const std::string& outPathname,
               size_t numThreads,
               const types::RowCol<size_t> dims,
               const std::vector<types::complex<T>>& writeData,
               cphd::Metadata& metadata,
               cphd::PVPBlock& pvpBlock)
{
    const size_t numChannels = 1;

    cphd::CPHDWriter writer(metadata,
                            outPathname,
                            std::vector<std::string>(),
                            numThreads);
    writer.writeMetadata(pvpBlock);
    writer.writePVPData(pvpBlock);
    for (size_t ii = 0; ii < numChannels; ++ii)
    {
        writer.writeCPHDData(writeData.data(), dims.area());
    }
}

bool checkData(const std::string& pathname,
               size_t numThreads,
               cphd::Metadata& metadata,
               cphd::PVPBlock& pvpBlock)
{
    cphd::CPHDReader reader(pathname, numThreads);

    if (metadata.pvp != reader.getMetadata().pvp)
    {
        return false;
    }
    if (pvpBlock != reader.getPVPBlock())
    {
        return false;
    }
    return true;
}

template <typename T>
bool runTest(bool /*scale*/,
             const std::vector<types::complex<T>>& writeData,
             cphd::Metadata& meta,
             cphd::PVPBlock& pvpBlock,
             const types::RowCol<size_t> dims)
{
    io::TempFile tempfile;
    const size_t numThreads = std::thread::hardware_concurrency();
    writeCPHD(tempfile.pathname(), numThreads, dims, writeData, meta, pvpBlock);
    return checkData(tempfile.pathname(), numThreads, meta, pvpBlock);
}

TEST_CASE(testPVPBlockSimple)
{
    const types::RowCol<size_t> dims(128, 256);
    const auto writeData = generateComplexData<types::zint16_t>(dims.area());
    const bool scale = false;
    cphd::Metadata meta = cphd::Metadata();
    cphd::setUpData(meta, dims, writeData);
    cphd::setPVPXML(meta.pvp);
    cphd::PVPBlock pvpBlock(meta.pvp, meta.data);
    std::vector<std::string> addedParams;
    setPVPBlock(dims,
                pvpBlock,
                false,
                false,
                false,
                false,
                false,
                false,
                addedParams);

    TEST_ASSERT_TRUE(runTest(scale, writeData, meta, pvpBlock, dims));
}

TEST_CASE(testPVPBlockOptional)
{
    const types::RowCol<size_t> dims(128, 256);
    const auto writeData = generateComplexData<types::zint16_t>(dims.area());
    const bool scale = false;
    cphd::Metadata meta = cphd::Metadata();
    cphd::setUpData(meta, dims, writeData);
    cphd::setPVPXML(meta.pvp);
    meta.pvp.setOffset(27, meta.pvp.fxN1);
    meta.pvp.setOffset(28, meta.pvp.fxN2);
    meta.data.numBytesPVP += 2 * 8;
    cphd::PVPBlock pvpBlock(meta.pvp, meta.data);
    std::vector<std::string> addedParams;
    setPVPBlock(dims,
                pvpBlock,
                false,
                true,
                true,
                false,
                false,
                false,
                addedParams);

    TEST_ASSERT_TRUE(runTest(scale, writeData, meta, pvpBlock, dims));
}

TEST_CASE(testPVPBlockAdditional)
{
    const types::RowCol<size_t> dims(128, 256);
    const auto writeData = generateComplexData<types::zint16_t>(dims.area());
    const bool scale = false;
    cphd::Metadata meta = cphd::Metadata();
    cphd::setUpData(meta, dims, writeData);
    cphd::setPVPXML(meta.pvp);
    meta.pvp.setCustomParameter(1, 27, "F8", "param1");
    meta.pvp.setCustomParameter(1, 28, "F8", "param2");
    meta.data.numBytesPVP += 2 * 8;
    cphd::PVPBlock pvpBlock(meta.pvp, meta.data);
    std::vector<std::string> addedParams;
    addedParams.push_back("param1");
    addedParams.push_back("param2");
    setPVPBlock(dims,
                pvpBlock,
                false,
                false,
                false,
                false,
                false,
                false,
                addedParams);

    TEST_ASSERT_TRUE(runTest(scale, writeData, meta, pvpBlock, dims));
}

TEST_MAIN(
        TEST_CHECK(testPVPBlockSimple);
        TEST_CHECK(testPVPBlockOptional);
        TEST_CHECK(testPVPBlockAdditional);
        )

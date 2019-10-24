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
#include <sys/Conf.h>
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
#include <TestCase.h>

namespace
{
size_t NUM_SUPPORT = 3;
size_t NUM_ROWS = 3;
size_t NUM_COLS = 4;

double getRandom()
{
    const double r = static_cast<double>(rand()) / static_cast<double>(RAND_MAX);
    return -1000.0  + r * 2000.0;
}

cphd::Vector3 getRandomVector3()
{
    cphd::Vector3 ret;
    ret[0] = getRandom();
    ret[1] = getRandom();
    ret[2] = getRandom();
    return ret;
}

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

void setPVPXML(cphd::Pvp& pvp)
{
    pvp.setData(pvp.txTime, 1, 1, "F8"); //Size, Offset, Format
    pvp.setData(pvp.txPos, 3, 2, "X=F8;Y=F8;Z=F8;");
    pvp.setData(pvp.txVel, 3, 5, "X=F8;Y=F8;Z=F8;");
    pvp.setData(pvp.rcvTime, 1, 0, "F8");
    pvp.setData(pvp.rcvPos, 3, 11, "X=F8;Y=F8;Z=F8;");
    pvp.setData(pvp.rcvVel, 3, 8, "X=F8;Y=F8;Z=F8;");
    pvp.setData(pvp.srpPos, 3, 14, "X=F8;Y=F8;Z=F8;");
    pvp.setData(pvp.aFDOP, 1, 26, "F8");
    pvp.setData(pvp.aFRR1, 1, 25, "F8");
    pvp.setData(pvp.aFRR2, 1, 17, "F8");
    pvp.setData(pvp.fx1, 1, 18, "F8");
    pvp.setData(pvp.fx2, 1, 19, "F8");
    pvp.setData(pvp.toa1, 1, 20, "F8");
    pvp.setData(pvp.toa2, 1, 21, "F8");
    pvp.setData(pvp.tdTropoSRP, 1, 22, "F8");
    pvp.setData(pvp.sc0, 1, 23, "F8");
    pvp.setData(pvp.scss, 1, 24, "F8");
}

void setVectorParameters(size_t channel,
                          size_t vector,
                          cphd::PVPBlock& pvpBlock)
{
    const double txTime = getRandom();
    pvpBlock.setTxTime(txTime, channel, vector);

    const cphd::Vector3 txPos = getRandomVector3();
    pvpBlock.setTxPos(txPos, channel, vector);

    const cphd::Vector3 txVel = getRandomVector3();
    pvpBlock.setTxVel(txVel, channel, vector);

    const double rcvTime = getRandom();
    pvpBlock.setRcvTime(rcvTime, channel, vector);

    const cphd::Vector3 rcvPos = getRandomVector3();
    pvpBlock.setRcvPos(rcvPos, channel, vector);

    const cphd::Vector3 rcvVel = getRandomVector3();
    pvpBlock.setRcvVel(rcvVel, channel, vector);

    const cphd::Vector3 srpPos = getRandomVector3();
    pvpBlock.setSRPPos(srpPos, channel, vector);

    const double aFDOP = getRandom();
    pvpBlock.setaFDOP(aFDOP, channel, vector);

    const double aFRR1 = getRandom();
    pvpBlock.setaFRR1(aFRR1, channel, vector);

    const double aFRR2 = getRandom();
    pvpBlock.setaFRR2(aFRR2, channel, vector);

    const double fx1 = getRandom();
    pvpBlock.setFx1(fx1, channel, vector);

    const double fx2 = getRandom();
    pvpBlock.setFx2(fx2, channel, vector);

    const double toa1 = getRandom();
    pvpBlock.setTOA1(toa1, channel, vector);

    const double toa2 = getRandom();
    pvpBlock.setTOA2(toa2, channel, vector);

    const double tdTropoSRP = getRandom();
    pvpBlock.setTdTropoSRP(tdTropoSRP, channel, vector);

    const double sc0 = getRandom();
    pvpBlock.setSC0(sc0, channel, vector);

    const double scss = getRandom();
    pvpBlock.setSCSS(scss, channel, vector);
}

void setUpMetadata(cphd::Metadata& metadata)
{
    //! We must have a collectType set
    metadata.collectionID.collectType = cphd::CollectType::MONOSTATIC;
    metadata.collectionID.setClassificationLevel("Unclassified");
    metadata.collectionID.releaseInfo = "Release";
    //! We must have a radar mode set
    metadata.collectionID.radarMode = cphd::RadarModeType::SPOTLIGHT;
    metadata.sceneCoordinates.iarp.ecf = getRandomVector3();
    metadata.sceneCoordinates.iarp.llh = cphd::LatLonAlt(0,0,0);
    metadata.sceneCoordinates.referenceSurface.planar.reset(new cphd::Planar());
    metadata.sceneCoordinates.referenceSurface.planar->uIax = getRandomVector3();
    metadata.sceneCoordinates.referenceSurface.planar->uIay = getRandomVector3();
    //! We must have corners set
    for (size_t ii = 0; ii < six::Corners<double>::NUM_CORNERS; ++ii)
    {
        metadata.sceneCoordinates.imageAreaCorners.getCorner(ii).setLat(0.0);
        metadata.sceneCoordinates.imageAreaCorners.getCorner(ii).setLon(0.0);
    }
    metadata.channel.fxFixedCphd = true;
    metadata.channel.toaFixedCphd = false;
    metadata.channel.srpFixedCphd = false;
    metadata.referenceGeometry.srp.ecf = getRandomVector3();
    metadata.referenceGeometry.srp.iac = getRandomVector3();
    metadata.referenceGeometry.monostatic.reset(new cphd::Monostatic());
    metadata.referenceGeometry.monostatic->arpPos = getRandomVector3();
    metadata.referenceGeometry.monostatic->arpVel = getRandomVector3();
}

void setUpData(cphd::Metadata& metadata)
{
    //! Must set the sample type (the type doesnt matter for compressed data)
    const size_t numChannels = 1;
    for (size_t ii = 0; ii < numChannels; ++ii)
    {
        metadata.data.channels.push_back(
                cphd::Data::Channel(128, 256));
    }
    //! Must set the sample type (the type doesnt matter for compressed data)
    metadata.data.signalArrayFormat = cphd::SignalArrayFormat::CF8;

    metadata.data.signalCompressionID = "Huffman";
    for (size_t ii = 0; ii < numChannels; ++ii)
    {
        metadata.data.channels[ii].compressedSignalSize = 128*256;
    }
    setUpMetadata(metadata);
}

template<typename T>
void writeSupportData(const std::string& outPathname, size_t numThreads,
        const std::vector<T>& writeData,
        cphd::Metadata& metadata,
        cphd::PVPBlock& pvpBlock)
{
    const size_t numChannels = 1;
    const std::vector<size_t> numVectors(numChannels, 128);

    for (size_t ii = 0; ii < numChannels; ++ii)
    {
        for (size_t jj = 0; jj < numVectors[ii]; ++jj)
        {
            setVectorParameters(ii, jj, pvpBlock);
        }
    }

    cphd::CPHDWriter writer(metadata, numThreads);
    writer.writeMetadata(outPathname, pvpBlock);

    size_t idx = 0;
    for (auto it = metadata.data.supportOffsetMap.begin(); it != metadata.data.supportOffsetMap.end(); ++it, idx += NUM_ROWS*NUM_COLS)
    {
        writer.writeSupportData(&writeData[idx], it->first);
    }
    writer.writePVPData(pvpBlock);
}

std::vector<sys::ubyte> checkSupportData(const std::string& pathname,
        size_t size,
        size_t numThreads,
        mem::SharedPtr<io::SeekableInputStream> inStream)
{
    cphd::CPHDReader reader(inStream, numThreads);
    const cphd::SupportBlock& supportBlock = reader.getSupportBlock();

    mem::ScopedArray<sys::ubyte> readPtr;
    supportBlock.readAll(numThreads, readPtr);

    std::vector<sys::ubyte> readData(readPtr.get(), readPtr.get() + reader.getMetadata().data.getAllSupportSize());
    return readData;
}

template<typename T>
bool compareVectors(const std::vector<sys::ubyte>& readData,
                    const T* writeData,
                    size_t writeDataSize)
{
    if (writeDataSize * sizeof(T) != readData.size())
    {
        std::cerr << "Size mismatch. Writedata size: "<< writeDataSize * sizeof(T)
                  << "ReadData size: " << readData.size() << "\n";
        return false;
    }
    const sys::ubyte* ptr = reinterpret_cast<const sys::ubyte*>(writeData);
    for (size_t ii = 0; ii < readData.size(); ++ii, ++ptr)
    {
        if (*ptr != readData[ii])
        {
            std::cerr << "Value mismatch at index " << ii << std::endl;
            std::cerr << "readData: " << readData[ii] << " " << "writeData: " << *ptr << "\n";
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
    mem::SharedPtr<io::SeekableInputStream> inStream(new io::FileInputStream(tempfile.pathname()));
    cphd::Metadata meta = cphd::Metadata();
    setUpData(meta);
    setSupport<T>(meta.data);
    setPVPXML(meta.pvp);
    cphd::PVPBlock pvpBlock(meta.data,
                            meta.pvp);
    writeSupportData(tempfile.pathname(), numThreads, writeData, meta, pvpBlock);
    const std::vector<sys::ubyte> readData =
            checkSupportData(tempfile.pathname(), NUM_SUPPORT*NUM_ROWS*NUM_COLS*sizeof(T), numThreads, inStream);
    return compareVectors(readData, &writeData[0], writeData.size());
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
}

int main(int argc, char** argv)
{
    try
    {
        TEST_CHECK(testSupportsInt);
        TEST_CHECK(testSupportsDouble);
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


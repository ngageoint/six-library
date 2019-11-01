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
#include <cphd/Wideband.h>
#include <cphd/Metadata.h>
#include <cphd/PVP.h>
#include <cphd/PVPBlock.h>
#include <cphd/ReferenceGeometry.h>
#include <TestCase.h>

/*!
 * Tests write and read of Signal Block with compressed data
 * Fails if values don't match
 */
namespace
{
std::vector<std::string> schemas(1);

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

std::vector<sys::ubyte> generateCompressedData(size_t length)
{
    std::vector<sys::ubyte> data(length);
    srand(0);
    for (size_t ii = 0; ii < data.size(); ++ii)
    {
        data[ii] = rand() % 16;
    }
    return data;
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
    metadata.collectionID.collectInfo.collectType = cphd::CollectType::MONOSTATIC;
    metadata.collectionID.collectInfo.setClassificationLevel("Unclassified");
    metadata.collectionID.releaseInfo = "Release";
    //! We must have a radar mode set
    metadata.collectionID.collectInfo.radarMode = cphd::RadarModeType::SPOTLIGHT;
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

void setUpData(const types::RowCol<size_t> dims,
               const std::vector<sys::ubyte>& writeData,
               cphd::Metadata& metadata)
{
    const size_t numChannels = 1;
    const std::vector<size_t> numVectors(numChannels, dims.row);

    for (size_t ii = 0; ii < numChannels; ++ii)
    {
        metadata.data.channels.push_back(
                cphd::Data::Channel(dims.row, dims.col));
    }
    //! Must set the sample type (the type doesnt matter for compressed data)
    metadata.data.signalArrayFormat = cphd::SignalArrayFormat::CF8;

    metadata.data.signalCompressionID = "Huffman";
    for (size_t ii = 0; ii < numChannels; ++ii)
    {
        metadata.data.channels[ii].compressedSignalSize = dims.area();
    }
    setUpMetadata(metadata);
}

void writeCompressedCPHD(const std::string& outPathname, size_t numThreads,
        const types::RowCol<size_t> dims,
        const std::vector<sys::ubyte>& writeData,
        cphd::Metadata& metadata,
        cphd::PVPBlock& pvpBlock)
{
    const size_t numChannels = 1;
    const std::vector<size_t> numVectors(numChannels, dims.row);

    for (size_t ii = 0; ii < numChannels; ++ii)
    {
        for (size_t jj = 0; jj < numVectors[ii]; ++jj)
        {
            setVectorParameters(ii, jj, pvpBlock);
        }
    }

    cphd::CPHDWriter writer(metadata, outPathname, std::vector<std::string>(), numThreads);

    writer.writeMetadata(pvpBlock);

    writer.writePVPData(pvpBlock);

    for (size_t ii = 0; ii < numChannels; ++ii)
    {
        writer.writeCPHDData(writeData.data(),1,ii);
    }
}

std::vector<sys::ubyte> checkCompressedData(const std::string& pathname,
        size_t numThreads,
        const types::RowCol<size_t>& dims,
        mem::SharedPtr<io::SeekableInputStream> inStream)
{

    cphd::CPHDReader reader(inStream, numThreads);
    const cphd::Wideband& wideband = reader.getWideband();
    std::vector<sys::ubyte> readData(dims.area());

    mem::BufferView<sys::ubyte> data(&readData[0], readData.size());
    for (size_t ii = 0; ii < reader.getMetadata().data.getNumChannels(); ++ii)
    {
        wideband.read(ii, data);
    }
    return readData;
}

bool compareVectors(const std::vector<sys::ubyte>& readData,
                    const std::vector<sys::ubyte>& writeData)
{
    for (size_t ii = 0; ii < readData.size(); ++ii)
    {
        if (writeData[ii] != readData[ii])
        {
            std::cerr << "Value mismatch at index " << ii << std::endl;
            return false;
        }
    }
    return true;
}

bool runTest(const std::vector<sys::ubyte>& writeData)
{
    io::TempFile tempfile;
    const size_t numThreads = sys::OS().getNumCPUs();
    const types::RowCol<size_t> dims(128, 256);
    mem::SharedPtr<io::SeekableInputStream> inStream(new io::FileInputStream(tempfile.pathname()));
    cphd::Metadata meta = cphd::Metadata();
    setUpData(dims, writeData, meta);
    setPVPXML(meta.pvp);
    cphd::PVPBlock pvpBlock(meta.data,
                            meta.pvp);

    writeCompressedCPHD(tempfile.pathname(), numThreads, dims, writeData, meta, pvpBlock);
    const std::vector<sys::ubyte> readData =
            checkCompressedData(tempfile.pathname(), numThreads,
            dims, inStream);
    return compareVectors(readData, writeData);
}

TEST_CASE(testCompressed)
{
    const types::RowCol<size_t> dims(128, 256);
    const std::vector<sys::ubyte> writeData =
            generateCompressedData(dims.area());
    TEST_ASSERT_TRUE(runTest(writeData));
}
}

int main(int argc, char** argv)
{
    try
    {
        TEST_CHECK(testCompressed);
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


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
#include <cphd/Wideband.h>
#include <cphd/Metadata.h>
#include <cphd/PVP.h>
#include <cphd/PVPBlock.h>
#include <cphd/CPHDWriter.h>
#include <cphd/CPHDReader.h>
#include <cphd/ReferenceGeometry.h>
#include <types/RowCol.h>
#include <cli/ArgumentParser.h>
#include <io/TempFile.h>
#include <io/FileInputStream.h>
#include <io/FileOutputStream.h>
#include <TestCase.h>

namespace
{

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
                          cphd::PVPBlock& PVPBlock)
{
    const double txTime = getRandom();
    PVPBlock.setTxTime(txTime, channel, vector);

    const cphd::Vector3 txPos = getRandomVector3();
    PVPBlock.setTxPos(txPos, channel, vector);

    const cphd::Vector3 txVel = getRandomVector3();
    PVPBlock.setTxVel(txVel, channel, vector);

    const double rcvTime = getRandom();
    PVPBlock.setRcvTime(rcvTime, channel, vector);

    const cphd::Vector3 rcvPos = getRandomVector3();
    PVPBlock.setRcvPos(rcvPos, channel, vector);

    const cphd::Vector3 rcvVel = getRandomVector3();
    PVPBlock.setRcvVel(rcvVel, channel, vector);

    const cphd::Vector3 srpPos = getRandomVector3();
    PVPBlock.setSRPPos(srpPos, channel, vector);

    const double aFDOP = getRandom();
    PVPBlock.setaFDOP(aFDOP, channel, vector);

    const double aFRR1 = getRandom();
    PVPBlock.setaFRR1(aFRR1, channel, vector);

    const double aFRR2 = getRandom();
    PVPBlock.setaFRR2(aFRR2, channel, vector);

    const double fx1 = getRandom();
    PVPBlock.setFx1(fx1, channel, vector);

    const double fx2 = getRandom();
    PVPBlock.setFx2(fx2, channel, vector);

    const double toa1 = getRandom();
    PVPBlock.setTOA1(toa1, channel, vector);

    const double toa2 = getRandom();
    PVPBlock.setTOA2(toa2, channel, vector);

    const double tdTropoSRP = getRandom();
    PVPBlock.setTdTropoSRP(tdTropoSRP, channel, vector);

    const double sc0 = getRandom();
    PVPBlock.setSC0(sc0, channel, vector);

    const double scss = getRandom();
    PVPBlock.setSCSS(scss, channel, vector);
}

template<typename T>
void setUpMetaData(const types::RowCol<size_t> dims,
                   const std::vector<std::complex<T> >& writeData,
                   cphd::Metadata& metadata)
{
    const size_t numChannels = 1;
    const std::vector<size_t> numVectors(numChannels, dims.row);

    for (size_t ii = 0; ii < numChannels; ++ii)
    {
        metadata.data.channels.push_back(
                cphd::Data::Channel(dims.row, dims.col));
    }

    //! Must set the sample type
    if (sizeof(writeData[0]) == 2)
    {
        metadata.data.signalArrayFormat = cphd::SignalArrayFormat::CI2;
    }
    else if (sizeof(writeData[0]) == 4)
    {
        metadata.data.signalArrayFormat = cphd::SignalArrayFormat::CI4;
    }
    else if (sizeof(writeData[0]) == 8)
    {
        metadata.data.signalArrayFormat = cphd::SignalArrayFormat::CF8;
    }

    //! We must have a collectType set
    metadata.collectionID.collectType = cphd::CollectType::MONOSTATIC;
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

template<typename T>
void writeCPHD(const std::string& outPathname, size_t numThreads,
        const types::RowCol<size_t> dims,
        const std::vector<std::complex<T> >& writeData,
        io::FileOutputStream& mStream,
        cphd::Metadata& metadata,
        cphd::PVPBlock& PVPBlock)
{
    const size_t numChannels = 1;
    const std::vector<size_t> numVectors(numChannels, dims.row);

    for (size_t ii = 0; ii < numChannels; ++ii)
    {
        for (size_t jj = 0; jj < numVectors[ii]; ++jj)
        {
            setVectorParameters(ii, jj, PVPBlock);
        }
    }

    cphd::CPHDWriter writer(metadata, numThreads);
    writer.writeMetadata(outPathname, PVPBlock, "Unclassified", "Release");
    writer.writePVPData(PVPBlock);
    for (size_t ii = 0; ii < numChannels; ++ii)
    {
        writer.writeCPHDData(&writeData[0],dims.area()*2);
    }
}

bool checkData(size_t numThreads,
               mem::SharedPtr<io::SeekableInputStream> inStream,
               cphd::Metadata& metadata,
               cphd::PVPBlock& PVPBlock)
{
    cphd::CPHDReader reader(inStream, numThreads);
    cphd::Wideband& wideband = reader.getWideband();

    if (metadata.pvp != reader.getMetadata().pvp)
    {
        return false;
    }
    if (PVPBlock != reader.getPVPBlock())
    {
        return false;
    }
    return true;
}

template<typename T>
bool runTest(bool scale, const std::vector<std::complex<T> >& writeData)
{
    io::TempFile tempfile;
    const size_t numThreads = sys::OS().getNumCPUs();
    const types::RowCol<size_t> dims(128, 256);
    const std::vector<double> scaleFactors =
            generateScaleFactors(dims.row, scale);
    io::FileOutputStream outStream;
    mem::SharedPtr<io::SeekableInputStream> inStream(new io::FileInputStream(tempfile.pathname()));
    cphd::Metadata meta = cphd::Metadata();

    setUpMetaData(dims, writeData, meta);
    setPVPXML(meta.pvp);

    cphd::PVPBlock PVPBlock(meta.data,
                            meta.pvp);

    writeCPHD(tempfile.pathname(), numThreads, dims, writeData, outStream, meta, PVPBlock);
    return checkData(numThreads, inStream, meta, PVPBlock);
}

TEST_CASE(testPVPBlock)
{
    const types::RowCol<size_t> dims(128, 256);
    const std::vector<std::complex<sys::Int16_T> > writeData =
            generateData<sys::Int16_T>(dims.area());
    const bool scale = false;
    TEST_ASSERT(runTest(scale, writeData))
}
}

int main(int argc, char** argv)
{
    try
    {
        TEST_CHECK(testPVPBlock);
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


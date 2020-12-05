/* =========================================================================
 * This file is part of cphd03-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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

#include <string>

#include <cphd03/CPHDWriter.h>
#include <cphd03/CPHDReader.h>
#include <cphd/Wideband.h>
#include <types/RowCol.h>
#include <sys/Filesystem.h>
namespace fs = sys::Filesystem;

#include "TestCase.h"

static std::string testName;

namespace
{
static const size_t MAX_SIZE_T = 1000000;
static const double MAX_DOUBLE = 1000000.0;
static const int64_t MAX_OFF_T = 1000000;
static const std::string FILE_NAME("temp.cphd03");
static const size_t NUM_IMAGES(3);
static const size_t NUM_THREADS(13);

double round(double num, int places)
{

    double f = num - std::floor(num);
    int place = 1;
    for (int i = 0; i < places; ++i)
    {
        place *= 10;
    }

    f *= place;

    if (f > 0.5)
    {
        f *= place;
        f = std::floor(f);
        f /= place;
    }
    else
    {
        f *= place;
        f = std::floor(f);
        f /= place;
    }
    return std::floor(num) + f;
}

double getRandomReal(double min = -MAX_DOUBLE, double max = MAX_DOUBLE)
{
    double r = static_cast<double>(rand()) / static_cast<double>(RAND_MAX);
    //! Round here to reduce floating point issues when
    //  writing to disk
    return round(min  + r * (max - min), 2);
}

size_t getRandomInt(size_t min = 0, size_t max = MAX_SIZE_T)
{
    return min + rand() % (max - min + 1);
}

int64_t getRandomOffT(int64_t min = -MAX_OFF_T,
                         int64_t max = MAX_OFF_T)
{
    return min + rand() % (max - min + 1);
}

six::Vector3 getRandomVector3()
{
    six::Vector3 vec;
    for (size_t ii = 0; ii < vec.size(); ++ii)
    {
        vec[ii] = getRandomReal();
    }
    return vec;
}

cphd::PolyXYZ getRandomPolyXYZ()
{
    six::PolyXYZ poly(3);
    for (size_t ii = 0; ii <= poly.order(); ++ii)
    {
        poly[ii] = getRandomVector3();
    }
    return poly;
}

cphd::Poly1D getRandomPoly1D()
{
    six::Poly1D poly(3);
    for (size_t ii = 0; ii <= poly.order(); ++ii)
    {
        poly[ii] = getRandomReal();
    }
    return poly;
}

cphd::Poly2D getRandomPoly2D()
{
    six::Poly2D poly(3, 3);
    for (size_t ii = 0; ii <= poly.orderX(); ++ii)
    {
        for (size_t jj = 0; jj <= poly.orderY(); ++jj)
        {
            poly[ii][jj] = getRandomReal();
        }
    }
    return poly;
}

void getRandomAntennaParameter(cphd03::AntennaParameters& param)
{
    param.xAxisPoly = getRandomPolyXYZ();
    param.yAxisPoly = getRandomPolyXYZ();
    param.frequencyZero = getRandomReal();
    param.electricalBoresight.reset(new cphd03::ElectricalBoresight());
    param.electricalBoresight->dcxPoly = getRandomPoly1D();
    param.electricalBoresight->dcyPoly = getRandomPoly1D();
    param.halfPowerBeamwidths.reset(new cphd03::HalfPowerBeamwidths());
    param.halfPowerBeamwidths->dcx = getRandomReal();
    param.halfPowerBeamwidths->dcy = getRandomReal();
    param.array.reset(new cphd03::GainAndPhasePolys());
    param.array->gainPoly = getRandomPoly2D();
    param.array->phasePoly = getRandomPoly2D();
    param.element.reset(new cphd03::GainAndPhasePolys());
    param.element->gainPoly = getRandomPoly2D();
    param.element->phasePoly = getRandomPoly2D();
    param.gainBSPoly = getRandomPoly1D();
    param.electricalBoresightFrequencyShift = getRandomInt(
            six::BooleanType::IS_FALSE, six::BooleanType::IS_TRUE);
    param.mainlobeFrequencyDilation = getRandomInt(
            six::BooleanType::IS_FALSE, six::BooleanType::IS_TRUE);
}

void buildRandomMetadata(cphd03::Metadata& metadata)
{
    //! Dummy collection information
    metadata.collectionInformation.collectorName = "DummyCollectorName";
    metadata.collectionInformation.illuminatorName = "DummyIlluminatorName";
    metadata.collectionInformation.coreName = "DummyCoreName";
    metadata.collectionInformation.collectType = getRandomInt(
            cphd::CollectType::MONOSTATIC, cphd::CollectType::BISTATIC);
    metadata.collectionInformation.radarMode = getRandomInt(
            cphd::RadarModeType::SPOTLIGHT, cphd::RadarModeType::SCANSAR);
    metadata.collectionInformation.radarModeID = "DummyRadarModeID";
    metadata.collectionInformation.setClassificationLevel("UNCLASSIFIED");
    metadata.collectionInformation.countryCodes.push_back("DummyCountryCode1");
    metadata.collectionInformation.countryCodes.push_back("DummyCountryCode2");
    metadata.collectionInformation.parameters.push_back(six::Parameter("DummyParameter1"));
    metadata.collectionInformation.parameters.push_back(six::Parameter("DummyParameter2"));

    //! Dummy CPHD Data
    metadata.data.sampleType = cphd::SampleType::RE32F_IM32F;
    metadata.data.numBytesVBP = getRandomInt(32, 64) * 8;
    metadata.data.numCPHDChannels = NUM_IMAGES;
    for (size_t ii = 0; ii < metadata.data.numCPHDChannels; ++ii)
    {
        metadata.data.arraySize.push_back(cphd03::ArraySize(
                getRandomInt(256, 512),
                getRandomInt(256, 512)));
    }

    //! Global data
    //! Domain Type needs to be set by the TOA vs FX implementation
    metadata.global.phaseSGN = getRandomInt(0, 1) == 1 ?
            cphd::PhaseSGN::PLUS_1 : cphd::PhaseSGN::MINUS_1;
    metadata.global.refFrequencyIndex = getRandomInt(0, 10);
    metadata.global.collectStart = cphd::DateTime(static_cast<double>(
            getRandomInt(0, MAX_SIZE_T)));
    metadata.global.collectDuration = getRandomReal(0.0, MAX_DOUBLE);
    metadata.global.txTime1 = getRandomReal(0.0, MAX_DOUBLE);
    metadata.global.txTime2 = getRandomReal(0.0, MAX_DOUBLE);
    for (size_t ii = 0; ii < six::LatLonAltCorners::NUM_CORNERS; ++ii)
    {
        metadata.global.imageArea.acpCorners.getCorner(ii).setLat(
                getRandomReal());
        metadata.global.imageArea.acpCorners.getCorner(ii).setLon(
                getRandomReal());
        metadata.global.imageArea.acpCorners.getCorner(ii).setAlt(
                getRandomReal());
    }
    metadata.global.imageArea.plane.reset(new cphd03::AreaPlane());
    metadata.global.imageArea.plane->referencePoint = six::ReferencePoint(
            getRandomReal(),
            getRandomReal(),
            getRandomReal(),
            getRandomReal(),
            getRandomReal());
    metadata.global.imageArea.plane->xDirection.unitVector =
            getRandomVector3();
    metadata.global.imageArea.plane->xDirection.spacing =
            getRandomReal();
    metadata.global.imageArea.plane->xDirection.elements =
            getRandomInt(10000, MAX_SIZE_T);
    metadata.global.imageArea.plane->xDirection.first = getRandomInt(
            0, metadata.global.imageArea.plane->xDirection.elements);

    metadata.global.imageArea.plane->yDirection.unitVector =
            getRandomVector3();
    metadata.global.imageArea.plane->yDirection.spacing =
            getRandomReal();
    metadata.global.imageArea.plane->yDirection.elements =
            getRandomInt(10000, MAX_SIZE_T);
    metadata.global.imageArea.plane->yDirection.first = getRandomInt(
            0, metadata.global.imageArea.plane->yDirection.elements);
    metadata.global.imageArea.plane->dwellTime.reset(
            new cphd03::DwellTimeParameters());
    metadata.global.imageArea.plane->dwellTime->codTimePoly =
            getRandomPoly2D();
    metadata.global.imageArea.plane->dwellTime->dwellTimePoly =
            getRandomPoly2D();

    //! Dummy channel
    for (size_t ii = 0; ii < 5; ++ii)
    {
        cphd03::ChannelParameters param;
        param.srpIndex = getRandomInt();
        param.nomTOARateSF = getRandomReal();
        param.fxCtrNom = getRandomReal();
        param.bwSavedNom = getRandomReal();
        param.toaSavedNom = getRandomReal();
        param.txAntIndex = getRandomInt();
        param.rcvAntIndex = getRandomInt();
        param.twAntIndex = getRandomInt();
        metadata.channel.parameters.push_back(param);
    }

    //! Dummy SRP
    metadata.srp.srpType = getRandomInt(
            cphd::SRPType::FIXEDPT, cphd::SRPType::STEPPED);
    metadata.srp.numSRPs =
            metadata.srp.srpType != cphd::SRPType::STEPPED ? 5 : 0;
    for (size_t ii = 0; ii < metadata.srp.numSRPs; ++ii)
    {
        if (metadata.srp.srpType == cphd::SRPType::FIXEDPT)
        {
            metadata.srp.srpPT.push_back(getRandomVector3());
        }
        if (metadata.srp.srpType == cphd::SRPType::PVTPOLY)
        {
            metadata.srp.srpPVTPoly.push_back(getRandomPolyXYZ());
        }
        if (metadata.srp.srpType == cphd::SRPType::PVVPOLY)
        {
            metadata.srp.srpPVVPoly.push_back(getRandomPolyXYZ());
        }
    }

    //! Dummy antenna
    metadata.antenna.reset(new cphd03::Antenna());

    //! Dummy vector parameters
    metadata.vectorParameters.txTime = getRandomOffT();
    metadata.vectorParameters.txPos = getRandomOffT();
    metadata.vectorParameters.rcvTime = getRandomOffT();
    metadata.vectorParameters.rcvPos = getRandomOffT();
    metadata.vectorParameters.srpTime = getRandomOffT();
    metadata.vectorParameters.srpPos = getRandomOffT();
    metadata.vectorParameters.tropoSRP = getRandomOffT();
    metadata.vectorParameters.ampSF = getRandomOffT();
}

void addFXParams(cphd03::Metadata& metadata)
{
    metadata.global.domainType = cphd::DomainType::FX;

    metadata.vectorParameters.fxParameters.reset(new cphd03::FxParameters());
    metadata.vectorParameters.fxParameters->Fx0 = getRandomInt();
    metadata.vectorParameters.fxParameters->Fx1 = getRandomInt();
    metadata.vectorParameters.fxParameters->Fx2 = getRandomInt();
    metadata.vectorParameters.fxParameters->FxSS = getRandomInt();
}

void addTOAParams(cphd03::Metadata& metadata)
{
    metadata.global.domainType = cphd::DomainType::TOA;

    metadata.vectorParameters.toaParameters.reset(new cphd03::TOAParameters());
    metadata.vectorParameters.toaParameters->deltaTOA0 = getRandomInt();
    metadata.vectorParameters.toaParameters->toaSS = getRandomInt();
}

void addOneWayParams(cphd03::Metadata& metadata)
{
    metadata.antenna->numTxAnt = getRandomInt();
    metadata.antenna->numRcvAnt = getRandomInt();
    for (size_t ii = 0; ii < 5; ++ii)
    {
        cphd03::AntennaParameters param;
        getRandomAntennaParameter(param);
        metadata.antenna->rcv.push_back(param);
    }
}

void addTwoWayParams(cphd03::Metadata& metadata)
{
    metadata.antenna->numTWAnt = getRandomInt();
    for (size_t ii = 0; ii < 5; ++ii)
    {
        cphd03::AntennaParameters param;
        getRandomAntennaParameter(param);
        metadata.antenna->twoWay.push_back(param);
    }
}

void writeCPHD(
        cphd03::VBM& vbm,
        cphd03::Metadata& metadata,
        std::vector<std::vector<std::complex<float> > >& data,
        std::vector<types::RowCol<size_t> >& dims)
{
    cphd03::CPHDWriter writer(metadata, FILE_NAME, NUM_THREADS);

    writer.writeMetadata(vbm);

    for (size_t ii = 0; ii < NUM_IMAGES; ++ii)
    {
        dims[ii] = types::RowCol<size_t>(
                metadata.getNumVectors(ii),
                metadata.getNumSamples(ii));

        data[ii].resize(dims[ii].area());
        for (size_t jj = 0; jj < data[ii].size(); ++jj)
        {
            data[ii][jj] = std::complex<float>(
                    getRandomReal(), getRandomReal());
        }

        writer.writeCPHDData(data[ii].data(), data[ii].size());
    }
}


void runCPHDTest(const std::string& testName_,
                 cphd03::Metadata& metadata)
{
    testName = testName_;

    metadata.data.numCPHDChannels = NUM_IMAGES;

    cphd03::VBM vbm(metadata.data, metadata.vectorParameters);
    for (size_t ii = 0; ii < NUM_IMAGES; ++ii)
    {
        for (size_t jj = 0; jj < metadata.getNumVectors(ii); ++jj)
        {
            vbm.setTxTime(getRandomReal(), ii, jj);
            vbm.setTxPos(getRandomVector3(), ii, jj);
            vbm.setRcvTime(getRandomReal(), ii, jj);
            vbm.setRcvPos(getRandomVector3(), ii, jj);
            if (vbm.haveSRPTime())
            {
                vbm.setSRPTime(getRandomReal(), ii, jj);
            }
            vbm.setSRPPos(getRandomVector3(), ii, jj);
            if (vbm.haveTropoSRP())
            {
                vbm.setTropoSRP(getRandomReal(), ii, jj);
            }
            if (vbm.haveAmpSF())
            {
                vbm.setAmpSF(getRandomReal(), ii, jj);
            }
            if (metadata.global.domainType == cphd::DomainType::FX)
            {
                vbm.setFx0(getRandomReal(), ii, jj);
                vbm.setFxSS(getRandomReal(), ii, jj);
                vbm.setFx1(getRandomReal(), ii, jj);
                vbm.setFx2(getRandomReal(), ii, jj);
            }
            else
            {
                vbm.setDeltaTOA0(getRandomReal(), ii, jj);
                vbm.setTOASS(getRandomReal(), ii, jj);
            }
        }
    }

    //std::vector<std::vector<std::byte> >vbm(NUM_IMAGES);
    std::vector<std::vector<std::complex<float> > >data(NUM_IMAGES);
    std::vector<types::RowCol<size_t> > dims(NUM_IMAGES);

    writeCPHD(vbm, metadata, data, dims);

    cphd03::CPHDReader reader(FILE_NAME, NUM_THREADS);
    cphd::Wideband& wideband = reader.getWideband();

    TEST_ASSERT_EQ(metadata, reader.getMetadata());
    TEST_ASSERT_EQ(vbm, reader.getVBM());

    std::vector<std::byte> readVBM;
    for (size_t ii = 0; ii < NUM_IMAGES; ++ii)
    {
        std::unique_ptr<std::byte[]> readData;
        TEST_ASSERT_EQ(reader.getNumVectors(ii), dims[ii].row);
        TEST_ASSERT_EQ(reader.getNumSamples(ii), dims[ii].col);

        wideband.read(ii,
                      0, cphd::Wideband::ALL,
                      0, cphd::Wideband::ALL,
                      NUM_THREADS, readData);

        const std::complex<float>* readBuffer =
                reinterpret_cast<std::complex<float>* >(readData.get());

        for (size_t jj = 0; jj < dims[ii].area(); ++jj)
        {
            TEST_ASSERT_EQ(readBuffer[jj], data[ii][jj]);
        }
    }

}

TEST_CASE(testWriteFXOneWay)
{
    testName = "testWriteFXOneWay";

    cphd03::Metadata metadata;
    buildRandomMetadata(metadata);
    addFXParams(metadata);
    addOneWayParams(metadata);

    runCPHDTest(testName, metadata);
}

TEST_CASE(testWriteFXTwoWay)
{
    testName = "testWriteFXTwoWay";

    cphd03::Metadata metadata;
    buildRandomMetadata(metadata);
    addFXParams(metadata);
    addTwoWayParams(metadata);

    runCPHDTest(testName, metadata);
}

TEST_CASE(testWriteTOAOneWay)
{
    testName = "testWriteTOAOneWay";

    cphd03::Metadata metadata;
    buildRandomMetadata(metadata);
    addTOAParams(metadata);
    addOneWayParams(metadata);

    runCPHDTest(testName, metadata);
}

TEST_CASE(testWriteTOATwoWay)
{
    testName = "testWriteTOATwoWay";

    cphd03::Metadata metadata;
    buildRandomMetadata(metadata);
    addTOAParams(metadata);
    addTwoWayParams(metadata);

    runCPHDTest(testName, metadata);
}
}

static int call_srand()
{
    const auto seed = 334;
    ::srand(seed);
    return seed;
}
static int unused_ = call_srand();

TEST_MAIN(
    TEST_CHECK(testWriteFXOneWay);
    TEST_CHECK(testWriteFXTwoWay);
    TEST_CHECK(testWriteTOAOneWay);
    TEST_CHECK(testWriteTOATwoWay);
    fs::remove(FILE_NAME);
    )


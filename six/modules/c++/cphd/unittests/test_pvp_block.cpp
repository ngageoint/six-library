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

#include <complex>
#include <thread>
#include <tuple>

#include <cphd/PVP.h>
#include <cphd/PVPBlock.h>
#include <cphd/TestDataGenerator.h>

#include "TestCase.h"

static constexpr size_t NUM_CHANNELS = 3;
static constexpr size_t NUM_VECTORS = 2;

static void call_srand()
{
    const auto f = []() { ::srand(174); return nullptr; };
    static const auto result = f();
    std::ignore = result;
}

void setVectorParameters(size_t channel,
                         size_t vector,
                         cphd::PVPBlock& pvpBlock,
                         cphd::PVPBlock& pvpBlock2)
{
    const double txTime = cphd::getRandom();
    pvpBlock.setTxTime(txTime, channel, vector);
    pvpBlock2.setTxTime(txTime, channel, vector);

    const cphd::Vector3 txPos = cphd::getRandomVector3();
    pvpBlock.setTxPos(txPos, channel, vector);
    pvpBlock2.setTxPos(txPos, channel, vector);

    const cphd::Vector3 txVel = cphd::getRandomVector3();
    pvpBlock.setTxVel(txVel, channel, vector);
    pvpBlock2.setTxVel(txVel, channel, vector);

    const double rcvTime = cphd::getRandom();
    pvpBlock.setRcvTime(rcvTime, channel, vector);
    pvpBlock2.setRcvTime(rcvTime, channel, vector);

    const cphd::Vector3 rcvPos = cphd::getRandomVector3();
    pvpBlock.setRcvPos(rcvPos, channel, vector);
    pvpBlock2.setRcvPos(rcvPos, channel, vector);

    const cphd::Vector3 rcvVel = cphd::getRandomVector3();
    pvpBlock.setRcvVel(rcvVel, channel, vector);
    pvpBlock2.setRcvVel(rcvVel, channel, vector);

    const cphd::Vector3 srpPos = cphd::getRandomVector3();
    pvpBlock.setSRPPos(srpPos, channel, vector);
    pvpBlock2.setSRPPos(srpPos, channel, vector);

    const double aFDOP = cphd::getRandom();
    pvpBlock.setaFDOP(aFDOP, channel, vector);
    pvpBlock2.setaFDOP(aFDOP, channel, vector);

    const double aFRR1 = cphd::getRandom();
    pvpBlock.setaFRR1(aFRR1, channel, vector);
    pvpBlock2.setaFRR1(aFRR1, channel, vector);

    const double aFRR2 = cphd::getRandom();
    pvpBlock.setaFRR2(aFRR2, channel, vector);
    pvpBlock2.setaFRR2(aFRR2, channel, vector);

    const double fx1 = cphd::getRandom();
    pvpBlock.setFx1(fx1, channel, vector);
    pvpBlock2.setFx1(fx1, channel, vector);

    const double fx2 = cphd::getRandom();
    pvpBlock.setFx2(fx2, channel, vector);
    pvpBlock2.setFx2(fx2, channel, vector);

    const double toa1 = cphd::getRandom();
    pvpBlock.setTOA1(toa1, channel, vector);
    pvpBlock2.setTOA1(toa1, channel, vector);

    const double toa2 = cphd::getRandom();
    pvpBlock.setTOA2(toa2, channel, vector);
    pvpBlock2.setTOA2(toa2, channel, vector);

    const double tdTropoSRP = cphd::getRandom();
    pvpBlock.setTdTropoSRP(tdTropoSRP, channel, vector);
    pvpBlock2.setTdTropoSRP(tdTropoSRP, channel, vector);

    const double sc0 = cphd::getRandom();
    pvpBlock.setSC0(sc0, channel, vector);
    pvpBlock2.setSC0(sc0, channel, vector);

    const double scss = cphd::getRandom();
    pvpBlock.setSCSS(scss, channel, vector);
    pvpBlock2.setSCSS(scss, channel, vector);
}

TEST_CASE(testPvpRequired)
{
    call_srand();
    cphd::Pvp pvp;
    cphd::setPVPXML(pvp);
    cphd::PVPBlock pvpBlock(NUM_CHANNELS,
                            std::vector<size_t>(NUM_CHANNELS, NUM_VECTORS),
                            pvp);

    for (size_t channel = 0; channel < NUM_CHANNELS; ++channel)
    {
        for (size_t vector = 0; vector < NUM_VECTORS; ++vector)
        {
            cphd::setVectorParameters(channel, vector, pvpBlock);
        }
    }
}

TEST_CASE(testPvpOptional)
{
    call_srand();
    cphd::Pvp pvp;
    cphd::setPVPXML(pvp);
    pvp.setOffset(28, pvp.ampSF);
    pvp.setOffset(27, pvp.fxN1);
    pvp.setOffset(29, pvp.fxN2);
    pvp.setCustomParameter(1, 30, "F8", "Param1");
    pvp.setCustomParameter(1, 31, "S10", "Param2");
    pvp.setCustomParameter(1, 32, "CI16", "Param3");
    pvp.appendTxAnt();
    pvp.appendRcvAnt();
    cphd::PVPBlock pvpBlock(NUM_CHANNELS,
                            std::vector<size_t>(NUM_CHANNELS, NUM_VECTORS),
                            pvp);

    for (size_t channel = 0; channel < NUM_CHANNELS; ++channel)
    {
        for (size_t vector = 0; vector < NUM_VECTORS; ++vector)
        {
            cphd::setVectorParameters(channel, vector, pvpBlock);

            const double ampSF = cphd::getRandom();
            pvpBlock.setAmpSF(ampSF, channel, vector);
            const double fxN1 = cphd::getRandom();
            pvpBlock.setFxN1(fxN1, channel, vector);
            const double fxN2 = cphd::getRandom();
            pvpBlock.setFxN2(fxN2, channel, vector);
            
            const cphd::PvpAntenna txAnt(
                cphd::getRandomVector3(),
                cphd::getRandomVector3(),
                cphd::getRandomVector2());
            pvpBlock.setTxAntenna(txAnt, channel, vector);
                 
            const cphd::PvpAntenna rcvAnt(
                cphd::getRandomVector3(),
                cphd::getRandomVector3(),
                cphd::getRandomVector2()); 
            pvpBlock.setRcvAntenna(rcvAnt, channel, vector);

            const double addedParam1 = cphd::getRandom();
            pvpBlock.setAddedPVP(addedParam1, channel, vector, "Param1");
            const std::string addedParam2 = "Parameter2";
            pvpBlock.setAddedPVP(addedParam2, channel, vector, "Param2");
            const cphd::zint32_t addedParam3(3,4);
            pvpBlock.setAddedPVP(addedParam3, channel, vector, "Param3");

            TEST_ASSERT_EQ(ampSF, pvpBlock.getAmpSF(channel, vector));
            TEST_ASSERT_EQ(fxN1, pvpBlock.getFxN1(channel, vector));
            TEST_ASSERT_EQ(fxN2, pvpBlock.getFxN2(channel, vector));
            TEST_ASSERT_EQ(txAnt, pvpBlock.getTxAntenna(channel, vector));
            TEST_ASSERT_EQ(rcvAnt, pvpBlock.getRcvAntenna(channel, vector));
            TEST_ASSERT_EQ(addedParam1, pvpBlock.getAddedPVP<double>(channel, vector, "Param1"));
            TEST_ASSERT_EQ(addedParam2, pvpBlock.getAddedPVP<std::string>(channel, vector, "Param2"));
            TEST_ASSERT_EQ(addedParam3, pvpBlock.getAddedPVP<cphd::zint32_t >(channel, vector, "Param3"));
        }
    }
}

TEST_CASE(testPvpThrow)
{
    call_srand();

    cphd::Pvp pvp;
    cphd::setPVPXML(pvp);
    pvp.setOffset(29, pvp.ampSF);
    pvp.setOffset(28, pvp.fxN1);
    TEST_EXCEPTION(pvp.setOffset(15, pvp.toaE1)); // Overwrite block
    pvp.setOffset(27, pvp.toaE1);

    pvp.setCustomParameter(1, 30, "F8", "Param1");
    pvp.setCustomParameter(1, 31, "F8", "Param2");
    TEST_EXCEPTION(pvp.setCustomParameter(1, 30, "F8", "Param1")); //Rewriting to existing memory block
    TEST_EXCEPTION(pvp.setCustomParameter(1, 30, "X=F8;YF8;", "Param1")); //
    TEST_EXCEPTION(pvp.setCustomParameter(1, 30, "X=F8;Y=F8;Z=", "Param1"));

    cphd::PVPBlock pvpBlock(NUM_CHANNELS,
                            std::vector<size_t>(NUM_CHANNELS, NUM_VECTORS),
                            pvp);

    for (size_t channel = 0; channel < NUM_CHANNELS; ++channel)
    {
        for (size_t vector = 0; vector < NUM_VECTORS; ++vector)
        {
            cphd::setVectorParameters(channel, vector, pvpBlock);

            const double ampSF = cphd::getRandom();
            pvpBlock.setAmpSF(ampSF, channel, vector);

            const double fxN1 = cphd::getRandom();
            pvpBlock.setFxN1(fxN1, channel, vector);

            const double toaE1 = cphd::getRandom();
            pvpBlock.setTOAE1(toaE1, channel, vector);

            TEST_ASSERT_EQ(toaE1, pvpBlock.getTOAE1(channel, vector));

            const double addedParam1 = cphd::getRandom();
            pvpBlock.setAddedPVP(addedParam1, channel, vector, "Param1");

            const double addedParam2 = cphd::getRandom();
            TEST_EXCEPTION(pvpBlock.setAddedPVP(addedParam2, channel, vector, "Param3"));
            TEST_EXCEPTION(pvpBlock.getAddedPVP<double>(channel, vector, "Param3"));

            const double fxN2 = cphd::getRandom();
            TEST_EXCEPTION(pvpBlock.setFxN2(fxN2, channel, vector));
            TEST_EXCEPTION(pvpBlock.getFxN2(channel, vector));
        }
    }

    cphd::Pvp pvp2;
    cphd::setPVPXML(pvp2);
    pvp2.setCustomParameter(1, 27, "F8", "Param1");
    cphd::PVPBlock pvpBlock2(NUM_CHANNELS,
                             std::vector<size_t>(NUM_CHANNELS,NUM_VECTORS),
                             pvp2);

        for (size_t channel = 0; channel < NUM_CHANNELS; ++channel)
    {
        for (size_t vector = 0; vector < NUM_VECTORS; ++vector)
        {
            cphd::setVectorParameters(channel, vector, pvpBlock2);

            const double addedParam1 = cphd::getRandom();
            pvpBlock2.setAddedPVP(addedParam1, channel, vector, "Param1");
            pvpBlock2.getAddedPVP<double>(channel, vector, "Param1");
        }
    }
}

TEST_CASE(testPvpEquality)
{
    call_srand();

    cphd::Pvp pvp1;
    cphd::setPVPXML(pvp1);
    pvp1.setOffset(28, pvp1.ampSF);
    pvp1.setOffset(27, pvp1.fxN1);
    pvp1.setOffset(29, pvp1.fxN2);
    pvp1.setCustomParameter(1, 30, "F8", "Param1");
    pvp1.setCustomParameter(1, 31, "CI8", "Param2");
    cphd::PVPBlock pvpBlock1(NUM_CHANNELS,
                             std::vector<size_t>(NUM_CHANNELS, NUM_VECTORS),
                             pvp1);

    cphd::Pvp pvp2;
    cphd::setPVPXML(pvp2);
    pvp2.setOffset(28, pvp2.ampSF);
    pvp2.setOffset(27, pvp2.fxN1);
    pvp2.setOffset(29, pvp2.fxN2);
    pvp2.setCustomParameter(1, 30, "F8", "Param1");
    pvp2.setCustomParameter(1, 31, "CI8", "Param2");
    cphd::PVPBlock pvpBlock2(NUM_CHANNELS,
                             std::vector<size_t>(NUM_CHANNELS, NUM_VECTORS),
                             pvp2);

    TEST_ASSERT_EQ(pvp1, pvp2);
    TEST_ASSERT_TRUE(pvpBlock1 == pvpBlock2);

    for (size_t channel = 0; channel < NUM_CHANNELS; ++channel)
    {
        for (size_t vector = 0; vector < NUM_VECTORS; ++vector)
        {
            setVectorParameters(channel, vector, pvpBlock1, pvpBlock2);
            const double ampSF = cphd::getRandom();
            pvpBlock1.setAmpSF(ampSF, channel, vector);
            pvpBlock2.setAmpSF(ampSF, channel, vector);

            const double fxN1 = cphd::getRandom();
            pvpBlock1.setFxN1(fxN1, channel, vector);
            pvpBlock2.setFxN1(fxN1, channel, vector);

            const double fxN2 = cphd::getRandom();
            pvpBlock1.setFxN2(fxN2, channel, vector);
            pvpBlock2.setFxN2(fxN2, channel, vector);

            const double addedParam1 = cphd::getRandom();
            pvpBlock1.setAddedPVP(addedParam1, channel, vector, "Param1");
            pvpBlock2.setAddedPVP(addedParam1, channel, vector, "Param1");

            const cphd::zint32_t addedParam2(3,4);
            pvpBlock1.setAddedPVP(addedParam2, channel, vector, "Param2");
            pvpBlock2.setAddedPVP(addedParam2, channel, vector, "Param2");
        }
    }
    TEST_ASSERT_EQ(pvpBlock1, pvpBlock2);
}

TEST_CASE(testLoadPVPBlockFromMemory)
{
    call_srand();

    // For ease of testing, we make the somewhat specious assumption
    // that an item of PVP data is equivalent to a double.
    static_assert(sizeof(double) == cphd::PVPType::WORD_BYTE_SIZE,
                  "This test is not valid for compilers with "
                  "sizeof(double) != 8");

    cphd::Pvp pvp;
    cphd::setPVPXML(pvp);
    std::vector<std::vector<double>> dataSource(NUM_CHANNELS);
    std::vector<const void*> pvpData(NUM_CHANNELS);
    const size_t elementsPerChannel = pvp.getReqSetSize() * NUM_VECTORS;
    for (size_t channel = 0; channel < NUM_CHANNELS; channel++)
    {
        dataSource[channel].reserve(elementsPerChannel);
        for (size_t ii = 0; ii < elementsPerChannel; ++ii)
        {
            dataSource[channel].push_back(channel + ii * NUM_CHANNELS);
        }
        pvpData[channel] = dataSource[channel].data();
    }

    cphd::PVPBlock pvpBlock(NUM_CHANNELS,
                            std::vector<size_t>(NUM_CHANNELS, NUM_VECTORS),
                            pvp,
                            pvpData);

    TEST_ASSERT_EQ(pvpBlock.getTxTime(0, 0), 0);
    TEST_ASSERT_EQ(pvpBlock.getTxTime(1, 0), 1);
    TEST_ASSERT_EQ(pvpBlock.getTxTime(2, 0), 2);

    TEST_ASSERT_EQ(pvpBlock.getTxTime(0, 1), pvp.getReqSetSize() * NUM_CHANNELS);
    TEST_ASSERT_EQ(pvpBlock.getTxTime(1, 1),
                   1 + pvp.getReqSetSize() * NUM_CHANNELS);
    TEST_ASSERT_EQ(pvpBlock.getTxTime(2, 1),
                   2 + pvp.getReqSetSize() * NUM_CHANNELS);

    TEST_ASSERT_EQ(pvpBlock.getTxPos(0, 0)[0], 3);
    TEST_ASSERT_EQ(pvpBlock.getTxPos(0, 0)[1], 6);
    TEST_ASSERT_EQ(pvpBlock.getTxPos(0, 0)[2], 9);
}

TEST_MAIN(
    TEST_CHECK(testPvpRequired);
    TEST_CHECK(testPvpOptional);
    TEST_CHECK(testPvpThrow);
    TEST_CHECK(testPvpEquality);
    TEST_CHECK(testLoadPVPBlockFromMemory);
    )

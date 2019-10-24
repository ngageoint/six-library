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

#include <cphd/PVP.h>
#include <cphd/PVPBlock.h>

#include "TestCase.h"

static const size_t NUM_CHANNELS = 3;
static const size_t NUM_VECTORS = 2;

namespace
{
// [-1000, 1000]
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

void setPVPXML(const std::string& testName,
                cphd::Pvp& pvp)
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

void setVectorParameters(const std::string& testName,
                          size_t channel,
                          size_t vector,
                          cphd::PVPBlock& pvpBlock,
                          cphd::PVPBlock& pvpBlock2)
{
    const double txTime = getRandom();
    pvpBlock.setTxTime(txTime, channel, vector);
    pvpBlock2.setTxTime(txTime, channel, vector);

    const cphd::Vector3 txPos = getRandomVector3();
    pvpBlock.setTxPos(txPos, channel, vector);
    pvpBlock2.setTxPos(txPos, channel, vector);

    const cphd::Vector3 txVel = getRandomVector3();
    pvpBlock.setTxVel(txVel, channel, vector);
    pvpBlock2.setTxVel(txVel, channel, vector);

    const double rcvTime = getRandom();
    pvpBlock.setRcvTime(rcvTime, channel, vector);
    pvpBlock2.setRcvTime(rcvTime, channel, vector);

    const cphd::Vector3 rcvPos = getRandomVector3();
    pvpBlock.setRcvPos(rcvPos, channel, vector);
    pvpBlock2.setRcvPos(rcvPos, channel, vector);

    const cphd::Vector3 rcvVel = getRandomVector3();
    pvpBlock.setRcvVel(rcvVel, channel, vector);
    pvpBlock2.setRcvVel(rcvVel, channel, vector);

    const cphd::Vector3 srpPos = getRandomVector3();
    pvpBlock.setSRPPos(srpPos, channel, vector);
    pvpBlock2.setSRPPos(srpPos, channel, vector);

    const double aFDOP = getRandom();
    pvpBlock.setaFDOP(aFDOP, channel, vector);
    pvpBlock2.setaFDOP(aFDOP, channel, vector);

    const double aFRR1 = getRandom();
    pvpBlock.setaFRR1(aFRR1, channel, vector);
    pvpBlock2.setaFRR1(aFRR1, channel, vector);

    const double aFRR2 = getRandom();
    pvpBlock.setaFRR2(aFRR2, channel, vector);
    pvpBlock2.setaFRR2(aFRR2, channel, vector);

    const double fx1 = getRandom();
    pvpBlock.setFx1(fx1, channel, vector);
    pvpBlock2.setFx1(fx1, channel, vector);

    const double fx2 = getRandom();
    pvpBlock.setFx2(fx2, channel, vector);
    pvpBlock2.setFx2(fx2, channel, vector);

    const double toa1 = getRandom();
    pvpBlock.setTOA1(toa1, channel, vector);
    pvpBlock2.setTOA1(toa1, channel, vector);

    const double toa2 = getRandom();
    pvpBlock.setTOA2(toa2, channel, vector);
    pvpBlock2.setTOA2(toa2, channel, vector);

    const double tdTropoSRP = getRandom();
    pvpBlock.setTdTropoSRP(tdTropoSRP, channel, vector);
    pvpBlock2.setTdTropoSRP(tdTropoSRP, channel, vector);

    const double sc0 = getRandom();
    pvpBlock.setSC0(sc0, channel, vector);
    pvpBlock2.setSC0(sc0, channel, vector);

    const double scss = getRandom();
    pvpBlock.setSCSS(scss, channel, vector);
    pvpBlock2.setSCSS(scss, channel, vector);
}

void testVectorParameters(const std::string& testName,
                          size_t channel,
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

    TEST_ASSERT_EQ(txTime, pvpBlock.getTxTime(channel, vector));
    TEST_ASSERT_EQ(txPos, pvpBlock.getTxPos(channel, vector));
    TEST_ASSERT_EQ(txVel, pvpBlock.getTxVel(channel, vector));
    TEST_ASSERT_EQ(rcvTime, pvpBlock.getRcvTime(channel, vector));
    TEST_ASSERT_EQ(rcvPos, pvpBlock.getRcvPos(channel, vector));
    TEST_ASSERT_EQ(rcvVel, pvpBlock.getRcvVel(channel, vector));
    TEST_ASSERT_EQ(srpPos, pvpBlock.getSRPPos(channel, vector));
    TEST_ASSERT_EQ(aFDOP, pvpBlock.getaFDOP(channel, vector));
    TEST_ASSERT_EQ(aFRR1, pvpBlock.getaFRR1(channel, vector));
    TEST_ASSERT_EQ(aFRR2, pvpBlock.getaFRR2(channel, vector));
    TEST_ASSERT_EQ(fx1, pvpBlock.getFx1(channel, vector));
    TEST_ASSERT_EQ(fx2, pvpBlock.getFx2(channel, vector));
    TEST_ASSERT_EQ(toa1, pvpBlock.getTOA1(channel, vector));
    TEST_ASSERT_EQ(toa2, pvpBlock.getTOA2(channel, vector));
    TEST_ASSERT_EQ(tdTropoSRP, pvpBlock.getTdTropoSRP(channel, vector));
    TEST_ASSERT_EQ(sc0, pvpBlock.getSC0(channel, vector));
    TEST_ASSERT_EQ(scss, pvpBlock.getSCSS(channel, vector));
}

TEST_CASE(testPvpRequired)
{
    cphd::Pvp pvp;
    setPVPXML(testName, pvp);
    cphd::PVPBlock pvpBlock(216,
                            NUM_CHANNELS,
                            std::vector<size_t>(NUM_CHANNELS, NUM_VECTORS),
                            pvp);

    for (size_t channel = 0; channel < NUM_CHANNELS; ++channel)
    {
        for (size_t vector = 0; vector < NUM_VECTORS; ++vector)
        {
            testVectorParameters(testName, channel, vector, pvpBlock);
        }
    }
}

TEST_CASE(testPvpOptional)
{
    cphd::Pvp pvp;
    setPVPXML(testName, pvp);
    pvp.ampSF.reset(new cphd::PVPType());
    pvp.setData(*pvp.ampSF, 1, 28, "F8");
    pvp.fxN1.reset(new cphd::PVPType());
    pvp.setData(*pvp.fxN1, 1, 27, "F8");
    pvp.fxN2.reset(new cphd::PVPType());
    pvp.setData(*pvp.fxN2, 1, 29, "F8");
    pvp.setData(1, 30, "F8", "Param1");
    pvp.setData(1, 31, "S10", "Param2");
    pvp.setData(1, 32, "CI16", "Param3");
    cphd::PVPBlock pvpBlock(2,
                            NUM_CHANNELS,
                            std::vector<size_t>(NUM_CHANNELS, NUM_VECTORS),
                            pvp);

    for (size_t channel = 0; channel < NUM_CHANNELS; ++channel)
    {
        for (size_t vector = 0; vector < NUM_VECTORS; ++vector)
        {
            testVectorParameters(testName, channel, vector, pvpBlock);

            const double ampSF = getRandom();
            pvpBlock.setAmpSF(ampSF, channel, vector);
            const double fxN1 = getRandom();
            pvpBlock.setFxN1(fxN1, channel, vector);
            const double fxN2 = getRandom();
            pvpBlock.setFxN2(fxN2, channel, vector);
            const double addedParam1 = getRandom();
            pvpBlock.setAddedPVP(addedParam1, channel, vector, "Param1");
            const std::string addedParam2 = "Param1";
            pvpBlock.setAddedPVP(addedParam2, channel, vector, "Param2");
            const std::complex<int> addedParam3(3,4);
            pvpBlock.setAddedPVP(addedParam3, channel, vector, "Param3");

            TEST_ASSERT_EQ(ampSF, pvpBlock.getAmpSF(channel, vector));
            TEST_ASSERT_EQ(fxN1, pvpBlock.getFxN1(channel, vector));
            TEST_ASSERT_EQ(fxN2, pvpBlock.getFxN2(channel, vector));
            TEST_ASSERT_EQ(addedParam1, pvpBlock.getAddedPVP<double>(channel, vector, "Param1"));
            TEST_ASSERT_EQ(addedParam2, pvpBlock.getAddedPVP<std::string>(channel, vector, "Param2"));
            TEST_ASSERT_EQ(addedParam3, pvpBlock.getAddedPVP<std::complex<int> >(channel, vector, "Param3"));
        }
    }
}

TEST_CASE(testPvpThrow)
{
    cphd::Pvp pvp;
    setPVPXML(testName, pvp);
    pvp.ampSF.reset(new cphd::PVPType());
    pvp.setData(*pvp.ampSF, 1, 29, "F8");
    pvp.fxN1.reset(new cphd::PVPType());
    pvp.setData(*pvp.fxN1, 1, 28, "F8");
    pvp.toaE1.reset(new cphd::PVPType());
    TEST_EXCEPTION(pvp.setData(*pvp.toaE1, 1, 15, "F8")); // Overwrite block
    TEST_EXCEPTION(pvp.setData(*pvp.toaE1, 3, 27, "F8")); // Overwrite block
    pvp.setData(*pvp.toaE1, 1, 27, "F8");

    pvp.setData(1, 30, "F8", "Param1");
    pvp.setData(1, 31, "F8", "Param2");
    TEST_EXCEPTION(pvp.setData(1, 30, "F8", "Param1")); //Rewriting to existing memory block
    TEST_EXCEPTION(pvp.setData(1, 30, "X=F8;YF8;", "Param1")); //
    TEST_EXCEPTION(pvp.setData(1, 30, "X=F8;Y=F8;Z=", "Param1"));

    cphd::PVPBlock pvpBlock(216, // NumBytes not validated yet
                            NUM_CHANNELS,
                            std::vector<size_t>(NUM_CHANNELS, NUM_VECTORS),
                            pvp);

    for (size_t channel = 0; channel < NUM_CHANNELS; ++channel)
    {
        for (size_t vector = 0; vector < NUM_VECTORS; ++vector)
        {
            testVectorParameters(testName, channel, vector, pvpBlock);

            const double ampSF = getRandom();
            pvpBlock.setAmpSF(ampSF, channel, vector);

            const double fxN1 = getRandom();
            pvpBlock.setFxN1(fxN1, channel, vector);

            const double toaE1 = getRandom();
            pvpBlock.setTOAE1(toaE1, channel, vector);

            TEST_ASSERT_EQ(toaE1, pvpBlock.getTOAE1(channel, vector));

            const double addedParam1 = getRandom();
            pvpBlock.setAddedPVP(addedParam1, channel, vector, "Param1");

            const double addedParam2 = getRandom();
            TEST_EXCEPTION(pvpBlock.setAddedPVP(addedParam2, channel, vector, "Param3"));
            TEST_EXCEPTION(pvpBlock.getAddedPVP<double>(channel, vector, "Param3"));

            const double fxN2 = getRandom();
            TEST_EXCEPTION(pvpBlock.setFxN2(fxN2, channel, vector));
            TEST_EXCEPTION(pvpBlock.getFxN2(channel, vector));
        }
    }

    cphd::Pvp pvp2;
    setPVPXML(testName, pvp2);
    pvp2.setData(1, 27, "F8", "Param1");
    cphd::PVPBlock pvpBlock2(216,
                            NUM_CHANNELS,
                            std::vector<size_t>(NUM_CHANNELS,NUM_VECTORS),
                            pvp2);

        for (size_t channel = 0; channel < NUM_CHANNELS; ++channel)
    {
        for (size_t vector = 0; vector < NUM_VECTORS; ++vector)
        {
            testVectorParameters(testName, channel, vector, pvpBlock2);

            const double addedParam1 = getRandom();
            pvpBlock2.setAddedPVP(addedParam1, channel, vector, "Param1");
            pvpBlock2.getAddedPVP<double>(channel, vector, "Param1");
        }
    }

    cphd::Pvp pvp3;
    setPVPXML(testName, pvp3);
    pvp3.setData(1, 27, "F8", "Param1");
    cphd::PVPBlock pvpBlock3(216,
                            NUM_CHANNELS,
                            std::vector<size_t>(NUM_CHANNELS,NUM_VECTORS),
                            pvp3);

        for (size_t channel = 0; channel < NUM_CHANNELS; ++channel)
    {
        for (size_t vector = 0; vector < NUM_VECTORS; ++vector)
        {
            testVectorParameters(testName, channel, vector, pvpBlock3);

            const double addedParam1 = getRandom();
            pvpBlock3.setAddedPVP(addedParam1, channel, vector, "Param1");
            pvpBlock3.getAddedPVP<double>(channel, vector, "Param1");
        }
    }
}

TEST_CASE(testPvpEquality)
{
    cphd::Pvp pvp1;
    setPVPXML(testName, pvp1);
    pvp1.ampSF.reset(new cphd::PVPType());
    pvp1.setData(*pvp1.ampSF, 1, 28, "F8");
    pvp1.fxN1.reset(new cphd::PVPType());
    pvp1.setData(*pvp1.fxN1, 1, 27, "F8");
    pvp1.fxN2.reset(new cphd::PVPType());
    pvp1.setData(*pvp1.fxN2, 1, 29, "F8");
    pvp1.setData(1, 30, "F8", "Param1");
    pvp1.setData(1, 31, "CI8", "Param2");
    cphd::PVPBlock pvpBlock1(216,
                            NUM_CHANNELS,
                            std::vector<size_t>(NUM_CHANNELS, NUM_VECTORS),
                            pvp1);

    cphd::Pvp pvp2;
    setPVPXML(testName, pvp2);
    pvp2.ampSF.reset(new cphd::PVPType());
    pvp2.setData(*pvp2.ampSF, 1, 28, "F8");
    pvp2.fxN1.reset(new cphd::PVPType());
    pvp2.setData(*pvp2.fxN1, 1, 27, "F8");
    pvp2.fxN2.reset(new cphd::PVPType());
    pvp2.setData(*pvp2.fxN2, 1, 29, "F8");
    pvp2.setData(1, 30, "F8", "Param1");
    pvp2.setData(1, 31, "CI8", "Param2");
    cphd::PVPBlock pvpBlock2(216,
                            NUM_CHANNELS,
                            std::vector<size_t>(NUM_CHANNELS, NUM_VECTORS),
                            pvp2);

    TEST_ASSERT_EQ(pvp1, pvp2);
    TEST_ASSERT_TRUE(pvpBlock1 == pvpBlock2);

    for (size_t channel = 0; channel < NUM_CHANNELS; ++channel)
    {
        for (size_t vector = 0; vector < NUM_VECTORS; ++vector)
        {
            setVectorParameters(testName, channel, vector, pvpBlock1, pvpBlock2);
            const double ampSF = getRandom();
            pvpBlock1.setAmpSF(ampSF, channel, vector);
            pvpBlock2.setAmpSF(ampSF, channel, vector);

            const double fxN1 = getRandom();
            pvpBlock1.setFxN1(fxN1, channel, vector);
            pvpBlock2.setFxN1(fxN1, channel, vector);

            const double fxN2 = getRandom();
            pvpBlock1.setFxN2(fxN2, channel, vector);
            pvpBlock2.setFxN2(fxN2, channel, vector);

            const double addedParam1 = getRandom();
            pvpBlock1.setAddedPVP(addedParam1, channel, vector, "Param1");
            pvpBlock2.setAddedPVP(addedParam1, channel, vector, "Param1");

            const std::complex<int> addedParam2(3,4);
            pvpBlock1.setAddedPVP(addedParam2, channel, vector, "Param2");
            pvpBlock2.setAddedPVP(addedParam2, channel, vector, "Param2");
        }
    }
    TEST_ASSERT_EQ(pvpBlock1, pvpBlock2);
}
}

int main(int , char** )
{
    ::srand(174);
    TEST_CHECK(testPvpRequired);
    TEST_CHECK(testPvpOptional);
    TEST_CHECK(testPvpThrow);
    TEST_CHECK(testPvpEquality);
    return 0;
}


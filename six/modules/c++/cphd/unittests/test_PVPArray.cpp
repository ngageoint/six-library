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
#include <cphd/PVPArray.h>

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
                          cphd::PVPArray& pvpArray,
                          cphd::PVPArray& pvpArray2)
{
    const double txTime = getRandom();
    pvpArray.setTxTime(txTime, channel, vector);
    pvpArray2.setTxTime(txTime, channel, vector);

    const cphd::Vector3 txPos = getRandomVector3();
    pvpArray.setTxPos(txPos, channel, vector);
    pvpArray2.setTxPos(txPos, channel, vector);

    const cphd::Vector3 txVel = getRandomVector3();
    pvpArray.setTxVel(txVel, channel, vector);
    pvpArray2.setTxVel(txVel, channel, vector);

    const double rcvTime = getRandom();
    pvpArray.setRcvTime(rcvTime, channel, vector);
    pvpArray2.setRcvTime(rcvTime, channel, vector);

    const cphd::Vector3 rcvPos = getRandomVector3();
    pvpArray.setRcvPos(rcvPos, channel, vector);
    pvpArray2.setRcvPos(rcvPos, channel, vector);

    const cphd::Vector3 rcvVel = getRandomVector3();
    pvpArray.setRcvVel(rcvVel, channel, vector);
    pvpArray2.setRcvVel(rcvVel, channel, vector);

    const cphd::Vector3 srpPos = getRandomVector3();
    pvpArray.setSRPPos(srpPos, channel, vector);
    pvpArray2.setSRPPos(srpPos, channel, vector);

    const double aFDOP = getRandom();
    pvpArray.setaFDOP(aFDOP, channel, vector);
    pvpArray2.setaFDOP(aFDOP, channel, vector);

    const double aFRR1 = getRandom();
    pvpArray.setaFRR1(aFRR1, channel, vector);
    pvpArray2.setaFRR1(aFRR1, channel, vector);

    const double aFRR2 = getRandom();
    pvpArray.setaFRR2(aFRR2, channel, vector);
    pvpArray2.setaFRR2(aFRR2, channel, vector);

    const double fx1 = getRandom();
    pvpArray.setFx1(fx1, channel, vector);
    pvpArray2.setFx1(fx1, channel, vector);

    const double fx2 = getRandom();
    pvpArray.setFx2(fx2, channel, vector);
    pvpArray2.setFx2(fx2, channel, vector);

    const double toa1 = getRandom();
    pvpArray.setTOA1(toa1, channel, vector);
    pvpArray2.setTOA1(toa1, channel, vector);

    const double toa2 = getRandom();
    pvpArray.setTOA2(toa2, channel, vector);
    pvpArray2.setTOA2(toa2, channel, vector);

    const double tdTropoSRP = getRandom();
    pvpArray.setTdTropoSRP(tdTropoSRP, channel, vector);
    pvpArray2.setTdTropoSRP(tdTropoSRP, channel, vector);

    const double sc0 = getRandom();
    pvpArray.setSC0(sc0, channel, vector);
    pvpArray2.setSC0(sc0, channel, vector);

    const double scss = getRandom();
    pvpArray.setSCSS(scss, channel, vector);
    pvpArray2.setSCSS(scss, channel, vector);
}

void testVectorParameters(const std::string& testName,
                          size_t channel,
                          size_t vector,
                          cphd::PVPArray& pvpArray)
{
    const double txTime = getRandom();
    pvpArray.setTxTime(txTime, channel, vector);

    const cphd::Vector3 txPos = getRandomVector3();
    pvpArray.setTxPos(txPos, channel, vector);

    const cphd::Vector3 txVel = getRandomVector3();
    pvpArray.setTxVel(txVel, channel, vector);

    const double rcvTime = getRandom();
    pvpArray.setRcvTime(rcvTime, channel, vector);

    const cphd::Vector3 rcvPos = getRandomVector3();
    pvpArray.setRcvPos(rcvPos, channel, vector);

    const cphd::Vector3 rcvVel = getRandomVector3();
    pvpArray.setRcvVel(rcvVel, channel, vector);

    const cphd::Vector3 srpPos = getRandomVector3();
    pvpArray.setSRPPos(srpPos, channel, vector);

    const double aFDOP = getRandom();
    pvpArray.setaFDOP(aFDOP, channel, vector);

    const double aFRR1 = getRandom();
    pvpArray.setaFRR1(aFRR1, channel, vector);

    const double aFRR2 = getRandom();
    pvpArray.setaFRR2(aFRR2, channel, vector);

    const double fx1 = getRandom();
    pvpArray.setFx1(fx1, channel, vector);

    const double fx2 = getRandom();
    pvpArray.setFx2(fx2, channel, vector);

    const double toa1 = getRandom();
    pvpArray.setTOA1(toa1, channel, vector);

    const double toa2 = getRandom();
    pvpArray.setTOA2(toa2, channel, vector);

    const double tdTropoSRP = getRandom();
    pvpArray.setTdTropoSRP(tdTropoSRP, channel, vector);

    const double sc0 = getRandom();
    pvpArray.setSC0(sc0, channel, vector);

    const double scss = getRandom();
    pvpArray.setSCSS(scss, channel, vector);

    TEST_ASSERT_EQ(txTime, pvpArray.getTxTime(channel, vector));
    TEST_ASSERT_EQ(txPos, pvpArray.getTxPos(channel, vector));
    TEST_ASSERT_EQ(txVel, pvpArray.getTxVel(channel, vector));
    TEST_ASSERT_EQ(rcvTime, pvpArray.getRcvTime(channel, vector));
    TEST_ASSERT_EQ(rcvPos, pvpArray.getRcvPos(channel, vector));
    TEST_ASSERT_EQ(rcvVel, pvpArray.getRcvVel(channel, vector));
    TEST_ASSERT_EQ(srpPos, pvpArray.getSRPPos(channel, vector));
    TEST_ASSERT_EQ(aFDOP, pvpArray.getaFDOP(channel, vector));
    TEST_ASSERT_EQ(aFRR1, pvpArray.getaFRR1(channel, vector));
    TEST_ASSERT_EQ(aFRR2, pvpArray.getaFRR2(channel, vector));
    TEST_ASSERT_EQ(fx1, pvpArray.getFx1(channel, vector));
    TEST_ASSERT_EQ(fx2, pvpArray.getFx2(channel, vector));
    TEST_ASSERT_EQ(toa1, pvpArray.getTOA1(channel, vector));
    TEST_ASSERT_EQ(toa2, pvpArray.getTOA2(channel, vector));
    TEST_ASSERT_EQ(tdTropoSRP, pvpArray.getTdTropoSRP(channel, vector));
    TEST_ASSERT_EQ(sc0, pvpArray.getSC0(channel, vector));
    TEST_ASSERT_EQ(scss, pvpArray.getSCSS(channel, vector));
}

TEST_CASE(testPvpRequired)
{
    cphd::PVPArray pvpArray(216,
                            NUM_CHANNELS,
                            std::vector<size_t>(NUM_CHANNELS, NUM_VECTORS));
    cphd::Pvp pvp;
    setPVPXML(testName, pvp);

    for (size_t channel = 0; channel < NUM_CHANNELS; ++channel)
    {
        for (size_t vector = 0; vector < NUM_VECTORS; ++vector)
        {
            testVectorParameters(testName, channel, vector, pvpArray);
        }
    }
}

TEST_CASE(testPvpOptional)
{
    size_t numAddedParams = 3;
    cphd::PVPArray pvpArray(2,
                            NUM_CHANNELS,
                            std::vector<size_t>(NUM_CHANNELS, NUM_VECTORS),
                            numAddedParams);
    cphd::Pvp pvp(numAddedParams);
    setPVPXML(testName, pvp);
    pvp.ampSF.reset(new cphd::PVPType());
    pvp.setData(*pvp.ampSF, 1, 28, "F8");
    pvp.fxN1.reset(new cphd::PVPType());
    pvp.setData(*pvp.fxN1, 1, 27, "F8");
    pvp.fxN2.reset(new cphd::PVPType());
    pvp.setData(*pvp.fxN2, 1, 29, "F8");
    pvp.setData(1, 30, "F8", "Param1", 0);
    pvp.setData(1, 31, "S10", "Param2", 1);
    pvp.setData(1, 32, "CI16", "Param3", 2);

    for (size_t channel = 0; channel < NUM_CHANNELS; ++channel)
    {
        for (size_t vector = 0; vector < NUM_VECTORS; ++vector)
        {
            testVectorParameters(testName, channel, vector, pvpArray);

            const double ampSF = getRandom();
            pvpArray.setAmpSF(pvp, ampSF, channel, vector);

            const double fxN1 = getRandom();
            pvpArray.setFxN1(pvp, fxN1, channel, vector);

            const double fxN2 = getRandom();
            pvpArray.setFxN2(pvp, fxN2, channel, vector);

            const double addedParam1 = getRandom();
            pvpArray.setAddedPVP(pvp, addedParam1, channel, vector, 0);

            const std::string addedParam2 = "Param1";
            pvpArray.setAddedPVP(pvp, addedParam2, channel, vector, 1);

            const std::complex<int> addedParam3(3,4);
            pvpArray.setAddedPVP(pvp, addedParam3, channel, vector, 2);

            TEST_ASSERT_EQ(ampSF, pvpArray.getAmpSF(channel, vector));
            TEST_ASSERT_EQ(fxN1, pvpArray.getFxN1(channel, vector));
            TEST_ASSERT_EQ(fxN2, pvpArray.getFxN2(channel, vector));
            TEST_ASSERT_EQ(addedParam1, pvpArray.getAddedPVP<double>(channel, vector, 0));
            TEST_ASSERT_EQ(addedParam2, pvpArray.getAddedPVP<std::string>(channel, vector, 1));
            TEST_ASSERT_EQ(addedParam3, pvpArray.getAddedPVP<std::complex<int> >(channel, vector, 2));
        }
    }
}

TEST_CASE(testPvpThrow)
{
    size_t numAddedParams = 2;
    cphd::PVPArray pvpArray(216, // NumBytes not validated yet
                            NUM_CHANNELS,
                            std::vector<size_t>(NUM_CHANNELS, NUM_VECTORS),
                            numAddedParams);
    cphd::Pvp pvp(numAddedParams);
    setPVPXML(testName, pvp);
    pvp.ampSF.reset(new cphd::PVPType());
    pvp.setData(*pvp.ampSF, 1, 29, "F8");
    pvp.fxN1.reset(new cphd::PVPType());
    pvp.setData(*pvp.fxN1, 1, 28, "F8");
    pvp.toaE1.reset(new cphd::PVPType());
    TEST_EXCEPTION(pvp.setData(*pvp.toaE1, 1, 15, "F8")); // Overwrite block
    TEST_EXCEPTION(pvp.setData(*pvp.toaE1, 3, 27, "F8")); // Overwrite block
    pvp.setData(*pvp.toaE1, 1, 27, "F8");

    pvp.setData(1, 30, "F8", "Param1", 0);
    pvp.setData(1, 31, "F8", "Param2", 1);
    TEST_EXCEPTION(pvp.setData(1, 30, "F8", "Param1", 3));
    TEST_EXCEPTION(pvp.setData(1, 30, "X=F8;YF8;", "Param1", 3));
    TEST_EXCEPTION(pvp.setData(1, 30, "X=F8;Y=F8;Z=", "Param1", 3));

    for (size_t channel = 0; channel < NUM_CHANNELS; ++channel)
    {
        for (size_t vector = 0; vector < NUM_VECTORS; ++vector)
        {
            testVectorParameters(testName, channel, vector, pvpArray);

            const double ampSF = getRandom();
            pvpArray.setAmpSF(pvp, ampSF, channel, vector);

            const double fxN1 = getRandom();
            pvpArray.setFxN1(pvp, fxN1, channel, vector);

            const double toaE1 = getRandom();
            pvpArray.setTOAE1(pvp, toaE1, channel, vector);
            TEST_ASSERT_EQ(toaE1, pvpArray.getTOAE1(channel, vector));

            const double addedParam1 = getRandom();
            pvpArray.setAddedPVP(pvp, addedParam1, channel, vector, 0);

            const double addedParam2 = getRandom();
            TEST_EXCEPTION(pvpArray.setAddedPVP(pvp, addedParam2, channel, vector, 2));
            TEST_EXCEPTION(pvpArray.getAddedPVP<double>(channel, vector, 3));
            pvpArray.setAddedPVP(pvp, addedParam1, channel, vector, 0);

            const double fxN2 = getRandom();
            TEST_EXCEPTION(pvpArray.setFxN2(pvp, fxN2, channel, vector));
            TEST_EXCEPTION(pvpArray.getFxN2(channel, vector));
        }
    }

    cphd::PVPArray pvpArray2(216,
                            NUM_CHANNELS,
                            std::vector<size_t>(NUM_CHANNELS,NUM_VECTORS));
    cphd::Pvp pvp2(numAddedParams);
    setPVPXML(testName, pvp2);
    pvp2.setData(1, 27, "F8", "Param1", 0);

        for (size_t channel = 0; channel < NUM_CHANNELS; ++channel)
    {
        for (size_t vector = 0; vector < NUM_VECTORS; ++vector)
        {
            testVectorParameters(testName, channel, vector, pvpArray2);

            const double addedParam1 = getRandom();
            TEST_EXCEPTION(pvpArray2.setAddedPVP(pvp2, addedParam1, channel, vector, 0));
            TEST_EXCEPTION(pvpArray2.getAddedPVP<double>(channel, vector, 0));
        }
    }

    cphd::PVPArray pvpArray3(216,
                            NUM_CHANNELS,
                            std::vector<size_t>(NUM_CHANNELS,NUM_VECTORS),
                            numAddedParams);
    cphd::Pvp pvp3;
    setPVPXML(testName, pvp3);
    TEST_EXCEPTION(pvp3.setData(1, 27, "F8", "Param1", 0));

        for (size_t channel = 0; channel < NUM_CHANNELS; ++channel)
    {
        for (size_t vector = 0; vector < NUM_VECTORS; ++vector)
        {
            testVectorParameters(testName, channel, vector, pvpArray3);

            const double addedParam1 = getRandom();
            TEST_EXCEPTION(pvpArray3.setAddedPVP(pvp3, addedParam1, channel, vector, 0));
            TEST_EXCEPTION(pvpArray3.getAddedPVP<double>(channel, vector, 0));
        }
    }
}

TEST_CASE(testPvpEquality)
{
    size_t numAddedParams = 2;
    cphd::PVPArray pvpArray1(216,
                            NUM_CHANNELS,
                            std::vector<size_t>(NUM_CHANNELS, NUM_VECTORS),
                            numAddedParams);
    cphd::PVPArray pvpArray2(216,
                            NUM_CHANNELS,
                            std::vector<size_t>(NUM_CHANNELS, NUM_VECTORS),
                            numAddedParams);

    TEST_ASSERT_TRUE(pvpArray1 == pvpArray2);

    cphd::Pvp pvp1(numAddedParams);
    setPVPXML(testName, pvp1);
    pvp1.ampSF.reset(new cphd::PVPType());
    pvp1.setData(*pvp1.ampSF, 1, 28, "F8");
    pvp1.fxN1.reset(new cphd::PVPType());
    pvp1.setData(*pvp1.fxN1, 1, 27, "F8");
    pvp1.fxN2.reset(new cphd::PVPType());
    pvp1.setData(*pvp1.fxN2, 1, 29, "F8");
    pvp1.setData(1, 30, "F8", "Param1", 0);
    pvp1.setData(1, 31, "CI8", "Param1", 1);

    cphd::Pvp pvp2(numAddedParams);
    setPVPXML(testName, pvp2);
    pvp2.ampSF.reset(new cphd::PVPType());
    pvp2.setData(*pvp2.ampSF, 1, 28, "F8");
    pvp2.fxN1.reset(new cphd::PVPType());
    pvp2.setData(*pvp2.fxN1, 1, 27, "F8");
    pvp2.fxN2.reset(new cphd::PVPType());
    pvp2.setData(*pvp2.fxN2, 1, 29, "F8");
    pvp2.setData(1, 30, "F8", "Param1", 0);
    pvp2.setData(1, 31, "CI8", "Param1", 1);

    TEST_ASSERT_EQ(pvp1, pvp2);

    for (size_t channel = 0; channel < NUM_CHANNELS; ++channel)
    {
        for (size_t vector = 0; vector < NUM_VECTORS; ++vector)
        {
            setVectorParameters(testName, channel, vector, pvpArray1, pvpArray2);
            const double ampSF = getRandom();
            pvpArray1.setAmpSF(pvp1, ampSF, channel, vector);
            pvpArray2.setAmpSF(pvp2, ampSF, channel, vector);

            const double fxN1 = getRandom();
            pvpArray1.setFxN1(pvp1, fxN1, channel, vector);
            pvpArray2.setFxN1(pvp2, fxN1, channel, vector);

            const double fxN2 = getRandom();
            pvpArray1.setFxN2(pvp1, fxN2, channel, vector);
            pvpArray2.setFxN2(pvp2, fxN2, channel, vector);

            const double addedParam1 = 5.0;
            pvpArray1.setAddedPVP(pvp1, addedParam1, channel, vector, 0);
            pvpArray2.setAddedPVP(pvp2, addedParam1, channel, vector, 0);

            const std::complex<int> addedParam2(3,4);
            pvpArray1.setAddedPVP(pvp1, addedParam2, channel, vector, 1);
            pvpArray2.setAddedPVP(pvp2, addedParam2, channel, vector, 1);
        }
    }
    TEST_ASSERT_EQ(pvpArray1, pvpArray2);
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


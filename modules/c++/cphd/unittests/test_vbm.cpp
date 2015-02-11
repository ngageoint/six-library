/* =========================================================================
 * This file is part of cphd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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

#include <cphd/VBM.h>

#include "TestCase.h"

static const size_t NUM_CHANNELS = 3;
static const size_t NUM_VECTORS = 7;

namespace
{
double getRandom()
{
    double r = static_cast<double>(rand()) / static_cast<double>(RAND_MAX);
    return -1000.0  + r * (1000.0 + 1000.0), 2;
}

cphd::Vector3 getRandomVector3()
{
    cphd::Vector3 ret;
    ret[0] = getRandom();
    ret[1] = getRandom();
    ret[2] = getRandom();
    return ret;
}

void testVectorParameters(const std::string& testName,
                          size_t channel,
                          size_t vector,
                          cphd::VBM& vbm)
{
    const double txTime = getRandom();
    vbm.setTxTime(txTime, channel, vector);

    const cphd::Vector3 txPos = getRandomVector3();
    vbm.setTxPos(txPos, channel, vector);

    const double rcvTime = getRandom();
    vbm.setRcvTime(rcvTime, channel, vector);

    const cphd::Vector3 rcvPos = getRandomVector3();
    vbm.setRcvPos(rcvPos, channel, vector);

    const double srpTime = getRandom();
    vbm.setSRPTime(srpTime, channel, vector);

    const cphd::Vector3 srpPos = getRandomVector3();
    vbm.setSRPPos(srpPos, channel, vector);

    const double tropoSrp = getRandom();
    vbm.setTropoSRP(tropoSrp, channel, vector);

    const double ampSF = getRandom();
    vbm.setAmpSF(ampSF, channel, vector);

    TEST_ASSERT_EQ(txTime, vbm.getTxTime(channel, vector));
    TEST_ASSERT_EQ(txPos, vbm.getTxPos(channel, vector));
    TEST_ASSERT_EQ(rcvTime, vbm.getRcvTime(channel, vector));
    TEST_ASSERT_EQ(rcvPos, vbm.getRcvPos(channel, vector));
    TEST_ASSERT_EQ(srpTime, vbm.getSRPTime(channel, vector));
    TEST_ASSERT_EQ(srpPos, vbm.getSRPPos(channel, vector));
    TEST_ASSERT_EQ(tropoSrp, vbm.getTropoSRP(channel, vector));
    TEST_ASSERT_EQ(ampSF, vbm.getAmpSF(channel, vector));
}

TEST_CASE(testVbmFx)
{
    cphd::VBM vbm(NUM_CHANNELS,
                  std::vector<size_t>(NUM_CHANNELS, NUM_VECTORS),
                  true,
                  true,
                  true,
                  cphd::DomainType::FX);

    for (size_t channel = 0; channel < NUM_CHANNELS; ++channel)
    {
        for (size_t vector = 0; vector < NUM_VECTORS; ++vector)
        {
            testVectorParameters(testName, channel, vector, vbm);

            const double fx0 = getRandom();
            vbm.setFx0(fx0, channel, vector);

            const double fxSS = getRandom();
            vbm.setFxSS(fxSS, channel, vector);

            const double fx1 = getRandom();
            vbm.setFx1(fx1, channel, vector);

            const double fx2 = getRandom();
            vbm.setFx2(fx2, channel, vector);

            TEST_ASSERT_EQ(fx0, vbm.getFx0(channel, vector));
            TEST_ASSERT_EQ(fxSS, vbm.getFxSS(channel, vector));
            TEST_ASSERT_EQ(fx1, vbm.getFx1(channel, vector));
            TEST_ASSERT_EQ(fx2, vbm.getFx2(channel, vector));
        }
    }
}

TEST_CASE(testVbmToa)
{
    cphd::VBM vbm(NUM_CHANNELS,
                  std::vector<size_t>(NUM_CHANNELS, NUM_VECTORS),
                  true,
                  true,
                  true,
                  cphd::DomainType::TOA);

    for (size_t channel = 0; channel < NUM_CHANNELS; ++channel)
    {
        for (size_t vector = 0; vector < NUM_VECTORS; ++vector)
        {
            testVectorParameters(testName, channel, vector, vbm);

            const double deltaToa0 = getRandom();
            vbm.setDeltaTOA0(deltaToa0, channel, vector);

            const double toaSS = getRandom();
            vbm.setTOASS(toaSS, channel, vector);

            TEST_ASSERT_EQ(deltaToa0, vbm.getDeltaTOA0(channel, vector));
            TEST_ASSERT_EQ(toaSS, vbm.getTOASS(channel, vector));
        }
    }
}

TEST_CASE(testVbmThrow)
{
    cphd::VBM vbmFx(NUM_CHANNELS,
                    std::vector<size_t>(NUM_CHANNELS, NUM_VECTORS),
                    false,
                    false,
                    false,
                    cphd::DomainType::FX);

    cphd::VBM vbmToa(NUM_CHANNELS,
                     std::vector<size_t>(NUM_CHANNELS, NUM_VECTORS),
                     false,
                     false,
                     false,
                     cphd::DomainType::TOA);

    for (size_t channel = 0; channel < NUM_CHANNELS; ++channel)
    {
        for (size_t vector = 0; vector < NUM_VECTORS; ++vector)
        {
            TEST_EXCEPTION(vbmFx.setSRPTime(0.0, channel, vector));
            TEST_EXCEPTION(vbmToa.setSRPTime(0.0, channel, vector));
            TEST_EXCEPTION(vbmFx.getSRPTime(channel, vector));
            TEST_EXCEPTION(vbmToa.getSRPTime(channel, vector));

            TEST_EXCEPTION(vbmFx.setTropoSRP(0.0, channel, vector));
            TEST_EXCEPTION(vbmToa.setTropoSRP(0.0, channel, vector));
            TEST_EXCEPTION(vbmFx.getTropoSRP(channel, vector));
            TEST_EXCEPTION(vbmToa.getTropoSRP(channel, vector));

            TEST_EXCEPTION(vbmFx.setAmpSF(0.0, channel, vector));
            TEST_EXCEPTION(vbmToa.setAmpSF(0.0, channel, vector));
            TEST_EXCEPTION(vbmFx.getAmpSF(channel, vector));
            TEST_EXCEPTION(vbmToa.getAmpSF(channel, vector));

            TEST_EXCEPTION(vbmFx.setDeltaTOA0(0.0, channel, vector));
            TEST_EXCEPTION(vbmFx.getDeltaTOA0(channel, vector));

            TEST_EXCEPTION(vbmFx.setTOASS(0.0, channel, vector));
            TEST_EXCEPTION(vbmFx.getTOASS(channel, vector));

            TEST_EXCEPTION(vbmToa.setFx0(0.0, channel, vector));
            TEST_EXCEPTION(vbmToa.getFx0(channel, vector));

            TEST_EXCEPTION(vbmToa.setFxSS(0.0, channel, vector));
            TEST_EXCEPTION(vbmToa.getFxSS(channel, vector));

            TEST_EXCEPTION(vbmToa.setFx1(0.0, channel, vector));
            TEST_EXCEPTION(vbmToa.getFx1(channel, vector));

            TEST_EXCEPTION(vbmToa.setFx2(0.0, channel, vector));
            TEST_EXCEPTION(vbmToa.getFx2(channel, vector));
        }
    }
}

TEST_CASE(testVbmCopy)
{
    cphd::VBM vbmFx(NUM_CHANNELS,
                    std::vector<size_t>(NUM_CHANNELS, NUM_VECTORS),
                    false,
                    false,
                    false,
                    cphd::DomainType::FX);

    cphd::VBM vbmToa(NUM_CHANNELS,
                     std::vector<size_t>(NUM_CHANNELS, NUM_VECTORS),
                     false,
                     false,
                     false,
                     cphd::DomainType::TOA);

    cphd::VBM vbmFxCopy(vbmFx);
    cphd::VBM vbmToaCopy(vbmToa);

    TEST_ASSERT_EQ(vbmFx, vbmFxCopy);
    TEST_ASSERT_EQ(vbmToa, vbmToaCopy);
}
}

int main(int , char** )
{
    ::srand(174);
    TEST_CHECK(testVbmFx);
    TEST_CHECK(testVbmToa);
    TEST_CHECK(testVbmThrow);
    TEST_CHECK(testVbmCopy);
    return 0;
}


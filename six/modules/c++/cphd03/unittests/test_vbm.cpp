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
#include <tuple>

#include <cphd03/VBM.h>
#include <cphd/TestDataGenerator.h>
#include "TestCase.h"

static constexpr size_t NUM_CHANNELS = 3;
static constexpr size_t NUM_VECTORS = 7;

static void call_srand()
{
    static const auto f = []() { constexpr auto seed = 174; ::srand(seed); return nullptr; };
    static const auto result = f();
    std::ignore = result;
}

void testVectorParameters(const std::string& testName_,
                          size_t channel,
                          size_t vector,
                          cphd03::VBM& vbm)
{
    const auto testName = testName_;

    const double txTime = cphd::getRandom();
    vbm.setTxTime(txTime, channel, vector);

    const cphd::Vector3 txPos = cphd::getRandomVector3();
    vbm.setTxPos(txPos, channel, vector);

    const double rcvTime = cphd::getRandom();
    vbm.setRcvTime(rcvTime, channel, vector);

    const cphd::Vector3 rcvPos = cphd::getRandomVector3();
    vbm.setRcvPos(rcvPos, channel, vector);

    const double srpTime = cphd::getRandom();
    vbm.setSRPTime(srpTime, channel, vector);

    const cphd::Vector3 srpPos = cphd::getRandomVector3();
    vbm.setSRPPos(srpPos, channel, vector);

    const double tropoSrp = cphd::getRandom();
    vbm.setTropoSRP(tropoSrp, channel, vector);

    const double ampSF = cphd::getRandom();
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
    call_srand();

    cphd03::VBM vbm(NUM_CHANNELS,
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

            const double fx0 = cphd::getRandom();
            vbm.setFx0(fx0, channel, vector);

            const double fxSS = cphd::getRandom();
            vbm.setFxSS(fxSS, channel, vector);

            const double fx1 = cphd::getRandom();
            vbm.setFx1(fx1, channel, vector);

            const double fx2 = cphd::getRandom();
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
    call_srand();

    cphd03::VBM vbm(NUM_CHANNELS,
                  std::vector<size_t>(NUM_CHANNELS, NUM_VECTORS),
                  true,
                  true,
                  true,
                  cphd::DomainType::TOA);

    for (size_t channel = 0; channel < NUM_CHANNELS; ++channel)
    {
        for (size_t vector = 0; vector < NUM_VECTORS; ++vector)
        {
            testVectorParameters("testVbmToa", channel, vector, vbm);

            const double deltaToa0 = cphd::getRandom();
            vbm.setDeltaTOA0(deltaToa0, channel, vector);

            const double toaSS = cphd::getRandom();
            vbm.setTOASS(toaSS, channel, vector);

            TEST_ASSERT_EQ(deltaToa0, vbm.getDeltaTOA0(channel, vector));
            TEST_ASSERT_EQ(toaSS, vbm.getTOASS(channel, vector));
        }
    }
}

TEST_CASE(testVbmThrow)
{
    call_srand();

    cphd03::VBM vbmFx(NUM_CHANNELS,
                    std::vector<size_t>(NUM_CHANNELS, NUM_VECTORS),
                    false,
                    false,
                    false,
                    cphd::DomainType::FX);

    cphd03::VBM vbmToa(NUM_CHANNELS,
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
    call_srand();

    cphd03::VBM vbmFx(NUM_CHANNELS,
                    std::vector<size_t>(NUM_CHANNELS, NUM_VECTORS),
                    false,
                    false,
                    false,
                    cphd::DomainType::FX);

    cphd03::VBM vbmToa(NUM_CHANNELS,
                     std::vector<size_t>(NUM_CHANNELS, NUM_VECTORS),
                     false,
                     false,
                     false,
                     cphd::DomainType::TOA);

    cphd03::VBM vbmFxCopy(vbmFx);
    cphd03::VBM vbmToaCopy(vbmToa);

    TEST_ASSERT_EQ(vbmFx, vbmFxCopy);
    TEST_ASSERT_EQ(vbmToa, vbmToaCopy);
}

TEST_CASE(testDataConstructor)
{
    call_srand();

    std::vector<std::vector<double> > actualData(NUM_CHANNELS);
    const size_t numValuesNeeded = 18;
    for (size_t ii = 0; ii < NUM_CHANNELS; ++ii)
    {
        actualData[ii].resize(numValuesNeeded * NUM_VECTORS);
        for (size_t jj = 0; jj < NUM_VECTORS; ++jj)
        {
            for (size_t kk = 0; kk < numValuesNeeded; ++kk)
            {
                actualData[ii][(jj * numValuesNeeded) + kk] =
                        (ii * numValuesNeeded) + kk;
            }
        }
    }

    std::vector<const void*> data(NUM_CHANNELS);
    for (size_t ii = 0; ii < NUM_CHANNELS; ++ii)
    {
        data[ii] = actualData[ii].data();
    }

    cphd03::VBM vbm(NUM_CHANNELS,
                  std::vector<size_t>(NUM_CHANNELS, NUM_VECTORS),
                  true,
                  true,
                  true,
                  cphd::DomainType::FX,
                  data);

    for (size_t ii = 0; ii < NUM_CHANNELS; ++ii)
    {
        for (size_t jj = 0; jj < NUM_VECTORS; ++jj)
        {
            const size_t idx = jj * numValuesNeeded;
            TEST_ASSERT_ALMOST_EQ(actualData[ii][idx], vbm.getTxTime(ii, jj));
            const cphd::Vector3 txPos = vbm.getTxPos(ii, jj);
            TEST_ASSERT_ALMOST_EQ(actualData[ii][idx + 1], txPos[0]);
            TEST_ASSERT_ALMOST_EQ(actualData[ii][idx + 2], txPos[1]);
            TEST_ASSERT_ALMOST_EQ(actualData[ii][idx + 3], txPos[2]);

            TEST_ASSERT_ALMOST_EQ(actualData[ii][idx + 4], vbm.getRcvTime(ii, jj));
            const cphd::Vector3 rcvPos = vbm.getRcvPos(ii, jj);
            TEST_ASSERT_ALMOST_EQ(actualData[ii][idx + 5], rcvPos[0]);
            TEST_ASSERT_ALMOST_EQ(actualData[ii][idx + 6], rcvPos[1]);
            TEST_ASSERT_ALMOST_EQ(actualData[ii][idx + 7], rcvPos[2]);

            TEST_ASSERT_ALMOST_EQ(actualData[ii][idx + 8], vbm.getSRPTime(ii, jj));
            const cphd::Vector3 srpPos = vbm.getSRPPos(ii, jj);
            TEST_ASSERT_ALMOST_EQ(actualData[ii][idx + 9], srpPos[0]);
            TEST_ASSERT_ALMOST_EQ(actualData[ii][idx + 10], srpPos[1]);
            TEST_ASSERT_ALMOST_EQ(actualData[ii][idx + 11], srpPos[2]);

            TEST_ASSERT_ALMOST_EQ(actualData[ii][idx + 12], vbm.getTropoSRP(ii, jj));
            TEST_ASSERT_ALMOST_EQ(actualData[ii][idx + 13], vbm.getAmpSF(ii, jj));

            TEST_ASSERT_ALMOST_EQ(actualData[ii][idx + 14], vbm.getFx0(ii, jj));
            TEST_ASSERT_ALMOST_EQ(actualData[ii][idx + 15], vbm.getFxSS(ii, jj));
            TEST_ASSERT_ALMOST_EQ(actualData[ii][idx + 16], vbm.getFx1(ii, jj));
            TEST_ASSERT_ALMOST_EQ(actualData[ii][idx + 17], vbm.getFx2(ii, jj));
        }
    }
}

TEST_MAIN(
    TEST_CHECK(testVbmFx);
    TEST_CHECK(testVbmToa);
    TEST_CHECK(testVbmThrow);
    TEST_CHECK(testVbmCopy);
    TEST_CHECK(testDataConstructor);
    )


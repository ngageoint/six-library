/* =========================================================================
* This file is part of six.sicd-c++
* =========================================================================
*
* (C) Copyright 2004 - 2016, MDA Information Systems LLC
*
* six.sicd-c++ is free software; you can redistribute it and/or modify
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

#include <import/six/sicd.h>
#include "TestCase.h"


TEST_CASE(fmRateFromDemodType)
{
    six::sicd::WaveformParameters params;
    params.rcvDemodType = six::DemodType::CHIRP;
    params.fillDerivedFields();
    TEST_ASSERT_EQ(params.rcvFMRate, 0);
}

TEST_CASE(demodTypeFromFmRate)
{
    six::sicd::WaveformParameters params;
    params.rcvFMRate = 0;
    params.fillDerivedFields();
    TEST_ASSERT(params.rcvDemodType == "CHIRP");
    TEST_ASSERT_EQ(params.rcvDemodType, six::DemodType::CHIRP);
}

TEST_CASE(pulseLength)
{
    six::sicd::WaveformParameters params;
    params.txRFBandwidth = 5;
    params.txFMRate = 0;
    params.fillDerivedFields();
    TEST_ASSERT_EQ(params.txPulseLength,
            std::numeric_limits<double>::infinity());
}

TEST_CASE(txFrequencies)
{
    six::sicd::RadarCollection radarCollection;
    radarCollection.waveform.resize(3);
    double startFrequencies[3] = { 5, 2, 11 };
    for (size_t ii = 0; ii < radarCollection.waveform.size(); ++ii)
    {
        radarCollection.waveform[ii].reset(new six::sicd::WaveformParameters);
        radarCollection.waveform[ii]->txFrequencyStart = startFrequencies[ii];
        radarCollection.waveform[ii]->txRFBandwidth = 2;
    }
    radarCollection.fillDerivedFields();
    TEST_ASSERT_EQ(radarCollection.txFrequencyMin, 2);
    TEST_ASSERT_EQ(radarCollection.txFrequencyMax, 13);
}

TEST_CASE(wfParamTxFrequencies)
{
    six::sicd::RadarCollection radarCollection;
    radarCollection.waveform.resize(1);
    radarCollection.waveform[0].reset(new six::sicd::WaveformParameters);
    radarCollection.txFrequencyMin = 1;
    radarCollection.txFrequencyMax = 5;
    radarCollection.fillDerivedFields();
    TEST_ASSERT_EQ(radarCollection.waveform[0]->txFrequencyStart, 1);
    TEST_ASSERT_EQ(radarCollection.waveform[0]->txRFBandwidth, 4);
}

TEST_MAIN(
    TEST_CHECK(fmRateFromDemodType);
    TEST_CHECK(demodTypeFromFmRate);
    TEST_CHECK(pulseLength);
    TEST_CHECK(txFrequencies);
    )

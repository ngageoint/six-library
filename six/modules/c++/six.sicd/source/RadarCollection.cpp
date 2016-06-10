/* =========================================================================
 * This file is part of six.sicd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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
#include <six/sicd/RadarCollection.h>
#include <six/Utilities.h>

namespace six
{
namespace sicd
{
TxStep::TxStep() :
    waveformIndex(Init::undefined<int>()),
    txPolarization(PolarizationType::NOT_SET)
{
}

TxStep* TxStep::clone() const
{
    return new TxStep(*this);
}

WaveformParameters::WaveformParameters() :
    txPulseLength(Init::undefined<double>()),
    txRFBandwidth(Init::undefined<double>()),
    txFrequencyStart(Init::undefined<double>()),
    txFMRate(Init::undefined<double>()),
    rcvDemodType(DemodType::NOT_SET),
    rcvWindowLength(Init::undefined<double>()),
    adcSampleRate(Init::undefined<double>()),
    rcvIFBandwidth(Init::undefined<double>()),
    rcvFrequencyStart(Init::undefined<double>()),
    rcvFMRate(Init::undefined<double>())
{
}

bool WaveformParameters::operator==(const WaveformParameters& rhs) const
{
    return (txPulseLength == rhs.txPulseLength &&
        txRFBandwidth == rhs.txRFBandwidth &&
        txFrequencyStart == rhs.txFrequencyStart &&
        txFMRate == rhs.txFMRate &&
        rcvDemodType == rhs.rcvDemodType &&
        rcvWindowLength == rhs.rcvWindowLength &&
        adcSampleRate == rhs.adcSampleRate &&
        rcvIFBandwidth == rhs.rcvIFBandwidth &&
        rcvFrequencyStart == rhs.rcvFrequencyStart &&
        rcvFMRate == rhs.rcvFMRate);
}

WaveformParameters* WaveformParameters::clone() const
{
    return new WaveformParameters(*this);
}

void WaveformParameters::fillDerivedFields()
{
    if (rcvDemodType == DemodType::CHIRP &&
        Init::isUndefined<double>(rcvFMRate))
    {
        rcvFMRate = 0;
    }

    if (rcvFMRate == 0 &&
        rcvDemodType == DemodType::NOT_SET)
    {
        rcvDemodType = DemodType::CHIRP;
    }

    if (Init::isUndefined<double>(txRFBandwidth) &&
        !Init::isUndefined<double>(txPulseLength) &&
        !Init::isUndefined<double>(txFMRate))
    {
        txRFBandwidth = txPulseLength * txFMRate;
    }

    if (!Init::isUndefined<double>(txRFBandwidth) &&
        Init::isUndefined<double>(txPulseLength) &&
        !Init::isUndefined<double>(txFMRate))
    {
        if (txFMRate != 0)
        {
            txPulseLength = txRFBandwidth / txFMRate;
        }
    }

    if (!Init::isUndefined<double>(txRFBandwidth) &&
        !Init::isUndefined<double>(txPulseLength) &&
        Init::isUndefined<double>(txFMRate))
    {
        if (txPulseLength != 0)
        {
            txFMRate = txRFBandwidth / txPulseLength;
        }
    }
}

ChannelParameters::ChannelParameters() :
    txRcvPolarization(DualPolarizationType::NOT_SET),
    rcvAPCIndex(Init::undefined<int>())
{
}

ChannelParameters* ChannelParameters::clone() const
{
    return new ChannelParameters(*this);
}

AreaDirectionParameters::AreaDirectionParameters() :
    unitVector(Init::undefined<Vector3>()),
    spacing(Init::undefined<double>()),
    elements(Init::undefined<size_t>()),
    first(Init::undefined<size_t>())
{
}

AreaDirectionParameters* AreaDirectionParameters::clone() const
{
    return new AreaDirectionParameters(*this);
}

std::ostream& operator<< (std::ostream& os, const AreaDirectionParameters& d)
{
    os << "AreaDirectionParameters::\n"
       << "  unitVector: " << toString(d.unitVector) << "\n"
       << "  spacing   : " << d.spacing << "\n"
       << "  elements  : " << d.elements << "\n"
       << "  first     : " << d.first << "\n";
     return os;
}

Segment::Segment() :
    startLine(Init::undefined<int>()),
    startSample(Init::undefined<int>()),
    endLine(Init::undefined<int>()),
    endSample(Init::undefined<int>()),
    identifier(Init::undefined<std::string>())
{
}

bool Segment::operator==(const Segment& rhs) const
{
    return (startLine == rhs.startLine &&
        startSample == rhs.startSample &&
        endLine == rhs.endLine &&
        endSample == rhs.endSample &&
        identifier == rhs.identifier);
}

Segment* Segment::clone() const
{

    return new Segment(*this);
}

AreaPlane::AreaPlane() :
    referencePoint(Init::undefined<ReferencePoint>()),
    xDirection(new AreaDirectionParameters()),
    yDirection(new AreaDirectionParameters()),
    orientation(OrientationType::NOT_SET)
{
}

AreaPlane* AreaPlane::clone() const
{
    return new AreaPlane(*this);
}

types::RowCol<double> AreaPlane::getAdjustedReferencePoint() const
{
    types::RowCol<double> refPt = referencePoint.rowCol;

    // NOTE: The calculation done by SICD producers appears to be
    //       orpRow = (numRows + 1) / 2.0
    //       This gives you a pixel-centered, 1-based ORP.  We want
    //       pixel-centered 0-based.  More generally than this, we need to
    //       account for the SICD FirstLine/FirstSample offset
    //
    refPt.row -= xDirection->first;
    refPt.col -= yDirection->first;

    return refPt;
}

Area::Area()
{
    const LatLonAlt initial = Init::undefined<LatLonAlt>();
    for (size_t ii = 0; ii < LatLonAltCorners::NUM_CORNERS; ++ii)
    {
        acpCorners.getCorner(ii) = initial;
    }
}

Area* Area::clone() const
{
    return new Area(*this);
}

RadarCollection* RadarCollection::clone() const
{
    return new RadarCollection(*this);
}

bool RadarCollection::operator==(const RadarCollection& rhs) const
{
    return (refFrequencyIndex == rhs.refFrequencyIndex &&
        txFrequencyMin == rhs.txFrequencyMin &&
        txFrequencyMax == rhs.txFrequencyMax &&
        txPolarization == rhs.txPolarization &&
        polarizationHVAnglePoly == rhs.polarizationHVAnglePoly &&
        txSequence == rhs.txSequence &&
        waveform == rhs.waveform &&
        rcvChannels == rhs.rcvChannels &&
        area == rhs.area &&
        parameters == rhs.parameters);
}

void RadarCollection::fillDerivedFields()
{
    // Transmit bandwidth
    if (!waveform.empty())
    {
        if (Init::isUndefined<double>(txFrequencyMin))
        {
            double derivedMin = std::numeric_limits<double>::infinity();
            for (size_t ii = 0; ii < waveform.size(); ++ii)
            {
                if (waveform[ii].get() != NULL)
                {
                    derivedMin = std::min(derivedMin,
                            waveform[ii]->txFrequencyStart);
                }
            }
            txFrequencyMin = derivedMin;
        }
        if (Init::isUndefined<double>(txFrequencyMax))
        {
            double derivedMax = -std::numeric_limits<double>::infinity();
            for (size_t ii = 0; ii < waveform.size(); ++ii)
            {
                if (waveform[ii].get() != NULL)
                {
                    derivedMax = std::max(derivedMax,
                        waveform[ii]->txFrequencyStart +
                        waveform[ii]->txRFBandwidth);
                }
            }
            txFrequencyMax = derivedMax;
        }

        for (size_t ii = 0; ii < waveform.size(); ++ii)
        {
            if (waveform[ii].get() != NULL)
            {
                waveform[ii]->fillDerivedFields();
            }
        }
    }

    if (waveform.size() == 1 &&
        waveform[0].get() != NULL)
    {
        if (Init::isUndefined<double>(waveform[0]->txFrequencyStart))
        {
            waveform[0]->txFrequencyStart = txFrequencyMin;
        }
        if (Init::isUndefined<double>(waveform[0]->txRFBandwidth))
        {
            waveform[0]->txRFBandwidth = txFrequencyMax - txFrequencyMin;
        }
    }
}
}
}

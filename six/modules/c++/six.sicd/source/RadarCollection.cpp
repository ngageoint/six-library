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

namespace
{
template <typename T>
types::RowCol<T> rotatePixel(size_t origNumCols, const types::RowCol<T>& pixel)
{
    types::RowCol<T> rotatedPixel;
    rotatedPixel.col = pixel.row;
    rotatedPixel.row = static_cast<T>(origNumCols) - pixel.col;
    return rotatedPixel;
}
}

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

const double WaveformParameters::WF_TOL = 1e-3;
const double WaveformParameters::WGT_TOL = 1e-3;
const char WaveformParameters::WF_INCONSISTENT_STR[] =
        "Waveform fields not consistent";

const double RadarCollection::WF_TOL = 1e-3;
const char RadarCollection::WF_INCONSISTENT_STR[] =
        "Waveform fields not consistent";

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
        Init::isUndefined(rcvFMRate))
    {
        rcvFMRate = 0;
    }

    if (rcvFMRate == 0 &&
        rcvDemodType == DemodType::NOT_SET)
    {
        rcvDemodType = DemodType::CHIRP;
    }

    if (Init::isUndefined(txRFBandwidth) &&
        !Init::isUndefined(txPulseLength) &&
        !Init::isUndefined(txFMRate))
    {
        txRFBandwidth = txPulseLength * txFMRate;
    }

    if (!Init::isUndefined(txRFBandwidth) &&
        Init::isUndefined(txPulseLength) &&
        !Init::isUndefined(txFMRate))
    {
        txPulseLength = txRFBandwidth / txFMRate;
    }

    if (!Init::isUndefined(txRFBandwidth) &&
        !Init::isUndefined(txPulseLength) &&
        Init::isUndefined(txFMRate))
    {
        txFMRate = txRFBandwidth / txPulseLength;
    }
}

bool WaveformParameters::validate(int refFrequencyIndex,
        logging::Logger& log) const
{
    bool valid = true;
    std::ostringstream messageBuilder;

    //2.8.3
    if (std::abs(txRFBandwidth / (txPulseLength * txFMRate) - 1) > WF_TOL)
    {
        messageBuilder.str("");
        messageBuilder << WF_INCONSISTENT_STR << std::endl
            << "SICD.RadarCollection.Waveform.WFParameters.TxFRBandwidth: "
            << txRFBandwidth << std::endl
            << "SICD.RadarCollection.TxFrequency.txFMRate * txPulseLength: "
            << txFMRate * txPulseLength << std::endl;
        log.error(messageBuilder.str());
        valid = false;
    }

    //2.8.4
    if (rcvDemodType == DemodType::CHIRP &&
        rcvFMRate != 0)
    {
        messageBuilder.str("");
        messageBuilder << WF_INCONSISTENT_STR << std::endl
            << "SICD.RadarCollection.Waveform.WFParameters.RcvDemodType: "
            << rcvDemodType << std::endl
            << "SICD.RadarCollection.Waveform.WFParameters.RcvFMRate: "
            << rcvFMRate << std::endl;
        log.error(messageBuilder.str());
        valid = false;
    }

    //2.8.5
    if (rcvDemodType == DemodType::STRETCH &&
        std::abs(rcvFMRate / txFMRate - 1) > WGT_TOL)
    {
        messageBuilder.str("");
        messageBuilder << WF_INCONSISTENT_STR << std::endl
            << "SICD.RadarCollection.Waveform.WFParameters.RcvDemodType: "
            << rcvDemodType << std::endl
            << "SICD.RadarCollection.Waveform.WFParameters.RcvFMRate: "
            << rcvFMRate << std::endl
            << "SICD.RadarCollection.Waveform.WFParameters.TxFMRate: "
            << txFMRate << std::endl;
        log.error(messageBuilder.str());
        valid = false;
    }

    //2.8.7
    //Absolute frequencies must be positive
    if (six::Init::isUndefined(refFrequencyIndex) &&
        txFrequencyStart <= 0)
    {
        messageBuilder.str("");
        messageBuilder << WF_INCONSISTENT_STR << std::endl
            << "SICD.RadarCollection.txFreqStart: "
            << txFrequencyStart << std::endl;
        log.error(messageBuilder.str());
        valid = false;
    }

    //2.8.8
    //Absolute frequencies must be positive
    if (six::Init::isUndefined(refFrequencyIndex) &&
            rcvFrequencyStart <= 0)
    {
        messageBuilder.str("");
        messageBuilder << WF_INCONSISTENT_STR << std::endl
            << "SICD.RadarCollection.rcvFreqStart: "
            << rcvFrequencyStart << std::endl;
        log.error(messageBuilder.str());
        valid = false;
    }

    //2.8.9
    if (six::Init::isDefined(txPulseLength) && (txPulseLength > rcvWindowLength)) // TxPulseLength is optional
    {
        messageBuilder.str("");
        messageBuilder << WF_INCONSISTENT_STR << std::endl
            << "SICD.RadarCollection.Waveform.WFParameters.TxPulseLength: "
            << txPulseLength << std::endl
            << "SICD.RadarCollection.Waveform.WFPArameters.RcvWindowLength: "
            << rcvWindowLength << std::endl;
        log.error(messageBuilder.str());
        valid = false;
    }

    //2.8.10
    if (six::Init::isDefined(rcvIFBandwidth) && (rcvIFBandwidth > adcSampleRate)) // RcvIFBandwidth is optional
    {
        messageBuilder.str("");
        messageBuilder << WF_INCONSISTENT_STR << std::endl
            << "SICD.RadarCollection.Waveform.WFParameters.RcvIFBandwidth: "
            << rcvIFBandwidth << std::endl
            << "SICD.RadarCollection.Waveform.WFPArameters.ADCSampleRate: "
            << adcSampleRate << std::endl;
        log.error(messageBuilder.str());
        valid = false;
    }

    //2.8.11
    if (rcvDemodType == DemodType::CHIRP && txRFBandwidth > adcSampleRate)
    {
        messageBuilder.str("");
        messageBuilder << WF_INCONSISTENT_STR << std::endl
            << "SICD.RadarCollection.Waveform.WFParameters.RcvDemodType: "
            << rcvDemodType << std::endl
            << "SICD.RadarCollection.Waveform.WFParameters.TxRFBandwidth: "
            << txRFBandwidth << std::endl
            << "SICD.RadarCollection.Waveform.WFPArameters.ADCSampleRate: "
            << adcSampleRate << std::endl;
        log.error(messageBuilder.str());
        valid = false;
    }

    //2.8.12
    if (six::Init::isDefined(rcvFrequencyStart)) // RcvFreqStart is optional
    {
        const auto freq_tol = (rcvWindowLength - txPulseLength) * txFMRate;
        if (rcvFrequencyStart >= (txFrequencyStart + txRFBandwidth + freq_tol) ||
            rcvFrequencyStart <= txFrequencyStart - freq_tol)
        {
            messageBuilder.str("");
                messageBuilder << WF_INCONSISTENT_STR << std::endl
                << "SICD.RadarCollection.Waveform.WFParameters.RcvFreqStart: "
                << rcvFrequencyStart << std::endl;
                log.error(messageBuilder.str());
                valid = false;
        }
    }

    return valid;
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
    refPt.row -= static_cast<double>(xDirection->first);
    refPt.col -= static_cast<double>(yDirection->first);

    return refPt;
}

const Segment& AreaPlane::getSegment(const std::string& segmentId) const
{
    for (size_t ii = 0; ii < segmentList.size(); ++ii)
    {
        if (segmentList[ii].get() && segmentList[ii]->identifier == segmentId)
        {
            return *segmentList[ii];
        }
    }
    throw except::Exception(Ctxt("No segment with identifier " + segmentId));
}

void AreaPlane::rotateToShadowsDown()
{
    switch(orientation)
    {
    case OrientationType::RIGHT:
        rotateCCW();
        rotateCCW();
        rotateCCW();
        break;
    case OrientationType::UP:
        rotateCCW();
        rotateCCW();
        break;
    case OrientationType::LEFT:
        rotateCCW();
        break;
    default:
        break;
    }
}

void AreaPlane::rotateCCW()
{
    switch (orientation)
    {
    case OrientationType::LEFT:
        orientation = OrientationType::DOWN;
        break;
    case OrientationType::DOWN:
        orientation = OrientationType::RIGHT;
        break;
    case OrientationType::RIGHT:
        orientation = OrientationType::UP;
        break;
    case OrientationType::UP:
        orientation = OrientationType::LEFT;
        break;
    default:
        break;
    }
    referencePoint.rowCol = rotatePixel(yDirection->elements, referencePoint.rowCol);

    std::swap(yDirection->elements, xDirection->elements);
    std::swap(yDirection->spacing, xDirection->spacing);
    std::swap(yDirection->unitVector, xDirection->unitVector);
    xDirection->unitVector *= -1;

    for (size_t ii = 0; ii < segmentList.size(); ++ii)
    {
        segmentList[ii]->rotateCCW(yDirection->elements);
    }
}

void Segment::rotateCCW(size_t /*numColumns*/)
{
    /*
     *   5   wth           --    ! is reference corner
     * !-----             |  |   2 right, 1 down becomes
     * | .  | 2 ht. -->   |  |   1 right, 2 up
     * |    |             |  |
     * ------             |. |
     *                    |  |
     *                    !--
     */
   
    const six::RowColDouble start(types::RowCol<int>(startSample * -1, startLine));
    const six::RowColDouble end(types::RowCol<int>(endSample * -1, endLine));
    startLine = static_cast<int>(start.row);
    startSample = static_cast<int>(start.col);
    endLine = static_cast<int>(end.row);
    endSample = static_cast<int>(end.col);
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

double RadarCollection::waveformMax() const
{
    double derivedMax = -std::numeric_limits<double>::infinity();
    for (size_t ii = 0; ii < waveform.size(); ++ii)
    {
        if (waveform[ii].get() != nullptr)
        {
            derivedMax = std::max(derivedMax,
                waveform[ii]->txFrequencyStart +
                waveform[ii]->txRFBandwidth);
        }
    }
    return derivedMax;
}

double RadarCollection::waveformMin() const
{
    double derivedMin = std::numeric_limits<double>::infinity();
    for (size_t ii = 0; ii < waveform.size(); ++ii)
    {
        if (waveform[ii].get() != nullptr)
        {
            derivedMin = std::min(derivedMin,
                waveform[ii]->txFrequencyStart);
        }
    }
    return derivedMin;
}

void RadarCollection::fillDerivedFields()
{
    // Transmit bandwidth
    if (!waveform.empty())
    {
        if (Init::isUndefined(txFrequencyMin))
        {
            txFrequencyMin = waveformMin();
        }
        if (Init::isUndefined(txFrequencyMax))
        {
            txFrequencyMax = waveformMax();
        }

        for (size_t ii = 0; ii < waveform.size(); ++ii)
        {
            if (waveform[ii].get() != nullptr)
            {
                waveform[ii]->fillDerivedFields();
            }
        }
    }

    if (waveform.size() == 1 &&
        waveform[0].get() != nullptr)
    {
        if (Init::isUndefined(waveform[0]->txFrequencyStart))
        {
            waveform[0]->txFrequencyStart = txFrequencyMin;
        }
        if (Init::isUndefined(waveform[0]->txRFBandwidth))
        {
            waveform[0]->txRFBandwidth = txFrequencyMax - txFrequencyMin;
        }
    }
}

bool RadarCollection::validate(logging::Logger& log) const
{
    bool valid = true;
    std::ostringstream messageBuilder;

    // 2.8 Waveform description consistency
    const auto wfMin = waveformMin();
    const auto wfMax = waveformMax();

    // 2.8.1
    if (wfMin != std::numeric_limits<double>::infinity() &&
        std::abs((wfMin / txFrequencyMin) - 1) > WF_TOL)
    {
        messageBuilder.str("");
        messageBuilder << WF_INCONSISTENT_STR << std::endl
            << "SICD.RadarCollection.Waveform.WFParameters.TxFreqStart: "
            << wfMin << std::endl
            << "SICD.RadarCollection.TxFrequency.Min: " << txFrequencyMin
            << std::endl;
        log.error(messageBuilder.str());
        valid = false;
    }

    //2.8.2
    if (wfMax != -std::numeric_limits<double>::infinity() &&
        std::abs((wfMax / txFrequencyMax) - 1) > WF_TOL)
    {
        messageBuilder.str("");
        messageBuilder << WF_INCONSISTENT_STR << std::endl
            << "SICD.RadarCollection.Waveform.WFParameters.TxFreqStart"
            << " + TxFRBandwidth: " << wfMax << std::endl
            << "SICD.RadarCollection.TxFrequency.Max: "
            << txFrequencyMax << std::endl;
        log.error(messageBuilder.str());
        valid = false;
    }

    //2.8.6
    //Absolute frequencies must be positive
    if (six::Init::isUndefined(refFrequencyIndex) && txFrequencyMin <= 0)
    {
        messageBuilder.str("");
        messageBuilder << WF_INCONSISTENT_STR << std::endl
            << "SICD.RadarCollection.txFrequencyMin: "
            << txFrequencyMin << std::endl;
        log.error(messageBuilder.str());
        valid = false;
    }

    for (size_t ii = 0; ii < waveform.size(); ++ii)
    {
        if (waveform[ii].get())
        {
            valid = waveform[ii]->validate(refFrequencyIndex, log) && valid;
        }
    }
    return valid;
}
}
}

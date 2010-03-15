/* =========================================================================
 * This file is part of six-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
 *
 * six-c++ is free software; you can redistribute it and/or modify
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
#include "six/RadarCollection.h"

using namespace six;

TxStep::TxStep()
{
    waveformIndex = Init::undefined<int>();
    txPolarization = POL_NOT_SET;
}
TxStep* TxStep::clone() const
{
    return new TxStep(*this);
}
ResolutionParameters* ResolutionParameters::clone() const
{
    return new ResolutionParameters(*this);
}
WaveformParameters::WaveformParameters()
{
    txPulseLength = Init::undefined<double>();
    txRFBandwidth = Init::undefined<double>();
    txFrequencyStart = Init::undefined<double>();
    txFMRate = Init::undefined<double>();
    rcvDemodType = DEMOD_NOT_SET;
    rcvWindowLength = Init::undefined<double>();
    adcSampleRate = Init::undefined<double>();
    rcvIFBandwidth = Init::undefined<double>();
    rcvFrequencyStart = Init::undefined<double>();
    rcvFMRate = Init::undefined<double>();
}
WaveformParameters* WaveformParameters::clone() const
{
    return new WaveformParameters(*this);
}
ChannelParameters::ChannelParameters()
{
    rcvAPCIndex = Init::undefined<int>();
    txRcvPolarization = DUAL_POL_NOT_SET;
}
ChannelParameters* ChannelParameters::clone() const
{
    return new ChannelParameters(*this);
}

AreaDirectionParameters::AreaDirectionParameters()
{
    unitVector = Init::undefined<Vector3>();
    spacing = Init::undefined<double>();
    elements = Init::undefined<unsigned long>();
    first = Init::undefined<unsigned long>();
}

AreaDirectionParameters* AreaDirectionParameters::clone() const
{
    return new AreaDirectionParameters(*this);
}
Segment::Segment()
{
    startLine = Init::undefined<int>();
    startSample = Init::undefined<int>();
    endLine = Init::undefined<int>();
    endSample = Init::undefined<int>();
    identifier = Init::undefined<std::string>();
}
Segment* Segment::clone() const
{

    return new Segment(*this);
}

AreaPlane::AreaPlane()
{
    xDirection = new AreaDirectionParameters();
    yDirection = new AreaDirectionParameters();
    referencePoint = Init::undefined<ReferencePoint>();
}

AreaPlane::~AreaPlane()
{
    for (unsigned int i = 0; i < segmentList.size(); ++i)
    {
        Segment* s = segmentList[i];
        delete s;
    }
    if (xDirection)
        delete xDirection;
    if (yDirection)
        delete yDirection;
}

Area::Area()
{
    LatLon initial = Init::undefined<LatLon>();
    apcCorners = std::vector<LatLon>(4, initial);
    plane = NULL;
}

Area::~Area()
{
    if (plane)
        delete plane;
}

AreaPlane* AreaPlane::clone() const
{
    AreaPlane* a = new AreaPlane();
    for (unsigned int i = 0; i < segmentList.size(); ++i)
    {
        Segment* s = segmentList[i];
        a->segmentList.push_back(s->clone());
    }
    a->xDirection = xDirection->clone();
    a->yDirection = yDirection->clone();
    return a;
}

Area* Area::clone() const
{
    Area *a = new Area(*this);
    a->plane = plane ? plane->clone() : NULL;
    return a;
}

RadarCollection::~RadarCollection()
{
    if (resolution)
        delete resolution;

    if (area)
        delete area;

    for (unsigned int i = 0; i < waveform.size(); ++i)
    {
        WaveformParameters* wfp = waveform[i];
        delete wfp;
    }
    for (unsigned int i = 0; i < rcvChannels.size(); ++i)
    {
        ChannelParameters* rcv = rcvChannels[i];
        delete rcv;
    }
}

RadarCollection* RadarCollection::clone() const
{
    RadarCollection* r = new RadarCollection(*this);
    if (resolution)
        r->resolution = resolution->clone();
    if (area)
        r->area = area->clone();
    for (unsigned int i = 0; i < waveform.size(); ++i)
    {
        WaveformParameters* wfp = waveform[i];
        r->waveform.push_back(wfp->clone());
    }
    for (unsigned int i = 0; i < rcvChannels.size(); ++i)
    {
        ChannelParameters* rcv = rcvChannels[i];
        r->rcvChannels.push_back(rcv->clone());
    }
    return r;
}

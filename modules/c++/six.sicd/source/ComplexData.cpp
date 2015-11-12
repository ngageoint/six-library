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
#include "six/sicd/ComplexData.h"

namespace six
{
namespace sicd
{
const char ComplexData::VENDOR_ID[] = "MDA-IS";

Data* ComplexData::clone() const
{
    return new ComplexData(*this);
}

ComplexData::ComplexData() :
    collectionInformation(new CollectionInformation()),
    imageData(new ImageData()),
    geoData(new GeoData()),
    grid(new Grid()),
    timeline(new Timeline()),
    position(new Position()),
    radarCollection(new RadarCollection()),
    imageFormation(new ImageFormation()),
    scpcoa(new SCPCOA()),
    mVersion("1.1.0")
{
    //only initialize the mandatory elements above
}

DateTime ComplexData::getCollectionStartDateTime() const
{
    return timeline->collectStart;
}

void ComplexData::getOutputPlaneOffsetAndExtent(
        types::RowCol<size_t>& offset,
        types::RowCol<size_t>& extent) const
{
    offset.row = offset.col = 0;

    const six::sicd::AreaPlane& areaPlane =
        *radarCollection->area->plane;
    extent.row = areaPlane.xDirection->elements;
    extent.col = areaPlane.yDirection->elements;

    if (imageFormation.get())
    {
        const std::string& segmentID =
                imageFormation->segmentIdentifier;

        const std::vector<mem::ScopedCloneablePtr<six::sicd::Segment> >&
                segmentList = radarCollection->area->plane->segmentList;

        for (size_t ii = 0; ii < segmentList.size(); ++ii)
        {
            const six::sicd::Segment& segment(*(segmentList[ii]));
            if (segment.identifier == segmentID)
            {
                // TODO: These should be offset by areaPlane.xDirection->first
                //       and areaPlane.yDirection->first, but the problem is
                //       that SICD producers are currently doing this
                //       incorrectly - they're setting the
                //       FirstLine/FirstSample values to 1 but then setting
                //       startLine and startSample to 0.
                offset.row = segment.startLine;
                offset.col = segment.startSample;
                extent.row = segment.getNumLines();
                extent.col = segment.getNumSamples();
                break;
            }
        }
    }
}
}
}

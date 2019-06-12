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
#ifndef __SIX_GEO_DATA_H__
#define __SIX_GEO_DATA_H__

#include "six/Types.h"
#include "six/Init.h"
#include "six/GeoInfo.h"
#include "six/Parameter.h"
#include "six/ParameterCollection.h"
#include "scene/ProjectionModel.h"
#include <mem/ScopedCloneablePtr.h>
#include <logging/Logger.h>

namespace six
{
namespace sicd
{
struct ImageData;

/*!
 *  \struct GeoData
 *  \brief SICD GeoData block
 *
 *  This block describes the geographic coordinates of the region
 *  covered by the image
 *
 *  Compiler-generated copy constructor and assignment operator are sufficient
 */
class GeoData
{
public:
    //!  Constructor, force WGS84, since spec does.
    GeoData() :
        earthModel(EarthModelType::WGS84)
    {
    }

    /*!
     *  Identifies the earth model used for
     *  latitude, longitude and height parameters.  All
     *  height values are Height Above Ellipsoid (HAE)
     */
    EarthModelType earthModel;

    /*!
     *  Scene Center Point in full image.  This is the
     *  precise location
     */
    SCP scp;

    /*!
     *  Parameters apply to image corners of the
     *  product projected to the same height as the SCP.
     *  These corners are an approximate geographic location
     *  not intended for analytical use
     */
    LatLonCorners imageCorners;

    /*!
     *  (Optional) indicates the full image includes both
     *  valid data and some zero-filled pixels.  Vector size
     *  is the number of vertices.
     */
    std::vector<LatLon> validData;

    /*!
     *  (Optional) Parameters that describe geographic features.
     *  Note that this may be used as a block inside of a block.
     */
    std::vector<mem::ScopedCopyablePtr<GeoInfo> > geoInfos;

    bool operator==(const GeoData& rhs) const;
    bool operator!=(const GeoData& rhs) const
    {
        return !(*this == rhs);
    }

    //Doesn't currently do anything
    void fillDerivedFields(const ImageData& imageData,
            const scene::ProjectionModel& model);
    bool validate(logging::Logger& log) const;

private:
    static const double ECF_THRESHOLD;
};
}
}

#endif

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
#ifndef __SIX_GEO_DATA_H__
#define __SIX_GEO_DATA_H__

#include "six/Types.h"
#include "six/Init.h"
#include "six/Parameter.h"

namespace six
{
namespace sicd
{

/*!
 *  \struct GeoInfo
 *  \brief (Optional) SICD GeoInfo parameters
 *
 *  Parameters describing geographic features.
 *  Note: the GeoInfo block may be used as a block within
 *  itself
 */
class GeoInfo
{
public:
    //!  Constructor
    GeoInfo()
    {
    }
    //!  Destructor, deletes all sub-nodes
    ~GeoInfo();

    //!  Clone, including all sub-nodes
    GeoInfo* clone() const;

    //!  (Optional) name of a geographic feature
    std::string name;

    //! (Optional) sub-nodes
    std::vector<GeoInfo*> geoInfos;

    //! (Optional) description of geographic feature
    std::vector<Parameter> desc;

    /*!
     *  This could be a point if one, a line if 2, or a polygon if > 2
     *  In other words, this simultaneously represents SICD's 
     *  GeoInfo/Point, GeoInfo/Line, and GeoInfo/Polygon
     *
     */
    std::vector<LatLon> geometryLatLon;
    std::vector<int> geometryLatLonIdx;

private:
    // Noncopyable
    GeoInfo(const GeoInfo& );
    const GeoInfo& operator=(const GeoInfo& );
};

/*!
 *  \struct GeoData
 *  \brief SICD GeoData block
 *
 *  This block describes the geographic coordinates of the region
 *  covered by the image
 */
class GeoData
{
public:
    //!  Constructor, force WGS84, since spec does.
    GeoData() :
        earthModel(EarthModelType::WGS84)
    {
    }

    //!  Destructor, deletes all GeoInfo objects
    ~GeoData();

    //!  Clone, including non-NULL GeoInfo objects
    GeoData* clone();

    /*!
     *  Identifies the earth model used for 
     *  latitude, longitude and height parameters.  All
     *  height values are Height Above Ellipsoid (HAE)
     */
    EarthModelType earthModel;

    /*!
     *  Scene Center Point in fulll image.  This is the
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
    std::vector<int> validDataIdx;

    /*!
     *  (Optional) Parameters that describe geographic features.
     *  Note that this may be used as a block inside of a block.
     */
    std::vector<GeoInfo*> geoInfos;

private:
    // Noncopyable
    GeoData(const GeoData& );
    const GeoData& operator=(const GeoData& );
};

}
}
#endif

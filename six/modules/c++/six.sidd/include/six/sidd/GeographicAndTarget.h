/* =========================================================================
 * This file is part of six.sidd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * six.sidd-c++ is free software; you can redistribute it and/or modify
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
#ifndef __SIX_GEOGRAPHIC_AND_TARGET_H__
#define __SIX_GEOGRAPHIC_AND_TARGET_H__

#include <memory>

#include <mem/ScopedCopyablePtr.h>
#include <six/Types.h>
#include <six/Init.h>
#include <six/ParameterCollection.h>
#include <six/GeoInfo.h>

namespace six
{
namespace sidd
{
/*!
 *  \struct TargetInformation
 *  \brief SIDD TargetInformation grouping
 *
 *  Provides target specific geographic information.
 */
struct TargetInformation
{
    //! SIDD Identifier set (min occurs one, max unbounded)
    ParameterCollection identifiers;

    //! (Optional) SIDD Footprint: target footprint as defined by polygonal shape
    mem::ScopedCopyablePtr<LatLonCorners> footprint;

    /*! 
     * (Optional) SIDD TargetInformationExtension
     * Generic extension.  Could be used to indicate type of target,
     * terrain, etc.
     */
    ParameterCollection targetInformationExtensions;
};

/*!
 *  \struct GeographicInformation
 *  \brief SIDD GeographicInfo grouping
 *
 *  Library name differs from spec. to keep API consistent
 *  The GeographicInformation object gives specifics about the geo-region
 *
 */
struct GeographicInformation
{
    //!  SIDD CountryCode Country code (1+)
    std::vector<std::string> countryCodes;

    //!  SIDD SecurityInfo (name differs for API consistency)
    std::string securityInformation;

    /*!
     *  SIDD GeographicInfoExtension (name differs for API consistency)
     *  Country identifier for this geographic region.
     */
    ParameterCollection geographicInformationExtensions;
};

//! Note that subRegion and geographicInformation are mutually exclusive!
/*!
 *  \struct GeographicCoverage
 *  \brief SIDD SubRegion and GeographicCoverage element group
 *
 *  This object contains the information associated with some geographic
 *  coverage
 */
class GeographicCoverage
{
public:
    //!  Constructor requires a RegionType to properly initialize
    GeographicCoverage(RegionType regionType);

    //!  This identifier determines if we are doing SubRegion or nesting
    RegionType regionType;

    //!  SIDD GeoregionIdentifier: an ID for the georegion
    ParameterCollection georegionIdentifiers;

    /*!
     *  The estimated ground footprint of the product (note this is required
     *  and is used to set the image corners
     */
    LatLonCorners footprint;

    //!  SIDD SubRegion info, mutually exclusive with geographicInformation
    std::vector<mem::ScopedCopyablePtr<GeographicCoverage> > subRegion;

    //!  SIDD: GeographicInfo, mutually exclusive with SubRegion
    mem::ScopedCopyablePtr<GeographicInformation> geographicInformation;
};

/*!
 *  \struct GeographicAndTarget
 *  \brief SIDD GoegraphicAndTarget block
 *
 *  Contains generic and extensible targeting and geographic region
 *  information
 *
 */
class GeographicAndTarget
{
public:
    //!  Constructor requires a RegionType to properly initialize
    GeographicAndTarget(RegionType regionType);

    //!  SIDD GeographicCoverage: Provides geo coverage information
    GeographicCoverage geographicCoverage;

    //!  (Optional, Unbounded) Provides target specific geo information
    std::vector<mem::ScopedCopyablePtr<TargetInformation> > targetInformation;

    // TODO: Finalize what this looks like for SIDD 1.1
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
};
}
}

#endif


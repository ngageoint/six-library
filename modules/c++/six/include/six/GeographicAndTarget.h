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
#ifndef __SIX_GEOGRAPHIC_AND_TARGET_H__
#define __SIX_GEOGRAPHIC_AND_TARGET_H__

#include "six/Types.h"
#include "six/Init.h"
#include "six/Parameter.h"

namespace six
{
/*!
 *  \struct TargetInformation
 *  \brief SIDD TargetInformation grouping
 *
 *  Provides target specific geographic information.
 */
struct TargetInformation
{
    //! SIDD Identifier set (min occurs zero, max unbounded)
    std::vector<Parameter> identifiers;

    //! SIDD Footprint: target footprint as defined by polygonal shape
    std::vector<Corners> footprints;

    /*! 
     * (Optional) SIDD TargetInformationExtension
     * Generic extension.  Could be used to indicate type of target,
     * terrain, etc.
     */
    std::vector<Parameter> targetInformationExtensions;

    //!  Clone any sub-objects if vectors are not empty
    TargetInformation* clone() const;
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
    std::vector<Parameter> geographicInformationExtensions;

    //!  Clone this element and its children
    GeographicInformation* clone() const;
};

//! Note that subRegion and geographicInformation are mutually exclusive!
/*!
 *  \struct GeographicCoverage
 *  \brief SIDD SubRegion and GeographicCoverage element group
 *
 *  This object contains the information associated with some geographic
 *  coverage
 */
struct GeographicCoverage
{
    //!  This identifier determines if we are doing SubRegion or nesting
    RegionType regionType;

    //!  SIDD GeoregionIdentifier: an ID for the georegion
    std::vector<Parameter> georegionIdentifiers;

    /*!
     *  The estimated ground footprint of the product (note this is required
     *  and is used to set the image corners
     */
    Corners footprint;

    //!  SIDD SubRegion info, mutually exclusive with geographicInformation
    std::vector<GeographicCoverage*> subRegion;

    //!  SIDD: GeographicInfo, mutually exclusive with SubRegion
    GeographicInformation* geographicInformation;

    //!  Constructor requires a RegionType to properly initialize
    GeographicCoverage(RegionType regionType);

    //!  Destructor.  Deletes the information if non-NULL, and any sub-regions
    ~GeographicCoverage();
    
    //!  Carefully clones the sub-regions and geo information section
    GeographicCoverage* clone() const;
};

/*!
 *  \struct GeographicAndTarget
 *  \brief SIDD GoegraphicAndTarget block
 *
 *  Contains generic and extensible targeting and geographic region
 *  information
 *
 */
struct GeographicAndTarget
{
    //!  SIDD GeographicCoverage: Provides geo coverage information
    GeographicCoverage* geographicCoverage;

    //!  (Optional, Unbounded) Provides target specific geo information
    std::vector<TargetInformation*> targetInformation;

    //!  Constructor, auto-initializes coverage object
    GeographicAndTarget(RegionType regionType) :
        geographicCoverage(NULL)
    {
        geographicCoverage = new GeographicCoverage(regionType);
    }

    //!  Delete geo coverage object and any targets
    ~GeographicAndTarget();

    //!  Clones geo coverage and any targets
    GeographicAndTarget* clone();
};

}

#endif

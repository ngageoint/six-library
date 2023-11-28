/* =========================================================================
 * This file is part of six.sicd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2019, MDA Information Systems LLC
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
#pragma once
#ifndef SIX_six_CollectionInformation_h_INCLUDED_
#define SIX_six_CollectionInformation_h_INCLUDED_

#include <std/string>
#include <std/optional>

#include <str/Encoding.h>

#include "six/Types.h"
#include "six/Init.h"
#include "six/Parameter.h"
#include "six/ParameterCollection.h"
#include "six/Exports.h"

namespace six
{
/*!
 *  \struct CollectionInformation
 *  \brief CollectionInfo parameter
 *
 *  Collection information used by SICD, representing the tag
 *  <CollectionInfo>.  This block contains general information about
 *  the collection.  There is nothing wrong with the SICD name, but
 *  for API consistency, naming is lengthened.
 */
struct SIX_SIX_API CollectionInformation
{

    /*!
     *  Radar platform identifier.  For bistatic
     *  collections, list the Receive platform
     */
    std::string collectorName;

    /*!
     *  (Optional) Radar platform identifier that provided
     *  the illumination.  For bistatic collections, list the
     *  transmit platform.
     */
    std::string illuminatorName;

    /*!
     *  Collection & imaging data set identifier.  Uniquely identifies
     *  imaging collections per profile
     *
     */
    std::string coreName;

    /*!
     *  (Optional) Collection type identifier.  Monotstatic collections include
     *  single platform collections with unique transmit and receive apertures
     *  Allowed values are "MONOSTATIC" and "BISTATIC"
     *
     */
    CollectType collectType;

    /*!
     *  Radar Mode, must be SPOTLIGHT, STRIPMAP, or
     *  DYNAMIC_STRIPMAP
     *
     */
    RadarModeType radarMode;

    /*!
     *  (Optional) RadarMode ModeID.  Value dependent on profile
     */
    std::string radarModeID;

    /*!
     *  (Optional) ReleaseInfo contains product release information.
     *  Required only for CPHD 1.0
     *  Default value is UNRESTRICTED
     */
    std::string releaseInfo;

    /*!
     *  (Optional)
     *  List of country codes for region covered by the image
     *
     */
    std::vector<std::string> countryCodes;

    /*!
     *  (Optional) Additional parameters
     */
    ParameterCollection parameters;

    //!  Constructor
    CollectionInformation();

    //!  Destructor
    virtual ~CollectionInformation() = default;

    //! Deep copy
    CollectionInformation* clone() const;

    virtual std::string getClassificationLevel() const;
    inline virtual void setClassificationLevel(const std::string& classification)
    {
        mClassification = str::u8FromNative(classification);
    }

    virtual void getClassificationLevel(std::u8string&) const;
    virtual void setClassificationLevel(const std::u8string& classification);

    //! Ostream operators for six::CollectionInformation type (utility only).
    SIX_SIX_API friend std::ostream& operator<<(std::ostream& os, const six::CollectionInformation& c);

  bool operator==(const CollectionInformation& rhs) const; // needs to be member-function for SWIG
  bool operator!=(const CollectionInformation& rhs) const  // needs to be member-function for SWIG
  {
    return !(*this == rhs);
  }

private:
    /*!
     *  Classification level
     */
    std::u8string mClassification;
};

}

#endif // SIX_six_CollectionInformation_h_INCLUDED_

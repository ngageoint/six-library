/* =========================================================================
 * This file is part of six-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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
#include "six/Init.h"
#include "six/Types.h"
#include <nitf/ImageSegmentComputer.h>
#include "six/Enums.h"

std::ostream& operator<<(std::ostream& os, const scene::LatLonAlt& latLonAlt)
{
    os << '(' << latLonAlt.getLat() << ',' << latLonAlt.getLon() << ','
            << latLonAlt.getAlt() << ')';
    return os;
}

/*
std::ostream& operator<<(std::ostream& os, const Corners& corners)
{
    os << "{" << corners.corner[0] << ',' << corners.corner[1] << ','
            << corners.corner[2] << ',' << corners.corner[3];
    return os;
}
*/

namespace six
{
const uint64_t Constants::IS_SIZE_MAX =
        nitf::ImageSegmentComputer::NUM_BYTES_MAX;
const uint64_t Constants::GT_SIZE_MAX = 4294967296LL;
const size_t Constants::ILOC_MAX = nitf::ImageSegmentComputer::ILOC_MAX;
const unsigned short Constants::GT_XML_KEY = 50909;
const char Constants::GT_XML_TAG[] = "XMLTag";

// TODO  SIDD spec says to mark the DES version as "01" in the NITF but
//       IC-ISM.xsd says the DESVersion attribute is fixed at 4
const int32_t Constants::DES_VERSION = 4;
const char Constants::DES_VERSION_STR[] = "01";

const char Constants::DES_USER_DEFINED_SUBHEADER_TAG[] = "XML_DATA_CONTENT";
const char Constants::DES_USER_DEFINED_SUBHEADER_ID[] = "XML_DATA_CONTENT_773";
const uint64_t Constants::DES_USER_DEFINED_SUBHEADER_LENGTH = 773;

const char Constants::SICD_DESSHSI[] =
        "SICD Volume 1 Design & Implementation Description Document";
const char Constants::SIDD_DESSHSI[] =
        "SIDD Volume 1 Design & Implementation Description Document";

const double Constants::EXCESS_BANDWIDTH_FACTOR = .885892941;

ImageMode getImageMode(RadarModeType radarMode)
{
    switch (radarMode)
    {
    case RadarModeType::STRIPMAP:
    case RadarModeType::DYNAMIC_STRIPMAP:
        return SCAN_MODE;
    case RadarModeType::NOT_SET:
        throw except::Exception(Ctxt("Radar mode not set"));
    case RadarModeType::SPOTLIGHT:
    default: // TODO: Not sure what to do for SCANSAR
        return FRAME_MODE;
    }
}

template<>
LatLonCorners::Corners() :
    upperLeft(Init::undefined<LatLon>()),
    upperRight(Init::undefined<LatLon>()),
    lowerRight(Init::undefined<LatLon>()),
    lowerLeft(Init::undefined<LatLon>())
{
}

template<>
LatLonAltCorners::Corners() :
    upperLeft(Init::undefined<LatLonAlt>()),
    upperRight(Init::undefined<LatLonAlt>()),
    lowerRight(Init::undefined<LatLonAlt>()),
    lowerLeft(Init::undefined<LatLonAlt>())
{
}

SCP::SCP() :
   ecf(Init::undefined<Vector3>()),
   llh(Init::undefined<LatLonAlt>())
{
}

static bool is_OTHER_(const std::string& v)
{
    // OTHER.* for  SIDD 3.0/SICD 1.3, not "OTHER"
    if ((v != "OTHER") && str::starts_with(v, "OTHER")) // i.e., "OTHER_foo"
    {
        // "where * = 0 or more characters that does not contain “:” (0x3A)."
        return v.find(':') == std::string::npos; // "OTHER:foo" is invalid
    }
    return false; // "OTHER" or "<something else>"
}

PolarizationType PolarizationType::toType_imp_(const std::string& v)
{
    // Handle OTHER.* for  SIDD 3.0/SICD 1.3
    if (is_OTHER_(v)) // handle "OTHER" with default_toType_()
    {
        PolarizationType retval = PolarizationType::OTHER;
        retval.other_ = v; // know "v" is a valid OTHER.* 
        return retval;
    }
    return default_toType_(v); // let default_toType_() throw the exception for "OTHER:foo"
}

std::string PolarizationType::toString_(bool throw_if_not_set) const
{
    // Handle OTHER.* for  SIDD 3.0/SICD 1.3
    if (is_OTHER_(other_))
    {
        return other_;
    }
    if (!other_.empty())
    {
        // other_ got set to something other than an OTHER string
        except::InvalidFormatException(Ctxt("Invalid enum value: " + other_));
    }
    return default_toString_(throw_if_not_set);
}

}


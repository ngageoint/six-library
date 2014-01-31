/* =========================================================================
 * This file is part of six-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2013, General Dynamics - Advanced Information Systems
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

#ifndef __SIX_SI_COMPLEX_COMMON_XML_PARSER_02X_H__
#define __SIX_SI_COMPLEX_COMMON_XML_PARSER_02X_H__

#include <six/SICommonXMLParser01x.h>

namespace six
{
// This is not a "real" version of SI common
// It matches the SICD 0.5 spec
class SICommonXMLParser02x : public SICommonXMLParser01x
{
public:
    SICommonXMLParser02x(const std::string& defaultURI,
                         bool addClassAttributes,
                         const std::string& siCommonURI,
                         logging::Logger* log = NULL,
                         bool ownLog = false);

    virtual XMLElem convertRadiometryToXML(
        const Radiometric *obj, 
        XMLElem parent = NULL) const;

    virtual void parseRadiometryFromXML(
        const XMLElem radiometricXML, 
        Radiometric *obj) const;
};
}

#endif

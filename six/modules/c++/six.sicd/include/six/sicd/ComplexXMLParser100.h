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
#ifndef __SIX_SICD_COMPLEX_XML_PARSER_100_H__
#define __SIX_SICD_COMPLEX_XML_PARSER_100_H__

#include <six/sicd/ComplexXMLParser10x.h>

namespace six
{
namespace sicd
{
class ComplexXMLParser100 : public ComplexXMLParser10x
{
public:
    ComplexXMLParser100(const std::string& version,
                        logging::Logger* log = nullptr,
                        bool ownLog = false);

    ComplexXMLParser100(const ComplexXMLParser100&) = delete;
    ComplexXMLParser100& operator=(const ComplexXMLParser100&) = delete;

protected:

    virtual XMLElem convertGeoInfoToXML(const GeoInfo *obj,
                                        XMLElem parent = nullptr) const;

};
}
}

#endif


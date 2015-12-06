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
#ifndef __SIX_SICD_COMPLEX_XML_PARSER_040_H__
#define __SIX_SICD_COMPLEX_XML_PARSER_040_H__

#include <six/sicd/ComplexXMLParser04x.h>

namespace six
{
namespace sicd
{
class ComplexXMLParser040 : public ComplexXMLParser04x
{
public:
    ComplexXMLParser040(const std::string& version,
                        logging::Logger* log = NULL,
                        bool ownLog = false);

protected:

    virtual XMLElem convertRMATToXML(const RMAT* obj, 
                                     XMLElem parent = NULL) const;
    virtual void parseRMATFromXML(const XMLElem rmatElem, RMAT* obj) const;

    virtual void convertDRateSFPolyToXML(const INCA* inca, XMLElem incaElem) const;
    virtual void parseDRateSFPolyFromXML(const XMLElem incaElem, INCA* inca) const;

};
}
}

#endif


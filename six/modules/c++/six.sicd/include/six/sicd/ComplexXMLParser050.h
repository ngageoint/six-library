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
#ifndef __SIX_SICD_COMPLEX_XML_PARSER_050_H__
#define __SIX_SICD_COMPLEX_XML_PARSER_050_H__

#include <six/sicd/ComplexXMLParser041.h>

namespace six
{
namespace sicd
{
class ComplexXMLParser050 : public ComplexXMLParser041
{
public:
    ComplexXMLParser050(const std::string& version,
                        logging::Logger* log = NULL,
                        bool ownLog = false);

protected:
    virtual XMLElem convertWeightTypeToXML(const WeightType& obj,
                                           XMLElem parent = NULL) const;

    virtual XMLElem convertImageFormationAlgoToXML(
        const PFA* pfa, const RMA* rma,
        const RgAzComp* rgAzComp,
        XMLElem parent = NULL) const;

    virtual void parseWeightTypeFromXML(const XMLElem gridRowColXML,
            mem::ScopedCopyablePtr<WeightType>& obj) const;

    virtual XMLElem convertMatchInformationToXML(const MatchInformation& obj,
                                                 XMLElem parent = NULL) const;
};
}
}

#endif

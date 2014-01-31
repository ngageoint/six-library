/* =========================================================================
 * This file is part of six.sicd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2013, General Dynamics - Advanced Information Systems
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

#include <six/sicd/ComplexXMLParser050.h>
#include <six/SICommonXMLParser02x.h>

namespace
{
typedef xml::lite::Element* XMLElem;
}

namespace six
{
namespace sicd
{
ComplexXMLParser050::ComplexXMLParser050(const std::string& version,
                                         logging::Logger* log,
                                         bool ownLog) :
    ComplexXMLParser041(version, false, std::auto_ptr<six::SICommonXMLParser>(
                           new six::SICommonXMLParser02x(
                               versionToURI(version), false,
                               versionToURI(version), log)),
                        log, ownLog)
{
}

XMLElem ComplexXMLParser050::convertWeightTypeToXML(
    const WeightType& obj,
    XMLElem parent) const
{
    XMLElem weightTypeXML = newElement("WgtType", parent);
    createString("WindowName", obj.windowName, weightTypeXML);

    common().addParameters("Parameter", obj.parameters, weightTypeXML);

    return weightTypeXML;
}

XMLElem ComplexXMLParser050::convertImageFormationAlgoToXML(
    const PFA* pfa, const RMA* rma,
    const RgAzComp* rgAzComp, XMLElem parent) const
{
    if (rgAzComp)
    {
        // TODO: Supporting this would require adding additional fields into
        //       RgAzComp that existed in 0.5 and were removed in 1.0.
        throw except::Exception(Ctxt(
            "RGAZCOMP exists in SICD 0.5 but library does not support it"));
    }
    else if (pfa && !rma)
    {
        return convertPFAToXML(pfa, parent);
    }
    else if (!pfa && rma)
    {
        return convertRMAToXML(rma, parent);
    }
    else
    {
        throw except::Exception(Ctxt(
            "Only one PFA or RMA can be defined in SICD 0.5"));
    }
}

void ComplexXMLParser050::parseWeightTypeFromXML(
    const XMLElem gridRowColXML,
    mem::ScopedCopyablePtr<WeightType>& obj) const
{
    const XMLElem weightType = getOptional(gridRowColXML, "WgtType");
    if (weightType)
    {
        obj.reset(new WeightType());
        parseString(getFirstAndOnly(weightType, "WindowName"),
                    obj->windowName);
        common().parseParameters(weightType, "Parameter", obj->parameters);
    }
    else
    {
        obj.reset();
    }
}
}
}

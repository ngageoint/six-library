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

#include <six/Utilities.h>
#include <six/SICommonXMLParser01x.h>
#include <six/sicd/ComplexXMLParser050.h>

namespace
{
typedef xml::lite::Element* XMLElem;

// This is not a "real" version of SI common
// It matches the SICD 0.5 spec
// Placing it here in an unnamed namespace to avoid any confusion - don't
// expect that this would ever be useful elsewhere
class SICommonXMLParser050 : public six::SICommonXMLParser01x
{
public:
    SICommonXMLParser050(const std::string& uri, logging::Logger* log) :
        six::SICommonXMLParser01x(uri, false, uri, log, false)
    {
    }

    virtual XMLElem convertRadiometryToXML(
        const six::Radiometric *obj,
        XMLElem parent = nullptr) const;

    virtual void parseRadiometryFromXML(
        const XMLElem radiometricXML,
        six::Radiometric *obj) const;
};

XMLElem SICommonXMLParser050::convertRadiometryToXML(
    const six::Radiometric* r, XMLElem parent) const
{
    std::string defaultURI = getSICommonURI();
    XMLElem rXML = newElement("Radiometric", getDefaultURI(), parent);

    if (!r->noiseLevel.noisePoly.empty())
    {
        createPoly2D("NoisePoly", defaultURI, r->noiseLevel.noisePoly, rXML);
    }

    if (!r->noiseLevel.noiseType.empty())
    {
        createString("NoiseLevelType", defaultURI, r->noiseLevel.noiseType,
                     rXML);
    }

    if (!r->rcsSFPoly.empty())
    {
        createPoly2D("RCSSFPoly", defaultURI, r->rcsSFPoly, rXML);
    }

    if (!r->betaZeroSFPoly.empty())
    {
        createPoly2D("BetaZeroSFPoly", defaultURI, r->betaZeroSFPoly, rXML);
    }

    if (!r->sigmaZeroSFPoly.empty())
    {
        createPoly2D("SigmaZeroSFPoly",
                     defaultURI,
                     r->sigmaZeroSFPoly,
                     rXML);
    }

    if (r->sigmaZeroSFIncidenceMap != six::AppliedType::NOT_SET)
    {
        createString(
            "SigmaZeroSFIncidenceMap",
            defaultURI,
            six::toString<six::AppliedType>(r->sigmaZeroSFIncidenceMap),
            rXML);
    }

    if (!r->gammaZeroSFPoly.empty())
    {
        createPoly2D("GammaZeroSFPoly", defaultURI, r->gammaZeroSFPoly, rXML);
    }

    if (r->gammaZeroSFIncidenceMap != six::AppliedType::NOT_SET)
    {
        createString(
            "GammaZeroSFIncidenceMap",
            defaultURI,
            six::toString<six::AppliedType>(r->gammaZeroSFIncidenceMap),
            rXML);
    }
    return rXML;
}

void SICommonXMLParser050::parseRadiometryFromXML(
    const XMLElem radiometricXML,
    six::Radiometric* radiometric) const
{
    XMLElem tmpElem = nullptr;

    tmpElem = getOptional(radiometricXML, "NoisePoly");
    if (tmpElem)
    {
        //optional
        parsePoly2D(tmpElem, radiometric->noiseLevel.noisePoly);
    }

    tmpElem = getOptional(radiometricXML, "NoiseLevelType");
    if (tmpElem)
    {
        //optional
        parseString(tmpElem, radiometric->noiseLevel.noiseType);
    }

    tmpElem = getOptional(radiometricXML, "RCSSFPoly");
    if (tmpElem)
    {
        //optional
        parsePoly2D(tmpElem, radiometric->rcsSFPoly);
    }

    tmpElem = getOptional(radiometricXML, "BetaZeroSFPoly");
    if (tmpElem)
    {
        //optional
        parsePoly2D(tmpElem, radiometric->betaZeroSFPoly);
    }

    tmpElem = getOptional(radiometricXML, "SigmaZeroSFPoly");
    if (tmpElem)
    {
        //optional
        parsePoly2D(tmpElem, radiometric->sigmaZeroSFPoly);
    }

    tmpElem = getOptional(radiometricXML, "SigmaZeroSFIncidenceMap");
    if (tmpElem)
    {
        //optional
        radiometric->sigmaZeroSFIncidenceMap =
            six::toType<six::AppliedType>(tmpElem->getCharacterData());
    }

    tmpElem = getOptional(radiometricXML, "GammaZeroSFPoly");
    if (tmpElem)
    {
        //optional
        parsePoly2D(tmpElem, radiometric->gammaZeroSFPoly);
    }

    tmpElem = getOptional(radiometricXML, "GammaZeroSFIncidenceMap");
    if (tmpElem)
    {
        //optional
        radiometric->gammaZeroSFIncidenceMap =
            six::toType<six::AppliedType>(tmpElem->getCharacterData());
    }
}
}

namespace six
{
namespace sicd
{
ComplexXMLParser050::ComplexXMLParser050(const std::string& version,
                                         logging::Logger* log,
                                         bool ownLog) :
    ComplexXMLParser041(version, false, std::unique_ptr<six::SICommonXMLParser>(
                           new ::SICommonXMLParser050(
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
    else if (!pfa && !rma)
    {
        //! This will occur when set to OTHER. We do not want to include
        //  a specialized image formation algorithm so we return nullptr.
        return nullptr;
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

XMLElem ComplexXMLParser050::convertMatchInformationToXML(
    const MatchInformation& matchInfo,
    XMLElem parent) const
{
    XMLElem matchInfoXML = newElement("MatchInfo", parent);

    for (size_t i = 0; i < matchInfo.types.size(); ++i)
    {
        const MatchType& mt = matchInfo.types[i];
        XMLElem mtXML = newElement("Collect", matchInfoXML);
        setAttribute(mtXML, "index", str::toString(i + 1));

        createString("CollectorName", mt.collectorName, mtXML);
        if (!mt.illuminatorName.empty())
            createString("IlluminatorName", mt.illuminatorName, mtXML);
        createString("CoreName", mt.matchCollects[0].coreName, mtXML);

        for (std::vector<std::string>::const_iterator it =
                mt.matchType.begin(); it != mt.matchType.end(); ++it)
        {
            createString("MatchType", *it, mtXML);
        }
        common().addParameters("Parameter", mt.matchCollects[0].parameters, mtXML);
    }

    // This is the one difference between SICD 0.4 and 0.5
    setAttribute(matchInfoXML, "size", str::toString(matchInfo.types.size()));

    return matchInfoXML;
}
}
}

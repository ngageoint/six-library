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
#include <set>

#include <six/Utilities.h>
#include <six/SICommonXMLParser01x.h>

namespace
{
typedef six::xml_lite::Element* XMLElem;
}

namespace six
{

SICommonXMLParser01x::SICommonXMLParser01x(
    const std::string& defaultURI,
    bool addClassAttributes,
    const std::string& siCommonURI,
    logging::Logger* log,
    bool ownLog) :
    SICommonXMLParser(defaultURI, addClassAttributes, 
                      siCommonURI, log, ownLog)
{
}

XMLElem SICommonXMLParser01x::convertCompositeSCPToXML(
    const ErrorStatistics* errorStatistics,
    XMLElem errorStatsXML) const
{
    std::string defaultURI = getSICommonURI();

    //TODO compositeSCP needs to be reworked
    if (errorStatistics->compositeSCP.get())
    {
        XMLElem scpXML = newElement("CompositeSCP", defaultURI, errorStatsXML);

        if (errorStatistics->compositeSCP->scpType == CompositeSCP::RG_AZ)
        {
            XMLElem rgAzXML = newElement("RgAzErr", defaultURI, scpXML);
            createDouble("Rg", defaultURI, errorStatistics->compositeSCP->xErr, rgAzXML);
            createDouble("Az", defaultURI, errorStatistics->compositeSCP->yErr, rgAzXML);
            createDouble("RgAz", defaultURI, errorStatistics->compositeSCP->xyErr,
                         rgAzXML);
        }
        else
        {
            XMLElem rgAzXML = newElement("RowColErr", defaultURI, scpXML);
            createDouble("Row", defaultURI, errorStatistics->compositeSCP->xErr,
                         rgAzXML);
            createDouble("Col", defaultURI, errorStatistics->compositeSCP->yErr,
                         rgAzXML);
            createDouble("RowCol", defaultURI, errorStatistics->compositeSCP->xyErr,
                         rgAzXML);
        }
        return scpXML;
    }
    return nullptr;
}

void SICommonXMLParser01x::parseCompositeSCPFromXML(
    const xml_lite::Element* errorStatsXML,
    ErrorStatistics* errorStatistics) const
{
    //! optional field
    XMLElem compositeSCPXML = getOptional(errorStatsXML, "CompositeSCP");

    //See if it's RgAzErr or RowColErr
    //optional
    if (compositeSCPXML)
    {
        errorStatistics->compositeSCP.reset(new CompositeSCP());
        XMLElem rgAzErrXML = getOptional(compositeSCPXML, "RgAzErr");
        if (rgAzErrXML)
        {
            errorStatistics->compositeSCP->scpType = CompositeSCP::RG_AZ;
            parseDouble(getFirstAndOnly(rgAzErrXML, "Rg"),
                        errorStatistics->compositeSCP->xErr);
            parseDouble(getFirstAndOnly(rgAzErrXML, "Az"),
                        errorStatistics->compositeSCP->yErr);
            parseDouble(getFirstAndOnly(rgAzErrXML, "RgAz"),
                        errorStatistics->compositeSCP->xyErr);
        }
        else
        {
            XMLElem rowColErrXML = getOptional(compositeSCPXML, "RowColErr");
            if (rowColErrXML)
            {
                errorStatistics->compositeSCP->scpType = CompositeSCP::ROW_COL;
                parseDouble(getFirstAndOnly(rowColErrXML, "Row"),
                            errorStatistics->compositeSCP->xErr);
                parseDouble(getFirstAndOnly(rowColErrXML, "Col"),
                            errorStatistics->compositeSCP->yErr);
                parseDouble(getFirstAndOnly(rowColErrXML, "RowCol"),
                            errorStatistics->compositeSCP->xyErr);
            }
        }
    }
}

XMLElem SICommonXMLParser01x::convertRadiometryToXML(
    const Radiometric *r, XMLElem parent) const
{
    std::string defaultURI = getSICommonURI();
    XMLElem rXML = newElement("Radiometric", getDefaultURI(), parent);

    if (!r->noiseLevel.noisePoly.empty())
    {
        createPoly2D("NoisePoly", defaultURI, r->noiseLevel.noisePoly, rXML);
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

    if (r->sigmaZeroSFIncidenceMap != AppliedType::NOT_SET)
    {
        createSixString(
            "SigmaZeroSFIncidenceMap",
            defaultURI,
            r->sigmaZeroSFIncidenceMap,
            rXML);
    }

    if (!r->gammaZeroSFPoly.empty())
    {
        createPoly2D("GammaZeroSFPoly", defaultURI, r->gammaZeroSFPoly, rXML);
    }

    if (r->gammaZeroSFIncidenceMap != AppliedType::NOT_SET)
    {
        createSixString(
            "GammaZeroSFIncidenceMap",
            defaultURI,
            r->gammaZeroSFIncidenceMap,
            rXML);
    }
    return rXML;
}

void SICommonXMLParser01x::parseRadiometryFromXML(
    const xml_lite::Element* radiometricXML,
    Radiometric* radiometric) const
{
    parseOptionalPoly2D(radiometricXML, "NoisePoly", radiometric->noiseLevel.noisePoly);
    parseOptionalPoly2D(radiometricXML, "RCSSFPoly", radiometric->rcsSFPoly);
    parseOptionalPoly2D(radiometricXML, "BetaZeroSFPoly", radiometric->betaZeroSFPoly);
    parseOptionalPoly2D(radiometricXML, "SigmaZeroSFPoly", radiometric->sigmaZeroSFPoly);

    XMLElem tmpElem = getOptional(radiometricXML, "SigmaZeroSFIncidenceMap");
    if (tmpElem)
    {
        //optional
        radiometric->sigmaZeroSFIncidenceMap = 
            six::toType<six::AppliedType>(tmpElem->getCharacterData());
    }

    parseOptionalPoly2D(radiometricXML, "GammaZeroSFPoly", radiometric->gammaZeroSFPoly);

    tmpElem = getOptional(radiometricXML, "GammaZeroSFIncidenceMap");
    if (tmpElem)
    {
        //optional
        radiometric->gammaZeroSFIncidenceMap = 
            six::toType<six::AppliedType>(tmpElem->getCharacterData());
    }
}

XMLElem SICommonXMLParser01x::convertMatchInformationToXML(
    const MatchInformation& matchInfo,
    XMLElem parent) const
{
    // This is SICD 0.4 format
    XMLElem matchInfoXML = newElement("MatchInfo", parent);

    for (size_t i = 0; i < matchInfo.types.size(); ++i)
    {
        const MatchType& mt = matchInfo.types[i];
        XMLElem mtXML = newElement("Collect", matchInfoXML);
        setAttribute(mtXML, "index", i + 1);

        createString("CollectorName", mt.collectorName, mtXML);
        if (!mt.illuminatorName.empty())
            createString("IlluminatorName", mt.illuminatorName, mtXML);
        createString("CoreName", mt.matchCollects[0].coreName, mtXML);

        for (std::vector<std::string>::const_iterator it =
                mt.matchType.begin(); it != mt.matchType.end(); ++it)
        {
            createString("MatchType", *it, mtXML);
        }
        addParameters("Parameter", mt.matchCollects[0].parameters, mtXML);
    }

    return matchInfoXML;
}

void SICommonXMLParser01x::parseMatchInformationFromXML(
    const xml_lite::Element* matchInfoXML,
    MatchInformation* matchInfo) const
{
    assert(matchInfoXML != nullptr);

    //This is SICD 0.4 format

    //TODO make sure there is at least one
    std::vector < XMLElem > typesXML;
    matchInfoXML->getElementsByTagName("Collect", typesXML);

    matchInfo->types.resize(typesXML.size());
    for (size_t i = 0; i < typesXML.size(); i++)
    {
        MatchType& type = matchInfo->types[i];

        parseString(getFirstAndOnly(typesXML[i], "CollectorName"), type.collectorName);

        XMLElem illuminatorElem = getOptional(typesXML[i], "IlluminatorName");
        if (illuminatorElem)
        {
            //optional
            parseString(illuminatorElem, type.illuminatorName);
        }

        // in version 0.4 we use the matchCollect object
        parseString(getFirstAndOnly(typesXML[i], "CoreName"), type.matchCollects[0].coreName);

        //optional
        std::vector < XMLElem > matchTypesXML;
        typesXML[i]->getElementsByTagName("MatchType", matchTypesXML);
        type.matchType.resize(matchTypesXML.size());
        for (size_t j = 0; j < matchTypesXML.size(); j++)
        {
            parseString(matchTypesXML[j], type.matchType[j]);
        }

        //optional --
        // in version 0.4 we use the matchCollect object
        parseParameters(typesXML[i], "Parameter", type.matchCollects[0].parameters);
    }
}

}

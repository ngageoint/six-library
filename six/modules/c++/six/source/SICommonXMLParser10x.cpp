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

#include <six/SICommonXMLParser10x.h>

namespace
{
typedef xml::lite::Element* XMLElem;
}

namespace six
{
SICommonXMLParser10x::SICommonXMLParser10x(
    const std::string& defaultURI,
    bool addClassAttributes,
    const std::string& siCommonURI,
    logging::Logger* log,
    bool ownLog) :
    SICommonXMLParser(defaultURI, addClassAttributes,
                      siCommonURI, log, ownLog)
{
}

XMLElem SICommonXMLParser10x::convertCompositeSCPToXML(
    const ErrorStatistics* errorStatistics,
    XMLElem errorStatsXML) const
{
    //! optional field
    std::string defaultURI = getSICommonURI();
    if (errorStatistics->compositeSCP.get())
    {
        XMLElem scpXML = newElement("CompositeSCP", defaultURI, errorStatsXML);

        createDouble("Rg", defaultURI, errorStatistics->compositeSCP->xErr,
                     scpXML);
        createDouble("Az", defaultURI, errorStatistics->compositeSCP->yErr,
                     scpXML);
        createDouble("RgAz", defaultURI, errorStatistics->compositeSCP->xyErr,
                     scpXML);
        return scpXML;
    }
    return NULL;
}

void SICommonXMLParser10x::parseCompositeSCPFromXML(
    const XMLElem errorStatsXML,
    ErrorStatistics* errorStatistics) const
{
    XMLElem compositeSCPXML = getOptional(errorStatsXML, "CompositeSCP");

    //optional
    if (compositeSCPXML)
    {
        errorStatistics->compositeSCP.reset(
                new six::CompositeSCP(CompositeSCP::RG_AZ));

        parseDouble(getFirstAndOnly(compositeSCPXML, "Rg"),
                    errorStatistics->compositeSCP->xErr);
        parseDouble(getFirstAndOnly(compositeSCPXML, "Az"),
                    errorStatistics->compositeSCP->yErr);
        parseDouble(getFirstAndOnly(compositeSCPXML, "RgAz"),
                    errorStatistics->compositeSCP->xyErr);
    }
}

XMLElem SICommonXMLParser10x::convertRadiometryToXML(
    const Radiometric *r, XMLElem parent) const
{
    std::string defaultURI = getSICommonURI();
    XMLElem rXML = newElement("Radiometric", getDefaultURI(), parent);

    if (!r->noiseLevel.noiseType.empty() && !r->noiseLevel.noisePoly.empty())
    {
        XMLElem noiseLevelXML = newElement("NoiseLevel", defaultURI, rXML);
        createString("NoiseLevelType", defaultURI, r->noiseLevel.noiseType, noiseLevelXML);
        createPoly2D("NoisePoly", defaultURI, r->noiseLevel.noisePoly, noiseLevelXML);
    }

    if (!r->rcsSFPoly.empty())
    {
        createPoly2D("RCSSFPoly", defaultURI, r->rcsSFPoly, rXML);
    }

    if (!r->sigmaZeroSFPoly.empty())
    {
        createPoly2D("SigmaZeroSFPoly",
                     defaultURI,
                     r->sigmaZeroSFPoly,
                     rXML);
    }

    if (!r->betaZeroSFPoly.empty())
    {
        createPoly2D("BetaZeroSFPoly", defaultURI, r->betaZeroSFPoly, rXML);
    }

    if (!r->gammaZeroSFPoly.empty())
    {
        createPoly2D("GammaZeroSFPoly", defaultURI, r->gammaZeroSFPoly, rXML);
    }

    return rXML;
}

void SICommonXMLParser10x::parseRadiometryFromXML(
    const XMLElem radiometricXML,
    Radiometric* radiometric) const
{
    XMLElem tmpElem = NULL;

    tmpElem = getOptional(radiometricXML, "NoiseLevel");
    if (tmpElem)
    {
        parseString(getFirstAndOnly(tmpElem, "NoiseLevelType"),
                    radiometric->noiseLevel.noiseType);
        parsePoly2D(getFirstAndOnly(tmpElem, "NoisePoly"),
                    radiometric->noiseLevel.noisePoly);
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

    tmpElem = getOptional(radiometricXML, "GammaZeroSFPoly");
    if (tmpElem)
    {
        //optional
        parsePoly2D(tmpElem, radiometric->gammaZeroSFPoly);
    }
}
XMLElem SICommonXMLParser10x::convertMatchInformationToXML(
    const MatchInformation& matchInfo,
    XMLElem parent) const
{
    XMLElem matchInfoXML = newElement("MatchInfo", parent);

    createInt("NumMatchTypes",
              static_cast<int>(matchInfo.types.size()),
              matchInfoXML);

    for (size_t ii = 0; ii < matchInfo.types.size(); ++ii)
    {
        const MatchType& mt = matchInfo.types[ii];
        XMLElem mtXML = newElement("MatchType", matchInfoXML);
        setAttribute(mtXML, "index", str::toString(ii + 1));

        createString("TypeID", mt.typeID, mtXML);
        createInt("CurrentIndex", mt.currentIndex, mtXML);
        createInt("NumMatchCollections",
                  static_cast<int>(mt.matchCollects.size()), mtXML);

        for (size_t jj = 0; jj < mt.matchCollects.size(); ++jj)
        {
            XMLElem mcXML = newElement("MatchCollection", mtXML);
            setAttribute(mcXML, "index", str::toString(jj + 1));

            createString("CoreName", mt.matchCollects[jj].coreName, mcXML);
            createInt("MatchIndex", mt.matchCollects[jj].matchIndex, mcXML);
            addParameters("Parameter", mt.matchCollects[jj].parameters, mcXML);
        }
    }

    return matchInfoXML;
}

void SICommonXMLParser10x::parseMatchInformationFromXML(
    const XMLElem matchInfoXML,
    MatchInformation* matchInfo) const
{
    size_t numMatchTypes = 0;
    parseInt(getFirstAndOnly(matchInfoXML, "NumMatchTypes"), numMatchTypes);
    if (numMatchTypes == 0)
    {
        throw except::Exception(Ctxt("NumMatchTypes cannot be zero"));
    }

    std::vector < XMLElem > typesXML;
    matchInfoXML->getElementsByTagName("MatchType", typesXML);

    //! validate the numMatchTypes
    if (typesXML.size() != numMatchTypes)
    {
        throw except::Exception(
            Ctxt("NumMatchTypes does not match number of MatchType fields"));
    }

    matchInfo->types.resize(typesXML.size());
    for (size_t ii = 0; ii < typesXML.size(); ii++)
    {
        MatchType& type = matchInfo->types[ii];

        parseString(getFirstAndOnly(typesXML[ii], "TypeID"), type.typeID);

        XMLElem curIndexElem = getOptional(typesXML[ii], "CurrentIndex");
        if (curIndexElem)
        {
            //optional
            parseInt(curIndexElem, type.currentIndex);
        }

        int numMatchCollections = 0;
        parseInt(getFirstAndOnly(typesXML[ii], "NumMatchCollections"),
                 numMatchCollections);

        std::vector < XMLElem > matchCollectionsXML;
        typesXML[ii]->getElementsByTagName("MatchCollection", matchCollectionsXML);

        //! validate the numMatchTypes
        if (matchCollectionsXML.size() !=
            static_cast<size_t>(numMatchCollections))
        {
            throw except::Exception(
                Ctxt("NumMatchCollections does not match number of " \
                     "MatchCollect fields"));
        }

        // Need to make sure this is resized properly - at MatchType
        // construction time, matchCollects is initialized to size 1, but in
        // SICD 1.1 this entire block may be missing.
        type.matchCollects.resize(matchCollectionsXML.size());
        for (size_t jj = 0; jj < matchCollectionsXML.size(); jj++)
        {
            MatchCollect& collect(type.matchCollects[jj]);

            parseString(getFirstAndOnly(
                matchCollectionsXML[jj], "CoreName"), collect.coreName);

            XMLElem matchIndexXML =
                getOptional(matchCollectionsXML[jj], "MatchIndex");
            if (matchIndexXML)
            {
                parseInt(matchIndexXML, collect.matchIndex);
            }

            parseParameters(
                matchCollectionsXML[jj], "Parameter", collect.parameters);
        }
    }
}
}

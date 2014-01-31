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
#include <set>

#include <six/SICommonXMLParser02x.h>

namespace
{
typedef xml::lite::Element* XMLElem;
}

namespace six
{
SICommonXMLParser02x::SICommonXMLParser02x(
    const std::string& defaultURI,
    bool addClassAttributes,
    const std::string& siCommonURI,
    logging::Logger* log,
    bool ownLog) :
    SICommonXMLParser01x(defaultURI, addClassAttributes,
                         siCommonURI, log, ownLog)
{
}

XMLElem SICommonXMLParser02x::convertRadiometryToXML(
    const Radiometric* r, XMLElem parent) const
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

    if (r->sigmaZeroSFIncidenceMap != AppliedType::NOT_SET)
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

    if (r->gammaZeroSFIncidenceMap != AppliedType::NOT_SET)
    {
        createString(
            "GammaZeroSFIncidenceMap",
            defaultURI,
            six::toString<six::AppliedType>(r->gammaZeroSFIncidenceMap),
            rXML);
    }
    return rXML;
}

void SICommonXMLParser02x::parseRadiometryFromXML(
    const XMLElem radiometricXML, 
    Radiometric* radiometric) const
{
    XMLElem tmpElem = NULL;

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

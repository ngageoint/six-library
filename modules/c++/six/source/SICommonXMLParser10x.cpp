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
        errorStatistics->compositeSCP.reset(new six::CompositeSCP());

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

}


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

#include <six/sicd/ComplexXMLParser040.h>
#include <six/SICommonXMLParser01x.h>

namespace
{
typedef xml::lite::Element* XMLElem;
}

namespace six
{
namespace sicd
{

ComplexXMLParser040::ComplexXMLParser040(const std::string& version,
                                         logging::Logger* log,
                                         bool ownLog) :
    ComplexXMLParser04x(version, true, std::auto_ptr<six::SICommonXMLParser>(
                           new six::SICommonXMLParser01x(
                               versionToURI(version), true,
                               versionToURI(version), log)),
                        log, ownLog)
{
}

XMLElem ComplexXMLParser040::convertRMATToXML(
    const RMAT* rmat,
    XMLElem rmaXML) const
{
    createString("ImageType", "RMAT", rmaXML);

    XMLElem rmatXML = newElement("RMAT", rmaXML);

    createDouble("RMRefTime", rmat->refTime, rmatXML);
    common().createVector3D("RMPosRef", rmat->refPos, rmatXML);
    common().createVector3D("RMVelRef", rmat->refVel, rmatXML);
    common().createPoly2D("CosDCACOAPoly", rmat->cosDCACOAPoly, rmatXML);
    createDouble("Kx1", rmat->kx1, rmatXML);
    createDouble("Kx2", rmat->kx2, rmatXML);
    createDouble("Ky1", rmat->ky1, rmatXML);
    createDouble("Ky2", rmat->ky2, rmatXML);

    return rmatXML;
}

void ComplexXMLParser040::parseRMATFromXML(const XMLElem rmatElem,
                                           RMAT* rmat) const
{
    parseDouble(getFirstAndOnly(rmatElem, "RMRefTime"), rmat->refTime);
    common().parseVector3D(getFirstAndOnly(rmatElem, "RMPosRef"), rmat->refPos);
    common().parseVector3D(getFirstAndOnly(rmatElem, "RMVelRef"), rmat->refVel);
    common().parsePoly2D(getFirstAndOnly(rmatElem, "CosDCACOAPoly"),
                rmat->cosDCACOAPoly);
    parseDouble(getFirstAndOnly(rmatElem, "Kx1"), rmat->kx1);
    parseDouble(getFirstAndOnly(rmatElem, "Kx2"), rmat->kx2);
    parseDouble(getFirstAndOnly(rmatElem, "Ky1"), rmat->ky1);
    parseDouble(getFirstAndOnly(rmatElem, "Ky2"), rmat->ky2);
}

void ComplexXMLParser040::convertDRateSFPolyToXML(
    const INCA* inca, XMLElem incaXML) const
{
    //! Poly1D in 0.4.0
    if (inca->dopplerRateScaleFactorPoly.orderX() != 0 &&
        inca->dopplerRateScaleFactorPoly.orderY() != 0)
    {
        throw except::Exception(Ctxt("Verify the poly is stored in 1D form"));
    }

    // Reshape the data into a Poly1D for writing
    six::Poly1D oneDPoly;
    if (inca->dopplerRateScaleFactorPoly.orderX() != 0)
    {
        oneDPoly = six::Poly1D(inca->dopplerRateScaleFactorPoly.orderX());
        for (size_t ii = 0; ii <= oneDPoly.order(); ++ii)
        {
            oneDPoly[ii] = inca->dopplerRateScaleFactorPoly[ii][0];
        }
    }
    else
    {
        oneDPoly = inca->dopplerRateScaleFactorPoly[0];
    }

    common().createPoly1D("DRateSFPoly",
        oneDPoly, incaXML);
}

void ComplexXMLParser040::parseDRateSFPolyFromXML(
    const XMLElem incaElem, INCA* inca) const
{
    //! Poly1D in 0.4.0
    Poly1D dRateSFPoly;
    common().parsePoly1D(getFirstAndOnly(incaElem, "DRateSFPoly"),
                         dRateSFPoly);

    // set x order -> 0, y order -> oldPoly
    inca->dopplerRateScaleFactorPoly = Poly2D(0, dRateSFPoly.order());
    inca->dopplerRateScaleFactorPoly.set(0, dRateSFPoly);
}

}
}


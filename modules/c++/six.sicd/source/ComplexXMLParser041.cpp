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

#include <six/sicd/ComplexXMLParser041.h>

namespace
{
typedef xml::lite::Element* XMLElem;
}

namespace six
{
namespace sicd
{

ComplexXMLParser041::ComplexXMLParser041(const std::string& version,
                                         logging::Logger* log,
                                         bool ownLog) :
    ComplexXMLParser04x(version, log, ownLog)
{
}

XMLElem ComplexXMLParser041::convertRMATToXML(
    const RMAT* rmat, 
    XMLElem rmaXML) const
{
    createString("ImageType", "RMAT", rmaXML);

    XMLElem rmatXML = newElement("RMAT", rmaXML);

    createDouble("RefTime", rmat->refTime, rmatXML);
    common().createVector3D("PosRef", rmat->refPos, rmatXML);
    common().createVector3D("UnitVelRef", rmat->refVel, rmatXML);
    common().createPoly1D("DistRLPoly", rmat->distRefLinePoly, rmatXML);
    common().createPoly2D("CosDCACOAPoly", rmat->cosDCACOAPoly, rmatXML);
    createDouble("Kx1", rmat->kx1, rmatXML);
    createDouble("Kx2", rmat->kx2, rmatXML);
    createDouble("Ky1", rmat->ky1, rmatXML);
    createDouble("Ky2", rmat->ky2, rmatXML);

    return rmatXML;
}

void ComplexXMLParser041::parseRMATFromXML(const XMLElem rmatElem, 
                                           RMAT* rmat) const
{
    parseDouble(getFirstAndOnly(rmatElem, "RefTime"), rmat->refTime);
    common().parseVector3D(getFirstAndOnly(rmatElem, "PosRef"), rmat->refPos);
    common().parseVector3D(getFirstAndOnly(rmatElem, "UnitVelRef"), rmat->refVel);
    common().parsePoly1D(getFirstAndOnly(rmatElem, "DistRLPoly"),
                rmat->distRefLinePoly);
    common().parsePoly2D(getFirstAndOnly(rmatElem, "CosDCACOAPoly"),
                rmat->cosDCACOAPoly);
    parseDouble(getFirstAndOnly(rmatElem, "Kx1"), rmat->kx1);
    parseDouble(getFirstAndOnly(rmatElem, "Kx2"), rmat->kx2);
    parseDouble(getFirstAndOnly(rmatElem, "Ky1"), rmat->ky1);
    parseDouble(getFirstAndOnly(rmatElem, "Ky2"), rmat->ky2);
}

}
}


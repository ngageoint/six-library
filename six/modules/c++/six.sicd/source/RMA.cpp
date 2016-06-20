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
#include "six/sicd/GeoData.h"
#include "six/sicd/Position.h"
#include "six/sicd/RMA.h"
#include "six/sicd/Utilities.h"

using namespace six;
using namespace six::sicd;

RMAT::RMAT() :
    refTime(Init::undefined<double>()),
    refPos(Init::undefined<Vector3>()),
    refVel(Init::undefined<Vector3>()),
    distRefLinePoly(Init::undefined<Poly1D>()),
    cosDCACOAPoly(Init::undefined<Poly2D>()),
    kx1(Init::undefined<double>()),
    kx2(Init::undefined<double>()),
    ky1(Init::undefined<double>()),
    ky2(Init::undefined<double>()),
    dopConeAngleRef(Init::undefined<double>())
{
}

bool RMAT::operator==(const RMAT& rhs) const
{
    return (refTime == rhs.refTime &&
        refPos == rhs.refPos &&
        refVel == rhs.refVel &&
        distRefLinePoly == rhs.distRefLinePoly &&
        cosDCACOAPoly == rhs.cosDCACOAPoly &&
        kx1 == rhs.kx1 &&
        kx2 == rhs.kx2 &&
        ky1 == rhs.ky1 &&
        ky2 == rhs.ky2 &&
        dopConeAngleRef == rhs.dopConeAngleRef);
}

void RMAT::fillDerivedFields(const Vector3& scp)
{
    setSCP(scp);

    if (!Init::isUndefined<Vector3>(refPos) &&
        !Init::isUndefined<Vector3>(refVel))
    {
        // RCA is a derived field
        if (Init::isUndefined<double>(dopConeAngleRef))
        {
            dopConeAngleRef = std::acos(refVel.unit().dot(uLOS())) *
                    math::Constants::RADIANS_TO_DEGREES;
        }
    }
}

void RMAT::setSCP(const Vector3& scp)
{
    mScp.reset(new Vector3(scp));
}

const Vector3& RMAT::scp() const
{
    if (!mScp.get())
    {
        throw except::Exception(Ctxt("mScp is NULL. Initialize with RMAT.setSCP()"));
    }
    return *mScp;
}

Vector3 RMAT::uLOS() const
{
    return (scp() - refPos).unit();
}

int RMAT::look() const
{
    Vector3 left = cross(refPos.unit(), refVel.unit());
    return Utilities::sign(left.dot(uLOS()));
}

RMCR::RMCR() :
    refPos(Init::undefined<Vector3>()),
    refVel(Init::undefined<Vector3>()),
    dopConeAngleRef(Init::undefined<double>())
{
}

void RMCR::fillDerivedFields(const Vector3& scp)
{
    setSCP(scp);

    if (!Init::isUndefined<Vector3>(refPos) &&
        !Init::isUndefined<Vector3>(refVel))
    {
        // RCA is a derived field
        if (Init::isUndefined<double>(dopConeAngleRef))
        {
            dopConeAngleRef = std::acos(refVel.unit().dot(uLOS())) *
                math::Constants::RADIANS_TO_DEGREES;
        }
    }
}

Vector3 RMCR::uLOS() const
{
    return (scp() - refPos).unit();
}

int RMCR::look() const
{
    Vector3 left = cross(refPos.unit(), refVel.unit());
    return Utilities::sign(left.dot(uLOS()));
}

void RMCR::setSCP(const Vector3& scp)
{
    mScp.reset(new Vector3(scp));
}

const Vector3& RMCR::scp() const
{
    if (!mScp.get())
    {
        throw except::Exception(Ctxt("mScp is NULL. Initialize with RMAT.setSCP()"));
    }
    return *mScp;
}

INCA::INCA() :
    timeCAPoly(Init::undefined<Poly1D>()),
    rangeCA(Init::undefined<double>()),
    freqZero(Init::undefined<double>()),
    dopplerRateScaleFactorPoly(Init::undefined<Poly2D>()),
    dopplerCentroidPoly(Init::undefined<Poly2D>()),
    dopplerCentroidCOA(Init::undefined<BooleanType>())
{
}

bool INCA::operator==(const INCA& rhs) const
{
    return (timeCAPoly == rhs.timeCAPoly &&
        rangeCA == rhs.rangeCA &&
        freqZero == rhs.freqZero &&
        dopplerRateScaleFactorPoly == rhs.dopplerRateScaleFactorPoly &&
        dopplerCentroidPoly == rhs.dopplerCentroidPoly &&
        dopplerCentroidCOA == rhs.dopplerCentroidCOA);
}

void INCA::fillDerivedFields(const Vector3& scp,
        double fc,
        const Position& position)
{
    setSCP(scp);
    setArpPoly(position.arpPoly);

    if (!Init::isUndefined<Poly1D>(timeCAPoly) &&
        !Init::isUndefined<PolyXYZ>(position.arpPoly) &&
        Init::isUndefined<double>(rangeCA))
    {
        if (Init::isUndefined<double>(rangeCA))
        {
            rangeCA = (caPos() - scp).norm();
        }
    }
}

Vector3 INCA::caPos() const
{
    //TODO: verify!!!
    return (*mArpPoly)(timeCAPoly(1));
}

Vector3 INCA::caVel() const
{
    return mArpPoly->derivative()(timeCAPoly(1));
}

void INCA::setSCP(const Vector3& scp)
{
    mScp.reset(new Vector3(scp));
}

const Vector3& INCA::scp() const
{
    if (!mScp.get())
    {
        throw except::Exception(Ctxt(
            "mScp is NULL. Initialize with RMAT.setSCP()"));
    }
    return *mScp;
}

void INCA::setArpPoly(const PolyXYZ& arpPoly)
{
    mArpPoly.reset(new PolyXYZ(arpPoly));
}

const PolyXYZ& INCA::arpPoly() const
{
    if (!mArpPoly.get())
    {
        throw except::Exception(Ctxt(
            "mArpPoly is NULL. Initialize with RMAT.setArpPoly()"));
    }
    return *mArpPoly;
}

void INCA::fillDefaultFields(double fc)
{
    if (!Init::isUndefined<double>(fc) &&
        Init::isUndefined<double>(freqZero))
    {
        freqZero = fc;
    }
}

RMA::RMA() : 
    algoType(RMAlgoType::NOT_SET)
{
}

void RMA::fillDerivedFields(const GeoData& geoData, 
        const Position& position,
        double fc)
{
    const Vector3& scp = geoData.scp.ecf;

    if (rmat.get())
    {
        rmat->fillDerivedFields(scp);
    }
    else if (rmcr.get())
    {
        rmcr->fillDerivedFields(scp);
    }
    else if (inca.get())
    {
        inca->fillDerivedFields(scp, fc, position);
    }
}


void RMA::fillDefaultFields(double fc)
{
    if (rmat.get())
    {
    }
    else if (rmcr.get())
    {
    }
    else if (inca.get())
    {
        inca->fillDefaultFields(fc);
    }
}


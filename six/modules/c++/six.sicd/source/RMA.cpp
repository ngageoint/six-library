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
#include "math/Utilities.h"
#include "six/sicd/GeoData.h"
#include "six/sicd/Position.h"
#include "six/sicd/RMA.h"
#include "six/sicd/SCPCOA.h"
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
    if (!Init::isUndefined(refPos) &&
        !Init::isUndefined(refVel))
    {
        if (Init::isUndefined(dopConeAngleRef))
        {
            dopConeAngleRef = derivedDcaRef(scp);
        }
    }
}

void RMAT::fillDefaultFields(const SCPCOA& scpcoa)
{
    if (Init::isUndefined(refPos) &&
        !Init::isUndefined(scpcoa.arpPos))
    {
        refPos = scpcoa.arpPos;
    }
    if (Init::isUndefined(refVel) &&
        !Init::isUndefined(scpcoa.arpVel))
    {
        refVel = scpcoa.arpVel;
    }
}

bool RMAT::validate(const Vector3& scp, logging::Logger& log)
{
    std::ostringstream messageBuilder;
    bool valid = true;

    // 2.12.3.2.5
    const double dcaRef = derivedDcaRef(scp);
    if (std::abs(dcaRef - dopConeAngleRef) > 1e-6)
    {
        messageBuilder.str("");
        messageBuilder << "RMA fields inconsistent." << std::endl
            << "RMA.RMAT.DopConeAngleRef: " << dopConeAngleRef
            << std::endl << "Derived RMA.RMAT.DopConeAngleRef: " << dcaRef;
        log.error(messageBuilder.str());
        valid = false;
    }
    return valid;
}

double RMAT::derivedDcaRef(const Vector3& scp) const
{
    return std::acos((refVel.unit()).dot(uLOS(scp))) *
        math::Constants::RADIANS_TO_DEGREES;
}

Vector3 RMAT::uYAT(const Vector3& scp) const
{
    return (refVel.unit() * -look(scp));
}

Vector3 RMAT::spn(const Vector3& scp) const
{
    return cross(uLOS(scp), uYAT(scp)).unit();
}

Vector3 RMAT::uXCT(const Vector3& scp) const
{
    return cross(uYAT(scp), spn(scp));
}

Vector3 RMAT::uLOS(const Vector3& scp) const
{
    return (scp - refPos).unit();
}

int RMAT::look(const Vector3& scp) const
{
    const Vector3 left = cross(refPos.unit(), refVel.unit());
    return math::sign(left.dot(uLOS(scp)));
}

RMCR::RMCR() :
    refPos(Init::undefined<Vector3>()),
    refVel(Init::undefined<Vector3>()),
    dopConeAngleRef(Init::undefined<double>())
{
}

void RMCR::fillDerivedFields(const Vector3& scp)
{
    if (!Init::isUndefined(refPos) &&
        !Init::isUndefined(refVel))
    {
        // RCA is a derived field
        if (Init::isUndefined(dopConeAngleRef))
        {
            dopConeAngleRef = derivedDcaRef(scp);
        }
    }
}

void RMCR::fillDefaultFields(const SCPCOA& scpcoa)
{
    if (Init::isUndefined(refPos) &&
        !Init::isUndefined(scpcoa.arpPos))
    {
        refPos = scpcoa.arpPos;
    }
    if (Init::isUndefined(refVel) &&
        !Init::isUndefined(scpcoa.arpVel))
    {
        refVel = scpcoa.arpVel;
    }
}

bool RMCR::validate(const Vector3& scp, logging::Logger& log)
{
    bool valid = true;
    std::ostringstream messageBuilder;

    // 2.12.3.3.5
    const double dcaRef = derivedDcaRef(scp);
    if (std::abs(dcaRef - dopConeAngleRef) > 1e-6)
    {
        messageBuilder.str("");
        messageBuilder << "RMA fields inconsistent." << std::endl
            << "RMA.RMCR.DopConeAngleRef: " << dopConeAngleRef << std::endl
            << "Derived RMA.RMCR.DopConeAngleRef: " << dcaRef;
        log.error(messageBuilder.str());
        valid = false;
    }
    return valid;
}

double RMCR::derivedDcaRef(const Vector3& scp) const
{
    return std::acos(refVel.unit().dot(uXRG(scp))) *
        math::Constants::RADIANS_TO_DEGREES;
}

Vector3 RMCR::uXRG(const Vector3& scp) const
{
    return (scp - refPos).unit();
}

Vector3 RMCR::uYCR(const Vector3& scp) const
{
    return cross(spn(scp), uXRG(scp));
}

Vector3 RMCR::spn(const Vector3& scp) const
{
    return (cross(refVel.unit(), uXRG(scp)) * look(scp)).unit();
}

int RMCR::look(const Vector3& scp) const
{
    const Vector3 left = cross(refPos.unit(), refVel.unit());
    return math::sign(left.dot(uXRG(scp)));
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
        const Position& position)
{
    if (!Init::isUndefined(timeCAPoly) &&
        !Init::isUndefined(position.arpPoly) &&
        Init::isUndefined(rangeCA))
    {
        if (Init::isUndefined(rangeCA))
        {
            rangeCA = derivedRangeCa(scp, position.arpPoly);
        }
    }
}

bool INCA::validate(const CollectionInformation& collectionInformation,
        const Vector3& scp, const PolyXYZ& arpPoly,
        double fc, logging::Logger& log) const
{
    bool valid = true;
    std::ostringstream messageBuilder;

    //2.12.3.4.3
    if (collectionInformation.radarMode == RadarModeType::SPOTLIGHT &&
        (!Init::isUndefined(dopplerCentroidPoly) ||
            !Init::isUndefined(dopplerCentroidCOA)))
    {
        messageBuilder.str("");
        messageBuilder << "RMA.INCA fields inconsistent." << std::endl
            << "RMA.INCA.DopplerCentroidPoly/DopplerCentroidCOA "
            << "should not be included for SPOTLIGHT collection.";
        log.error(messageBuilder.str());
        valid = false;
    }

    //2.12.3.4.4
    if (collectionInformation.radarMode != RadarModeType::SPOTLIGHT &&
        (Init::isUndefined(dopplerCentroidPoly) ||
            Init::isUndefined(dopplerCentroidCOA)))
    {
        messageBuilder.str("");
        messageBuilder << "RMA.INCA fields inconsistent." << std::endl
            << "RMA.INCA.DopplerCentroidPoly/COA required "
            << "for non-SPOTLIGHT collection.";
        log.error(messageBuilder.str());
        valid = false;
    }

    // 2.12.3.4.9
    if (std::abs((freqZero / fc) - 1) > 1e-3)
    {
        messageBuilder.str("");
        messageBuilder << "RMA.INCA.FreqZero is typically the center transmit frequency" << std::endl
            << "RMA.INCA.FreqZero: " << freqZero
            << "Center transmit frequency: " << fc;
        log.warn(messageBuilder.str());
        valid = false;
    }

    // 2.12.3.4.10
    if (derivedRangeCa(scp, arpPoly) - rangeCA > 1e-2)
    {
        messageBuilder.str("");
        messageBuilder << "RMA.INCA fields inconsistent." << std::endl
            << "RMA.INCA.rangeCA: " << rangeCA
            << "Derived RMA.INCA.rangeCA: " << derivedRangeCa(scp, arpPoly);
        log.error(messageBuilder.str());
        valid = false;
    }

    return valid;
}

double INCA::derivedRangeCa(const Vector3& scp, const PolyXYZ& arpPoly) const
{
    return (caPos(arpPoly) - scp).norm();
}

Vector3 INCA::caPos(const PolyXYZ& arpPoly) const
{
    return arpPoly(timeCAPoly[0]);
}

Vector3 INCA::caVel(const PolyXYZ& arpPoly) const
{
    return (arpPoly.derivative())(timeCAPoly[0]);
}

Vector3 INCA::uRG(const Vector3& scp, const PolyXYZ& arpPoly) const
{
    return (scp - caPos(arpPoly)).unit();
}

Vector3 INCA::uAZ(const Vector3& scp, const PolyXYZ& arpPoly) const
{
    return cross(spn(scp, arpPoly), uRG(scp, arpPoly));
}

Vector3 INCA::spn(const Vector3& scp, const PolyXYZ& arpPoly) const
{
    return (cross(uRG(scp, arpPoly), caVel(arpPoly)) *
            -look(scp, arpPoly)).unit();
}

int INCA::look(const Vector3& scp, const PolyXYZ& arpPoly) const
{
    return math::sign(left(arpPoly).dot(uRG(scp, arpPoly)));
}

Vector3 INCA::left(const PolyXYZ& arpPoly) const
{
    return cross(caPos(arpPoly).unit(), caVel(arpPoly).unit());
}

void INCA::fillDefaultFields(double fc)
{
    if (!Init::isUndefined(fc) &&
        Init::isUndefined(freqZero))
    {
        freqZero = fc;
    }
}

RMA::RMA() :
    algoType(RMAlgoType::NOT_SET)
{
}

void RMA::fillDerivedFields(const GeoData& geoData,
        const Position& position)
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
        inca->fillDerivedFields(scp, position);
    }
}


void RMA::fillDefaultFields(const SCPCOA& scpcoa, double fc)
{
    if (rmat.get())
    {
        rmat->fillDefaultFields(scpcoa);
    }
    else if (rmcr.get())
    {
        rmcr->fillDefaultFields(scpcoa);
    }
    else if (inca.get())
    {
        inca->fillDefaultFields(fc);
    }
}

bool RMA::validate(const CollectionInformation& collectionInformation,
        const Vector3& scp, const PolyXYZ& arpPoly, double fc,
        logging::Logger& log) const
{
    if (rmat.get())
    {
        return rmat->validate(scp, log);
    }
    else if (rmcr.get())
    {
        return rmcr->validate(scp, log);
    }
    else if (inca.get())
    {
        return inca->validate(collectionInformation, scp, arpPoly, fc, log);
    }

    std::ostringstream messageBuilder;
    messageBuilder << "Exactly one of RMA->RMAT, RMA->RMCR, RMA->INCA "
        << "must exist.";
    log.error(messageBuilder.str());
    return false;
}

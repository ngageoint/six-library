/* =========================================================================
 * This file is part of scene-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * scene-c++ is free software; you can redistribute it and/or modify
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
#include <math/Constants.h>
#include <math/Utilities.h>
#include "scene/EllipsoidModel.h"

namespace scene
{
const double WGS84EllipsoidModel::EQUATORIAL_RADIUS_METERS = 6378137.0;
const double WGS84EllipsoidModel::POLAR_RADIUS_METERS = 6356752.3142;

EllipsoidModel::EllipsoidModel(Units unitsVal,
        AngularUnits angularUnitsVal, double eqRadiusVal,
        double polRadiusVal) : units(INVALID_UNITS),
            angularUnits(INVALID_ANGULAR_UNITS)
{
    setUnits(unitsVal);
    setAngularUnits(angularUnitsVal);
    setEquatorialRadius(eqRadiusVal);
    setPolarRadius(polRadiusVal);

}

EllipsoidModel::EllipsoidModel(const EllipsoidModel & m)
{
    *this = m;
}

EllipsoidModel&
EllipsoidModel::operator=(const EllipsoidModel & m)
{
    setUnits(m.getUnits());
    setAngularUnits(m.getAngularUnits());
    setEquatorialRadius(m.getEquatorialRadius());
    setPolarRadius(m.getPolarRadius());

    return *this;
}

Units EllipsoidModel::getUnits() const
{
    return units;
}

AngularUnits EllipsoidModel::getAngularUnits() const
{
    return angularUnits;
}

double EllipsoidModel::getEquatorialRadius() const
{
    return equatorialRadius;
}

double EllipsoidModel::getPolarRadius() const
{
    return polarRadius;
}

double EllipsoidModel::calculateFlattening() const
{
    double flattening = getEquatorialRadius() - getPolarRadius();
    flattening /= getEquatorialRadius();
    return flattening;
}

double EllipsoidModel::calculateEccentricity() const
{
    double ecc = 1.0 - math::square(getPolarRadius())/math::square(getEquatorialRadius());
    ecc = sqrt(ecc);
    return ecc;
}

void EllipsoidModel::setUnits(Units val)
{
    //if the units value is different from the previous one,
    //update the radius values so they are in the new units.
    if(units == INVALID_UNITS || units != val)
    {
        if ((val != METERS) && (val != FEET))
        {
            throw except::Exception(Ctxt("Invalid value"));
        }
        units = val;

        if (val == METERS)
        {
            //do feet to meters
            equatorialRadius *= math::Constants::FEET_TO_METERS;
            polarRadius *= math::Constants::FEET_TO_METERS;
        }
        else if (val == FEET)
        {
            //do meters to feet
            equatorialRadius *= math::Constants::METERS_TO_FEET;
            polarRadius *= math::Constants::METERS_TO_FEET;
        }
    }
}

void EllipsoidModel::setAngularUnits(AngularUnits val)
{
    if(angularUnits == INVALID_ANGULAR_UNITS || angularUnits != val)
    {
        if(val == DEGREES || val == RADIANS)
        {
            angularUnits = val;
        }
        else
        {
            throw except::Exception(Ctxt("Invalid value"));
        }
    }
}

void EllipsoidModel::setEquatorialRadius(double val)
{
    if(val > 0)
    {
        equatorialRadius = val;
    }
    else
    {
        throw except::Exception(Ctxt("Invalid value"));
    }
}

void EllipsoidModel::setPolarRadius(double val)
{
    if(val > 0)
    {
        polarRadius = val;
    }
    else
    {
        throw except::Exception(Ctxt("Invalid value"));
    }
}

WGS84EllipsoidModel::WGS84EllipsoidModel()
{
    setUnits(METERS);
    setAngularUnits(RADIANS);
    initRadiusValues();
}

WGS84EllipsoidModel::WGS84EllipsoidModel(Units unitsVal,
        AngularUnits angularUnitsVal)
{
    setUnits(unitsVal);
    setAngularUnits(angularUnitsVal);
    initRadiusValues();
}

WGS84EllipsoidModel::WGS84EllipsoidModel(
        const WGS84EllipsoidModel & m)
{
    *this = m;
    initRadiusValues();
}

void WGS84EllipsoidModel::initRadiusValues()
{
    EllipsoidModel::setEquatorialRadius(EQUATORIAL_RADIUS_METERS);
    EllipsoidModel::setPolarRadius(POLAR_RADIUS_METERS);

    // If units are feet, update the values
    if (getUnits() == FEET)
    {
        //do meters to feet
        equatorialRadius *= math::Constants::METERS_TO_FEET;
        polarRadius *= math::Constants::METERS_TO_FEET;
    }
}

void WGS84EllipsoidModel::setEquatorialRadius(double)
{
    //can't change the value - do some error
    throw except::Exception(Ctxt("Tried to change the equatorial radius - not supported"));
}

void WGS84EllipsoidModel::setPolarRadius(double)
{
    throw except::Exception(Ctxt("Tried to change the polar radius - not supported"));

}

EllipsoidModel&
WGS84EllipsoidModel::operator=(const EllipsoidModel & m)
{
    setUnits(m.getUnits());
    setAngularUnits(m.getAngularUnits());

    return *this;
}

Vector3 WGS84EllipsoidModel::getNormalVector(const Vector3& point) const
{
    Vector3 normalVector;
    normalVector[0] = point[0] / math::square(getEquatorialRadius());
    normalVector[1] = point[1] / math::square(getEquatorialRadius());
    normalVector[2] = point[2] / math::square(getPolarRadius());

    return normalVector.unit();
}
}

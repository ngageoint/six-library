/* =========================================================================
 * This file is part of scene-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2013, General Dynamics - Advanced Information Systems
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
#include "scene/EllipsoidModel.h"

scene::EllipsoidModel::EllipsoidModel(scene::Units unitsVal,
        scene::AngularUnits angularUnitsVal, double eqRadiusVal,
        double polRadiusVal) : units(INVALID_UNITS),
            angularUnits(INVALID_ANGULAR_UNITS)
{
    setUnits(unitsVal);
    setAngularUnits(angularUnitsVal);
    setEquatorialRadius(eqRadiusVal);
    setPolarRadius(polRadiusVal);

}

scene::EllipsoidModel::EllipsoidModel(const scene::EllipsoidModel & m)
{
    *this = m;
}

scene::EllipsoidModel&
scene::EllipsoidModel::operator=(const scene::EllipsoidModel & m)
{
    setUnits(m.getUnits());
    setAngularUnits(m.getAngularUnits());
    setEquatorialRadius(m.getEquatorialRadius());
    setPolarRadius(m.getPolarRadius());

    return *this;
}

scene::Units scene::EllipsoidModel::getUnits() const
{
    return units;
}

scene::AngularUnits scene::EllipsoidModel::getAngularUnits() const
{
    return angularUnits;
}

double scene::EllipsoidModel::getEquatorialRadius() const
{
    return equatorialRadius;
}

double scene::EllipsoidModel::getPolarRadius() const
{
    return polarRadius;
}

double scene::EllipsoidModel::calculateFlattening() const
{
    double flattening = equatorialRadius - polarRadius;
    flattening /= equatorialRadius;
    return flattening;
}

double scene::EllipsoidModel::calculateEccentricity() const
{
    double ecc = 1.0 - pow(polarRadius,2)/pow(equatorialRadius,2);
    ecc = sqrt(ecc);
    return ecc;
}

void scene::EllipsoidModel::setUnits(scene::Units val)
{
    //if the units value is different from the previous one,
    //update the radius values so they are in the new units.
    if(units == INVALID_UNITS || units != val)
    {
        if( val == scene::METERS)
        {
            //do feet to meters
            equatorialRadius *= math::Constants::FEET_TO_METERS;
            polarRadius *= math::Constants::FEET_TO_METERS;
            units = val;
        }
        else if(val == scene::FEET)
        {
            //do meters to feet
            equatorialRadius *= math::Constants::METERS_TO_FEET;
            polarRadius *= math::Constants::METERS_TO_FEET;
            units = val;
        }
        else
        {

            throw except::Exception(Ctxt("Invalid value"));
        }
    }
}

void scene::EllipsoidModel::setAngularUnits(scene::AngularUnits val)
{
    if(angularUnits == INVALID_ANGULAR_UNITS || angularUnits != val)
    {
        if(val == scene::DEGREES || val == scene::RADIANS)
        {
            angularUnits = val;
        }
        else
        {
            throw except::Exception(Ctxt("Invalid value"));
        }
    }
}

void scene::EllipsoidModel::setEquatorialRadius(double val)
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

void scene::EllipsoidModel::setPolarRadius(double val)
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

scene::WGS84EllipsoidModel::WGS84EllipsoidModel()
{
    setUnits(scene::METERS);
    setAngularUnits(scene::RADIANS);
    initRadiusValues();
}

scene::WGS84EllipsoidModel::WGS84EllipsoidModel(scene::Units unitsVal,
        scene::AngularUnits angularUnitsVal)
{
    setUnits(unitsVal);
    setAngularUnits(angularUnitsVal);
    initRadiusValues();
}

scene::WGS84EllipsoidModel::WGS84EllipsoidModel(
        const scene::WGS84EllipsoidModel & m)
{
    *this = m;
    initRadiusValues();
}

void scene::WGS84EllipsoidModel::initRadiusValues()
{
    //these values are in meters
    equatorialRadius = 6378137.0;
    polarRadius = 6356752.3142;

    //if units are feet, update the values
    if(getUnits() == scene::FEET)
    {
        //do meters to feet
        equatorialRadius *= math::Constants::METERS_TO_FEET;
        polarRadius *= math::Constants::METERS_TO_FEET;
    }
}

void scene::WGS84EllipsoidModel::setEquatorialRadius(double val)
{
    //can't change the value - do some error
    throw except::Exception(Ctxt("Tried to change the equatorial radius - not supported"));
}

void scene::WGS84EllipsoidModel::setPolarRadius(double val)
{
    throw except::Exception(Ctxt("Tried to change the polar radius - not supported"));

}

scene::EllipsoidModel&
scene::WGS84EllipsoidModel::operator=(const scene::EllipsoidModel & m)
{
    setUnits(m.getUnits());
    setAngularUnits(m.getAngularUnits());

    return *this;
}


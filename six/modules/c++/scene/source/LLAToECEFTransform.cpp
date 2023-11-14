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
#include "scene/LLAToECEFTransform.h"
#include <math/Utilities.h>

scene::LLAToECEFTransform::LLAToECEFTransform(const EllipsoidModel *initVals)
 : CoordinateTransform(initVals)
{
}

scene::LLAToECEFTransform* scene::LLAToECEFTransform::clone() const
{
    scene::LLAToECEFTransform *newTransform =
        new LLAToECEFTransform(this->getEllipsoidModel());
    return newTransform;
}

scene::Vector3 scene::LLAToECEFTransform::transform(const LatLonAlt& lla) const
{
    Vector3 ecef{};

    const LatLonAlt mylla = lla;

    if (std::abs(mylla.getLatRadians()) > M_PI/2
	|| std::abs(mylla.getLonRadians()) > M_PI)
    {
	//invalid lla coordinate
	std::ostringstream str;
	str <<  "Invalid lla coordinate: ";
	str << "lat=";
	str << lla.getLatRadians();
	str << ", lon=";
	str << lla.getLonRadians();
	str << ", alt=";
	str << lla.getAlt();

	throw except::InvalidFormatException(Ctxt(str));
    }

    //do conversion here; store result in ecef struct

    double r = computeRadius(mylla);
    const double flatLat = computeLatitude(mylla.getLatRadians());

    double sinlat, coslat;
    math::SinCos(mylla.getLatRadians(), sinlat, coslat);
    double sinlon, coslon;
    math::SinCos(mylla.getLonRadians(), sinlon, coslon);
    double sinflatlat, cosflatlat;
    math::SinCos(flatLat, sinflatlat, cosflatlat);

    ecef[0] = (r * cosflatlat * coslon) + (mylla.getAlt() * coslat * coslon);
    ecef[1] = (r * cosflatlat * sinlon) + (mylla.getAlt() * coslat * sinlon);
    ecef[2] = (r * sinflatlat) + (mylla.getAlt() * sinlat);

    return ecef;
}

double scene::LLAToECEFTransform::computeRadius(const LatLonAlt& lla) const
{
    const double f = model->calculateFlattening();

    const double flatLat = computeLatitude(lla.getLatRadians());

    double denominator = (1.0 / math::square(1.0 - f)) - 1.0;
    denominator *= math::square(sin(flatLat));
    denominator += 1.0;

    double flatRadius = model->getEquatorialRadius();
    flatRadius = math::square(flatRadius);
    flatRadius /= denominator;
    flatRadius = sqrt(flatRadius);

    return flatRadius;
}

double scene::LLAToECEFTransform::computeLatitude(const double lat) const
{
    const double f = model->calculateFlattening();

    double flatLat = math::square((1.0 - f));
    flatLat *= tan(lat);
    flatLat = atan(flatLat);

    return flatLat;
}


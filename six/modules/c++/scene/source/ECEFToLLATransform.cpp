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
#include "scene/ECEFToLLATransform.h"
#include <math/Utilities.h>

scene::ECEFToLLATransform::ECEFToLLATransform(const EllipsoidModel *initVals)
 : CoordinateTransform(initVals)
{
}

scene::ECEFToLLATransform* scene::ECEFToLLATransform::clone() const
{
    scene::ECEFToLLATransform *newTransform =
        new ECEFToLLATransform(this->getEllipsoidModel());
    return newTransform;
}

scene::LatLonAlt
scene::ECEFToLLATransform::transform(const Vector3& ecef) const
{
   LatLonAlt lla;

   const Vector3 myecef = ecef;

   //do conversion here; store result in lla struct

   double initLat = getInitialLatitude(myecef);
   double tempLat = computeLatitude(myecef, initLat);
   double threshold = 0.0;

   int idx = 0;
   do
   {
      if (idx++ > 4) break;

      lla.setLatRadians(tempLat);
      //recompute reducedLatitude
      initLat = computeReducedLatitude(tempLat);
      //recompute latitude
      tempLat = computeLatitude(myecef, initLat);

      threshold = lla.getLatRadians() - tempLat;
   }
   while (std::abs(threshold) > .00000000000000000001);

   lla.setLatRadians(tempLat);
   lla.setLonRadians(computeLongitude(myecef));
   lla.setAlt(computeAltitude(myecef, lla.getLatRadians()));

   return lla;
}

double scene::ECEFToLLATransform::computeLongitude(const Vector3& ecef)
{
    double longitude = 0;
    if (ecef[0] != 0)
    {
        longitude = ecef[1] / ecef[0];
    }
    longitude = atan(longitude);

    if (ecef[0] < 0 && ecef[1] < 0)
    {
        longitude -= M_PI;
    }
    else if (ecef[0] < 0 && ecef[1] > 0)
    {
        longitude += M_PI;
    }

    return longitude;
}

double scene::ECEFToLLATransform::computeAltitude(const Vector3& ecef,
                                                  double latitude) const
{
    const double f = model->calculateFlattening();
    const double e_squared = 1.0 - math::square((1.0 - f));
    double s = math::square(ecef[0]) + math::square(ecef[1]);
    s = sqrt(s);

    double sin_latitude, cos_latitude;
    math::SinCos(latitude, sin_latitude, cos_latitude);

    const double radius_curvature = model->getEquatorialRadius()
                              / (sqrt(1 - (e_squared *
                              math::square(sin_latitude))));

    double altitude = e_squared * radius_curvature * sin_latitude;
    altitude += ecef[2];
    altitude *= sin_latitude;
    altitude += s * cos_latitude;
    altitude -= radius_curvature;

    return altitude;
}

double
scene::ECEFToLLATransform::getInitialLatitude(const Vector3& ecef) const
{
    const double f = model->calculateFlattening();
    double s = math::square(ecef[0]) + math::square(ecef[1]);
    s = sqrt(s);

    double initLat = 0;
    if (s != 0)
    {
        initLat = ecef[2] / ((1.0 - f) * s);
        initLat = atan(initLat);
    }

    return initLat;
}

double
scene::ECEFToLLATransform::computeReducedLatitude(double latitude) const
{
    const double f = model->calculateFlattening();
    double sin_latitude, cos_latitude;
    math::SinCos(latitude, sin_latitude, cos_latitude);
    const double denominator = cos_latitude;
    double reducedLatitude = ((1.0 - f) * sin_latitude) / denominator;
    reducedLatitude = atan(reducedLatitude);

    return reducedLatitude;
}

double
scene::ECEFToLLATransform::computeLatitude(const Vector3& ecef,
                                           double reducedLatitude) const
{
    const double r = model->getEquatorialRadius();
    const double f = model->calculateFlattening();
    const double e_squared = 1.0 - math::square((1.0 - f));
    double s = math::square(ecef[0]) + math::square(ecef[1]);
    s = sqrt(s);

    double sin_reducedLatitude, cos_reducedLatitude;
    math::SinCos(reducedLatitude, sin_reducedLatitude, cos_reducedLatitude);

    double numerator;
    numerator = e_squared * (1.0 - f);
    numerator /= 1.0 - e_squared;
    numerator *= r * pow(sin_reducedLatitude, 3);
    numerator += ecef[2];

    const double denominator = s - (e_squared * r * pow(cos_reducedLatitude, 3));

    double latitude = 0;
    if (denominator != 0)
    {
        latitude = atan(numerator / denominator);
    }

    return latitude;
}

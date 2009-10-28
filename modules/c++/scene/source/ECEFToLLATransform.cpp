/* =========================================================================
 * This file is part of scene-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
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

scene::ECEFToLLATransform::ECEFToLLATransform()
 : CoordinateTransform()
{
}

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

scene::LatLonAlt scene::ECEFToLLATransform::transform(const Vector3& ecef)
{
   LatLonAlt lla;

   Vector3 myecef = ecef;

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
                                                  double latitude)
{
    double altitude;

    double f = model->calculateFlattening();
    double e_squared = 1.0 - pow((1.0 - f), 2);
    double s = pow(ecef[0], 2) + pow(ecef[1], 2);
    s = sqrt(s);

    double radius_curvature = model->getEquatorialRadius()
                              / (sqrt(1 - (e_squared * pow(sin(latitude), 2))));

    altitude = e_squared * radius_curvature * sin(latitude);
    altitude += ecef[2];
    altitude *= sin(latitude);
    altitude += s * cos(latitude);
    altitude -= radius_curvature;

    return altitude;
}

double scene::ECEFToLLATransform::getInitialLatitude(const Vector3& ecef)
{
    double initLat;

    double f = model->calculateFlattening();
    double s = pow(ecef[0], 2) + pow(ecef[1], 2);
    s = sqrt(s);

    if (s == 0)
    {
        initLat = 0;
    }
    else
    {
        initLat = ecef[2] / ((1.0 - f) * s);
        initLat = atan(initLat);
    }

    return initLat;
}

double scene::ECEFToLLATransform::computeReducedLatitude(double latitude)
{
    double f = model->calculateFlattening();
    double denominator = cos(latitude);
    double reducedLatitude = ((1.0 - f) * sin(latitude)) / denominator;
    reducedLatitude = atan(reducedLatitude);

    return reducedLatitude;
}

double scene::ECEFToLLATransform::computeLatitude(const Vector3& ecef,
                                                  double reducedLatitude)
{
    double latitude;

    double r = model->getEquatorialRadius();
    double f = model->calculateFlattening();
    double e_squared = 1.0 - pow((1.0 - f), 2);
    double s = pow(ecef[0], 2) + pow(ecef[1], 2);
    s = sqrt(s);

    double numerator;
    numerator = e_squared * (1.0 - f);
    numerator /= 1.0 - e_squared;
    numerator *= r * pow(sin(reducedLatitude), 3);
    numerator += ecef[2];

    double denominator;
    denominator = s - (e_squared * r * pow(cos(reducedLatitude), 3));

    latitude = 0;
    if (denominator != 0)
    {
        latitude = atan(numerator / denominator);
    }

    return latitude;
}


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
#include <scene/Utilities.h>

#include <string>

#include <scene/sys_Conf.h>
#include <except/Exception.h>
#include <str/Convert.h>

#include <scene/LLAToECEFTransform.h>
#include <scene/ECEFToLLATransform.h>
#include <scene/GridECEFTransform.h>

namespace scene
{
Vector3 Utilities::latLonToECEF(LatLonAlt latLon)
{
    scene::LLAToECEFTransform toECEF;
    return toECEF.transform(latLon);
}

Vector3 Utilities::latLonToECEF(LatLon latLon)
{
    const scene::LatLonAlt lla(latLon.getLat(), latLon.getLon());
    return latLonToECEF(lla);
}

LatLonAlt Utilities::ecefToLatLon(Vector3 vec)
{
    scene::ECEFToLLATransform toLLA;
    return toLLA.transform(vec);
}

LatLonAlt Utilities::ecefToLatLon(const GeographicGridECEFTransform& gridTransform, size_t row, size_t col)
{
    return ecefToLatLon(
        gridTransform.rowColToECEF(static_cast<double>(row), static_cast<double>(col)));
}

double Utilities::remapZeroTo360(double degree)
{
    double delta = degree;
    while (delta < 0.)
    {
        delta += 360.;
        if (degree == delta)
        {
            throw except::Exception(Ctxt(
                "Value [" + std::to_string(degree) +
                "] is too small to remap into the [0:360] range"));
        }
    }
    while (delta > 360.)
    {
        delta -= 360.;
        if (degree == delta)
        {
            throw except::Exception(Ctxt(
                "Value [" + std::to_string(degree) +
                "] is too large to remap into the [0:360] range"));
        }
    }
    return delta;
}

double Utilities::remapMinus180To180(double degree)
{
    double outDegree = Utilities::remapZeroTo360(degree);
    if (outDegree > 180) {
        return outDegree - 360;
    }
    return outDegree;
}

}

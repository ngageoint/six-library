/* =========================================================================
 * This file is part of six-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
 *
 * six-c++ is free software; you can redistribute it and/or modify
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
#include "six/sidd/Measurement.h"

using namespace six;
using namespace six::sidd;

Measurement::Measurement(ProjectionType projectionType) :
    projection(NULL)
{
    pixelFootprint = Init::undefined<RowColInt>();
    if (projectionType == ProjectionType::PROJECTION_GEOGRAPHIC)
    {
        projection = new GeographicProjection();
    }
    else if (projectionType == ProjectionType::PROJECTION_CYLINDRICAL)
    {
        projection = new CylindricalProjection();
    }
    else if (projectionType == ProjectionType::PROJECTION_PLANE)
    {
        projection = new PlaneProjection();
    }
    else if (projectionType == ProjectionType::PROJECTION_POLYNOMIAL)
    {
        projection = new PolynomialProjection();
    }
}

Measurement* Measurement::clone()
{
    Measurement* m = new Measurement(ProjectionType::PROJECTION_NOT_SET);
    if (projection)
        m->projection = projection->clone();
    m->pixelFootprint = pixelFootprint;
    m->arpPoly = arpPoly;
    return m;
}

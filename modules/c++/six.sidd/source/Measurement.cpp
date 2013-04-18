/* =========================================================================
 * This file is part of six.sidd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2013, General Dynamics - Advanced Information Systems
 *
 * six.sidd-c++ is free software; you can redistribute it and/or modify
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

    if (projectionType == ProjectionType::GEOGRAPHIC)
        projection.reset(new GeographicProjection());

    else if (projectionType == ProjectionType::CYLINDRICAL)
        projection.reset(new CylindricalProjection());

    else if (projectionType == ProjectionType::PLANE)
        projection.reset(new PlaneProjection());

    else if (projectionType == ProjectionType::POLYNOMIAL)
        projection.reset(new PolynomialProjection());
}

Measurement* Measurement::clone()
{
    return new Measurement(*this);
}


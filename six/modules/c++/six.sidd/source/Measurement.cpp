/* =========================================================================
 * This file is part of six.sidd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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
#include <six/sidd/Measurement.h>

namespace six
{
namespace sidd
{
Measurement::Measurement(ProjectionType projectionType) :
    pixelFootprint(Init::undefined<RowColInt>())
{
    switch (projectionType)
    {
    case ProjectionType::GEOGRAPHIC:
        projection.reset(new GeographicProjection());
        break;
    case ProjectionType::CYLINDRICAL:
        projection.reset(new CylindricalProjection());
        break;
    case ProjectionType::PLANE:
        projection.reset(new PlaneProjection());
        break;
    case ProjectionType::POLYNOMIAL:
        projection.reset(new PolynomialProjection());
        break;
    default:
        // TODO: Should we throw or is it valid they wouldn't know this
        //       at construction time?
        break;
    }
}
}
}

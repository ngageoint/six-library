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
bool PolynomialProjection::equalTo(const Projection& rhs) const
{
    PolynomialProjection const* projection = dynamic_cast<PolynomialProjection const*>(&rhs);
    if (projection != nullptr)
    {
        this->operator_eq(*projection);
    }
    return false;
}
bool PolynomialProjection::operator_eq(const PolynomialProjection& rhs) const
{
    return (rowColToLat == rhs.rowColToLat &&
        rowColToLon == rhs.rowColToLon &&
        rowColToAlt == rhs.rowColToAlt &&
        latLonToRow == rhs.latLonToRow &&
        latLonToCol == rhs.latLonToCol &&
        projectionType == rhs.projectionType &&
        referencePoint == rhs.referencePoint);
}

bool MeasurableProjection::operator_eq(const MeasurableProjection& rhs) const
{
    return (sampleSpacing == rhs.sampleSpacing &&
        timeCOAPoly == rhs.timeCOAPoly &&
        projectionType == rhs.projectionType &&
        referencePoint == rhs.referencePoint);
}
bool MeasurableProjection::equalTo(const Projection& rhs) const
{
    auto projection = dynamic_cast<const MeasurableProjection*>(&rhs);
    if (projection != nullptr)
    {
        return this->operator_eq(*projection);
    }
    return false;
}

bool GeographicProjection::operator_eq(const GeographicProjection& rhs) const
{
    return this->projectionType == rhs.projectionType;
}
bool GeographicProjection::equalTo(const Projection& rhs) const
{
    auto projection = dynamic_cast<const GeographicProjection*>(&rhs);
    if (projection != nullptr)
    {
        return this->operator_eq(*projection);
    }
    return false;
}

bool CylindricalProjection::operator_eq(const CylindricalProjection& rhs) const
{
    return (stripmapDirection == rhs.stripmapDirection &&
        curvatureRadius == rhs.curvatureRadius &&
        MeasurableProjection::operator_eq(rhs));
}
bool CylindricalProjection::equalTo(const Projection& rhs) const
{
    auto projection = dynamic_cast<const CylindricalProjection*>(&rhs);
    if (projection != nullptr)
    {
        return this->operator_eq(*projection);
    }
    return false;
}

bool PlaneProjection::operator_eq(const PlaneProjection& rhs) const
{
    return MeasurableProjection::operator_eq(rhs);
}
bool PlaneProjection::equalTo(const Projection& rhs) const
{
    auto projection = dynamic_cast<const PlaneProjection*>(&rhs);
    if (projection != nullptr)
    {
        return this->operator_eq(*projection);
    }
    return false;
}

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

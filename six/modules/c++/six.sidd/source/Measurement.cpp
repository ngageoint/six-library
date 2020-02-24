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
    if (projection != NULL)
    {
        return (rowColToLat == projection->rowColToLat &&
            rowColToLon == projection->rowColToLon &&
            rowColToAlt == projection->rowColToAlt &&
            latLonToRow == projection->latLonToRow &&
            latLonToCol == projection->latLonToCol &&
            projectionType == projection->projectionType &&
            referencePoint == projection->referencePoint);
    }
    return false;
}

bool MeasurableProjection::operator==(const MeasurableProjection& rhs) const
{
    return (sampleSpacing == rhs.sampleSpacing &&
        timeCOAPoly == rhs.timeCOAPoly &&
        projectionType == rhs.projectionType &&
        referencePoint == rhs.referencePoint);
}

bool MeasurableProjection::equalTo(const Projection& rhs) const
{
    const MeasurableProjection* projection = dynamic_cast<const MeasurableProjection*>(&rhs);
    if (projection != NULL)
    {
        return *this == *projection;
    }
    return false;
}

bool GeographicProjection::equalTo(const Projection& rhs) const
{
    const GeographicProjection* projection = dynamic_cast<const GeographicProjection*>(&rhs);
    if (projection != NULL)
    {
        return *this == *projection;
    }
    return false;
}

bool CylindricalProjection::operator==(const CylindricalProjection& rhs) const
{
    return (stripmapDirection == rhs.stripmapDirection &&
        curvatureRadius == rhs.curvatureRadius &&
        MeasurableProjection::operator==(rhs));
}

bool CylindricalProjection::equalTo(const Projection& rhs) const
{
    const CylindricalProjection* projection = dynamic_cast<const CylindricalProjection*>(&rhs);
    if (projection != NULL)
    {
        return *this == *projection;
    }
    return false;
}

bool PlaneProjection::operator==(const PlaneProjection& rhs) const
{
    return *(dynamic_cast<const MeasurableProjection*>(this)) == *(dynamic_cast<const MeasurableProjection*>(&rhs));
}

bool PlaneProjection::equalTo(const Projection& rhs) const
{
    const PlaneProjection* projection = dynamic_cast<const PlaneProjection*>(&rhs);
    if (projection != NULL)
    {
        return *this == *projection;
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

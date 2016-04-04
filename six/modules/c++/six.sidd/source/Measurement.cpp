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
#include "six/sidd/Measurement.h"

using namespace six;
using namespace six::sidd;

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

bool MeasurableProjection::equalTo(const Projection& rhs) const
{
    MeasurableProjection const* projection = dynamic_cast<MeasurableProjection const*>(&rhs);
    if (projection != NULL)
    {
        return (sampleSpacing == projection->sampleSpacing &&
            timeCOAPoly == projection->timeCOAPoly &&
            projectionType == projection->projectionType &&
            referencePoint == projection->referencePoint);
    }
    return false;
}

bool GeographicProjection::equalTo(const Projection& rhs) const
{
    GeographicProjection const* projection = dynamic_cast<GeographicProjection const*>(&rhs);
    if (projection != NULL)
    {
        return *(dynamic_cast<MeasurableProjection const*>(this)) == *(dynamic_cast<MeasurableProjection const*>(projection));
    }
    return false;
}

bool CylindricalProjection::equalTo(const Projection& rhs) const
{
    CylindricalProjection const* projection = dynamic_cast<CylindricalProjection const*>(&rhs);
    if (projection != NULL)
    {
        return (stripmapDirection == projection->stripmapDirection && 
            curvatureRadius == projection->curvatureRadius &&
            *(dynamic_cast<MeasurableProjection const*>(this)) == *(dynamic_cast<MeasurableProjection const*>(projection)));
    }
    return false;
}

bool PlaneProjection::equalTo(const Projection& rhs) const
{
    PlaneProjection const* projection = dynamic_cast<PlaneProjection const*>(&rhs);
    if (projection != NULL)
    {
        return *(dynamic_cast<MeasurableProjection const*>(this)) == *(dynamic_cast<MeasurableProjection const*>(projection));
    }
    return false;
}

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


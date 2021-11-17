/* =========================================================================
 * This file is part of six.sicd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2017, MDA Information Systems LLC
 *
 * six.sicd-c++ is free software; you can redistribute it and/or modify
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

#include <six/sicd/AreaPlaneUtility.h>
#include <six/sicd/GeoLocator.h>

namespace six
{
namespace sicd
{
GeoLocator::GeoLocator(const ComplexData& complexData, bool shadowsDown):
    mEcefToLla(/*needed by ICC*/), mRowColToEcef(buildTransformer(complexData, shadowsDown))
{
}

LatLonAlt GeoLocator::geolocate(const RowColDouble& rowCol) const
{
    return mEcefToLla.transform(mRowColToEcef.rowColToECEF(rowCol));
}

scene::PlanarGridECEFTransform
GeoLocator::buildTransformer(const ComplexData& complexData, bool shadowsDown) const
{
    std::unique_ptr<ComplexData> dataClone(
        static_cast<ComplexData*>(complexData.clone()));
    if (!AreaPlaneUtility::hasAreaPlane(*dataClone))
    {
        AreaPlaneUtility::setAreaPlane(*dataClone);
    }
    AreaPlane& plane = *dataClone->radarCollection->area->plane;
    if (shadowsDown)
    {
        plane.rotateToShadowsDown();
    }
    const RowColDouble spacing(plane.xDirection->spacing,
                               plane.yDirection->spacing);
    return scene::PlanarGridECEFTransform(spacing,
            plane.getAdjustedReferencePoint(),
            plane.xDirection->unitVector,
            plane.yDirection->unitVector,
            plane.referencePoint.ecef);
}
}
}

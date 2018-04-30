/* =========================================================================
 * This file is part of six.sicd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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

#include <memory>
#include <algorithm>

#include <sys/Conf.h>
#include <except/Exception.h>
#include <str/Convert.h>
#include <mem/ScopedArray.h>
#include <six/sicd/SlantPlanePixelTransformer.h>

namespace six
{
namespace sicd
{

SlantPlanePixelTransformer::SlantPlanePixelTransformer(
    const six::sicd::ComplexData& data,
    const scene::SceneGeometry& geom,
    const scene::ProjectionModel& projection) :
    mGeom(geom),
    mProjection(projection),
    mSicdData(*reinterpret_cast<six::sicd::ComplexData*>(data.clone())),
    mGroundPlaneNormal(mGeom.getReferencePosition())
{
    mGroundPlaneNormal.normalize();
}

scene::Vector3 SlantPlanePixelTransformer::toECEF(
    const types::RowCol<double>& pixel) const
{
    //! convert slant pixel to meters from scene center
    const types::RowCol<double> imagePt(mSicdData.pixelToImagePoint(pixel));

    //! project into ground plane -- ecef coords
    double timeCOA(0.0);
    return mProjection.imageToScene(imagePt,
                                    mGeom.getReferencePosition(),
                                    mGroundPlaneNormal,
                                    &timeCOA);
}

scene::LatLonAlt SlantPlanePixelTransformer::toLLA(
    const types::RowCol<double>& pixel) const
{
    //! project then convert ECEF to LLA
    return scene::Utilities::ecefToLatLon(toECEF(pixel));
}

scene::LatLon SlantPlanePixelTransformer::toLatLon(
    const types::RowCol<double>& pixel) const
{
    //! project then convert LLA to LatLon
    scene::LatLonAlt lla = toLLA(pixel);
    return scene::LatLon(lla.getLat(), lla.getLon());
}

}
}

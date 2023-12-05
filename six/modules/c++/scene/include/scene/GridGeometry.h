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
#pragma once
#ifndef __SCENE_GRID_GEOMETRY_H__
#define __SCENE_GRID_GEOMETRY_H__

#include "scene/Types.h"
#include "scene/Exports.h"

#include <math/poly/OneD.h>
#include <math/poly/TwoD.h>

namespace scene
{
class SIX_SCENE_API GridGeometry
{
public:
    enum { MAX_ITER = 50 };

    virtual ~GridGeometry();

    /**
     *  Project a point from the ground to the geometric grid.
     *
     * \param groundPt    A point lying in the scene, defined in ECEF coordinates.
     */
    virtual Vector3 gridToScene(const Vector3& gridPt,
                                double height) const = 0;

    /**
     *  Project a point from the ground to the geometric grid.
     *
     * \param groundPt    A point lying in the scene, defined in ECEF coordinates.
     */
    virtual Vector3 sceneToGrid(const Vector3& groundPt) const = 0;
};


class SIX_SCENE_API PlanarGridGeometry : public GridGeometry
{
public:
    PlanarGridGeometry(const Vector3& row,
                       const Vector3& col,
                       const Vector3& refPt,
                       const math::poly::OneD<Vector3>& arpPoly,
                       const math::poly::TwoD<double>& timeCOAPoly);

    /**
     *  Project a point from the geometric grid to the ground, using
     *  range-doppler projection.  Both the input grid point and the
     *  output scene point are in ECEF coordinates.  Height refers to the
     *  desired altitude of the scene point above the earth ellipsoid model.
     *
     * \param gridPt    A point lying on the geometric grid, defined in ECEF coordinates.
     * \param height    The height of the desired ground point in the scene
     */
    virtual Vector3 gridToScene(const Vector3& gridPt,
                                double height) const;

    /**
     *  This function calls the protected version of sceneToGrid
     *  iteratively, beginning with the center of aperture and adjusting
     *  the time value until the algorithm converges on a point in the
     *  geometric grid.
     *
     * \param groundPt    A point lying in the scene, defined in ECEF coordinates.
     */
    virtual Vector3 sceneToGrid(const Vector3& groundPt) const;

private:
    /**
     *  Project a point from the ground to the geometric grid, using
     *  range-doppler projection at the given time.  Both the input scene
     *  point and the output grid point are in ECEF coordinates.
     *
     * \param groundPt    A point lying in the scene, defined in ECEF coordinates.
     * \param time        The collection time to use when projecting the ground point into the geometric grid.
     */
    Vector3 sceneToGrid(const Vector3& groundPt, double time) const;

private:
    Vector3 mRow;
    Vector3 mCol;
    Vector3 mRefPt;
    math::poly::OneD<Vector3> mARPPoly;
    math::poly::OneD<Vector3> mARPVelPoly;
    math::poly::TwoD<double> mTimeCOAPoly;
};
}

#endif

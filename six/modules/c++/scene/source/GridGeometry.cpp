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
#include <scene/GridGeometry.h>

#include <scene/sys_Conf.h>
#include <except/Exception.h>
#include <scene/Utilities.h>
#include <scene/LocalCoordinateTransform.h>

namespace scene
{
GridGeometry::~GridGeometry()
{
}

PlanarGridGeometry::PlanarGridGeometry(
        const Vector3& row,
        const Vector3& col,
        const Vector3& refPt,
        const math::poly::OneD<Vector3>& arpPoly,
        const math::poly::TwoD<double>& timeCOAPoly) :
    mRow(row),
    mCol(col),
    mRefPt(refPt),
    mARPPoly(arpPoly),
    mARPVelPoly(mARPPoly.derivative()),
    mTimeCOAPoly(timeCOAPoly)
{
}

scene::Vector3
scene::PlanarGridGeometry::gridToScene(const scene::Vector3& gridPt,
                                       double height) const
{
    scene::Vector3 p = gridPt;
    scene::LatLonAlt lla = scene::Utilities::ecefToLatLon(p);

    // Compute up vector at output plane point
    scene::ENUCoordinateTransform enu(lla);
    scene::Vector3 up = enu.getUnitVectorZ();

    // arpPos to output plane point
    scene::Vector3 disp(p - mRefPt);
    const types::RowCol<double> rgAz(disp.dot(mRow), disp.dot(mCol));
    const double time = mTimeCOAPoly(rgAz.row, rgAz.col);
    Vector3 arpPos = mARPPoly(time);
    Vector3 arpVel = mARPVelPoly(time);
    scene::Vector3 arpPosToPop = gridPt - arpPos;

    scene::Vector3 deltaP;
    scene::Vector3 pFd;
    scene::Vector3 lineOfSight;
    math::linear::MatrixMxN<3, 1> rHat;
    math::linear::MatrixMxN<3, 1> f;
    math::linear::MatrixMxN<3, 3> b;
    math::linear::MatrixMxN<3, 3> i3 = 
            math::linear::identityMatrix<3, double>();

    // Iteratively solve for deltaP until we converge on a point in the scene 
    // with the specified height.  F is a vector of condition functions on the 
    // range, doppler, and height of the point, and B is a matrix containing 
    // the partial derivatives of F.  We are done when the magnitude of 
    // deltaP is close to zero.
    for (size_t i = 0; i < MAX_ITER; ++i)
    {
        lineOfSight = p - arpPos; // arpPos to p

        double rMag = lineOfSight.norm();
        rHat = lineOfSight.matrix() * -1.0 / rMag;

        // Fr, Fd, and Ft
        f(0, 0) = rMag - arpPosToPop.norm();
        f(1, 0) = arpVel.dot(lineOfSight) / rMag -
                arpVel.dot(arpPosToPop) / arpPosToPop.norm();
        f(2, 0) = lla.getAlt() - height; // Use the altitude of the current p

        // B[0] = dFr/dP
        b(0, 0) = -rHat(0, 0);
        b(0, 1) = -rHat(1, 0);
        b(0, 2) = -rHat(2, 0);

        // B[1] = dFd/dP
        pFd = ((i3 - (rHat * rHat.transpose())) * arpVel.matrix()) / rMag;
        b(1, 0) = pFd[0];
        b(1, 1) = pFd[1];
        b(1, 2) = pFd[2];

        // B[2] = dFt/dP
        b(2, 0) = up[0];
        b(2, 1) = up[1];
        b(2, 2) = up[2];

        deltaP = math::linear::inverse<3, double>(b) * (-1.0 * f);

        // Adjust p by the deltaP offsets for the next iteration,
        // and update the lla object
        p += deltaP;
        lla = scene::Utilities::ecefToLatLon(p);
        enu = scene::ENUCoordinateTransform(lla);
        up = enu.getUnitVectorZ();

        if (deltaP.norm() < 0.00001)
            return p;
    }

    throw except::Exception(Ctxt("Point failed to converge"));
}


scene::Vector3
scene::PlanarGridGeometry::sceneToGrid(
        const scene::Vector3& groundPt, double time) const
{
    scene::Vector3 p = groundPt;

    // arpPos to ground point
    Vector3 arpPos = mARPPoly(time);
    Vector3 arpVel = mARPVelPoly(time);
    scene::Vector3 arpPosToPg = groundPt - arpPos;

    // Output plane normal
    Vector3 zHat = math::linear::cross(mRow, mCol);
    zHat.normalize();

    scene::Vector3 deltaP;
    scene::Vector3 pFd;
    scene::Vector3 lineOfSight;
    math::linear::MatrixMxN<3, 1> rHat;
    math::linear::MatrixMxN<3, 1> f;
    math::linear::MatrixMxN<3, 3> b;
    math::linear::MatrixMxN<3, 3> i3 = 
            math::linear::identityMatrix<3, double>();

    // Iteratively solve for deltaP until we converge on a point that lies
    // in the planar grid.  F is a vector of condition functions on the 
    // range, doppler, and height of the point, and B is a matrix containing 
    // the partial derivatives of F.  We are done when the magnitude of 
    // deltaP is close to zero.
    for (size_t i = 0; i < MAX_ITER; ++i)
    {
        lineOfSight = p - arpPos; // arpPos to p

        double rMag = lineOfSight.norm();
        rHat = lineOfSight.matrix() * -1.0 / rMag;

        // Fr, Fd, and Ft
        f(0, 0) = rMag - arpPosToPg.norm();
        f(1, 0) = arpVel.dot(lineOfSight) / rMag -
                arpVel.dot(arpPosToPg) / arpPosToPg.norm();
        f(2, 0) = zHat.dot(p - mRefPt);

        // B[0] = dFr/dP
        b(0, 0) = -rHat(0, 0);
        b(0, 1) = -rHat(1, 0);
        b(0, 2) = -rHat(2, 0);

        // B[1] = dFd/dP
        pFd = ((i3 - (rHat * rHat.transpose())) * arpVel.matrix()) / rMag;
        b(1, 0) = pFd[0];
        b(1, 1) = pFd[1];
        b(1, 2) = pFd[2];

        // B[2] = dFt/dP
        b(2, 0) = zHat[0];
        b(2, 1) = zHat[1];
        b(2, 2) = zHat[2];

        deltaP = math::linear::inverse<3, double>(b) * (-1.0 * f);

        // Adjust p by the deltaP offsets for the next iteration
        p += deltaP;

        if (deltaP.norm() < 0.00001)
            return p;
    }

    throw except::Exception(Ctxt("Point failed to converge"));
}

scene::Vector3
scene::PlanarGridGeometry::sceneToGrid(const scene::Vector3& groundPt) const
{
    Vector3 currentGroundPt(groundPt);

    // Get the height of the input scene point
    const scene::LatLonAlt lla = scene::Utilities::ecefToLatLon(currentGroundPt);
    const double height = lla.getAlt();

    // Set initial time to center of aperture
    double time = mTimeCOAPoly(0, 0);
    Vector3 currentGridPt = sceneToGrid(currentGroundPt, time);
    for (size_t i = 0; i < MAX_ITER; ++i)
    {
        // Project currentGridPt back into the scene and see how close we are
        // to the input scene point.
        Vector3 newGroundPt = gridToScene(currentGridPt, height);
        Vector3 deltaP = groundPt - newGroundPt;

        // If the new point is close enough to the input scene point, we're done
        if (deltaP.norm() < 0.00001)
            return currentGridPt;

        // Otherwise add the delta to the point and try again
        currentGroundPt += (groundPt - newGroundPt);

        // Using the current 'guessed' time, project the scene point 
        // into the grid to get a new grid point
        currentGridPt = sceneToGrid(currentGroundPt, time);
        Vector3 outputPlaneDisp = currentGridPt - mRefPt;

        const types::RowCol<double> rgAz(outputPlaneDisp.dot(mRow),
                                   outputPlaneDisp.dot(mCol));

        // Find the new time value associated with the currentGridPt
        time = mTimeCOAPoly(rgAz.row, rgAz.col);
    }

    throw except::Exception(Ctxt("Point failed to converge"));
}
}

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
#ifndef __SCENE_LOCAL_COORDINATE_TRANSFORM_H__
#define __SCENE_LOCAL_COORDINATE_TRANSFORM_H__

#include <math/Utilities.h>

#include "scene/Types.h"

namespace scene
{


class LocalCoordinateTransform
{
public:
    LocalCoordinateTransform(LatLonAlt& refPt) : mReferencePoint(refPt) 
    {
        mBasis = math::linear::constantMatrix<3, 3, double>(0.0);
    }

    virtual ~LocalCoordinateTransform() {}

    virtual Vector3 
            toECEFPoint(Vector3 localPoint);
    virtual Vector3 
            toLocalPoint(Vector3 ecefPoint);

    virtual math::linear::MatrixMxN<3, 3> getTransformMatrix();
    virtual Vector3 getUnitVectorX();//R
    virtual Vector3 getUnitVectorY();//C
    virtual Vector3 getUnitVectorZ();//RxC
    virtual void rotate90();

protected:

    math::linear::MatrixMxN<3, 3> mBasis;
    LatLonAlt mReferencePoint;
};

class ENUCoordinateTransform : public LocalCoordinateTransform
{
public:
    ENUCoordinateTransform(LatLonAlt& refPt) : LocalCoordinateTransform(refPt)
    {
        const double lon = refPt.getLonRadians();
        const double lat = refPt.getLatRadians();
        double sinLon, cosLon;
        math::SinCos(lon, sinLon, cosLon);
        double sinLat, cosLat;
        math::SinCos(lat, sinLat, cosLat);

        // East
        mBasis(XYZ_X, 0) = -sinLon;
        mBasis(XYZ_X, 1) = cosLon;
        mBasis(XYZ_X, 2) = 0.0;

        // North
        mBasis(XYZ_Y, 0) = -sinLat * cosLon;
        mBasis(XYZ_Y, 1) = -sinLat * sinLon;
        mBasis(XYZ_Y, 2) = cosLat;

        // Up
        mBasis(XYZ_Z, 0) = cosLat * cosLon;
        mBasis(XYZ_Z, 1) = cosLat * sinLon;
        mBasis(XYZ_Z, 2) = sinLat;
    }

    virtual ~ENUCoordinateTransform() {}
};

class NEDCoordinateTransform : public LocalCoordinateTransform
{
public:
    NEDCoordinateTransform(LatLonAlt& refPt) : LocalCoordinateTransform(refPt)
    {
        const double lon = refPt.getLonRadians();
        const double lat = refPt.getLatRadians();
        double sinLon, cosLon;
        math::SinCos(lon, sinLon, cosLon);
        double sinLat, cosLat;
        math::SinCos(lat, sinLat, cosLat);

        // North
        mBasis(XYZ_X, 0) = -sinLat * cosLon;
        mBasis(XYZ_X, 1) = -sinLat * sinLon;
        mBasis(XYZ_X, 2) = cosLat;

        // East
        mBasis(XYZ_Y, 0) = -sinLon;
        mBasis(XYZ_Y, 1) = cosLon;
        mBasis(XYZ_Y, 2) = 0.0;

        // Down
        mBasis(XYZ_Z, 0) = -cosLat * cosLon;
        mBasis(XYZ_Z, 1) = -cosLat * sinLon;
        mBasis(XYZ_Z, 2) = -sinLat;
    }

    virtual ~NEDCoordinateTransform() {}
};

}

#endif


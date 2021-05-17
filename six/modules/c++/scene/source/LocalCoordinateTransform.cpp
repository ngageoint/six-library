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
#include "scene/LocalCoordinateTransform.h"
#include "scene/LLAToECEFTransform.h"

using namespace scene;

Vector3 scene::LocalCoordinateTransform::toECEFPoint(Vector3 localPoint)
{
    scene::LLAToECEFTransform converter;
    const Vector3 ecefRefPoint = converter.transform(mReferencePoint);
    return (mBasis.transpose() * localPoint) + ecefRefPoint;
}

Vector3 scene::LocalCoordinateTransform::toLocalPoint(Vector3 ecefPoint)
{
    scene::LLAToECEFTransform converter;
    const Vector3 ecefRefPoint = converter.transform(mReferencePoint);
    return mBasis * (ecefPoint - ecefRefPoint);
}

math::linear::MatrixMxN<3, 3> 
scene::LocalCoordinateTransform::getTransformMatrix()
{
    return mBasis;
}

Vector3 scene::LocalCoordinateTransform::getUnitVectorX()
{
    Vector3 v;
    v[0] = mBasis(XYZ_X, 0);
    v[1] = mBasis(XYZ_X, 1);
    v[2] = mBasis(XYZ_X, 2);

    return v;
}

Vector3 scene::LocalCoordinateTransform::getUnitVectorY()
{
    Vector3 v;
    v[0] = mBasis(XYZ_Y, 0);
    v[1] = mBasis(XYZ_Y, 1);
    v[2] = mBasis(XYZ_Y, 2);

    return v;
}

Vector3 scene::LocalCoordinateTransform::getUnitVectorZ()
{
    Vector3 v;
    v[0] = mBasis(XYZ_Z, 0);
    v[1] = mBasis(XYZ_Z, 1);
    v[2] = mBasis(XYZ_Z, 2);

    return v;
}

void scene::LocalCoordinateTransform::rotate90()
{
    //set x = -y, y = x

    Vector3 tmpX;
    tmpX[0] = -mBasis(XYZ_Y, 0);
    tmpX[1] = -mBasis(XYZ_Y, 1);
    tmpX[2] = -mBasis(XYZ_Y, 2);

    Vector3 tmpY;
    tmpY[0] = mBasis(XYZ_X, 0);
    tmpY[1] = mBasis(XYZ_X, 1);
    tmpY[2] = mBasis(XYZ_X, 2);

    //x = -y
    mBasis(XYZ_X, 0) = tmpX[0];
    mBasis(XYZ_X, 1) = tmpX[1];
    mBasis(XYZ_X, 2) = tmpX[2];

    //y = x
    mBasis(XYZ_Y, 0) = tmpY[0];
    mBasis(XYZ_Y, 1) = tmpY[1];
    mBasis(XYZ_Y, 2) = tmpY[2];

    //z = x cross y
    Vector3 tmpZ = math::linear::cross(tmpX, tmpY);

    mBasis(XYZ_Z, 0) = tmpZ[0];
    mBasis(XYZ_Z, 1) = tmpZ[1];
    mBasis(XYZ_Z, 2) = tmpZ[2];
}



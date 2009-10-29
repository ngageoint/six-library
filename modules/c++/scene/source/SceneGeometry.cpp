/* =========================================================================
 * This file is part of scene-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
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
#include "scene/SceneGeometry.h"
#include "scene/ECEFToLLATransform.h"

using namespace scene;

scene::SceneGeometry::SceneGeometry(Vector3 arpVel, 
                                    Vector3 arpPos, 
                                    Vector3 refPos,
                                    Vector3* row,
                                    Vector3* col)
    : mVa(arpVel), mPa(arpPos), mPo(refPos), mSideOfTrack(1),
      mR(row), mC(col)
{
    // Compute slant plane vectors
    mXs = mPa - mPo;
    mXs.normalize();


    mZs = math::linear::cross(mXs, mVa);
    mZs.normalize();

    // Figure out if we are pointing up or down
    if (mZs.dot(mPo) < 0)
    {
        mSideOfTrack = -1;
    }
    mZs *= mSideOfTrack;

    mYs = math::linear::cross(mZs, mXs);

    
    // Transform mRefPosition to LLA; compute 'up'
    LatLonAlt lla = Utilities::ecefToLatLon(mPo);

    double sinLat = sin(lla.getLatRadians());
    double cosLat = cos(lla.getLatRadians());
    double sinLon = sin(lla.getLonRadians());
    double cosLon = cos(lla.getLonRadians());

    // mZg is also up
    mZg[0] = cosLat * cosLon;
    mZg[1] = cosLat * sinLon;
    mZg[2] = sinLat;

    mNorth[0] = -sinLat * cosLon;
    mNorth[1] = -sinLat * sinLon;
    mNorth[2] = cosLat;

    // Calculate ground range
    mRg = mXs - mZg * (mXs.dot(mZg));
    mRg *= -1;

    // Calculate ground track
    mVg = mVa - mZg * (mVa.dot(mZg));



}

void scene::SceneGeometry::setImageVectors(Vector3* row, Vector3* col)
{
    mR = row;
    mC = col;
}

Vector3 scene::SceneGeometry::getRowVector() const
{
    if (! mR )
        throw except::Exception(Ctxt("The Row vector is not set!"));
    return *mR;
}

Vector3 scene::SceneGeometry::getColVector() const
{
    if (! mC )
        throw except::Exception(Ctxt("The Column vector is not set!"));

    return *mC;
}

SideOfTrack scene::SceneGeometry::getSideOfTrack() const 
{ 
    return (SideOfTrack)mSideOfTrack; 
}

double scene::SceneGeometry::getImageAngle(Vector3 vec) const
{
    if (mR == NULL || mC == NULL)
    {
        throw except::Exception("Row and Column vectors must be initialized");
    }

    return atan2(mC->dot(vec),
                 mR->dot(vec)) * RADIANS_TO_DEGREES;
}

double scene::SceneGeometry::getGrazingAngle() const
{
    return asin(mXs.dot(mZg)) * RADIANS_TO_DEGREES;
}

double scene::SceneGeometry::getTiltAngle() const
{
    return atan2(mZg.dot(mYs), mZg.dot(mZs)) * RADIANS_TO_DEGREES;
}

double scene::SceneGeometry::getDopplerConeAngle() const
{
    return acos((-1.0 * mXs).dot(mVa)) * RADIANS_TO_DEGREES;
}

double scene::SceneGeometry::getSquintAngle() const
{
    Vector3 z = mPa / mPa.norm();
    Vector3 x = mVa - z * (mVa.dot(z));
    x.normalize();

    Vector3 y = math::linear::cross(x, z);
    return atan2((-1.0 * mXs).dot(y), (-1.0 * mXs).dot(x)) * RADIANS_TO_DEGREES;
}

double scene::SceneGeometry::getSlopeAngle() const
{
    return acos(mZs.dot(mZg)) * RADIANS_TO_DEGREES;
}


double scene::SceneGeometry::getAzimuthAngle() const
{
    Vector3 east = math::linear::cross(mNorth, mZg);
    return atan2(east.dot(mXs), mNorth.dot(mXs)) * RADIANS_TO_DEGREES;
}

double scene::SceneGeometry::getRotationAngle() const
{
    return -getImageAngle(mRg * -1);
}

Vector3 scene::SceneGeometry::getMultiPathVector() const
{
    double scale = mXs.dot(mZg) / mZs.dot(mZg);
    return mXs - (mZs * scale);
}

double scene::SceneGeometry::getMultiPathAngle() const
{
    return getImageAngle(getMultiPathVector());
}

Vector3 scene::SceneGeometry::getNorthVector() const
{
    if (mR == NULL || mC == NULL)
    {
        throw except::Exception("Row and Column vectors must be initialized");
    }

    Vector3 planeZ = math::linear::cross(*mR, *mC);
    double scale = mNorth.dot(planeZ) / mZs.dot(planeZ);
    return  mNorth - (mZs * scale);
}

double scene::SceneGeometry::getNorthAngle() const
{
    return getImageAngle(getNorthVector());
}

Vector3 scene::SceneGeometry::getLayoverVector() const
{
    if (mR == NULL || mC == NULL)
    {
        throw except::Exception("Row and Column vectors must be initialized");
    }

    Vector3 planeZ = math::linear::cross(*mR, *mC);
    return planeZ - (mZs / mZs.dot(planeZ));
}

AngleMagnitude scene::SceneGeometry::getLayover() const
{
    Vector3 layoverVec = getLayoverVector();

    AngleMagnitude layover;
    layover.angle = getImageAngle(layoverVec); 
    layover.magnitude = layoverVec.norm();

    return layover;
}

Vector3 scene::SceneGeometry::getShadowVector() const
{
    if (mR == NULL || mC == NULL)
    {
        throw except::Exception("Row and Column vectors must be initialized");
    }

    Vector3 planeZ = math::linear::cross(*mR, *mC);
    Vector3 groundShadow = mZg - (mXs / mXs.dot(mZg));
    double scale = groundShadow.dot(planeZ)/mZs.dot(planeZ);
    return groundShadow - mZs * scale;
}

AngleMagnitude scene::SceneGeometry::getShadow() const
{
    Vector3 shadowVec = getShadowVector();

    AngleMagnitude shadow;
    shadow.angle = getImageAngle(shadowVec); 
    shadow.magnitude = shadowVec.norm();
    return shadow;
}

void scene::SceneGeometry::getGroundResolution(double resRg,
                                               double resAz,
                                               double& resRow,
                                               double& resCol) const
{
    double grazingAngle = getGrazingAngle();
    double tiltAngle = getTiltAngle();
    double rotAngle = getRotationAngle();

    double cosRot = cos(rotAngle * DEGREES_TO_RADIANS);
    double sinRot = sin(rotAngle * DEGREES_TO_RADIANS);
    double sin2Rot = sin(2 * rotAngle * DEGREES_TO_RADIANS);
    double secGraz = 1.0 / cos(grazingAngle * DEGREES_TO_RADIANS);
    double tanGraz = tan(grazingAngle * DEGREES_TO_RADIANS);
    double secTilt = 1.0 / cos(tiltAngle * DEGREES_TO_RADIANS);
    double tanTilt = tan(tiltAngle * DEGREES_TO_RADIANS);

    double kr1 = (pow(sinRot, 2) * tanGraz * tanTilt - sin2Rot * secGraz)
                 * tanGraz* tanTilt + pow(cosRot * secGraz, 2);

    double kr2 = pow(sinRot * secTilt, 2);

    double kc1 = (pow(sinRot, 2) * secGraz - sin2Rot * tanGraz * tanTilt)
                 * secGraz + pow(cosRot * tanGraz * tanTilt, 2);

    double kc2 = pow(cosRot * secTilt, 2);

    resRow = sqrt(kr1 * pow(resAz, 2) + kr2 * pow(resRg, 2));
    resCol = sqrt(kc1 * pow(resAz, 2) + kc2 * pow(resRg, 2));
}


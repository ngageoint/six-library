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

namespace
{
inline
double square(double val)
{
    return (val * val);
}
}

namespace scene
{
SceneGeometry::SceneGeometry(const Vector3& arpVel,
                             const Vector3& arpPos,
                             const Vector3& refPos) :
    mVa(arpVel),
    mPa(arpPos),
    mPo(refPos)
{
    initialize();
}

SceneGeometry::SceneGeometry(const Vector3& arpVel,
                             const Vector3& arpPos,
                             const Vector3& refPos,
                             const Vector3& row,
                             const Vector3& col) :
    mVa(arpVel),
    mPa(arpPos),
    mPo(refPos),
    mR(new Vector3(row)),
    mC(new Vector3(col))
{
    initialize();
}

SceneGeometry::~SceneGeometry()
{
}

void SceneGeometry::initialize()
{
    // Compute slant plane vectors
    mXs = mPa - mPo;
    mXs.normalize();


    mZs = math::linear::cross(mXs, mVa);
    mZs.normalize();

    // Figure out if we are pointing up or down
    mSideOfTrack = (mZs.dot(mPo) < 0) ? -1 : 1;
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

void SceneGeometry::setImageVectors(const Vector3& row,
                                    const Vector3& col)
{
    mR.reset(new Vector3(row));
    mC.reset(new Vector3(col));
}

Vector3 SceneGeometry::getRowVector() const
{
    if (mR.get() == NULL)
    {
        throw except::Exception(Ctxt("The Row vector is not set!"));
    }
    return *mR;
}

Vector3 SceneGeometry::getColVector() const
{
    if (mC.get() == NULL)
    {
        throw except::Exception(Ctxt("The Column vector is not set!"));
    }

    return *mC;
}

SideOfTrack SceneGeometry::getSideOfTrack() const
{ 
    return (SideOfTrack)mSideOfTrack; 
}

double SceneGeometry::getImageAngle(Vector3 vec) const
{
    return atan2(getColVector().dot(vec),
                 getRowVector().dot(vec)) * RADIANS_TO_DEGREES;
}

double SceneGeometry::getGrazingAngle() const
{
    return asin(mXs.dot(mZg)) * RADIANS_TO_DEGREES;
}

double SceneGeometry::getTiltAngle() const
{
    return atan2(mZg.dot(mYs), mZg.dot(mZs)) * RADIANS_TO_DEGREES;
}

double SceneGeometry::getDopplerConeAngle() const
{
    Vector3 normVa = mVa / mVa.norm();
    return acos((-1.0 * mXs).dot(normVa)) * RADIANS_TO_DEGREES;
}

double SceneGeometry::getSquintAngle() const
{
    Vector3 z = mPa / mPa.norm();
    Vector3 x = mVa - z * (mVa.dot(z));
    x.normalize();

    Vector3 y = math::linear::cross(x, z);
    return atan2((-1.0 * mXs).dot(y), (-1.0 * mXs).dot(x)) * RADIANS_TO_DEGREES;
}

double SceneGeometry::getSlopeAngle() const
{
    return acos(mZs.dot(mZg)) * RADIANS_TO_DEGREES;
}


double SceneGeometry::getAzimuthAngle() const
{
    Vector3 east = math::linear::cross(mNorth, mZg);
    return atan2(east.dot(mXs), mNorth.dot(mXs)) * RADIANS_TO_DEGREES;
}

double SceneGeometry::getHeadingAngle() const
{
    Vector3 east = math::linear::cross(mNorth, mZg);
    return atan2(east.dot(mVa), mNorth.dot(mVa)) * RADIANS_TO_DEGREES;
}

double SceneGeometry::getRotationAngle() const
{
    return -getImageAngle(mRg * -1);
}

Vector3 SceneGeometry::getMultiPathVector() const
{
    double scale = mXs.dot(mZg) / mZs.dot(mZg);
    return mXs - (mZs * scale);
}

double SceneGeometry::getMultiPathAngle() const
{
    return getImageAngle(getMultiPathVector());
}

Vector3 SceneGeometry::getNorthVector() const
{
    const Vector3 planeZ = math::linear::cross(getRowVector(), getColVector());
    const double scale = mNorth.dot(planeZ) / mZs.dot(planeZ);
    return  mNorth - (mZs * scale);
}

double SceneGeometry::getNorthAngle() const
{
    return getImageAngle(getNorthVector());
}

Vector3 SceneGeometry::getLayoverVector() const
{
    const Vector3 planeZ = math::linear::cross(getRowVector(), getColVector());
    return planeZ - (mZs / mZs.dot(planeZ));
}

AngleMagnitude SceneGeometry::getLayover() const
{
    Vector3 layoverVec = getLayoverVector();

    AngleMagnitude layover;
    layover.angle = getImageAngle(layoverVec); 
    layover.magnitude = layoverVec.norm();

    return layover;
}

double SceneGeometry::getETPLayoverAngle() const
{
    const double slopeAngleRad = getSlopeAngle() * DEGREES_TO_RADIANS;

    const Vector3 layoverDir = mZs - 1 / cos(slopeAngleRad) * mZs;
    const Vector3 east = math::linear::cross(mNorth, mZg);
    const double etpLayoverAngleRad =
            atan2(east.dot(layoverDir), mNorth.dot(layoverDir));

    return (etpLayoverAngleRad * RADIANS_TO_DEGREES);
}

Vector3 SceneGeometry::getShadowVector() const
{
    const Vector3 planeZ = math::linear::cross(getRowVector(), getColVector());
    const Vector3 groundShadow = mZg - (mXs / mXs.dot(mZg));
    const double scale = groundShadow.dot(planeZ)/mZs.dot(planeZ);
    return groundShadow - mZs * scale;
}

AngleMagnitude SceneGeometry::getShadow() const
{
    Vector3 shadowVec = getShadowVector();

    AngleMagnitude shadow;
    shadow.angle = getImageAngle(shadowVec); 
    shadow.magnitude = shadowVec.norm();
    return shadow;
}

void SceneGeometry::getGroundResolution(double resRg,
                                        double resAz,
                                        double& resRow,
                                        double& resCol) const
{
    const Vector3 z = math::linear::cross(getRowVector(), getColVector());
    const double grazingAngleRad = asin(mXs.dot(z));
    const double tiltAngleRad = atan2(z.dot(mYs), z.dot(mZs));
    const double rotAngleRad = getRotationAngle() * DEGREES_TO_RADIANS;

    const double cosRot = cos(rotAngleRad);
    const double sinRot = sin(rotAngleRad);
    const double sin2Rot = sin(2 * rotAngleRad);
    const double secGraz = 1.0 / cos(grazingAngleRad);
    const double tanGraz = tan(grazingAngleRad);
    const double secTilt = 1.0 / cos(tiltAngleRad);
    const double tanTilt = tan(tiltAngleRad);

    const double sinRotSq = square(sinRot);

    const double kr1 = (sinRotSq * tanGraz * tanTilt - sin2Rot * secGraz)
                 * tanGraz* tanTilt + square(cosRot * secGraz);

    const double kr2 = square(sinRot * secTilt);

    const double kc1 = (sinRotSq * secGraz - sin2Rot * tanGraz * tanTilt)
                 * secGraz + square(cosRot * tanGraz * tanTilt);

    const double kc2 = square(cosRot * secTilt);

    const double resRgSq = square(resRg);
    const double resAzSq = square(resAz);

    resRow = sqrt(kr1 * resRgSq + kr2 * resAzSq);
    resCol = sqrt(kc1 * resRgSq + kc2 * resAzSq);
}
}

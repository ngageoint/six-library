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

#include <math.h>

#include <math/Utilities.h>
#include <math/Constants.h>
#include <scene/Utilities.h>
#include <scene/ECEFToLLATransform.h>
#include <scene/SceneGeometry.h>

namespace
{

inline
scene::Vector3 normalizedCross(const scene::Vector3& lhs,
                               const scene::Vector3& rhs)
{
    scene::Vector3 crossVec(math::linear::cross(lhs, rhs));
    crossVec.normalize();
    return crossVec;
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
                             const Vector3& imageRow,
                             const Vector3& imageCol) :
    mVa(arpVel),
    mPa(arpPos),
    mPo(refPos),
    mImageRow(new Vector3(imageRow)),
    mImageCol(new Vector3(imageCol))
{
    initialize();
}

SceneGeometry::SceneGeometry(const Vector3& arpVel,
                             const Vector3& arpPos,
                             const Vector3& refPos,
                             const Vector3& imageRow,
                             const Vector3& imageCol,
                             const Vector3& opX,
                             const Vector3& opY) :
    mVa(arpVel),
    mPa(arpPos),
    mPo(refPos),
    mImageRow(new Vector3(imageRow)),
    mImageCol(new Vector3(imageCol)),
    mXo(new Vector3(opX)),
    mYo(new Vector3(opY)),
    mZo(new Vector3(normalizedCross(*mXo, *mYo)))
{
    initialize();
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
    const LatLonAlt lla = Utilities::ecefToLatLon(mPo);

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
    mVg = getETPGroundTrack();
}

Vector3 SceneGeometry::getGroundTrack(const Vector3& normalVec) const
{
    return mVa - normalVec * (mVa.dot(normalVec));
}

void SceneGeometry::setImageVectors(const Vector3& row,
                                    const Vector3& col)
{
    mImageRow.reset(new Vector3(row));
    mImageCol.reset(new Vector3(col));
}

void SceneGeometry::setOutputPlaneVectors(const Vector3& opX,
                                          const Vector3& opY)
{
    mXo.reset(new Vector3(opX));
    mYo.reset(new Vector3(opY));
    mZo.reset(new Vector3(normalizedCross(*mXo, *mYo)));
}

Vector3 SceneGeometry::getImageRowVector() const
{
    if (mImageRow.get() == nullptr)
    {
        throw except::Exception(Ctxt("The image row vector is not set!"));
    }
    return *mImageRow;
}

Vector3 SceneGeometry::getImageColVector() const
{
    if (mImageCol.get() == nullptr)
    {
        throw except::Exception(Ctxt("The image column vector is not set!"));
    }

    return *mImageCol;
}

Vector3 SceneGeometry::getOPXVector() const
{
    if (mXo.get() == nullptr)
    {
        throw except::Exception(Ctxt("The output plane x vector is not set!"));
    }
    return *mXo;
}

Vector3 SceneGeometry::getOPYVector() const
{
    if (mYo.get() == nullptr)
    {
        throw except::Exception(Ctxt("The output plane y vector is not set!"));
    }
    return *mYo;
}

Vector3 SceneGeometry::getOPZVector() const
{
    if (mZo.get() == nullptr)
    {
        throw except::Exception(Ctxt("The output plane z vector is not set!"));
    }
    return *mZo;
}

SideOfTrack SceneGeometry::getSideOfTrack() const
{
    return (SideOfTrack)mSideOfTrack;
}

double SceneGeometry::getImageAngle(const Vector3& vec) const
{
    return atan2(getImageColVector().dot(vec),
                 getImageRowVector().dot(vec)) * math::Constants::RADIANS_TO_DEGREES;
}

double SceneGeometry::getGrazingAngle(const Vector3& normalVec) const
{
    return asin(mXs.dot(normalVec)) * math::Constants::RADIANS_TO_DEGREES;
}

double SceneGeometry::getTiltAngle(const Vector3& normalVec) const
{
    return atan2(normalVec.dot(mYs),
                 normalVec.dot(mZs)) * math::Constants::RADIANS_TO_DEGREES;
}

double SceneGeometry::getDopplerConeAngle() const
{
    const Vector3 normVa = mVa / mVa.norm();
    return acos((-1.0 * mXs).dot(normVa)) * math::Constants::RADIANS_TO_DEGREES;
}

double SceneGeometry::getSquintAngle() const
{
    // Define the geocentric plane orthogonal to the ECEF ARP vector
    const Vector3 z = mPa / mPa.norm();

    // Project the slant plane x vector into the above defined plane
    Vector3 projXs = mXs - mXs.dot(z) * z;
    projXs.normalize();

    // Project the velocity vector into the same plane
    Vector3 projVa = mVa - mVa.dot(z) * z;
    projVa.normalize();

    // Compute the unsigned angle between the projected velocity vector and
    // the projection of the vector from the ARP to the ORP.
    // NOTE: Since the slant plane x vector is defined as pointing from the
    //       ORP to the ARP, its projection is negated so that the vector
    //       points from the ARP to the ORP.
    const double squintAngle = acos(-1.0 * projXs.dot(projVa)) *
            math::Constants::RADIANS_TO_DEGREES;
    return squintAngle;
}

double SceneGeometry::getSlopeAngle(const Vector3& normalVec) const
{
    return acos(mZs.dot(normalVec)) * math::Constants::RADIANS_TO_DEGREES;
}

double SceneGeometry::getAzimuthAngle() const
{
    const Vector3 east = math::linear::cross(mNorth, mZg);
    return Utilities::remapZeroTo360(
            atan2(east.dot(mXs), mNorth.dot(mXs)) *
            math::Constants::RADIANS_TO_DEGREES);
}

double SceneGeometry::getHeadingAngle() const
{
    const Vector3 east = math::linear::cross(mNorth, mZg);
    return atan2(east.dot(mVa), mNorth.dot(mVa)) * math::Constants::RADIANS_TO_DEGREES;
}

double SceneGeometry::getRotationAngle() const
{
    return -getImageAngle(mRg * -1);
}

Vector3 SceneGeometry::getMultiPathVector(const Vector3& normalVec) const
{
    const double scale = mXs.dot(normalVec) / mZs.dot(normalVec);
    return mXs - (mZs * scale);
}

double SceneGeometry::getMultiPathAngle() const
{
    return getImageAngle(getMultiPathVector());
}

double SceneGeometry::getOPGroundTrackAngle() const
{
    return getImageAngle(getOPGroundTrack());
}

double SceneGeometry::getOPAngle(const Vector3& vec) const
{
    // Use the vector's projections onto the OP X, Y vectors
    // and use atan2 in a way such that 0 starts at mYo and +90 is mXo
    double opAngle = atan2(getOPXVector().dot(vec), getOPYVector().dot(vec)) *
        math::Constants::RADIANS_TO_DEGREES;

    // Keep it in range
    if (opAngle < 0)
    {
        opAngle += 360;
    }
    else if (opAngle >= 360)
    {
        opAngle -= 360;
    }

    return opAngle;
}

double SceneGeometry::getOPNorthAngle() const
{
    return getOPAngle(mNorth);
}

double SceneGeometry::getOPLayoverAngle() const
{
    const Vector3 planeZ = getOPZVector();
    return getOPAngle(planeZ - (mZs / mZs.dot(planeZ)));
}

double SceneGeometry::getOPShadowAngle() const
{
    const Vector3 planeZ = getOPZVector();
    const Vector3 groundShadow = planeZ - (mXs / mXs.dot(planeZ));
    const double scale = groundShadow.dot(planeZ) / mZs.dot(planeZ);
    return getOPAngle(groundShadow - mZs * scale);
}

Vector3 SceneGeometry::getNorthVector() const
{
    const Vector3 planeZ = math::linear::cross(getImageRowVector(), getImageColVector());
    const double scale = mNorth.dot(planeZ) / mZs.dot(planeZ);
    return  mNorth - (mZs * scale);
}

double SceneGeometry::getNorthAngle() const
{
    return getImageAngle(getNorthVector());
}

Vector3 SceneGeometry::getLayoverVector(const Vector3& planeZ) const
{
    return planeZ - (mZs / mZs.dot(planeZ));
}

Vector3 SceneGeometry::getLayoverVector() const
{
    return getLayoverVector(math::linear::cross(getImageRowVector(),
                                                getImageColVector()));
}

AngleMagnitude SceneGeometry::getLayover() const
{
    const Vector3 layoverVec = getLayoverVector();

    AngleMagnitude layover;
    layover.angle = getImageAngle(layoverVec);
    layover.magnitude = layoverVec.norm();

    return layover;
}

double SceneGeometry::getETPLayoverAngle() const
{
    const double slopeAngleRad =
            getETPSlopeAngle() * math::Constants::DEGREES_TO_RADIANS;

    const Vector3 layoverDir = mZs - 1 / cos(slopeAngleRad) * mZs;
    const Vector3 east = math::linear::cross(mNorth, mZg);
    const double etpLayoverAngleRad =
            atan2(east.dot(layoverDir), mNorth.dot(layoverDir));

    return Utilities::remapZeroTo360(
            etpLayoverAngleRad * math::Constants::RADIANS_TO_DEGREES);
}

Vector3 SceneGeometry::getShadowVector() const
{
    const Vector3 planeZ = math::linear::cross(getImageRowVector(), getImageColVector());
    const Vector3 groundShadow = mZg - (mXs / mXs.dot(mZg));
    const double scale = groundShadow.dot(planeZ)/mZs.dot(planeZ);
    return groundShadow - mZs * scale;
}

AngleMagnitude SceneGeometry::getShadow() const
{
    const Vector3 shadowVec = getShadowVector();

    AngleMagnitude shadow;
    shadow.angle = getImageAngle(shadowVec);
    shadow.magnitude = shadowVec.norm();
    return shadow;
}

types::RowCol<double>
SceneGeometry::getGroundResolution(const types::RgAz<double>& res) const
{
    const Vector3 z = math::linear::cross(getImageRowVector(), getImageColVector());
    const double grazingAngleRad = asin(mXs.dot(z));
    const double tiltAngleRad = atan2(z.dot(mYs), z.dot(mZs));
    const double rotAngleRad =
            getRotationAngle() * math::Constants::DEGREES_TO_RADIANS;

    const double cosRot = cos(rotAngleRad);
    const double sinRot = sin(rotAngleRad);
    const double sin2Rot = sin(2 * rotAngleRad);
    const double secGraz = 1.0 / cos(grazingAngleRad);
    const double tanGraz = tan(grazingAngleRad);
    const double secTilt = 1.0 / cos(tiltAngleRad);
    const double tanTilt = tan(tiltAngleRad);

    const double sinRotSq = math::square(sinRot);

    const double kr1 = (sinRotSq * tanGraz * tanTilt - sin2Rot * secGraz)
                 * tanGraz* tanTilt + math::square(cosRot * secGraz);

    const double kr2 = math::square(sinRot * secTilt);

    const double kc1 = (sinRotSq * secGraz - sin2Rot * tanGraz * tanTilt)
                 * secGraz + math::square(cosRot * tanGraz * tanTilt);

    const double kc2 = math::square(cosRot * secTilt);

    const types::RgAz<double> resSq(math::square(res.rg), math::square(res.az));

    const types::RowCol<double> groundRes(
            sqrt(kr1 * resSq.rg + kr2 * resSq.az),
            sqrt(kc1 * resSq.rg + kc2 * resSq.az));
    return groundRes;
}
}


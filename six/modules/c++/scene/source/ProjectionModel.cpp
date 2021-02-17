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
#include "scene/ProjectionModel.h"

#include <limits>
#include <string>

#include <math/Utilities.h>
#include "scene/ECEFToLLATransform.h"
#include "scene/Utilities.h"

#undef min
#undef max

namespace
{
const double EARTH_ROTATION_RATE = 0.000072921150; // Radians / sec

const double DELTA_GP_MAX = 0.0000001;

// TODO: Should this be a static method instead?
scene::Vector3 computeUnitVector(const scene::LatLonAlt& latLon)
{
    const double latRad = latLon.getLatRadians();
    const double lonRad = latLon.getLonRadians();

    const double cosLat = std::cos(latRad);

    scene::Vector3 unitVector;
    unitVector[0] = cosLat * std::cos(lonRad);
    unitVector[1] = cosLat * std::sin(lonRad);
    unitVector[2] = std::sin(latRad);

    return unitVector;
}

template<typename PolyType>
PolyType verboseDerivative(const PolyType& polynomial, const std::string& name)
{
    if (polynomial.empty())
    {
        throw except::Exception(Ctxt("Unable to take derivative of " + name +
                ". Polynomial is empty."));
    }
    return polynomial.derivative();
}
}

namespace scene
{
ProjectionModel::
ProjectionModel(const Vector3& slantPlaneNormal,
                const Vector3& scp,
                const math::poly::OneD<Vector3>& arpPoly,
                const math::poly::TwoD<double>& timeCOAPoly,
                int lookDir,
                const Errors& errors) :
    mSlantPlaneNormal(slantPlaneNormal),
    mSCP(scp),
    mARPPoly(arpPoly),
    mARPVelPoly(verboseDerivative(arpPoly, "arpPoly")),
    mTimeCOAPoly(timeCOAPoly),
    mLookDir(lookDir),
    mErrors(errors)
{
    mSlantPlaneNormal.normalize();
}

ProjectionModel::~ProjectionModel()
{
}

/*!
 *  Calculations for section 5.2 in SICD Image Projections:
 *  R/Rdot Contour Ground Plane Intersection
 *
 */
Vector3
ProjectionModel::contourToGroundPlane(double rCOA, double rDotCOA,
                                      const Vector3& arpCOA,
                                      const Vector3& velCOA,
                                      const Vector3& groundPlaneNormal,
                                      const Vector3& groundRefPoint) const
{
    // Compute the ARP distance from the plane (ARP Z)
    Vector3 tmp(arpCOA - groundRefPoint);
    const double arpZ = tmp.dot(groundPlaneNormal);

    // Compute the ARP ground plane nadir
    const Vector3 arpGround(arpCOA - groundPlaneNormal * arpZ);

    // Compute the ground plane distance from the ARP nadir to
    // the circle of constant range.
    if (std::abs(arpZ) > std::abs(rCOA))
    {
        throw except::Exception(Ctxt(
                  "No solution: arpZ = " + std::to_string(arpZ) + ", rCOA = " +
                  std::to_string(rCOA)));
    }
    const double groundRange = sqrt(rCOA * rCOA - arpZ * arpZ);

    // Compute cos and sin of the grazing angle
    const double cosGraz( groundRange / rCOA );
    const double sinGraz( arpZ / rCOA );

    // Compute the velocity components normal to the ground
    // plane and parallel to the ground plane
    const double vz = velCOA.dot(groundPlaneNormal);

    const double vmag = velCOA.norm();

    if (std::abs(vz) >= std::abs(vmag))
    {
        throw except::Exception(Ctxt(
                  "No solution: vz = " + std::to_string(vz) + ", vmag = " +
                  std::to_string(vmag)));
    }
    const double vx = sqrt(vmag * vmag - vz * vz);

    // Orient the x direction in the ground plane such that
    // vx > 0.  Compute unit vectors unitX and unitY
    const Vector3 unitX = (velCOA - groundPlaneNormal * vz) / vx;
    const Vector3 unitY = cross(groundPlaneNormal, unitX);

    // Compute the cosine of the azimuth angle to the ground
    // plane point
    const double cosAzimuth =
        (-rDotCOA + vz * sinGraz) / (vx * cosGraz);
    if (cosAzimuth < -1.0 || cosAzimuth > 1.0)
    {
        throw except::Exception(Ctxt(
                  "No solution: cosAzimuth = " + std::to_string(cosAzimuth)));
    }

    const double sinAzimuth =
        mLookDir * sqrt(1.0 - cosAzimuth * cosAzimuth);

    return Vector3(arpGround + unitX * groundRange * cosAzimuth +
                          unitY * groundRange * sinAzimuth);
}


types::RowCol<double>
ProjectionModel::sceneToImage(const Vector3& scenePoint,
                              const AdjustableParams& delta,
                              double* oTimeCOA) const
{
    // For scenePoint, we will compute the spherical earth
    // unit ground plane normal (uGPN)
    Vector3 groundRefPoint(scenePoint);
    Vector3 groundPlaneNormal(groundRefPoint);
    groundPlaneNormal.normalize();

    // Set initial ground plane position to the scenePoint
    Vector3 groundPlanePoint(scenePoint);

    for (size_t i = 0; i < MAX_ITER; ++i)
    {

        // We are projecting the ground plane point to the image
        // plane point.
        Vector3 diff(mSCP - groundPlanePoint);

        // Dist contains the projection difference
        double dist = diff.dot(mImagePlaneNormal) * mScaleFactor.value();

        Vector3 imagePlanePoint =
            groundPlanePoint + mSlantPlaneNormal * dist;

        // Compute the imageCoordinates for the plane point
        types::RowCol<double> imageGridPoint =
            computeImageCoordinates(imagePlanePoint);

        // Find out if scene point is the same as the guessed output
        // of imageToScene
        diff = scenePoint - imageToScene(imageGridPoint,
                                         groundRefPoint,
                                         groundPlaneNormal,
                                         delta,
                                         oTimeCOA);

        dist = diff.norm();

        if (dist < DELTA_GP_MAX)
            return imageGridPoint;

        // Otherwise we are not so lucky, add to our point
        // the difference
        groundPlanePoint += diff;

    }

    throw except::Exception(Ctxt("Point failed to converge"));
}

Vector3
ProjectionModel::imageToScene(const types::RowCol<double>& imageGridPoint,
                              const Vector3& groundRefPoint,
                              const Vector3& groundPlaneNormal,
                              const AdjustableParams& delta,
                              double *oTimeCOA) const
{

    // Compute the timeCOA
    const double timeCOA = mTimeCOAPoly(imageGridPoint.row,
                                        imageGridPoint.col);

    if (oTimeCOA != nullptr)
    {
        *oTimeCOA = timeCOA;
    }

    // Compute ARP position
    Vector3 arpCOA = mARPPoly(timeCOA);

    // Compute ARP velocity
    Vector3 velCOA = mARPVelPoly(timeCOA);

    double r;
    double rDot;

    computeContour(arpCOA, velCOA, timeCOA,
                   imageGridPoint,
                   &r,
                   &rDot);

    // Adjustable parameters are applied after computing R/Rdot contours
    // Adjustable parameters do not affect Rdot
    imageToSceneAdjustment(delta, timeCOA, r, arpCOA, velCOA);

    return contourToGroundPlane(r, rDot,
                                arpCOA, velCOA,
                                groundPlaneNormal,
                                groundRefPoint);
}

Vector3 ProjectionModel::imageToScene(
        const types::RowCol<double>& imageGridPoint,
        double height,
        const AdjustableParams& delta,
        double heightThreshold,
        size_t maxNumIters) const
{
    // Sanity checks
    if (heightThreshold <= 0)
    {
        throw except::Exception(Ctxt("Height threshold must be positive"));
    }

    if (maxNumIters < 1)
    {
        throw except::Exception(Ctxt(
                "Max number of iterations must be positive"));
    }

    // 1. Compute the geodetic ground plane normal at the SCP
    //    Note that this is different than the value passed in to the other
    //    imageToScene() overloading which is the spherical earth GPN (see
    //    section 5.1 for details)
    const ECEFToLLATransform ecefToLatLon;
    const LatLonAlt scpLatLon = ecefToLatLon.transform(mSCP);
    Vector3 groundPlaneNormal = computeUnitVector(scpLatLon);

    Vector3 groundRefPoint =
            mSCP + (height - scpLatLon.getAlt()) * groundPlaneNormal;

    // Compute contour just once
    double r;
    double rDot;
    const double timeCOA = mTimeCOAPoly(imageGridPoint.row,
                                        imageGridPoint.col);
    Vector3 arpCOA = mARPPoly(timeCOA);
    Vector3 velCOA = mARPVelPoly(timeCOA);
    computeContour(arpCOA, velCOA, timeCOA, imageGridPoint, &r, &rDot);

    // Adjustable parameters are applied after computing R/Rdot contours
    // Adjustable parameters do not affect Rdot
    imageToSceneAdjustment(delta, timeCOA, r, arpCOA, velCOA);

    Vector3 gppECEF;
    Vector3 uUP;
    double deltaHeight(std::numeric_limits<double>::max());
    for (size_t iter = 0; iter < maxNumIters; ++iter)
    {
        // 2. Compute precise projection along the R/Rdot contour to ground
        //    plane
        gppECEF = contourToGroundPlane(r, rDot,
                                       arpCOA, velCOA,
                                       groundPlaneNormal,
                                       groundRefPoint);

        const LatLonAlt gppLatLon = ecefToLatLon.transform(gppECEF);

        // 3. Compute unit vector in increasing height direction at ground
        //    plane point position
        uUP = computeUnitVector(gppLatLon);

        deltaHeight = gppLatLon.getAlt() - height;

        // 4. Test to see if GPP is sufficiently close to the HAE surface
        //    If not, adjust the ground plane normal and ground ref point and
        //    try again
        // NOTE: The only way deltaHeight would be negative would be if we had
        //       a concave surface, but we know that we'll always have a
        //       convex surface (we're using the WGS-84 ellipsoid).  On the
        //       off-chance that we're slightly negative due to a numerical
        //       precision issue though, we should still quit... The
        //       alternative would be to take std::abs(deltaHeight), but if
        //       this is actually above the threshold, then we've got some
        //       major precision issue and iterating again may just make
        //       things worse.
        if (deltaHeight <= heightThreshold)
        {
            break;
        }

        groundPlaneNormal = uUP;
        groundRefPoint = gppECEF - deltaHeight * uUP;
    }

    // 5. Compute the unit slant plane normal vector that's tangent to the
    //    R/Rdot contour at the GPP.  This points away from the center of the
    //    earth and in a direction of increasing HAE at the GPP.
    Vector3 uSPN = math::linear::cross(velCOA * mLookDir, gppECEF - arpCOA);
    uSPN.normalize();

    // 6. Compute the straight line projection from the GPP along the slant
    //    plane normal to point SLP.  SLP is located very close to the precise
    //    R/Rdot contour intersection with the constant height surface.
    const Vector3 SF = uUP.dot(uSPN);
    Vector3 subTerm;
    for (size_t ii = 0; ii < 3; ++ii)
    {
        subTerm[ii] = deltaHeight / SF[ii] * uSPN[ii];
    }

    const LatLonAlt SLP = ecefToLatLon.transform(gppECEF - subTerm);

    // 7. Assign surface point SPP position by adjusting its height to be on
    //    the HAE surface
    const LatLonAlt SPP(SLP.getLat(), SLP.getLon(), height);
    return scene::Utilities::latLonToECEF(SPP);
}

void ProjectionModel::imageToSceneAdjustment(const AdjustableParams& delta,
                                             double timeCOA,
                                             double& r,
                                             Vector3& arpCOA,
                                             Vector3& velCOA) const
{
    // Now Add adjustable parameters to ARP and ARP-Velocity
    // Adjustable parameters are in RIC, but ARP and ARP-Velocity are in ECEF
    // Therefore, we need to convert RIC to ECEF before proceeding
    math::linear::MatrixMxN<3, 3> ricEcfToEcef =
            getRICtoECEFTransformMatrix(0.0, timeCOA);
    const Vector3 adjustedARP = mAdjustableParams.getARPVector();
    const Vector3 deltaARP = delta.getARPVector();
    const Vector3 adjustedVel = mAdjustableParams.getARPVelocityVector();
    const Vector3 deltaVel = delta.getARPVelocityVector();

    //Adjustable Parameters are assumed to be in RIC_ECF
    Vector3 arpAddition;
    Vector3 velAddition;
    switch (mErrors.mFrameType.mValue)
    {
    case FrameType::RIC_ECF:
        arpAddition =
                ricEcfToEcef * (deltaARP.matrix() + adjustedARP.matrix());
        velAddition =
                ricEcfToEcef * (deltaVel.matrix() + adjustedVel.matrix());
        break;
    case FrameType::RIC_ECI:
    {
        const math::linear::MatrixMxN<3, 3> ricEciToEcef =
                getRICtoECEFTransformMatrix(EARTH_ROTATION_RATE, timeCOA);
        arpAddition = ricEciToEcef * deltaARP.matrix() +
                ricEcfToEcef * adjustedARP.matrix();
        velAddition = ricEciToEcef * deltaVel.matrix() +
                ricEcfToEcef * adjustedVel.matrix();
    }
        break;
    case FrameType::ECF:
        arpAddition = deltaARP.matrix() + ricEcfToEcef * adjustedARP.matrix();
        velAddition = deltaVel.matrix() + ricEcfToEcef * adjustedVel.matrix();
        break;
    case FrameType::NOT_SET:
    default:
        throw except::Exception(Ctxt(
                "Reference Frame for error parameters undefined"));
    }

    arpCOA = arpCOA + arpAddition;
    velCOA = velCOA + velAddition;
    r = r + mAdjustableParams[AdjustableParams::RANGE_BIAS] +
            delta[AdjustableParams::RANGE_BIAS];
}

math::linear::MatrixMxN<3, 3> ProjectionModel::getRICtoECEFTransformMatrix(
        double earthInitialSpin,
        double timeCOA) const
{
    const Vector3 rARP = mARPPoly(timeCOA);
    const Vector3 vARP = mARPVelPoly(timeCOA);
    Vector3 omega = 0.0;
    omega[2] = earthInitialSpin;

    const Vector3 V = vARP + math::linear::cross(omega, rARP);
    Vector3 radial = rARP;
    radial.normalize();

    Vector3 crossTrack = math::linear::cross(rARP, V);
    crossTrack.normalize();

    const Vector3 inTrack = math::linear::cross(crossTrack, radial);

    math::linear::MatrixMxN<3, 3> ecefToRIC;
    ecefToRIC.col(0, radial.matrix());
    ecefToRIC.col(1, inTrack.matrix());
    ecefToRIC.col(2, crossTrack.matrix());

    return ecefToRIC;
}

math::linear::MatrixMxN<3, 3> ProjectionModel::getRICtoECEFTransformMatrix(
        double earthInitialSpin,
        const types::RowCol<double>& imageGridPoint) const
{
    return getRICtoECEFTransformMatrix(earthInitialSpin,
                                       mTimeCOAPoly(imageGridPoint.row,
                                                    imageGridPoint.col));
}

math::linear::MatrixMxN<2, 2> ProjectionModel::slantToImagePartials(
        const types::RowCol<double>& imageGridPoint,
        double delta) const
{
    // First, compute slant plane vectors
    const double timeCOA =
            mTimeCOAPoly(imageGridPoint.row, imageGridPoint.col);
    const Vector3 rARP = mARPPoly(timeCOA);
    const Vector3 vARP = mARPVelPoly(timeCOA);
    const Vector3 imageGridPointECEF = imageGridToECEF(imageGridPoint);
    Vector3 slantRange = imageGridPointECEF - rARP;
    slantRange.normalize();
    Vector3 slantNormal = math::linear::cross(slantRange, vARP);
    slantNormal.normalize();
    Vector3 slantAzimuth = math::linear::cross(slantNormal, slantRange);
    slantAzimuth.normalize();
    const types::RowCol<double> rangePerturb =
            sceneToImage(imageGridPointECEF + delta * slantRange);
    const types::RowCol<double> azimuthPerturb =
            sceneToImage(imageGridPointECEF + delta * slantAzimuth);
    math::linear::MatrixMxN<2, 2> partials(0.0);
    partials[0][0] = (imageGridPoint.row - rangePerturb.row) / delta;
    partials[0][1] = (imageGridPoint.row - azimuthPerturb.row) / delta;
    partials[1][0] = (imageGridPoint.col - rangePerturb.col) / delta;
    partials[1][1] = (imageGridPoint.col - azimuthPerturb.col) / delta;
    return partials;
}

math::linear::MatrixMxN<3, 7> ProjectionModel::imageToSceneSensorPartials(
        const types::RowCol<double>& imageGridPoint,
        double height,
        const Vector3& scenePoint,
        double delta) const
{
    AdjustableParams deltaParams; // Initialized to all 0's
    math::linear::MatrixMxN<3, 7> jacobian(0.0);

    for (size_t idx = 0; idx < 7; ++idx)
    {
        deltaParams.mParams[idx] = delta;
        Vector3 scenePointDelta =
                imageToScene(imageGridPoint, height, deltaParams);
        scenePointDelta = (scenePointDelta - scenePoint) * (1/delta);
        jacobian.col(idx, scenePointDelta.matrix());
        deltaParams.mParams[idx] = 0.0;
    }

    return jacobian;
}

math::linear::MatrixMxN<3, 7> ProjectionModel::imageToSceneSensorPartials(
        const types::RowCol<double>& imageGridPoint,
        double height,
        double delta) const
{
    const Vector3 scenePt = imageToScene(imageGridPoint, height);
    return imageToSceneSensorPartials(imageGridPoint, height, scenePt, delta);
}

math::linear::MatrixMxN<3, 2> ProjectionModel::imageToScenePartials(
        const types::RowCol<double>& imageGridPoint,
        double height,
        const Vector3& scenePoint,
        double delta) const
{
    types::RowCol<double> imageGridPointDelta(imageGridPoint.row + delta,
                                              imageGridPoint.col);
    Vector3 scenePointDelta = imageToScene(imageGridPointDelta, height);
    scenePointDelta = (scenePointDelta - scenePoint) * (1 / delta);
    math::linear::MatrixMxN<3, 2> jacobian(0.0);
    jacobian.col(0, scenePointDelta.matrix());

    imageGridPointDelta.row = imageGridPoint.row;
    imageGridPointDelta.col = imageGridPoint.col + delta;
    scenePointDelta = imageToScene(imageGridPointDelta, height);
    scenePointDelta = (scenePointDelta - scenePoint) * (1 / delta);
    jacobian.col(1, scenePointDelta.matrix());

    return jacobian;
}

math::linear::MatrixMxN<3, 2> ProjectionModel::imageToScenePartials(
        const types::RowCol<double>& imageGridPoint,
        double height,
        double delta) const
{
    const Vector3 scenePt = imageToScene(imageGridPoint, height);
    return imageToScenePartials(imageGridPoint, height, scenePt, delta);
}

math::linear::MatrixMxN<3, 1> ProjectionModel::imageToSceneHeightPartial(
        const types::RowCol<double>& imageGridPoint,
        double height,
        const Vector3& scenePoint,
        double delta) const
{
    const double deltaHeight = height + delta;
    const Vector3 scenePointDelta = imageToScene(imageGridPoint, deltaHeight);

    const math::linear::MatrixMxN<3, 1> jacobian =
            ((scenePointDelta - scenePoint) * (1.0 / delta)).matrix();
    return jacobian;
}

math::linear::MatrixMxN<3, 1> ProjectionModel::imageToSceneHeightPartial(
        const types::RowCol<double>& imageGridPoint,
        double height,
        double delta) const
{
    const Vector3 scenePt = imageToScene(imageGridPoint, height);
    return imageToSceneHeightPartial(imageGridPoint, height, scenePt, delta);
}

math::linear::MatrixMxN<2, 7> ProjectionModel::sceneToImageSensorPartials(
        const Vector3& scenePoint,
        const types::RowCol<double>& imageGridPoint,
        double delta) const
{
    AdjustableParams deltaParams; // Initialized to all 0's
    math::linear::MatrixMxN<2, 7> jacobian(0.0);

    for (size_t idx = 0; idx < 7; ++idx)
    {
        deltaParams.mParams[idx] = delta;
        const types::RowCol<double> imageGridPointDelta =
                sceneToImage(scenePoint, deltaParams);
        jacobian(0, idx) =
                (imageGridPointDelta.row - imageGridPoint.row) * (1 / delta);
        jacobian(1, idx) =
                (imageGridPointDelta.col - imageGridPoint.col) * (1 / delta);
        deltaParams.mParams[idx] = 0.0;
    }

    return jacobian;
}

math::linear::MatrixMxN<2, 7> ProjectionModel::sceneToImageSensorPartials(
        const Vector3& scenePoint,
        double delta) const
{
    const types::RowCol<double> imagePt = sceneToImage(scenePoint);
    return sceneToImageSensorPartials(scenePoint, imagePt, delta);
}

math::linear::MatrixMxN<2, 3> ProjectionModel::sceneToImagePartials(
        const Vector3& scenePoint,
        const types::RowCol<double>& imageGridPoint,
        double delta) const
{
    math::linear::MatrixMxN<2,3,double> jacobian(0.0);

    for (size_t idx = 0; idx < 3; ++idx)
    {
        Vector3 scenePointDelta = scenePoint;
        scenePointDelta[idx] = scenePointDelta[idx] + delta;
        types::RowCol<double> imageGridPointDelta =
                sceneToImage(scenePointDelta);
        jacobian(0, idx) =
                (imageGridPointDelta.row - imageGridPoint.row) * (1 / delta);
        jacobian(1, idx) =
                (imageGridPointDelta.col - imageGridPoint.col) * (1 / delta);
    }

    return jacobian;
}

math::linear::MatrixMxN<2, 3> ProjectionModel::sceneToImagePartials(
        const Vector3& scenePoint,
        double delta) const
{
    const types::RowCol<double> imagePt = sceneToImage(scenePoint);
    return sceneToImagePartials(scenePoint, imagePt, delta);
}

math::linear::MatrixMxN<7, 7> ProjectionModel::getErrorCovariance(
        const Vector3& scenePoint,
        double timeCOA) const
{
    const Vector3 rARP = mARPPoly(timeCOA);
    const Vector3 vARP = mARPVelPoly(timeCOA);
    Vector3 range = rARP - scenePoint;
    range.normalize();
    Vector3 normal = math::linear::cross(range,vARP);
    normal.normalize();

    // Normal to ellipsoid is gradient at position
    // X-Axis Ellipsoid radius also semi-major axis
    const double a = WGS84EllipsoidModel::EQUATORIAL_RADIUS_METERS;
    // Y-Axis Ellipsoid radius
    const double b = WGS84EllipsoidModel::EQUATORIAL_RADIUS_METERS;
    // Z-Axis Ellipsoid radius
    const double c = WGS84EllipsoidModel::POLAR_RADIUS_METERS;
    Vector3 up;
    up[0] = 2 * scenePoint[0] / math::square(a);
    up[1] = 2 * scenePoint[1] / math::square(b);
    up[2] = 2 * scenePoint[2] / math::square(c);
    up.normalize();

    normal = normal * mLookDir;
    const Vector3 azimuth = math::linear::cross(normal,range);

    const double rv = up.dot(range);
    const double av = up.dot(azimuth);

    // Tropo Error
    // a should be semi-major axis of reference ellipsoid
    static const double TROPOSPHERE_SCALE_HEIGHT_METERS = 7000.0;
    const double Htrop = 1 + TROPOSPHERE_SCALE_HEIGHT_METERS / a;
    const double htrop = sqrt((math::square(Htrop) - 1 + math::square(rv)));
    const double ftrop = Htrop / htrop;
    Vector3 R = scenePoint;
    R.normalize();
    const double graze = asin(range.dot(R));
    math::linear::MatrixMxN<2, 1> bTrop;
    bTrop(0, 0) = -1.0 * ftrop;
    bTrop(1, 0) = av * math::square(cos(graze)) / math::square(htrop);
    math::linear::MatrixMxN<2, 2> tropMat =
            bTrop * mErrors.mTropoErrorCovar * bTrop.transpose();
    math::linear::MatrixMxN<3, 3> tropMat3D(0.0);
    tropMat3D.addInPlace(tropMat, 0, 0);

    // Iono Error
    static const double IONOSPHERE_SCALE_HEIGHT_METERS = 350000.0;
    const double Va = azimuth.dot(vARP);
    const Vector3 denominator = scenePoint - rARP;
    const double omegaA = Va / denominator.norm();
    const double Hion = 1 + IONOSPHERE_SCALE_HEIGHT_METERS / a;
    const double hion = sqrt(math::square(Hion) - 1 + math::square(rv));
    const double fion = Hion / hion;
    math::linear::MatrixMxN<2 ,2> bIon(0.0);
    bIon(0, 0) = -1.0 * fion;
    bIon(1, 1) = fion / omegaA;
    bIon(1, 0) = av * math::square(cos(graze)) / math::square(hion);
    math::linear::MatrixMxN<2, 2> ionMat =
            bIon * mErrors.mIonoErrorCovar * bIon.transpose();
    math::linear::MatrixMxN<3, 3> ionMat3D(0.0);
    ionMat3D.addInPlace(ionMat, 0, 0);

    math::linear::MatrixMxN<3, 3> raToEcef;
    raToEcef.col(0, range.matrix());
    raToEcef.col(1, azimuth.matrix());
    raToEcef.col(2, normal.matrix());
    math::linear::MatrixMxN<3, 3> ricToEcef =
            getRICtoECEFTransformMatrix(0.0, timeCOA);
    math::linear::MatrixMxN<3, 3> raToRic =
            ricToEcef.transpose() * raToEcef;

    math::linear::MatrixMxN<3, 3> totalMat =
            raToRic * tropMat3D * raToRic.transpose() +
            raToRic * ionMat3D * raToRic.transpose();

    math::linear::MatrixMxN<7, 7> errorCovar(mErrors.mSensorErrorCovar);
    errorCovar.addInPlace(totalMat, 0, 0);

    return errorCovar;
}

math::linear::MatrixMxN<7, 7> ProjectionModel::getErrorCovariance(
        const Vector3& scenePoint,
        const types::RowCol<double>& imageGridPoint) const
{
    return getErrorCovariance(scenePoint,
                              mTimeCOAPoly(imageGridPoint.row,
                                           imageGridPoint.col));
}

math::linear::MatrixMxN<7, 7> ProjectionModel::getErrorCovariance(
        const Vector3& scenePoint) const
{
    return getErrorCovariance(scenePoint, sceneToImage(scenePoint));
}

math::linear::MatrixMxN<7, 7> ProjectionModel::getErrorCovariance() const
{
    return getErrorCovariance(mSCP);
}

math::linear::MatrixMxN<2, 2> ProjectionModel::getUnmodeledErrorCovariance(
        const types::RowCol<double>& imageGridPoint) const
{
    // unmodeled error in expected to be defined in the slant plane. Therefore
    // it is necessary to propagate the slant plane error to the image plane
    const math::linear::MatrixMxN<2, 2>
            unmodeledCovar(mErrors.mUnmodeledErrorCovar);
    const math::linear::MatrixMxN<2,2> slantToImageJacobian =
            slantToImagePartials(imageGridPoint);

    // Propagate unmodeled error from slant plane to image plane
    const math::linear::MatrixMxN<2, 2> returnMatrix =
            slantToImageJacobian * unmodeledCovar *
            slantToImageJacobian.transpose();
    return returnMatrix;
}

ProjectionModelWithImageVectors::ProjectionModelWithImageVectors(
    const Vector3& slantPlaneNormal,
    const Vector3& imagePlaneRowVector,
    const Vector3& imagePlaneColVector,
    const Vector3& scp,
    const math::poly::OneD<Vector3>& arpPoly,
    const math::poly::TwoD<double>& timeCOAPoly,
    int lookDir,
    const Errors& errors) :
    ProjectionModel(slantPlaneNormal,
                    scp,
                    arpPoly,
                    timeCOAPoly,
                    lookDir,
                    errors),
    mImagePlaneRowVector(imagePlaneRowVector),
    mImagePlaneColVector(imagePlaneColVector)
{
    mImagePlaneNormal = math::linear::cross(imagePlaneRowVector,
                                            imagePlaneColVector);
    mImagePlaneNormal.normalize();

    mScaleFactor = mSlantPlaneNormal.dot(mImagePlaneNormal);
}

types::RowCol<double>
ProjectionModelWithImageVectors::computeImageCoordinates(
        const Vector3& imagePlanePoint) const
{
    // Delta IPP = xrow * uRow + ycol * uCol
    Vector3 delta(imagePlanePoint - mSCP);

    // What is the x contribution?
    return types::RowCol<double>(delta.dot(mImagePlaneRowVector),
                                 delta.dot(mImagePlaneColVector));
}

Vector3
ProjectionModelWithImageVectors::imageGridToECEF(
        const types::RowCol<double> gridPt) const
{
    return (mSCP +
            gridPt.row * mImagePlaneRowVector +
            gridPt.col * mImagePlaneColVector);
}

RangeAzimProjectionModel::
RangeAzimProjectionModel(const math::poly::OneD<double>& polarAnglePoly,
                         const math::poly::OneD<double>& ksfPoly,
                         const Vector3& slantPlaneNormal,
                         const Vector3& imagePlaneRowVector,
                         const Vector3& imagePlaneColVector,
                         const Vector3& scp,
                         const math::poly::OneD<Vector3>& arpPoly,
                         const math::poly::TwoD<double>& timeCOAPoly,
                         int lookDir,
                         const Errors& errors) :
    ProjectionModelWithImageVectors(slantPlaneNormal,
                                    imagePlaneRowVector,
                                    imagePlaneColVector,
                                    scp,
                                    arpPoly,
                                    timeCOAPoly,
                                    lookDir,
                                    errors),
    mPolarAnglePoly(polarAnglePoly),
    mPolarAnglePolyPrime(verboseDerivative(mPolarAnglePoly, "mPolarAnglePoly")),
    mKSFPoly(ksfPoly),
    mKSFPolyPrime(verboseDerivative(mKSFPoly, "mKSFPoly"))
{
}

void RangeAzimProjectionModel::
computeContour(const Vector3& arpCOA,
               const Vector3& velCOA,
               double timeCOA,
               const types::RowCol<double>& imageGridPoint,
               double* r,
               double* rDot) const
{
    double thetaCOA = mPolarAnglePoly(timeCOA);
    double dThetaDt = mPolarAnglePolyPrime(timeCOA);

    double ksf = mKSFPoly(thetaCOA);
    double dKSFDTheta = mKSFPolyPrime(thetaCOA);

    double cosTheta = cos(thetaCOA);
    double sinTheta = sin(thetaCOA);

    double slopeRadial =
        imageGridPoint.row * cosTheta +
        imageGridPoint.col * sinTheta;

    double slopeCrossRadial =
        -imageGridPoint.row * sinTheta +
        imageGridPoint.col * cosTheta;

    double dR = ksf * slopeRadial;

    double dDrDTheta = dKSFDTheta * slopeRadial + ksf * slopeCrossRadial;
    double dRDot = dDrDTheta * dThetaDt;

    Vector3 vec = arpCOA - mSCP;
    *r = vec.norm();

    *rDot = velCOA.dot(vec) / *r;

    *r += dR;
    *rDot += dRDot;

}


RangeZeroProjectionModel::
RangeZeroProjectionModel(const math::poly::OneD<double>& timeCAPoly,
                         const math::poly::TwoD<double>& dsrfPoly,
                         double rangeCA,
                         const Vector3& slantPlaneNormal,
                         const Vector3& imagePlaneRowVector,
                         const Vector3& imagePlaneColVector,
                         const Vector3& scp,
                         const math::poly::OneD<Vector3>& arpPoly,
                         const math::poly::TwoD<double>& timeCOAPoly,
                         int lookDir,
                         const Errors& errors) :
    ProjectionModelWithImageVectors(slantPlaneNormal,
                                    imagePlaneRowVector,
                                    imagePlaneColVector,
                                    scp,
                                    arpPoly,
                                    timeCOAPoly,
                                    lookDir,
                                    errors),
    mTimeCAPoly(timeCAPoly),
    mDSRFPoly(dsrfPoly),
    mRangeCA(rangeCA)
{
}

void RangeZeroProjectionModel::
computeContour(const Vector3& /*arpCOA*/,
               const Vector3& /*velCOA*/,
               double timeCOA,
               const types::RowCol<double>& imageGridPoint,
               double* r,
               double* rDot) const
{

    // Time of closest approach
    double timeCA = mTimeCAPoly(imageGridPoint.col);

    // Time Difference
    double deltaTimeCOA = timeCOA - timeCA;

    // Velocity at closest approach
    double velocityMagCA = mARPVelPoly(timeCA).norm();

    double t = deltaTimeCOA * velocityMagCA;

    double dsrf = mDSRFPoly(imageGridPoint.row, imageGridPoint.col);

    double rangeCA = mRangeCA + imageGridPoint.row;

    *r = sqrt(rangeCA * rangeCA + dsrf * (t * t));
    *rDot = dsrf / (*r) * t * velocityMagCA;
}

PlaneProjectionModel::
PlaneProjectionModel(const Vector3& slantPlaneNormal,
                     const Vector3& imagePlaneRowVector,
                     const Vector3& imagePlaneColVector,
                     const Vector3& scp,
                     const math::poly::OneD<Vector3>& arpPoly,
                     const math::poly::TwoD<double>& timeCOAPoly,
                     int lookDir,
                     const Errors& errors) :
    ProjectionModelWithImageVectors(slantPlaneNormal,
                                    imagePlaneRowVector,
                                    imagePlaneColVector,
                                    scp,
                                    arpPoly,
                                    timeCOAPoly,
                                    lookDir,
                                    errors)
{
}

void PlaneProjectionModel::
computeContour(const Vector3& arpCOA,
               const Vector3& velCOA,
               double /*timeCOA*/,
               const types::RowCol<double>& imageGridPoint,
               double* r,
               double* rDot) const
{
    const Vector3 vec = arpCOA - imageGridToECEF(imageGridPoint);
    *r = vec.norm();

    *rDot = velCOA.dot(vec) / *r;
}

GeodeticProjectionModel::GeodeticProjectionModel(
        const Vector3& slantPlaneNormal,
        const Vector3& scp,
        const math::poly::OneD<Vector3>& arpPoly,
        const math::poly::TwoD<double>& timeCOAPoly,
        int lookDir,
        const Errors& errors) :
    ProjectionModel(slantPlaneNormal,
                    scp,
                    arpPoly,
                    timeCOAPoly,
                    lookDir,
                    errors)
{
    mImagePlaneNormal = scp;
    mImagePlaneNormal.normalize();

    mScaleFactor = mSlantPlaneNormal.dot(mImagePlaneNormal);
}

types::RowCol<double> GeodeticProjectionModel::
computeImageCoordinates(const Vector3& imagePlanePoint) const
{
    ECEFToLLATransform ecefTransform;
    const LatLonAlt refPt = ecefTransform.transform(mSCP);
    const LatLonAlt lla = ecefTransform.transform(imagePlanePoint);
    return types::RowCol<double>((refPt.getLat() - lla.getLat()) * 3600.0,
                                 (lla.getLon() - refPt.getLon()) * 3600.0);
}

void GeodeticProjectionModel::
computeContour(const Vector3& arpCOA,
               const Vector3& velCOA,
               double /*timeCOA*/,
               const types::RowCol<double>& imageGridPoint,
               double* r,
               double* rDot) const
{
    // TODO: This implementation is identical to PlaneProjectionModel but not
    //       sure how to avoid this
    const Vector3 vec = arpCOA - imageGridToECEF(imageGridPoint);
    *r = vec.norm();

    *rDot = velCOA.dot(vec) / *r;
}

Vector3 GeodeticProjectionModel::imageGridToECEF(
        const types::RowCol<double> gridPt) const
{
    const LatLonAlt refPt = Utilities::ecefToLatLon(mSCP);
    const LatLonAlt lla(refPt.getLat() - gridPt.row / 3600.0,
                        refPt.getLon() + gridPt.col / 3600.0,
                        refPt.getAlt());
    return scene::Utilities::latLonToECEF(lla);
}
}

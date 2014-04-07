/* =========================================================================
 * This file is part of scene-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2013, General Dynamics - Advanced Information Systems
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

#include <limits>

#include "scene/ProjectionModel.h"
#include "scene/ECEFToLLATransform.h"
#include "scene/Utilities.h"

namespace
{
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
}

namespace scene
{
ProjectionModel::
ProjectionModel(const Vector3& slantPlaneNormal,
                const Vector3& imagePlaneRowVector,
                const Vector3& imagePlaneColVector,
                const Vector3& scp,
                const math::poly::OneD<Vector3>& arpPoly,
                const math::poly::TwoD<double>& timeCOAPoly,
                int lookDir) :
    mSlantPlaneNormal(slantPlaneNormal),
    mImagePlaneRowVector(imagePlaneRowVector),
    mImagePlaneColVector(imagePlaneColVector),
    mSCP(scp),
    mARPPoly(arpPoly),
    mTimeCOAPoly(timeCOAPoly),
    mLookDir(lookDir)
    
{
    mImagePlaneNormal =
        math::linear::cross(imagePlaneRowVector, imagePlaneColVector);
    
    mSlantPlaneNormal.normalize();
    mImagePlaneNormal.normalize();
    
    mScaleFactor = mSlantPlaneNormal.dot(mImagePlaneNormal);
    
    mARPVelPoly = mARPPoly.derivative();
    
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
                                      double timeCOA,
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
                  "No solution: arpZ = " + str::toString(arpZ) + ", rCOA = " +
                  str::toString(rCOA)));
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
                  "No solution: vz = " + str::toString(vz) + ", vmag = " +
                  str::toString(vmag)));
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
                  "No solution: cosAzimuth = " + str::toString(cosAzimuth)));
    }
    
    const double sinAzimuth =
        mLookDir * sqrt(1.0 - cosAzimuth * cosAzimuth);
    
    return Vector3(arpGround + unitX * groundRange * cosAzimuth +
                          unitY * groundRange * sinAzimuth);
}


types::RowCol<double>
ProjectionModel::sceneToImage(const Vector3& scenePoint,
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
        double dist = diff.dot(mImagePlaneNormal) * mScaleFactor;
        
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
                              double *oTimeCOA) const
{
    
    // Compute the timeCOA
    double timeCOA = mTimeCOAPoly(imageGridPoint.row,
                                  imageGridPoint.col);
    
    if (oTimeCOA != NULL)
        *oTimeCOA = timeCOA;
    
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
    
    return contourToGroundPlane(r, rDot,
                                arpCOA, velCOA, timeCOA,
                                groundPlaneNormal,
                                groundRefPoint);
}

Vector3 ProjectionModel::imageToScene(
        const types::RowCol<double>& imageGridPoint,
        double height,
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
    const Vector3 arpCOA = mARPPoly(timeCOA);
    const Vector3 velCOA = mARPVelPoly(timeCOA);
    computeContour(arpCOA, velCOA, timeCOA, imageGridPoint, &r, &rDot);

    Vector3 gppECEF;
    Vector3 uUP;
    double deltaHeight(std::numeric_limits<double>::max());
    for (size_t iter = 0; iter < maxNumIters; ++iter)
    {
        // 2. Compute precise projection along the R/Rdot contour to ground
        //    plane
        gppECEF = contourToGroundPlane(r, rDot,
                                       arpCOA, velCOA, timeCOA,
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

RangeAzimProjectionModel::
RangeAzimProjectionModel(const math::poly::OneD<double>& polarAnglePoly,
                         const math::poly::OneD<double>& ksfPoly,
                         const Vector3& slantPlaneNormal,
                         const Vector3& imagePlaneRowVector,
                         const Vector3& imagePlaneColVector,
                         const Vector3& scp,
                         const math::poly::OneD<Vector3>& arpPoly,
                         const math::poly::TwoD<double>& timeCOAPoly,
                         int lookDir) :
    ProjectionModel(slantPlaneNormal,
                    imagePlaneRowVector,
                    imagePlaneColVector,
                    scp,
                    arpPoly,
                    timeCOAPoly,
                    lookDir),
    mPolarAnglePoly(polarAnglePoly), mKSFPoly(ksfPoly)
{
    mPolarAnglePolyPrime = mPolarAnglePoly.derivative();
    mKSFPolyPrime = mKSFPoly.derivative();
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
                         int lookDir) :
    ProjectionModel(slantPlaneNormal,
                    imagePlaneRowVector,
                    imagePlaneColVector,
                    scp,
                    arpPoly,
                    timeCOAPoly,
                    lookDir),
    mTimeCAPoly(timeCAPoly), mDSRFPoly(dsrfPoly), mRangeCA(rangeCA) {}

void RangeZeroProjectionModel::
computeContour(const Vector3& arpCOA,
               const Vector3& velCOA,
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
                     int lookDir) :
    ProjectionModel(slantPlaneNormal,
                    imagePlaneRowVector,
                    imagePlaneColVector,
                    scp,
                    arpPoly,
                    timeCOAPoly,
                    lookDir) {}


void PlaneProjectionModel::
computeContour(const Vector3& arpCOA,
               const Vector3& velCOA,
               double timeCOA,
               const types::RowCol<double>& imageGridPoint,
               double* r,
               double* rDot) const
{
    Vector3 vec(
        mSCP +
        (mImagePlaneRowVector * imageGridPoint.row) +
        (mImagePlaneColVector * imageGridPoint.col)
        );
    vec = arpCOA - vec;
    *r = vec.norm();
    
    *rDot = velCOA.dot(vec) / *r;
    
}
}

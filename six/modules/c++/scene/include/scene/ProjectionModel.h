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
#ifndef __SCENE_PROJECTION_MODEL_H__
#define __SCENE_PROJECTION_MODEL_H__

#include <sys/Optional.h>
#include <math/poly/OneD.h>
#include <math/poly/TwoD.h>

#include <scene/Types.h>
#include <scene/GridECEFTransform.h>
#include <scene/AdjustableParams.h>
#include <scene/Errors.h>

namespace scene
{
class ProjectionModel
{
public:
    enum { MAX_ITER = 50 };

    virtual ~ProjectionModel();

    /*!
     *  Utility function that evaluates the TimeCOAPoly at the
     *  given pixel.
     */
    inline double computeImageTime(const types::RowCol<double> pixel) const
    {
        return mTimeCOAPoly(pixel.row, pixel.col);
    }

    /*!
     *  Utility function that evaluates the ARPPoly at the
     *  given time.
     */
    inline Vector3 computeARPPosition(const double time) const
    {
        return mARPPoly(time);
    }

    /*!
     *  Utility function that evaluates the ARPVelPoly at the
     *  given time.
     */
    inline Vector3 computeARPVelocity(const double time) const
    {
        return mARPVelPoly(time);
    }

    /*!
     *  Implements the transform from a plane point to row column
     *  by subtracting of the SCP projecting into row and column
     *  contributions.
     */
    virtual types::RowCol<double>
        computeImageCoordinates(const Vector3& imagePlanePoint) const = 0;

    virtual Vector3
    imageGridToECEF(const types::RowCol<double> gridPt) const = 0;

    /*!
     *  Virtual method to compute the R/Rdot contour from an
     *  image grid point.  Described in Chapter 4 of
     *  SICD Image Projections.  This routine is specific to IFP
     *  algorithm and to grid type, and so is provided in sub-classes
     *
     */
    virtual void computeContour(const Vector3& arpCOA,
                                const Vector3& velCOA,
                                double timeCOA,
                                const types::RowCol<double>& imageGridPoint,
                                double* r,
                                double* rDot) const = 0;
        
    /*!
     *  Calculations for section 5.2 in SICD Image Projections:
     *  R/Rdot Contour Ground Plane Intersection
     *
     */
    Vector3
        contourToGroundPlane(double rCOA, double rDotCOA,
                             const Vector3& arpCOA,
                             const Vector3& velCOA,
                             const Vector3& groundPlaneNormal,
                             const Vector3& groundRefPoint) const;

    /*!
     *  Implements chapter 6 Precise Scene to Image Grid
     *  Projection from SICD Image Projections, 6.1
     *  Scene to Image: Single Scene Point
     *
     *  This method is iterative.  Using a scenePoint, it iteratively
     *  projects this point into an image plane point, and
     *  converting that to a rg/az coord.  These rg/az coords
     *  are used in a IFP algorithm/grid-type specific way to
     *  find the range/range-rate contour (called R/Rdot in the doc).
     *  Finally this is used to perform a projection to the ground plane
     *  and compare with the reference until convergence is reached.
     *
     *  Note that in the document, Scene and Ground are used
     *  interchangeably,
     *  and sometimes even Image and Slant are as well.
     *
     *  \param scenePoint A scene (ground) point in 3-space
     *  \param delta Delta values to apply for the adjustable parameters
     *  \param oTimeCOA [output] An optional ptr to the timeCOA,
     *  which, if nullptr is not set.
     *  \return An continuous surface image point
     *
     */
    types::RowCol<double> sceneToImage(const Vector3& scenePoint,
                                       const AdjustableParams& delta,
                                       double* oTimeCOA = nullptr) const;

    // Same as above but with a delta of all 0's
    types::RowCol<double> sceneToImage(const Vector3& scenePoint,
                                       double* oTimeCOA = nullptr) const
    {
        return sceneToImage(scenePoint, AdjustableParams(), oTimeCOA);
    }

    /*!
     *  Implements (Slant plane) Image to Scene (Ground plane)
     *  projection using computerContour and contourToGroundPlane
     *
     *  \param imageGridPoint A point in the image surface (continuous)
     *  \param A ground plane reference point
     *  \param groundPlaneNormal A parameter which could just be the
     *  normalized ground plane reference point
     *  \param delta Delta values to apply for the adjustable parameters
     *  \param oTimeCOA [output] optional evaluation of TimeCOAPoly at
     *                  imageGridPoint.row, imageGridPoint.col
     *  \return A scene (ground) point in 3 space
     */
    Vector3 imageToScene(const types::RowCol<double>& imageGridPoint,
                         const Vector3& groundRefPoint,
                         const Vector3& groundPlaneNormal,
                         const AdjustableParams& delta,
                         double* oTimeCOA = nullptr) const;

    // Same as above but with a delta of all 0's
    Vector3 imageToScene(const types::RowCol<double>& imageGridPoint,
                         const Vector3& groundRefPoint,
                         const Vector3& groundPlaneNormal,
                         double* oTimeCOA = nullptr) const
    {
        return imageToScene(imageGridPoint,
                            groundRefPoint,
                            groundPlaneNormal,
                            AdjustableParams(),
                            oTimeCOA);
    }

    /*!
     * Implements chapter 9 Precise R/Rdot To Constant HAE Surface Projection
     * from SICD Image Projections, 9.1 Constant Height Surface & Surface
     * Normal
     *
     * This method is iterative.  It computes the R/Rdot projection to one
     * or more ground planes that are tangent to the constant height surface.
     * Each ground plane projection point computed is slightly above the
     * constant HAE surface.  The final surface position is computed by
     * projecting from the final ground plane projection point down to the
     * HAE surface
     *
     *  \param imageGridPoint A point (meters) in the image surface
     *  (continuous)
     *  \param height Surface height (meters) above the WGS-84 reference
     *  ellipsoid
     *  \param delta Delta values to apply for the adjustable parameters
     *  \param heightThreshold Height threshold (meters) for convergence of
     *  iterative projection sequence.  Must be positive.  Default value will
     *  yield highly accurate projection results, including for very short
     *  range and step grazing angle geometries.  Reducing this threshold will
     *  have negligible improvements in projection accuracy.
     *  \maxNumIters Maximum number of iterations to perform.  Even for very
     *  large images, the maximum iteration count required is expected to be
     *  2 for the default heightThreshold.
     *
     *  \return A scene (ground) point in 3 space at the desired height
     *
     */
    Vector3 imageToScene(const types::RowCol<double>& imageGridPoint,
                         double height,
                         const AdjustableParams& delta = AdjustableParams(),
                         double heightThreshold = 1.0,
                         size_t maxNumIters = 3) const;

    math::linear::MatrixMxN<2, 2> slantToImagePartials(
            const types::RowCol<double>& imageGridPoint,
            double delta = 0.0001) const;

    /*!
     * Computes sensor partials for imageToScene()
     * Provides a Jacobian matrix of form [ARP-RIC, Vel-RIC, Rbias]
     */
    math::linear::MatrixMxN<3, 7> imageToSceneSensorPartials(
            const types::RowCol<double>& imageGridPoint,
            double height,
            const Vector3& scenePoint,
            double delta = 0.0001) const;

    // Same as above but computes scene point via imageToScene()
    math::linear::MatrixMxN<3, 7> imageToSceneSensorPartials(
            const types::RowCol<double>& imageGridPoint,
            double height,
            double delta = 0.0001) const;

    /*
     * Computes partials for imageToScene() (row and col)
     */
    math::linear::MatrixMxN<3, 2> imageToScenePartials(
            const types::RowCol<double>& imageGridPoint,
            double height,
            const Vector3& scenePoint,
            double delta = 0.0001) const;

    // Same as above but computes scene point via imageToScene()
    math::linear::MatrixMxN<3, 2> imageToScenePartials(
            const types::RowCol<double>& imageGridPoint,
            double height,
            double delta = 0.0001) const;

    /*
     * Computes partial derivative for imageToScene() w.r.t. height
     */
    math::linear::MatrixMxN<3, 1> imageToSceneHeightPartial(
            const types::RowCol<double>& imageGridPoint,
            double height,
            const Vector3& scenePoint,
            double delta = 0.0001) const;

    // Same as above but computes scene point via imageToScene()
    math::linear::MatrixMxN<3, 1> imageToSceneHeightPartial(
            const types::RowCol<double>& imageGridPoint,
            double height,
            double delta = 0.0001) const;

    /*
     * Computes sensor partials for sceneToImage()
     */
    math::linear::MatrixMxN<2, 7> sceneToImageSensorPartials(
            const Vector3& scenePoint,
            const types::RowCol<double>& imageGridPoint,
            double delta = 0.0001) const;

    // Same as above but computes image grid point via sceneToImage()
    math::linear::MatrixMxN<2,7,double> sceneToImageSensorPartials(
            const Vector3& scenePoint,
            double delta = 0.0001) const;

    /*!
     * Computes partials for sceneToImage() (row and col)
     */
    math::linear::MatrixMxN<2, 3> sceneToImagePartials(
            const Vector3& scenePoint,
            const types::RowCol<double>& imageGridPoint,
            double delta = 0.0001) const;

    // Same as above but computes image grid point via sceneToImage()
    math::linear::MatrixMxN<2, 3> sceneToImagePartials(
            const Vector3& scenePoint,
            double delta = 0.0001) const;

    /*!
     * Provides sensor error covariance matrix with tropo and iono errors
     * rolled in
     */
    math::linear::MatrixMxN<7, 7> getErrorCovariance(
                const Vector3& scenePoint,
                double timeCOA) const;

    // Same as above but gets timeCOA from imageGridPoint
    math::linear::MatrixMxN<7, 7> getErrorCovariance(
            const Vector3& scenePoint,
            const types::RowCol<double>& imageGridPoint) const;

    // Same as above but gets imageGridPoint by using sceneToImage()
    math::linear::MatrixMxN<7, 7> getErrorCovariance(
            const Vector3& scenePoint) const;

    // Same as above but uses the SCP as scenePoint
    math::linear::MatrixMxN<7, 7> getErrorCovariance() const;

    math::linear::MatrixMxN<2, 2> getUnmodeledErrorCovariance(
            const types::RowCol<double>& imageGridPoint) const;

    AdjustableParams& getAdjustableParams()
    {
        return mAdjustableParams;
    }

    const AdjustableParams& getAdjustableParams() const
    {
        return mAdjustableParams;
    }

    Errors& getErrors()
    {
        return mErrors;
    }

    const Errors& getErrors() const
    {
        return mErrors;
    }

protected:
    // Inheriting classes must initialize mImagePlaneNormal and mScaleFactor
    // in their constructors
    // TODO: Could make these virtual methods instead but this would make some
    //       methods cost more ops
    ProjectionModel(const Vector3& slantPlaneNormal,
                    const Vector3& scp,
                    const math::poly::OneD<Vector3>& arpPoly,
                    const math::poly::TwoD<double>& timeCOAPoly,
                    int lookDir,
                    const Errors& errors = Errors());

    // Returns matrix for RIC to ECEF coordinate transform.
    // Set earthInitialSpin equal to 0 for RIC_ECF
    math::linear::MatrixMxN<3, 3> getRICtoECEFTransformMatrix(
            double earthInitialSpin,
            double timeCOA) const;

    math::linear::MatrixMxN<3, 3> getRICtoECEFTransformMatrix(
            double earthInitialSpin,
            const types::RowCol<double>& imageGridPoint) const;

    void imageToSceneAdjustment(const AdjustableParams& delta,
                                double timeCOA,
                                double& r,
                                Vector3& arpCOA,
                                Vector3& velCOA) const;

protected:
    Vector3 mSlantPlaneNormal;
    Vector3 mImagePlaneNormal;
    Vector3 mSCP;
    double mScaleFactor = 0.0;
    math::poly::OneD<Vector3> mARPPoly;
    math::poly::OneD<Vector3> mARPVelPoly;
    math::poly::TwoD<double> mTimeCOAPoly;
    int mLookDir;

    AdjustableParams mAdjustableParams;
    Errors mErrors;
};

class ProjectionModelWithImageVectors : public ProjectionModel
{
public:
    ProjectionModelWithImageVectors(
        const Vector3& slantPlaneNormal,
        const Vector3& imagePlaneRowVector,
        const Vector3& imagePlaneColVector,
        const Vector3& scp,
        const math::poly::OneD<Vector3>& arpPoly,
        const math::poly::TwoD<double>& timeCOAPoly,
        int lookDir,
        const Errors& errors = Errors());

    virtual types::RowCol<double>
        computeImageCoordinates(const Vector3& imagePlanePoint) const;

    virtual Vector3
    imageGridToECEF(const types::RowCol<double> gridPt) const;

protected:
    Vector3 mImagePlaneRowVector;
    Vector3 mImagePlaneColVector;
};

class RangeAzimProjectionModel : public ProjectionModelWithImageVectors
{
public:
    RangeAzimProjectionModel(const math::poly::OneD<double>& polarAnglePoly,
                             const math::poly::OneD<double>& ksfPoly,
                             const Vector3& slantPlaneNormal,
                             const Vector3& imagePlaneRowVector,
                             const Vector3& imagePlaneColVector,
                             const Vector3& scp,
                             const math::poly::OneD<Vector3>& arpPoly,
                             const math::poly::TwoD<double>& timeCOAPoly,
                             int lookDir,
                             const Errors& errors = Errors());

    virtual void computeContour(const Vector3& arpCOA,
                                const Vector3& velCOA,
                                double timeCOA,
                                const types::RowCol<double>& imageGridPoint,
                                double* r,
                                double* rDot) const;

private:
    math::poly::OneD<double> mPolarAnglePoly;
    math::poly::OneD<double> mPolarAnglePolyPrime;
    math::poly::OneD<double> mKSFPoly;
    math::poly::OneD<double> mKSFPolyPrime;
};

class RangeZeroProjectionModel : public ProjectionModelWithImageVectors
{
public:
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
                             const Errors& errors = Errors());

    virtual void computeContour(const Vector3& arpCOA,
                                const Vector3& velCOA,
                                double timeCOA,
                                const types::RowCol<double>& imageGridPoint,
                                double* r,
                                double* rDot) const;

private:
    math::poly::OneD<double> mTimeCAPoly;
    math::poly::TwoD<double> mDSRFPoly;
    double mRangeCA;
};

class PlaneProjectionModel : public ProjectionModelWithImageVectors
{
public:
    PlaneProjectionModel(const Vector3& slantPlaneNormal,
                         const Vector3& imagePlaneRowVector,
                         const Vector3& imagePlaneColVector,
                         const Vector3& scp,
                         const math::poly::OneD<Vector3>& arpPoly,
                         const math::poly::TwoD<double>& timeCOAPoly,
                         int lookDir,
                         const Errors& errors = Errors());

    virtual void computeContour(const Vector3& arpCOA,
                                const Vector3& velCOA,
                                double timeCOA,
                                const types::RowCol<double>& imageGridPoint,
                                double* r,
                                double* rDot) const;
};

typedef PlaneProjectionModel XRGYCRProjectionModel;
typedef PlaneProjectionModel XCTYATProjectionModel;

class GeodeticProjectionModel : public ProjectionModel
{
public:
    GeodeticProjectionModel(const Vector3& slantPlaneNormal,
                            const Vector3& scp,
                            const math::poly::OneD<Vector3>& arpPoly,
                            const math::poly::TwoD<double>& timeCOAPoly,
                            int lookDir,
                            const Errors& errors = Errors());

    virtual types::RowCol<double>
    computeImageCoordinates(const Vector3& imagePlanePoint) const;

    virtual void computeContour(const Vector3& arpCOA,
                                const Vector3& velCOA,
                                double timeCOA,
                                const types::RowCol<double>& imageGridPoint,
                                double* r,
                                double* rDot) const;

    virtual Vector3 imageGridToECEF(const types::RowCol<double> gridPt) const;

};
}

#endif


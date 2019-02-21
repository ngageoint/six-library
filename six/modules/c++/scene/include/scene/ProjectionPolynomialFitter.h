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
#ifndef __SCENE_PROJECTION_POLYNOMIAL_FITTER_H__
#define __SCENE_PROJECTION_POLYNOMIAL_FITTER_H__

#include <math/poly/Fit.h>
#include <scene/GridECEFTransform.h>
#include <scene/ProjectionModel.h>
#include <math/linear/Matrix2D.h>

namespace scene
{
/*!
 * \class ProjectionPolynomialFitter
 * \brief Used to fit output --> slant and/or time COA polynomials based on
 * sampling sceneToImage() across the output plane
 * versa
 */
class ProjectionPolynomialFitter
{
public:
    static const size_t DEFAULTS_POINTS_1D;


    /* Samples a numPoints1D x numPoints1D grid of points that spans
     * outExtent using sceneToImage().
     *
     * \param projModel Projection model that knows how to use sceneToImage()
     * to convert from an ECEF location to meters from the slant plane SCP
     * \param gridTransform Transform that knows how to convert from
     * output row/col pixel space to ECEF space
     * \param outPixelStart Output space start pixel (i.e. if this is
     * non-zero, it indicates the reference point used with gridTransform is
     * with respect to a global space.  This basically translates to the
     * segment's startLine/startSample values for a multi-segment SICD).
     * \param outExtent Output extent in pixels
     * \param numPoints1D Number of points to use in each direction when
     * sampling the grid.  Defaults to 10.
     */
    ProjectionPolynomialFitter(
            const ProjectionModel& projModel,
            const GridECEFTransform& gridTransform,
            const types::RowCol<double>& outPixelStart,
            const types::RowCol<size_t>& outExtent,
            size_t numPoints1D = DEFAULTS_POINTS_1D);

    /* Samples a numPoints1D x numPoints1D grid of points that spans
     * the extent of a polygon using sceneToImage().
     *
     * \param projModel Projection model that knows how to use sceneToImage()
     * to convert from an ECEF location to meters from the slant plane SCP
     * \param gridTransform Transform that knows how to convert from
     * output row/col pixel space to ECEF space
     * \param fullExtent Full extent of the global output plane.
     * \param outPixelStart Output space start pixel (i.e. if this is
     * non-zero, it indicates the reference point used with gridTransform is
     * with respect to a global space.  This basically translates to the
     * segment's startLine/startSample values for a multi-segment SICD).
     * \param outExtent Output extent in pixels
     * \param polygon The polygon of output plane pixel coordinate used to
     * determine the grid of points.
     * \param numPoints1D Number of points to use in each direction when
     * sampling the grid.  Defaults to 10.
     */
    ProjectionPolynomialFitter(
            const ProjectionModel& projModel,
            const GridECEFTransform& gridTransform,
            const types::RowCol<size_t>& fullExtent,
            const types::RowCol<double>& outPixelStart,
            const types::RowCol<size_t>& outExtent,
            const std::vector<types::RowCol<double> >& polygon,
            size_t numPoints1D = DEFAULTS_POINTS_1D);

    // Returns the output plane rows used during sampling in case you want to
    // do your own polynomial fitting
    const math::linear::Matrix2D<double>& getOutputPlaneRows() const
    {
        return mOutputPlaneRows;
    }

    // Returns the output plane cols used during sampling in case you want to
    // do your own polynomial fitting
    const math::linear::Matrix2D<double>& getOutputPlaneCols() const
    {
        return mOutputPlaneCols;
    }

    // Returns the scene coordinates (i.e. meters from the slant plane SCP)
    // computed for each grid sample in case you want to do your own
    // polynomial fitting
    const math::linear::Matrix2D<types::RowCol<double> >&
    getSceneCoordinates() const
    {
        return mSceneCoordinates;
    }

    // Returns the time center of aperture computed for each grid sample in
    // case you want to do your own polynomial fitting
    const math::linear::Matrix2D<double>& getTimeCOA() const
    {
        return mTimeCOA;
    }

    /*
     * Uses the samples computed in the constructor to fit pixel-based
     * output --> slant polynomials of the specified order.  Optionally
     * computes mean residual errors in these polynomials (mean of the squares
     * of the differences).
     *
     * interimSceneCenter and interimSampleSpacing are also in the slant
     * plane.  These represent the image at the point when the output to slant
     * polynomials will be applied.  They're intended to allow for any
     * image upsampling, etc. that may be done while still in the slant plane.
     * If no such upsampling is done, set interimSceneCenter = inSceneCenter
     * and interimSampleSpacing = the original input slant plane sample
     * spacing.
     *
     * \param inPixelStart Original input slant plane start pixel (i.e. if
     * this is non-zero, it indicates an AOI)
     * \param inSceneCenter Original input slant plane scene center pixel in
     * row/col
     * \param interimSceneCenter Scene center pixel in row/col of the image
     * in the slant plane when outputToSlantRow/Col polynomials are applied
     * \param interimSampleSpacing Sample spacing of the image in the slant
     * plane when outputToSlantRow/Col polynomials are applied
     * \param polyOrderX Polynomial order to use when fitting the polynomials
     * in the x direction
     * \param polyOrderY Polynomial order to use when fitting the polynomials
     * in the y direction
     * \param outputToSlantRow [output] Output to slant row polynomial.
     * This is pixel-based.
     * \param outputToSlantCol [output] Output to slant col polynomial.
     * This is pixel-based.
     * \param meanResidualErrorRow [output] Optional.  Mean residual error in
     * outputToSlantRow.
     * \param meanResidualErrorCol [output] Optional.  Mean residual error in
     * outputToSlantCol.
     */
    void fitOutputToSlantPolynomials(
            const types::RowCol<size_t>& inPixelStart,
            const types::RowCol<double>& inSceneCenter,
            const types::RowCol<double>& interimSceneCenter,
            const types::RowCol<double>& interimSampleSpacing,
            size_t polyOrderX,
            size_t polyOrderY,
            math::poly::TwoD<double>& outputToSlantRow,
            math::poly::TwoD<double>& outputToSlantCol,
            double* meanResidualErrorRow = NULL,
            double* meanResidualErrorCol = NULL) const;

    /*
     * Uses the samples computed in the constructor to fit pixel-based
     * slant --> output polynomials of the specified order.  Optionally
     * computes mean residual errors in these polynomials (mean of the squares
     * of the differences).
     *
     * interimSceneCenter and interimSampleSpacing are also in the slant
     * plane.  These represent the image at the point when the output to slant
     * polynomials will be applied.  They're intended to allow for any
     * image upsampling, etc. that may be done while still in the slant plane.
     * If no such upsampling is done, set interimSceneCenter = inSceneCenter
     * and interimSampleSpacing = the original input slant plane sample
     * spacing.
     *
     * \param inPixelStart Original input slant plane start pixel (i.e. if
     * this is non-zero, it indicates an AOI)
     * \param inSceneCenter Original input slant plane scene center pixel in
     * row/col
     * \param interimSceneCenter Scene center pixel in row/col of the image
     * in the slant plane when slantToOutputToRow/Col polynomials are applied
     * \param interimSampleSpacing Sample spacing of the image in the slant
     * plane when slantToOutputRow/Col polynomials are applied
     * \param polyOrderX Polynomial order to use when fitting the polynomials
     * in the x direction
     * \param polyOrderY Polynomial order to use when fitting the polynomials
     * in the y direction
     * \param slantToOutputRow [output] Slant to output row polynomial.
     * This is pixel-based.
     * \param slantToOutputCol [output] Slant to output col polynomial.
     * This is pixel-based.
     * \param meanResidualErrorRow [output] Optional.  Mean residual error in
     * slantToOutputRow.
     * \param meanResidualErrorCol [output] Optional.  Mean residual error in
     * slantToOutputCol.
     */
    void fitSlantToOutputPolynomials(
            const types::RowCol<size_t>& inPixelStart,
            const types::RowCol<double>& inSceneCenter,
            const types::RowCol<double>& interimSceneCenter,
            const types::RowCol<double>& interimSampleSpacing,
            size_t polyOrderX,
            size_t polyOrderY,
            math::poly::TwoD<double>& slantToOutputRow,
            math::poly::TwoD<double>& slantToOutputCol,
            double* meanResidualErrorRow = NULL,
            double* meanResidualErrorCol = NULL) const;

    /*
     * Uses the samples computed in the constructor to fit a time COA
     * polynomial of the specified order in units of meters from
     * outSceneCenter.  Optionally computes mean residual errors in these
     * polynomials (mean of the squares of the differences).
     *
     * \param outSceneCenter Output space scene center pixel in row/col.  For
     * multi-segment SICDs, this is in the output grid with respect to this
     * SICD, not the global space (i.e. it is close to the center of the
     * output extent).
     * \param outSampleSpacing Output space sample spacing
     * \param polyOrderX Polynomial order to use when fitting the polynomials
     * in the x direction
     * \param polyOrderY Polynomial order to use when fitting the polynomials
     * in the y direction
     * \param timeCOAPoly [output] Time center of aperture polynomial
     * (in meters from outSceneCenter)
     * \param meanResidualError [output] Optional.  Mean residual error in
     * timeCOAPoly.
     */
    void fitTimeCOAPolynomial(
            const types::RowCol<double>& outSceneCenter,
            const types::RowCol<double>& outSampleSpacing,
            size_t polyOrderX,
            size_t polyOrderY,
            math::poly::TwoD<double>& timeCOAPoly,
            double* meanResidualError = NULL) const;

    /*
     * Uses the samples computed in the constructor to fit a time COA
     * polynomial of the specified order in units of 0-based pixels from the
     * upper-left corner.  Optionally computes mean residual errors in these
     * polynomials (mean of the squares of the differences).
     *
     * \param outPixelShift This offset is subtracted from each pixel value
     * used to fit the polynomial, so if you want a polynomial in units of
     * 1-based pixels from the upper-left corner, you would set this to
     * (-1, -1).
     * \param polyOrderX Polynomial order to use when fitting the polynomials
     * in the x direction
     * \param polyOrderY Polynomial order to use when fitting the polynomials
     * in the y direction
     * \param timeCOAPoly [output] Time center of aperture polynomial
     * (in pixels from the upper-left corner)
     * \param meanResidualError [output] Optional.  Mean residual error in
     * timeCOAPoly.
     */
    void fitPixelBasedTimeCOAPolynomial(
            const types::RowCol<double>& outPixelShift,
            size_t polyOrderX,
            size_t polyOrderY,
            math::poly::TwoD<double>& timeCOAPoly,
            double* meanResidualError = NULL) const;

    // Same as above but allows an arbitrary transform to be applied to the
    // row and column samples
    template <typename RowTransformT, typename ColTransformT>
    void fitPixelBasedTimeCOAPolynomial(
            const RowTransformT& rowTransform,
            const ColTransformT& colTransform,
            size_t polyOrderX,
            size_t polyOrderY,
            math::poly::TwoD<double>& timeCOAPoly,
            double* meanResidualError = NULL) const
    {
        math::linear::Matrix2D<double> rowMapping(mNumPoints1D, mNumPoints1D);
        math::linear::Matrix2D<double> colMapping(mNumPoints1D, mNumPoints1D);

        for (size_t ii = 0; ii < mNumPoints1D; ++ii)
        {
            for (size_t jj = 0; jj < mNumPoints1D; ++jj)
            {
                // Allow the caller to transform the row/col that we fit to
                rowMapping(ii, jj) = rowTransform(mOutputPlaneRows(ii,jj));
                colMapping(ii, jj) = colTransform(mOutputPlaneCols(ii,jj));
            }
        }

        // Now fit the polynomial
        timeCOAPoly = math::poly::fit(rowMapping, colMapping, mTimeCOA,
                                      polyOrderX, polyOrderY);

        // Optionally report the residual error
        if (meanResidualError)
        {
            double errorSum(0.0);

            for (size_t ii = 0; ii < mNumPoints1D; ++ii)
            {
                for (size_t jj = 0; jj < mNumPoints1D; ++jj)
                {
                    const double row(rowMapping(ii, jj));
                    const double col(colMapping(ii, jj));

                    const double diff = mTimeCOA(ii, jj) - timeCOAPoly(row, col);
                    errorSum += diff * diff;
                }
            }

            *meanResidualError = errorSum / (mNumPoints1D * mNumPoints1D);
        }
    }

private:
    void projectToSlantPlane(const ProjectionModel& projModel,
                             const GridECEFTransform& gridTransform,
                             const types::RowCol<double>& outPixelStart,
                             const types::RowCol<double>& currentOffset,
                             size_t row,
                             size_t col);

    void getSlantPlaneSamples(
            const types::RowCol<size_t>& inPixelStart,
            const types::RowCol<double>& inSceneCenter,
            const types::RowCol<double>& interimSceneCenter,
            const types::RowCol<double>& interimSampleSpacing,
            math::linear::Matrix2D<double>& slantPlaneRows,
            math::linear::Matrix2D<double>& slantPlaneCols) const;

private:
    const size_t mNumPoints1D;
    math::linear::Matrix2D<double> mOutputPlaneRows;
    math::linear::Matrix2D<double> mOutputPlaneCols;
    math::linear::Matrix2D<types::RowCol<double> > mSceneCoordinates;
    math::linear::Matrix2D<double> mTimeCOA;
};
}

#endif

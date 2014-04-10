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
#ifndef __SCENE_PROJECTION_POLYNOMIAL_FITTER_H__
#define __SCENE_PROJECTION_POLYNOMIAL_FITTER_H__

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
     * \param outExtent Output extent in pixels
     * \param numPoints1D Number of points to use in each direction when
     * sampling the grid.  Defaults to 10.
     */
    ProjectionPolynomialFitter(
            const ProjectionModel& projModel,
            const GridECEFTransform& gridTransform,
            const types::RowCol<size_t>& outExtent,
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
     * when outputToSlantRow/Col polynomials are applied
     * \param interimSampleSpacing Sample spacing of the image when
     * outputToSlantRow/Col polynomials are applied
     * \param outPixelStart Output space start pixel (i.e. if this is
     * non-zero, it indicates the SCP used with gridTransform is with respect
     * to a global space.  This basically translates to the segment's
     * startLine/startSample values for a multi-segment SICD).
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
            const types::RowCol<double>& outPixelStart,
            size_t polyOrderX,
            size_t polyOrderY,
            math::poly::TwoD<double>& outputToSlantRow,
            math::poly::TwoD<double>& outputToSlantCol,
            double* meanResidualErrorRow = NULL,
            double* meanResidualErrorCol = NULL) const;

    /*
     * Uses the samples computed in the constructor to fit a time COA
     * polynomial of the specified order in units of meters from
     * outSceneCenter.  Optionally computes mean residual errors in these
     * polynomials (mean of the squares of the differences).
     *
     * \param outSceneCenter Output space scene center pixel in row/col
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
     * \param outPixelStart Output space start pixel (i.e. if this is
     * non-zero, it indicates the SCP used with gridTransform is with respect
     * to a global space.  This basically translates to the segment's
     * startLine/startSample values for a multi-segment SICD).
     * This offset is subtracted from each pixel value used to fit the
     * polynomial, so if you want a polynomial in units of 1-based pixels from
     * the upper-left corner, you would set this to (-1, -1) plus whatever
     * compensation is needed if it's a multi-segment SICD.
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
            const types::RowCol<double>& outPixelStart,
            size_t polyOrderX,
            size_t polyOrderY,
            math::poly::TwoD<double>& timeCOAPoly,
            double* meanResidualError = NULL) const;

private:
    const size_t mNumPoints1D;
    math::linear::Matrix2D<double> mOutputPlaneRows;
    math::linear::Matrix2D<double> mOutputPlaneCols;
    math::linear::Matrix2D<types::RowCol<double> > mSceneCoordinates;
    math::linear::Matrix2D<double> mTimeCOA;
};
}

#endif

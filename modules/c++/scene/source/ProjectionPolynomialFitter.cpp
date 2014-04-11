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

#include <math/poly/Fit.h>
#include <scene/ProjectionPolynomialFitter.h>

namespace scene
{
const size_t ProjectionPolynomialFitter::DEFAULTS_POINTS_1D = 10;

ProjectionPolynomialFitter::ProjectionPolynomialFitter(
        const ProjectionModel& projModel,
        const GridECEFTransform& gridTransform,
        const types::RowCol<size_t>& outExtent,
        size_t numPoints1D) :
    mNumPoints1D(numPoints1D),
    mOutputPlaneRows(numPoints1D, numPoints1D),
    mOutputPlaneCols(numPoints1D, numPoints1D),
    mSceneCoordinates(numPoints1D,
                      numPoints1D,
                      types::RowCol<double>(0.0, 0.0)),
    mTimeCOA(numPoints1D, numPoints1D)
{
    // Want to sample [0, outExtent) in the loop below
    // That is, we are marching through the output grid in pixel space
    const types::RowCol<double> skip(
        static_cast<double>(outExtent.row - 1) / (mNumPoints1D - 1),
        static_cast<double>(outExtent.col - 1) / (mNumPoints1D - 1));

    types::RowCol<double> currentOffset(0.0, 0.0);

    for (size_t ii = 0;
         ii < mNumPoints1D;
         ++ii, currentOffset.row += skip.row)
    {
        currentOffset.col = 0;

        for (size_t jj = 0;
             jj < mNumPoints1D;
             ++jj, currentOffset.col += skip.col)
        {
            // Rows, these are essentially meshgrid-like (replicated)
            mOutputPlaneRows(ii, jj) = currentOffset.row;
            // This represents columns
            mOutputPlaneCols(ii, jj) = currentOffset.col;

            // Find ECEF location of this output plane pixel
            const scene::Vector3 sPos =
                    gridTransform.rowColToECEF(currentOffset);

            // Call sceneToImage() to get meters from the slant plane SCP
            double timeCOA(0.0);
            mSceneCoordinates(ii, jj) =
                    projModel.sceneToImage(sPos, &timeCOA);
            mTimeCOA(ii, jj) = timeCOA;
        }
    }
}

void ProjectionPolynomialFitter::fitOutputToSlantPolynomials(
        const types::RowCol<size_t>& inPixelStart,
        const types::RowCol<double>& inSceneCenter,
        const types::RowCol<double>& interimSceneCenter,
        const types::RowCol<double>& interimSampleSpacing,
        const types::RowCol<double>& outPixelStart,
        const types::RowCol<double>& outPixelScaleFactor,
        size_t polyOrderX,
        size_t polyOrderY,
        math::poly::TwoD<double>& outputToSlantRow,
        math::poly::TwoD<double>& outputToSlantCol,
        double* meanResidualErrorRow,
        double* meanResidualErrorCol) const
{
    const types::RowCol<double> ratio(interimSceneCenter / inSceneCenter);

    const types::RowCol<double> aoiOffset(inPixelStart.row * ratio.row,
                                          inPixelStart.col * ratio.col);

    math::linear::Matrix2D<double> slantPlaneRows(mNumPoints1D, mNumPoints1D);
    math::linear::Matrix2D<double> slantPlaneCols(mNumPoints1D, mNumPoints1D);
    math::linear::Matrix2D<double> outputPlaneRows(mNumPoints1D, mNumPoints1D);
    math::linear::Matrix2D<double> outputPlaneCols(mNumPoints1D, mNumPoints1D);

    for (size_t ii = 0; ii < mNumPoints1D; ++ii)
    {
        for (size_t jj = 0; jj < mNumPoints1D; ++jj)
        {
            // sceneCoord is in meters from the slant plane SCP
            // So, divide by the sample spacing to get it in pixels, then
            // offset by the slant plane SCP pixel.  Need to further offset to
            // take non-zero inPixelStart into account.
            const types::RowCol<double> sceneCoord(mSceneCoordinates(ii, jj));

            slantPlaneRows(ii, jj) =
                    sceneCoord.row / interimSampleSpacing.row +
                    interimSceneCenter.row - aoiOffset.row;

            slantPlaneCols(ii, jj) =
                    sceneCoord.col / interimSampleSpacing.col +
                    interimSceneCenter.col - aoiOffset.col;

            // Allow the caller to scale and offset the row/col that we fit to
            outputPlaneRows(ii, jj) =
                    mOutputPlaneRows(ii,jj) * outPixelScaleFactor.row -
                    outPixelStart.row;

            outputPlaneCols(ii, jj) =
                    mOutputPlaneCols(ii,jj) * outPixelScaleFactor.col -
                    outPixelStart.col;
        }
    }

    // Now fit the polynomials
    outputToSlantRow = math::poly::fit(outputPlaneRows,
                                       outputPlaneCols,
                                       slantPlaneRows,
                                       polyOrderX, polyOrderY);

    outputToSlantCol = math::poly::fit(outputPlaneRows,
                                       outputPlaneCols,
                                       slantPlaneCols,
                                       polyOrderX, polyOrderY);

    // Optionally report the residual error
    if (meanResidualErrorRow || meanResidualErrorCol)
    {
        double errorSumRow(0.0);
        double errorSumCol(0.0);

        for (size_t ii = 0; ii < mNumPoints1D; ++ii)
        {
            for (size_t jj = 0; jj < mNumPoints1D; ++jj)
            {
                const double row(outputPlaneRows(ii, jj));
                const double col(outputPlaneCols(ii, jj));

                double diff =
                        slantPlaneRows(ii, jj) - outputToSlantRow(row, col);
                errorSumRow += diff * diff;

                diff = slantPlaneCols(ii, jj) - outputToSlantCol(row, col);
                errorSumCol += diff * diff;
            }
        }

        const size_t numPoints = mNumPoints1D * mNumPoints1D;
        if (meanResidualErrorRow)
        {
            *meanResidualErrorRow = errorSumRow / numPoints;
        }
        if (meanResidualErrorCol)
        {
            *meanResidualErrorCol = errorSumCol / numPoints;
        }
    }
}

void ProjectionPolynomialFitter::fitOutputToSlantPolynomials(
        const types::RowCol<size_t>& inPixelStart,
        const types::RowCol<double>& inSceneCenter,
        const types::RowCol<double>& interimSceneCenter,
        const types::RowCol<double>& interimSampleSpacing,
        const types::RowCol<double>& outPixelStart,
        size_t polyOrderX,
        size_t polyOrderY,
        math::poly::TwoD<double>& outputToSlantRow,
        math::poly::TwoD<double>& outputToSlantCol,
        double* meanResidualErrorRow,
        double* meanResidualErrorCol) const
{
    fitOutputToSlantPolynomials(inPixelStart,
                                inSceneCenter,
                                interimSceneCenter,
                                interimSampleSpacing,
                                types::RowCol<double>(1.0, 1.0),
                                polyOrderX,
                                polyOrderY,
                                outputToSlantRow,
                                outputToSlantCol,
                                meanResidualErrorRow,
                                meanResidualErrorCol);
}

void ProjectionPolynomialFitter::fitTimeCOAPolynomial(
        const types::RowCol<double>& outSceneCenter,
        const types::RowCol<double>& outSampleSpacing,
        size_t polyOrderX,
        size_t polyOrderY,
        math::poly::TwoD<double>& timeCOAPoly,
        double* meanResidualError) const
{
    math::linear::Matrix2D<double> rowMapping(mNumPoints1D, mNumPoints1D);
    math::linear::Matrix2D<double> colMapping(mNumPoints1D, mNumPoints1D);

    for (size_t ii = 0; ii < mNumPoints1D; ++ii)
    {
        for (size_t jj = 0; jj < mNumPoints1D; ++jj)
        {
            // Need to map output plane pixels to meters from the output
            // plane SCP
            rowMapping(ii, jj) =
                    (mOutputPlaneRows(ii,jj) - outSceneCenter.row) *
                    outSampleSpacing.row;

            colMapping(ii, jj) =
                    (mOutputPlaneCols(ii,jj) - outSceneCenter.col) *
                    outSampleSpacing.col;
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

void ProjectionPolynomialFitter::fitPixelBasedTimeCOAPolynomial(
        const types::RowCol<double>& outPixelStart,
        size_t polyOrderX,
        size_t polyOrderY,
        math::poly::TwoD<double>& timeCOAPoly,
        double* meanResidualError) const
{
    math::linear::Matrix2D<double> rowMapping(mNumPoints1D, mNumPoints1D);
    math::linear::Matrix2D<double> colMapping(mNumPoints1D, mNumPoints1D);

    for (size_t ii = 0; ii < mNumPoints1D; ++ii)
    {
        for (size_t jj = 0; jj < mNumPoints1D; ++jj)
        {
            // Allow the caller to offset the row/col that we fit to
            rowMapping(ii, jj) = mOutputPlaneRows(ii,jj) - outPixelStart.row;
            colMapping(ii, jj) = mOutputPlaneCols(ii,jj) - outPixelStart.col;
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
}

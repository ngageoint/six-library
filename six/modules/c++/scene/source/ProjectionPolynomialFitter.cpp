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

#include <scene/ProjectionPolynomialFitter.h>
#include <polygon/PolygonMask.h>

namespace
{
class Shift
{
public:
    Shift(double shift) :
        mShift(shift)
    {
    }

    inline double operator()(double input) const
    {
        return (input - mShift);
    }

private:
    const double mShift;
};
}

namespace scene
{
const size_t ProjectionPolynomialFitter::DEFAULTS_POINTS_1D = 10;

ProjectionPolynomialFitter::ProjectionPolynomialFitter(
    const ProjectionModel& projModel,
    const GridECEFTransform& gridTransform,
    const types::RowCol<double>& outPixelStart,
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
    // Want to sample [outPixelStart, outPixelStart + outExtent) in the loop
    // below.  That is, we are marching through the portion of interest of the
    // output grid in pixel space.  In the case of multi-segment SICDs, we'll
    // only sample our part of the grid defined by outPixelStart and
    // outExtent.
    const types::RowCol<double> skip(
        static_cast<double>(outExtent.row - 1) / (mNumPoints1D - 1),
        static_cast<double>(outExtent.col - 1) / (mNumPoints1D - 1));

    types::RowCol<double> currentOffset(outPixelStart);

    for (size_t ii = 0;
         ii < mNumPoints1D;
         ++ii, currentOffset.row += skip.row)
    {
        currentOffset.col = outPixelStart.col;

        for (size_t jj = 0;
             jj < mNumPoints1D;
             ++jj, currentOffset.col += skip.col)
        {
            projectToSlantPlane(projModel, gridTransform, outPixelStart,
                                currentOffset, ii, jj);
        }
    }
}

ProjectionPolynomialFitter::ProjectionPolynomialFitter(
        const ProjectionModel& projModel,
        const GridECEFTransform& gridTransform,
        const types::RowCol<size_t>& fullExtent,
        const types::RowCol<double>& outPixelStart,
        const types::RowCol<size_t>& outExtent,
        const std::vector<types::RowCol<double> >& polygon,
        size_t numPoints1D) :
    mNumPoints1D(numPoints1D),
    mOutputPlaneRows(numPoints1D, numPoints1D),
    mOutputPlaneCols(numPoints1D, numPoints1D),
    mSceneCoordinates(numPoints1D,
                      numPoints1D,
                      types::RowCol<double>(0.0, 0.0)),
    mTimeCOA(numPoints1D, numPoints1D)
{
    // Get bounding rectangle of output plane polygon.
    double minRow =  std::numeric_limits<double>::max();
    double maxRow = -std::numeric_limits<double>::max();
    double minCol =  std::numeric_limits<double>::max();
    double maxCol = -std::numeric_limits<double>::max();

    for (size_t ii = 0; ii < polygon.size(); ++ii)
    {
        minRow = std::min(minRow, polygon[ii].row);
        maxRow = std::max(maxRow, polygon[ii].row);
        minCol = std::min(minCol, polygon[ii].col);
        maxCol = std::max(maxCol, polygon[ii].col);
    }

    if (minRow > static_cast<double>(fullExtent.row) ||
        maxRow < 0 ||
        minCol > static_cast<double>(fullExtent.col) ||
        maxCol < 0)
    {
        throw except::Exception(Ctxt(
            "Bounding rectangle is outside of output extent"));
    }

    // Only interested in pixels inside the fullExtent.
    minRow = std::max(minRow, 0.0);
    minCol = std::max(minCol, 0.0);
    maxRow = std::min(maxRow, static_cast<double>(fullExtent.row) - 1);
    maxCol = std::min(maxCol, static_cast<double>(fullExtent.col) - 1);

    // Get size_t extent of the set of points.
    const size_t minRowI = static_cast<size_t>(std::ceil(minRow));
    const size_t minColI = static_cast<size_t>(std::ceil(minCol));
    const size_t maxRowI = static_cast<size_t>(std::floor(maxRow));
    const size_t maxColI = static_cast<size_t>(std::floor(maxCol));

    if (minRowI > maxRowI || minColI > maxColI)
    {
        throw except::Exception(Ctxt(
            "Bounding rectangle has no area"));
    }

    // The offset and extent are relative to the entire global output plane.
    const types::RowCol<double> boundingOffset(
        static_cast<double>(minRowI),
        static_cast<double>(minColI));
    const types::RowCol<size_t> boundingExtent(maxRowI - minRowI + 1,
                                               maxColI - minColI + 1);

    // Get the PolygonMas. For each row of the polygon this will determine
    // the first and last column of the row inside the convex hull of the
    // polygon sent in.
    const polygon::PolygonMask polygonMask(polygon, fullExtent);
    
    // Compute a delta in the row direction as if the entire bounding row 
    // extent will be covered by the point grid.
    const double initialDeltaRow =
        static_cast<double>(boundingExtent.row - 1) / (numPoints1D - 1);

    // Scale factor for shrinking the row extent.
    const double shrinkFactor = 0.1;

    // Shring the row extent a bit.
    const double deltaToRemove = initialDeltaRow * shrinkFactor;

    // Get the new row start and end values.
    const size_t newStartRow = static_cast<size_t>(
        std::ceil(boundingOffset.row + deltaToRemove));
    const size_t newEndRow = static_cast<size_t>(
        std::floor(boundingOffset.row + boundingExtent.row - 1 -
                   deltaToRemove));

    // Check the new row extent.
    if (newStartRow > newEndRow)
    {
        throw except::Exception(Ctxt(
            "New bounding rectangle has no area"));
    }

    // Compute the row exent.
    const size_t newExtentRow = (newEndRow - newStartRow + 1);
    
    // Compute the delta in the row direction for the new extent.
    const double newDeltaRow =
         static_cast<double>(newExtentRow - 1) / 
         static_cast<double>(numPoints1D - 1);

    double currentOffsetRow = static_cast<double>(newStartRow);
    for (size_t ii = 0; ii < numPoints1D; ++ii, currentOffsetRow += newDeltaRow)
    {
        double currentRow = std::floor(currentOffsetRow);
        size_t row = static_cast<size_t>(currentRow);

        // Get the start column and number of columns inside the polygon row
        // the current row.
        const types::Range colRange = polygonMask.getRange(row);

        // Check that there are internal points.
        if (colRange.mNumElements == 0)
        {
            throw except::Exception(Ctxt(
                "Column range has no elements"));
        }

        // Compute the delta in the column direction to cover the internal
        // points.
        const double newDeltaCol =
            static_cast<double>(colRange.mNumElements - 1) /
            static_cast<double>(numPoints1D - 1);

        double currentCol = static_cast<double>(colRange.mStartElement);
        for (size_t jj = 0; jj < numPoints1D; ++jj, currentCol += newDeltaCol)
        {
            const types::RowCol<double> currentOffset(currentRow, currentCol);
            projectToSlantPlane(projModel, gridTransform, outPixelStart,
                currentOffset, ii, jj);
        }
    }
}

void ProjectionPolynomialFitter::projectToSlantPlane(
    const ProjectionModel& projModel,
    const GridECEFTransform& gridTransform,
    const types::RowCol<double>& outPixelStart,
    const types::RowCol<double>& currentOffset,
    size_t row,
    size_t col)
{
    // Get the coordinate relative to the outPixelStart.
    mOutputPlaneRows(row, col) = currentOffset.row - outPixelStart.row;
    mOutputPlaneCols(row, col) = currentOffset.col - outPixelStart.col;

    // Find ECEF of the output plane pixel.
    const scene::Vector3 ecef =
        gridTransform.rowColToECEF(currentOffset);

    // Project ECEF coordinate into the slant plane and get meters from
    // the slant plane scene center point.
    double timeCOA(0.0);
    mSceneCoordinates(row, col) = projModel.sceneToImage(ecef, &timeCOA);
    mTimeCOA(row, col) = timeCOA;
}

void ProjectionPolynomialFitter::getSlantPlaneSamples(
        const types::RowCol<size_t>& inPixelStart,
        const types::RowCol<double>& inSceneCenter,
        const types::RowCol<double>& interimSceneCenter,
        const types::RowCol<double>& interimSampleSpacing,
        math::linear::Matrix2D<double>& slantPlaneRows,
        math::linear::Matrix2D<double>& slantPlaneCols) const
{
    const types::RowCol<double> ratio(interimSceneCenter / inSceneCenter);

    const types::RowCol<double> aoiOffset(inPixelStart.row * ratio.row,
                                          inPixelStart.col * ratio.col);

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
        }
    }
}

void ProjectionPolynomialFitter::fitOutputToSlantPolynomials(
        const types::RowCol<size_t>& inPixelStart,
        const types::RowCol<double>& inSceneCenter,
        const types::RowCol<double>& interimSceneCenter,
        const types::RowCol<double>& interimSampleSpacing,
        size_t polyOrderX,
        size_t polyOrderY,
        math::poly::TwoD<double>& outputToSlantRow,
        math::poly::TwoD<double>& outputToSlantCol,
        double* meanResidualErrorRow,
        double* meanResidualErrorCol) const
{
    // Collect up slant plane pixel locations for the output plane samples we
    // have
    math::linear::Matrix2D<double> slantPlaneRows(mNumPoints1D, mNumPoints1D);
    math::linear::Matrix2D<double> slantPlaneCols(mNumPoints1D, mNumPoints1D);
    getSlantPlaneSamples(inPixelStart,
                         inSceneCenter,
                         interimSceneCenter,
                         interimSampleSpacing,
                         slantPlaneRows,
                         slantPlaneCols);

    // Now fit the polynomials
    outputToSlantRow = math::poly::fit(mOutputPlaneRows,
                                       mOutputPlaneCols,
                                       slantPlaneRows,
                                       polyOrderX, polyOrderY);

    outputToSlantCol = math::poly::fit(mOutputPlaneRows,
                                       mOutputPlaneCols,
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
                const double row(mOutputPlaneRows(ii, jj));
                const double col(mOutputPlaneCols(ii, jj));

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

void ProjectionPolynomialFitter::fitSlantToOutputPolynomials(
        const types::RowCol<size_t>& inPixelStart,
        const types::RowCol<double>& inSceneCenter,
        const types::RowCol<double>& interimSceneCenter,
        const types::RowCol<double>& interimSampleSpacing,
        size_t polyOrderX,
        size_t polyOrderY,
        math::poly::TwoD<double>& slantToOutputRow,
        math::poly::TwoD<double>& slantToOutputCol,
        double* meanResidualErrorRow,
        double* meanResidualErrorCol) const
{
    // Collect up slant plane pixel locations for the output plane samples we
    // have
    math::linear::Matrix2D<double> slantPlaneRows(mNumPoints1D, mNumPoints1D);
    math::linear::Matrix2D<double> slantPlaneCols(mNumPoints1D, mNumPoints1D);
    getSlantPlaneSamples(inPixelStart,
                         inSceneCenter,
                         interimSceneCenter,
                         interimSampleSpacing,
                         slantPlaneRows,
                         slantPlaneCols);

    // Now fit the polynomials
    slantToOutputRow = math::poly::fit(slantPlaneRows,
                                       slantPlaneCols,
                                       mOutputPlaneRows,
                                       polyOrderX, polyOrderY);

    slantToOutputCol = math::poly::fit(slantPlaneRows,
                                       slantPlaneCols,
                                       mOutputPlaneCols,
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
                const double row(slantPlaneRows(ii, jj));
                const double col(slantPlaneCols(ii, jj));

                double diff =
                        mOutputPlaneRows(ii, jj) - slantToOutputRow(row, col);
                errorSumRow += diff * diff;

                diff = mOutputPlaneCols(ii, jj) - slantToOutputCol(row, col);
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
        const types::RowCol<double>& outPixelShift,
        size_t polyOrderX,
        size_t polyOrderY,
        math::poly::TwoD<double>& timeCOAPoly,
        double* meanResidualError) const
{
    fitPixelBasedTimeCOAPolynomial<Shift, Shift>(Shift(outPixelShift.row),
                                                 Shift(outPixelShift.col),
                                                 polyOrderX,
                                                 polyOrderY,
                                                 timeCOAPoly,
                                                 meanResidualError);
}
}

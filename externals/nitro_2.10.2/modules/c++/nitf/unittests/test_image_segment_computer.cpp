/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2018, MDA Information Systems LLC
 *
 * NITRO is free software; you can redistribute it and/or modify
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
 * License along with this program; if not, If not,
 * see <http://www.gnu.org/licenses/>.
 *
 */

#include <vector>
#include <sstream>

#include <nitf/coda-oss.hpp>
#include <nitf/ImageSegmentComputer.h>
#include <nitf/ImageBlocker.hpp>

#include "TestCase.h"

namespace
{
TEST_CASE(testBlockSizedBoundaries)
{
    // This test is meant to run through a lot of cases just to make sure
    // we never have a segment that is not a multiple of the block size.

    // A very large case that will break into a lot of image segments
    const types::RowCol<size_t> dims(1000000, 100000);

    std::vector<size_t> blockSizes;
    blockSizes.push_back(128);
    blockSizes.push_back(256);
    blockSizes.push_back(512);
    blockSizes.push_back(1024);
    blockSizes.push_back(2048);

    // Multiple of number of rows
    blockSizes.push_back(1000);

    // Add a few prime numbers just to make this difficult
    blockSizes.push_back(677);
    blockSizes.push_back(991);
    blockSizes.push_back(1021);
    blockSizes.push_back(1459);

    std::vector<size_t> dataSizes;
    dataSizes.push_back(1);
    dataSizes.push_back(4);
    dataSizes.push_back(8);

    for (size_t rowIdx = 0; rowIdx < blockSizes.size(); ++rowIdx)
    {
        for (size_t colIdx = 0; colIdx < blockSizes.size(); ++colIdx)
        {
            for (size_t dataIdx = 0; dataIdx < dataSizes.size(); ++dataIdx)
            {
                nitf::ImageSegmentComputer computer(
                        dims.row, dims.col, dataSizes[dataIdx],
                        nitf::ImageSegmentComputer::ILOC_MAX,
                        nitf::ImageSegmentComputer::NUM_BYTES_MAX,
                        blockSizes[rowIdx], blockSizes[colIdx]);

                size_t totalRows = 0;
                const std::vector<nitf::ImageSegmentComputer::Segment> segments =
                        computer.getSegments();
                for (size_t ii = 0; ii < segments.size(); ++ii)
                {
                    TEST_ASSERT_EQ(segments[ii].firstRow % blockSizes[rowIdx], 0);

                    // Only test this if we are not the last segment. The final
                    // segment is not guaranteed to be a multiple of block size.
                    if (ii != segments.size() - 1)
                    {
                        TEST_ASSERT_EQ(segments[ii].numRows % blockSizes[rowIdx], 0);
                    }

                    totalRows += segments[ii].numRows;
                }

                // Make sure we are using all the rows
                TEST_ASSERT_EQ(totalRows, dims.row);
            }
        }
    }

}

TEST_CASE(testPerfectSplitting)
{
    // This will split into two perfectly sized segments.
    const size_t blockSize = 1024;

    const size_t ilocBlockSizeMultiple =
            nitf::ImageSegmentComputer::ILOC_MAX -
            (nitf::ImageSegmentComputer::ILOC_MAX % blockSize);

    const types::RowCol<size_t> dims(
            blockSize * (ilocBlockSizeMultiple / blockSize) * 2,
            blockSize * 5);

    nitf::ImageSegmentComputer computer(
            dims.row, dims.col, 1,
            ilocBlockSizeMultiple,
            ilocBlockSizeMultiple * dims.col,
            blockSize, blockSize);

    const std::vector<nitf::ImageSegmentComputer::Segment> segments =
            computer.getSegments();
    TEST_ASSERT_EQ(segments.size(), 2);
    TEST_ASSERT_EQ(segments[0].firstRow, 0);
    TEST_ASSERT_EQ(segments[0].numRows, ilocBlockSizeMultiple);
    TEST_ASSERT_EQ(segments[1].firstRow, ilocBlockSizeMultiple);
    TEST_ASSERT_EQ(segments[1].numRows, ilocBlockSizeMultiple);
}

TEST_CASE(testOneRowOver)
{
    const size_t blockSize = 1024;

    const size_t ilocBlockSizeMultiple =
            nitf::ImageSegmentComputer::ILOC_MAX -
            (nitf::ImageSegmentComputer::ILOC_MAX % blockSize);

    const types::RowCol<size_t> dims(
            blockSize * (ilocBlockSizeMultiple / blockSize) + 1,
            blockSize * 5);

    nitf::ImageSegmentComputer computer(
            dims.row, dims.col, 1,
            ilocBlockSizeMultiple,
            ilocBlockSizeMultiple * dims.col,
            blockSize, blockSize);

    const std::vector<nitf::ImageSegmentComputer::Segment> segments =
            computer.getSegments();
    TEST_ASSERT_EQ(segments.size(), 2);
    TEST_ASSERT_EQ(segments[0].firstRow, 0);
    TEST_ASSERT_EQ(segments[0].numRows, ilocBlockSizeMultiple);
    TEST_ASSERT_EQ(segments[1].firstRow, ilocBlockSizeMultiple);
    TEST_ASSERT_EQ(segments[1].numRows, 1);
}

static size_t ceilingDivide(size_t numerator, size_t denominator)
{
    if (denominator == 0)
    {
        throw except::Exception(Ctxt("Attempted division by 0"));
    }
    return (numerator / denominator) + (numerator % denominator != 0);
}

TEST_CASE(testKnownCase)
{
    // This tests a known case that was segmenting with too many rows
    // before the update was put in place to for image segments to be
    // a multiple of blockSize.
    const size_t blockSize = 1024;

    const types::RowCol<size_t> dims(383735, 37287);

    nitf::ImageSegmentComputer computer(
            dims.row, dims.col, 1,
            nitf::ImageSegmentComputer::ILOC_MAX,
            nitf::ImageSegmentComputer::NUM_BYTES_MAX,
            blockSize, blockSize);

    const size_t expectedNumRows = 99328;
    const std::vector<nitf::ImageSegmentComputer::Segment> segments =
            computer.getSegments();
    TEST_ASSERT_EQ(segments.size(), 4);
    TEST_ASSERT_EQ(segments[0].firstRow, 0);
    TEST_ASSERT_EQ(segments[0].numRows, expectedNumRows);
    TEST_ASSERT_EQ(segments[1].firstRow, expectedNumRows);
    TEST_ASSERT_EQ(segments[1].numRows, expectedNumRows);
    TEST_ASSERT_EQ(segments[2].firstRow, expectedNumRows * 2);
    TEST_ASSERT_EQ(segments[2].numRows, expectedNumRows);
    TEST_ASSERT_EQ(segments[3].firstRow, expectedNumRows * 3);
    TEST_ASSERT_EQ(segments[3].numRows, dims.row - segments[3].firstRow);

    std::vector<size_t> numRowsPerSegment(segments.size());
    for (size_t ii = 0; ii < numRowsPerSegment.size(); ++ii)
    {
        numRowsPerSegment[ii] = segments[ii].numRows;
    }

    const nitf::ImageBlocker blocker(numRowsPerSegment,
                                     dims.col,
                                     blockSize,
                                     blockSize);

    const size_t expectedSize = blockSize * ceilingDivide(dims.col, blockSize) * blockSize;

    for (size_t row = 0; row < dims.row; row += blockSize)
    {
        // Just make sure we don't throw an exception here. If the segment
        // boundaries are not setup correctly, it will throw at the segment
        // boundary because the next image row is not a multiple of the
        // block size.
        const size_t size = blocker.getNumBytesRequired(
                row, std::min(blockSize, dims.row - row), 1);
        TEST_ASSERT_EQ(size, expectedSize);
    }

}
}


TEST_MAIN
(
    (void)argc;
    (void)argv;

    TEST_CHECK(testBlockSizedBoundaries);
    TEST_CHECK(testPerfectSplitting);
    TEST_CHECK(testOneRowOver);
    TEST_CHECK(testKnownCase);
    )
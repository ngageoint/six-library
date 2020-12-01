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

#include <nitf/ImageBlocker.hpp>

#include "TestCase.h"

namespace
{
TEST_CASE(testSingleSegmentNoLeftovers)
{
    // 4 rows of blocks and 5 cols of blocks
    static const size_t NUM_ROWS = 12;
    static const size_t NUM_COLS = 20;
    static const size_t NUM_ROWS_PER_BLOCK = 3;
    static const size_t NUM_COLS_PER_BLOCK = 4;

    // 1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16 17 18 19 20
    // 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40
    // ...
    // 220 ...                                                  240
    std::vector<size_t> input(NUM_ROWS * NUM_COLS);
    for (size_t ii = 0; ii < input.size(); ++ii)
    {
        input[ii] = ii + 1;
    }

    const nitf::ImageBlocker blocker(NUM_ROWS,
                                     NUM_COLS,
                                     NUM_ROWS_PER_BLOCK,
                                     NUM_COLS_PER_BLOCK);

    static const size_t NUM_OUTPUT_PIXELS = NUM_ROWS * NUM_COLS;
    TEST_ASSERT_EQ(blocker.getNumBytesRequired<size_t>(0, NUM_ROWS),
                   NUM_OUTPUT_PIXELS * sizeof(size_t));

    std::vector<size_t> output(NUM_OUTPUT_PIXELS, 99999);
    blocker.block(&input[0], 0, NUM_ROWS, &output[0]);

    for (size_t rowBlock = 0, idx = 0, rowOffset = 1;
         rowBlock < 4;
         ++rowBlock, rowOffset += NUM_COLS * NUM_ROWS_PER_BLOCK)
    {
        for (size_t colBlock = 0, colOffset = 0;
             colBlock < 5;
             ++colBlock, colOffset += NUM_COLS_PER_BLOCK)
        {
            for (size_t row = 0; row < NUM_ROWS_PER_BLOCK; ++row)
            {
                for (size_t col = 0; col < NUM_COLS_PER_BLOCK; ++col, ++idx)
                {
                    std::ostringstream ostr;
                    ostr << "Row block " << rowBlock << ", col block "
                         << colBlock << ", row " << row << ", col " << col;

                    const size_t expectedVal =
                            rowOffset + row * NUM_COLS + colOffset + col;

                    TEST_ASSERT_EQ_MSG(ostr.str(), output[idx], expectedVal);
                }
            }
        }
    }
}

TEST_CASE(testSingleSegmentPadCols)
{
    // 4 rows of blocks and 3 cols of blocks with 1 pad col
    static const size_t NUM_ROWS = 12;
    static const size_t NUM_COLS = 20;
    static const size_t NUM_ROWS_PER_BLOCK = 3;
    static const size_t NUM_COLS_PER_BLOCK = 7;

    // 1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16 17 18 19 20
    // 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40
    // ...
    // 220 ...                                                  240
    std::vector<size_t> input(NUM_ROWS * NUM_COLS);
    for (size_t ii = 0; ii < input.size(); ++ii)
    {
        input[ii] = ii + 1;
    }

    const nitf::ImageBlocker blocker(NUM_ROWS,
                                     NUM_COLS,
                                     NUM_ROWS_PER_BLOCK,
                                     NUM_COLS_PER_BLOCK);

    static const size_t NUM_OUTPUT_PIXELS = NUM_ROWS * (NUM_COLS + 1);
    TEST_ASSERT_EQ(blocker.getNumBytesRequired<size_t>(0, NUM_ROWS),
                   NUM_OUTPUT_PIXELS * sizeof(size_t));

    std::vector<size_t> output(NUM_OUTPUT_PIXELS, 99999);
    blocker.block(&input[0], 0, NUM_ROWS, &output[0]);

    for (size_t rowBlock = 0, idx = 0, rowOffset = 1;
         rowBlock < 4;
         ++rowBlock, rowOffset += NUM_COLS * NUM_ROWS_PER_BLOCK)
    {
        for (size_t colBlock = 0, colOffset = 0;
             colBlock < 3;
             ++colBlock, colOffset += NUM_COLS_PER_BLOCK)
        {
            for (size_t row = 0; row < NUM_ROWS_PER_BLOCK; ++row)
            {
                for (size_t col = 0; col < NUM_COLS_PER_BLOCK; ++col, ++idx)
                {
                    std::ostringstream ostr;
                    ostr << "Row block " << rowBlock << ", col block "
                         << colBlock << ", row " << row << ", col " << col;

                    const size_t expectedVal = (colOffset + col >= NUM_COLS) ?
                            0 : rowOffset + row * NUM_COLS + colOffset + col;

                    TEST_ASSERT_EQ_MSG(ostr.str(), output[idx], expectedVal);
                }
            }
        }
    }
}

TEST_CASE(testSingleSegmentPadRowsAndPadCols)
{
    // 2 rows of blocks and 3 cols of blocks with 4 pad rows and 1 pad col
    static const size_t NUM_ROWS = 12;
    static const size_t NUM_COLS = 20;
    static const size_t NUM_ROWS_PER_BLOCK = 8;
    static const size_t NUM_COLS_PER_BLOCK = 7;

    // 1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16 17 18 19 20
    // 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40
    // ...
    // 220 ...                                                  240
    std::vector<size_t> input(NUM_ROWS * NUM_COLS);
    for (size_t ii = 0; ii < input.size(); ++ii)
    {
        input[ii] = ii + 1;
    }

    const nitf::ImageBlocker blocker(NUM_ROWS,
                                     NUM_COLS,
                                     NUM_ROWS_PER_BLOCK,
                                     NUM_COLS_PER_BLOCK);

    static const size_t NUM_OUTPUT_PIXELS = (NUM_ROWS + 4) * (NUM_COLS + 1);
    TEST_ASSERT_EQ(blocker.getNumBytesRequired<size_t>(0, NUM_ROWS),
                   NUM_OUTPUT_PIXELS * sizeof(size_t));

    std::vector<size_t> output(NUM_OUTPUT_PIXELS, 99999);
    blocker.block(&input[0], 0, NUM_ROWS, &output[0]);

    for (size_t rowBlock = 0, idx = 0, rowOffset = 1;
         rowBlock < 2;
         ++rowBlock, rowOffset += NUM_COLS * NUM_ROWS_PER_BLOCK)
    {
        for (size_t colBlock = 0, colOffset = 0;
             colBlock < 3;
             ++colBlock, colOffset += NUM_COLS_PER_BLOCK)
        {
            for (size_t row = 0; row < NUM_ROWS_PER_BLOCK; ++row)
            {
                for (size_t col = 0; col < NUM_COLS_PER_BLOCK; ++col, ++idx)
                {
                    std::ostringstream ostr;
                    ostr << "Row block " << rowBlock << ", col block "
                         << colBlock << ", row " << row << ", col " << col;

                    const size_t expectedVal =
                            (rowBlock * NUM_ROWS_PER_BLOCK + row >= NUM_ROWS ||
                             colOffset + col >= NUM_COLS) ?
                                    0 :
                                    rowOffset + row * NUM_COLS + colOffset + col;

                    TEST_ASSERT_EQ_MSG(ostr.str(), output[idx], expectedVal);
                }
            }
        }
    }
}

TEST_CASE(testMultipleSegmentsNoLeftovers)
{
    // 4 rows of blocks and 5 cols of blocks
    static const size_t NUM_ROWS = 12;
    static const size_t NUM_COLS = 20;
    static const size_t NUM_ROWS_PER_BLOCK = 3;
    static const size_t NUM_COLS_PER_BLOCK = 4;

    // 1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16 17 18 19 20
    // 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40
    // ...
    // 220 ...                                                  240
    std::vector<size_t> input(NUM_ROWS * NUM_COLS);
    for (size_t ii = 0; ii < input.size(); ++ii)
    {
        input[ii] = ii + 1;
    }

    // Want these to hit block boundaries
    std::vector<size_t> numRowsPerSegment(2);
    numRowsPerSegment[0] = 3;
    numRowsPerSegment[1] = 9;

    const nitf::ImageBlocker blocker(numRowsPerSegment,
                                     NUM_COLS,
                                     NUM_ROWS_PER_BLOCK,
                                     NUM_COLS_PER_BLOCK);

    static const size_t NUM_OUTPUT_PIXELS = NUM_ROWS * NUM_COLS;
    TEST_ASSERT_EQ(blocker.getNumBytesRequired<size_t>(0, NUM_ROWS),
                   NUM_OUTPUT_PIXELS * sizeof(size_t));

    std::vector<size_t> output(NUM_OUTPUT_PIXELS, 99999);
    blocker.block(&input[0], 0, NUM_ROWS, &output[0]);

    for (size_t rowBlock = 0, idx = 0, rowOffset = 1;
         rowBlock < 4;
         ++rowBlock, rowOffset += NUM_COLS * NUM_ROWS_PER_BLOCK)
    {
        for (size_t colBlock = 0, colOffset = 0;
             colBlock < 5;
             ++colBlock, colOffset += NUM_COLS_PER_BLOCK)
        {
            for (size_t row = 0; row < NUM_ROWS_PER_BLOCK; ++row)
            {
                for (size_t col = 0; col < NUM_COLS_PER_BLOCK; ++col, ++idx)
                {
                    std::ostringstream ostr;
                    ostr << "Row block " << rowBlock << ", col block "
                         << colBlock << ", row " << row << ", col " << col;

                    const size_t expectedVal =
                            rowOffset + row * NUM_COLS + colOffset + col;

                    TEST_ASSERT_EQ_MSG(ostr.str(), output[idx], expectedVal);
                }
            }
        }
    }
}

TEST_CASE(testMultipleSegmentsPartialRowsOnSegmentBoundaries)
{
    // 5 rows of blocks (because of segment layout) and 5 cols of blocks
    static const size_t NUM_ROWS = 12;
    static const size_t NUM_COLS = 20;
    static const size_t NUM_ROWS_PER_BLOCK = 3;
    static const size_t NUM_COLS_PER_BLOCK = 4;

    // 1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16 17 18 19 20
    // 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40
    // ...
    // 220 ...                                                  240
    std::vector<size_t> input(NUM_ROWS * NUM_COLS);
    for (size_t ii = 0; ii < input.size(); ++ii)
    {
        input[ii] = ii + 1;
    }

    // First segment will get one pad row
    // Second segment will get two pad rows
    std::vector<size_t> numRowsPerSegment(2);
    numRowsPerSegment[0] = 5;
    numRowsPerSegment[1] = 7;

    const nitf::ImageBlocker blocker(numRowsPerSegment,
                                     NUM_COLS,
                                     NUM_ROWS_PER_BLOCK,
                                     NUM_COLS_PER_BLOCK);

    static const size_t NUM_OUTPUT_PIXELS = (NUM_ROWS + 3) * NUM_COLS;
    TEST_ASSERT_EQ(blocker.getNumBytesRequired<size_t>(0, NUM_ROWS),
                   NUM_OUTPUT_PIXELS * sizeof(size_t));

    std::vector<size_t> output(NUM_OUTPUT_PIXELS, 99999);
    blocker.block(&input[0], 0, NUM_ROWS, &output[0]);

    for (size_t rowBlock = 0, idx = 0, imageRowBase = 0;
         rowBlock < 5;
         ++rowBlock)
    {
        size_t imageRow;
        for (size_t colBlock = 0, colOffset = 0;
             colBlock < 5;
             ++colBlock, colOffset += NUM_COLS_PER_BLOCK)
        {
            imageRow = imageRowBase;

            for (size_t row = 0; row < NUM_ROWS_PER_BLOCK; ++row)
            {
                const bool padRow = (rowBlock == 1 && row >= 2 ) ||
                        (rowBlock >= 4 && row >= 1);

                for (size_t col = 0; col < NUM_COLS_PER_BLOCK; ++col, ++idx)
                {
                    std::ostringstream ostr;
                    ostr << "Row block " << rowBlock << ", col block "
                         << colBlock << ", row " << row << ", col " << col;

                    const size_t expectedVal = padRow ?
                            0 : 1 + imageRow * NUM_COLS + colOffset + col;

                    TEST_ASSERT_EQ_MSG(ostr.str(), output[idx], expectedVal);
                }

                if (!padRow)
                {
                    ++imageRow;
                }
            }
        }

        imageRowBase = imageRow;
    }
}

TEST_CASE(testMultipleSegmentsPartialRowsOnSegmentBoundariesWithPadCols)
{
    // 5 rows of blocks (because of segment layout) and 3 cols of blocks with 1
    // pad col
    static const size_t NUM_ROWS = 12;
    static const size_t NUM_COLS = 20;
    static const size_t NUM_ROWS_PER_BLOCK = 3;
    static const size_t NUM_COLS_PER_BLOCK = 7;

    // 1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16 17 18 19 20
    // 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40
    // ...
    // 220 ...                                                  240
    std::vector<size_t> input(NUM_ROWS * NUM_COLS);
    for (size_t ii = 0; ii < input.size(); ++ii)
    {
        input[ii] = ii + 1;
    }

    // First segment will get one pad row
    // Second segment will get two pad rows
    std::vector<size_t> numRowsPerSegment(2);
    numRowsPerSegment[0] = 5;
    numRowsPerSegment[1] = 7;

    const nitf::ImageBlocker blocker(numRowsPerSegment,
                                     NUM_COLS,
                                     NUM_ROWS_PER_BLOCK,
                                     NUM_COLS_PER_BLOCK);

    static const size_t NUM_OUTPUT_PIXELS = (NUM_ROWS + 3) * (NUM_COLS + 1);
    TEST_ASSERT_EQ(blocker.getNumBytesRequired<size_t>(0, NUM_ROWS),
                   NUM_OUTPUT_PIXELS * sizeof(size_t));

    std::vector<size_t> output(NUM_OUTPUT_PIXELS, 99999);
    blocker.block(&input[0], 0, NUM_ROWS, &output[0]);

    for (size_t rowBlock = 0, idx = 0, imageRowBase = 0;
         rowBlock < 5;
         ++rowBlock)
    {
        size_t imageRow;
        for (size_t colBlock = 0, colOffset = 0;
             colBlock < 3;
             ++colBlock, colOffset += NUM_COLS_PER_BLOCK)
        {
            imageRow = imageRowBase;

            for (size_t row = 0; row < NUM_ROWS_PER_BLOCK; ++row)
            {
                const bool padRow = (rowBlock == 1 && row >= 2 ) ||
                        (rowBlock >= 4 && row >= 1);

                for (size_t col = 0; col < NUM_COLS_PER_BLOCK; ++col, ++idx)
                {
                    std::ostringstream ostr;
                    ostr << "Row block " << rowBlock << ", col block "
                         << colBlock << ", row " << row << ", col " << col;

                    const size_t expectedVal =
                            (padRow || colOffset + col >= NUM_COLS) ?
                                    0 :
                                    1 + imageRow * NUM_COLS + colOffset + col;

                    TEST_ASSERT_EQ_MSG(ostr.str(), output[idx], expectedVal);
                }

                if (!padRow)
                {
                    ++imageRow;
                }
            }
        }

        imageRowBase = imageRow;
    }
}

TEST_CASE(testBlockPartialImage)
{
    // 7 rows of blocks and 5 cols of blocks
    static const size_t NUM_ROWS = 21;
    static const size_t NUM_COLS = 20;
    static const size_t NUM_ROWS_PER_BLOCK = 3;
    static const size_t NUM_COLS_PER_BLOCK = 4;

    // 1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16 17 18 19 20
    // 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40
    // ...
    // 220 ...                                                  240

    // But only do the middle three blocks - must land on block boundary
    static const size_t START_ROW = 6;
    static const size_t NUM_ROWS_TO_BLOCK = 9;
    std::vector<size_t> input(NUM_ROWS_TO_BLOCK * NUM_COLS);
    static const size_t BASE_VAL = 1 + START_ROW * NUM_COLS;
    for (size_t ii = 0; ii < input.size(); ++ii)
    {
        input[ii] = ii + BASE_VAL;
    }

    const nitf::ImageBlocker blocker(NUM_ROWS,
                                     NUM_COLS,
                                     NUM_ROWS_PER_BLOCK,
                                     NUM_COLS_PER_BLOCK);

    static const size_t NUM_OUTPUT_PIXELS = NUM_ROWS_TO_BLOCK * NUM_COLS;
    TEST_ASSERT_EQ(blocker.getNumBytesRequired<size_t>(START_ROW,
                                                       NUM_ROWS_TO_BLOCK),
                   NUM_OUTPUT_PIXELS * sizeof(size_t));

    std::vector<size_t> output(NUM_OUTPUT_PIXELS, 99999);
    blocker.block(&input[0], START_ROW, NUM_ROWS_TO_BLOCK, &output[0]);

    for (size_t rowBlock = 0, idx = 0, rowOffset = BASE_VAL;
         rowBlock < 3;
         ++rowBlock, rowOffset += NUM_COLS * NUM_ROWS_PER_BLOCK)
    {
        for (size_t colBlock = 0, colOffset = 0;
             colBlock < 5;
             ++colBlock, colOffset += NUM_COLS_PER_BLOCK)
        {
            for (size_t row = 0; row < NUM_ROWS_PER_BLOCK; ++row)
            {
                for (size_t col = 0; col < NUM_COLS_PER_BLOCK; ++col, ++idx)
                {
                    std::ostringstream ostr;
                    ostr << "Row block " << rowBlock << ", col block "
                         << colBlock << ", row " << row << ", col " << col;

                    const size_t expectedVal =
                            rowOffset + row * NUM_COLS + colOffset + col;

                    TEST_ASSERT_EQ_MSG(ostr.str(), output[idx], expectedVal);
                }
            }
        }
    }
}
}

TEST_MAIN(
    (void)argc;
    (void)argv;

    TEST_CHECK(testSingleSegmentNoLeftovers);
    TEST_CHECK(testSingleSegmentPadCols);
    TEST_CHECK(testSingleSegmentPadRowsAndPadCols);
    TEST_CHECK(testMultipleSegmentsNoLeftovers);
    TEST_CHECK(testMultipleSegmentsPartialRowsOnSegmentBoundaries);
    TEST_CHECK(testMultipleSegmentsPartialRowsOnSegmentBoundariesWithPadCols);
    TEST_CHECK(testBlockPartialImage);
    )

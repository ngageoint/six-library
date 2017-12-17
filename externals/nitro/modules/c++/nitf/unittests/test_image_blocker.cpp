/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2017, MDA Information Systems LLC
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

    const nitf::ImageBlocker blocker(std::vector<size_t>(1, NUM_ROWS),
                                     NUM_COLS,
                                     NUM_ROWS_PER_BLOCK,
                                     NUM_COLS_PER_BLOCK);

    std::vector<size_t> output(NUM_ROWS * NUM_COLS, 99999);
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

    const nitf::ImageBlocker blocker(std::vector<size_t>(1, NUM_ROWS),
                                     NUM_COLS,
                                     NUM_ROWS_PER_BLOCK,
                                     NUM_COLS_PER_BLOCK);

    std::vector<size_t> output(NUM_ROWS * (NUM_COLS + 1), 99999);
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

    const nitf::ImageBlocker blocker(std::vector<size_t>(1, NUM_ROWS),
                                     NUM_COLS,
                                     NUM_ROWS_PER_BLOCK,
                                     NUM_COLS_PER_BLOCK);

    std::vector<size_t> output((NUM_ROWS + 4) * (NUM_COLS + 1), 99999);
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
}

int main(int /*argc*/, char** /*argv*/)
{
    TEST_CHECK(testSingleSegmentNoLeftovers);
    TEST_CHECK(testSingleSegmentPadCols);
    TEST_CHECK(testSingleSegmentPadRowsAndPadCols);

    return 0;
}

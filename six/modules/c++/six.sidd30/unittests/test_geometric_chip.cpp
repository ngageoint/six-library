/* =========================================================================
 * This file is part of six.sidd30-c++
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2016, MDA Information Systems LLC
 *
 * six.sidd30-c++ is free software; you can redistribute it and/or modify
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

#include <six/sidd30/DownstreamReprocessing.h>
#include "TestCase.h"

TEST_CASE(testGeometricChip)
{
    six::sidd30::GeometricChip chip;
    chip.chipSize.row = 1000;
    chip.chipSize.col = 1000;

    chip.originalUpperLeftCoordinate.row = 146.9986;
    chip.originalUpperLeftCoordinate.col = 462.7939;

    chip.originalUpperRightCoordinate.row = 346.9986;
    chip.originalUpperRightCoordinate.col = 365.9017;

    chip.originalLowerRightCoordinate.row = 256.5206;
    chip.originalLowerRightCoordinate.col = 179.1415;

    chip.originalLowerLeftCoordinate.row = 56.5206;
    chip.originalLowerLeftCoordinate.col = 276.0336;

    // Convert from full --> chip
    const six::RowColDouble fullCoord(180.2995, 415.5702);
    static const double TOL = 0.001;

    const six::RowColDouble computedChipCoord =
            chip.getChipCoordinateFromFullImage(fullCoord);
    TEST_ASSERT_ALMOST_EQ_EPS(computedChipCoord.row, 134.6943, TOL);
    TEST_ASSERT_ALMOST_EQ_EPS(computedChipCoord.col, 227.2724, TOL);

    // Convert back from chip --> full
    const six::RowColDouble computedFullCoord =
            chip.getFullImageCoordinateFromChip(computedChipCoord);
    TEST_ASSERT_ALMOST_EQ_EPS(computedFullCoord.row, fullCoord.row, TOL);
    TEST_ASSERT_ALMOST_EQ_EPS(computedFullCoord.col, fullCoord.col, TOL);
}

TEST_MAIN(
    TEST_CHECK(testGeometricChip);
)


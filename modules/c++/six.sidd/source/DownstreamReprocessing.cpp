/* =========================================================================
 * This file is part of six.sidd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2013, General Dynamics - Advanced Information Systems
 *
 * six.sidd-c++ is free software; you can redistribute it and/or modify
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
#include "six/sidd/DownstreamReprocessing.h"

namespace six
{
namespace sidd
{
RowColDouble GeometricChip::getFullImageCoordinateFromChip(
        const RowColDouble& chip) const
{
    // From section 5.1.1 of SIDD Volume 1

    // Normalize the chip coordinates
    const double u = chip.row / (chipSize.row - 1);
    const double v = chip.col / (chipSize.col - 1);
    const double uv = u * v;

    // Compute original full image row/col coordinate bilinear coefficients
    const RowColDouble A = originalUpperLeftCoordinate;
    const RowColDouble B =
            originalLowerLeftCoordinate - originalUpperLeftCoordinate;
    const RowColDouble D =
            originalUpperRightCoordinate - originalUpperLeftCoordinate;
    const RowColDouble F =
            originalUpperLeftCoordinate + originalLowerRightCoordinate -
            originalUpperRightCoordinate - originalLowerLeftCoordinate;

    // Compute full image row and col coordinates
    const RowColDouble fullImage(
            A.row + u * B.row + v * D.row + uv * F.row,
            A.col + u * B.col + v * D.col + uv * F.col);
    return fullImage;
}

RowColDouble
GeometricChip::getChipCoordinateFromFullImage(const RowColDouble& full) const
{
    if (originalUpperLeftCoordinate.row == originalUpperRightCoordinate.row &&
        originalLowerLeftCoordinate.row == originalLowerLeftCoordinate.row &&
        originalUpperLeftCoordinate.row < originalLowerLeftCoordinate.row &&
        originalUpperLeftCoordinate.col == originalLowerLeftCoordinate.col &&
        originalUpperRightCoordinate.col == originalLowerRightCoordinate.col &&
        originalUpperLeftCoordinate.col < originalUpperRightCoordinate.col)
    {
        return RowColDouble(full.row - originalUpperLeftCoordinate.row,
                            full.col - originalUpperLeftCoordinate.col);
    }
    else
    {
        // TODO: Derive equations to do 5.1.1 in reverse
        throw except::NotImplementedException(Ctxt(
                "Only square chipping is implemented currently"));
    }
}
}
}

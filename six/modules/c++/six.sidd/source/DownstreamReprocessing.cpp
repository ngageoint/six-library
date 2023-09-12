/* =========================================================================
 * This file is part of six.sidd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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
    const auto u = chip.row / static_cast<double>(chipSize.row - 1);
    const auto v = chip.col / static_cast<double>(chipSize.col - 1);
    const auto uv = u * v;

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
    // TODO: Since the chip is always rectangular, the F term will always be
    //       0 which means the uv term itself zeros out as well.
    const RowColDouble fullImage(
            A.row + u * B.row + v * D.row + uv * F.row,
            A.col + u * B.col + v * D.col + uv * F.col);
    return fullImage;
}

RowColDouble
GeometricChip::getChipCoordinateFromFullImage(const RowColDouble& full) const
{
    const RowColDouble A = originalUpperLeftCoordinate;
    const RowColDouble B =
            originalLowerLeftCoordinate - originalUpperLeftCoordinate;
    const RowColDouble D =
            originalUpperRightCoordinate - originalUpperLeftCoordinate;

    // NOTE: This is based on the logic above - the uv term is always 0 since
    //       F is always 0.  Should update the SIDD spec to clarify this.
    const double v =
            (full.col - A.col - full.row * B.col / B.row +
                 A.row * B.col / B.row) /
            (D.col - D.row * B.col / B.row);

    const double u = (full.row - A.row - v * D.row) / B.row;

    const RowColDouble chip(u * static_cast<double>(chipSize.row - 1),
                            v * static_cast<double>(chipSize.col - 1));
    return chip;
}

bool GeometricChip::operator==(const GeometricChip& rhs) const
{
    return (chipSize == rhs.chipSize &&
        originalUpperLeftCoordinate == rhs.originalUpperLeftCoordinate &&
        originalUpperRightCoordinate == rhs.originalUpperRightCoordinate &&
        originalLowerLeftCoordinate == rhs.originalLowerLeftCoordinate &&
        originalLowerRightCoordinate == rhs.originalLowerRightCoordinate);
}

bool ProcessingEvent::operator==(const ProcessingEvent& rhs) const
{
    return (applicationName == rhs.applicationName &&
        appliedDateTime == rhs.appliedDateTime &&
        interpolationMethod == rhs.interpolationMethod &&
        descriptor == rhs.descriptor);
}
}
}

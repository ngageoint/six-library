/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2017, MDA Information Systems LLC
 * (C) Copyright 2022, Maxar Technologies, Inc.
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

#include "nitf/J2KCompressionParameters.hpp"

j2k::CompressionParameters::CompressionParameters(const types::RowCol<size_t>& rawImageDims, const types::RowCol<size_t>& tileDims,
    double compressionRatio, size_t numResolutions) noexcept:
    mRawImageDims(rawImageDims), mTileDims(tileDims), mCompressionRatio(compressionRatio), mNumResolutions(numResolutions),
    mNumRowsOfTiles((mRawImageDims.row / mTileDims.row) + ((mRawImageDims.row % mTileDims.row) ? 1 : 0)),
    mNumColsOfTiles((mRawImageDims.col / mTileDims.col) + ((mRawImageDims.col % mTileDims.col) ? 1 : 0)),
    mNumTiles(mNumRowsOfTiles * mNumColsOfTiles)
{
}

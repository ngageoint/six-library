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
#ifndef NITF_J2KCompressionParameters_hpp_INCLUDED_
#define NITF_J2KCompressionParameters_hpp_INCLUDED_
#pragma once

#include <types/RowCol.h>

#include "nitf/exports.hpp"

namespace j2k
{
class NITRO_NITFCPP_API CompressionParameters final
{
    const types::RowCol<size_t> mRawImageDims;
    const types::RowCol<size_t> mTileDims;
    const double mCompressionRatio = 1.0;
    const size_t mNumResolutions = 6;
    const size_t mNumRowsOfTiles;     //! Number of rows of tiles (rounded up if a partial tile is present).
    const size_t mNumColsOfTiles;     //! Number of columns of tiles (rounded up if a partial tile is present).
    const size_t mNumTiles;     //! Total number of tiles (includes partial tiles).

public:
   /*!
    * Constructor
    *
    * \param rawImageDims The dimensions of the raw image.
    *
    * \param tileDims The desired tile dimensions to use when tiling the
    * raw image data.
    *
    * \param compressionRatio Represents the factor of compression (i.e a
    * times compressed).
    *
    * \param numResolutions Number of DWT decompositions + 1. Defaults to
    * 6.
    */
    CompressionParameters(const types::RowCol<size_t>& rawImageDims, const types::RowCol<size_t>& tileDims,
            double compressionRatio = 1, size_t numResolutions = 6) noexcept;
    CompressionParameters(const CompressionParameters&) = default;
    CompressionParameters& operator=(const CompressionParameters&) = delete;
    CompressionParameters(CompressionParameters&&) = default;
    CompressionParameters& operator=(CompressionParameters&&) = delete;
    ~CompressionParameters() = default;

    types::RowCol<size_t> getRawImageDims() const noexcept
    {
        return mRawImageDims;
    }

    types::RowCol<size_t> getTileDims() const noexcept
    {
        return mTileDims;
    }

    double getCompressionRatio() const noexcept
    {
        return mCompressionRatio;
    }

    size_t getNumResolutions() const noexcept
    {
        return mNumResolutions;
    }

    size_t getNumRowsOfTiles() const noexcept
    {
        return mNumRowsOfTiles;
    }

    size_t getNumColsOfTiles() const noexcept
    {
        return mNumColsOfTiles;
    }

    size_t getNumTiles() const noexcept
    {
        return mNumTiles;
    }
};
}

#endif // NITF_J2KCompressionParameters_hpp_INCLUDED_


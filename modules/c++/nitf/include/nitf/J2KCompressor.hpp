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

#ifndef NITF_J2KCompressor_hpp_INCLUDED_
#define NITF_J2KCompressor_hpp_INCLUDED_
#pragma once

#include <std/cstddef> // std::byte
#include <vector>
#include <std/span>

#include <io/SeekableStreams.h>
#include <sys/Conf.h>
#include <types/RowCol.h>
#include <types/Range.h>
#include <gsl/gsl.h>

#include "nitf/exports.hpp"
#include "nitf/J2KCompressionParameters.hpp"

namespace j2k
{
    /*!
     * \class OPJCompressor
     * \brief Compresses a raw image using openjpeg.  Compressed image is blocked
     * in a format that aligns with JPEG-2000 compression (each block/tile is
     * compressed independently) and with NITF file format expectations (a
     * J2K tile == a NITF block)
     */
    class NITRO_NITFCPP_API Compressor final
    {
        CompressionParameters mCompressionParams;
        size_t mNumThreads = 1;

    public:
        /*!
         * Constructor
         *
         * \param The J2K compression parameters.
         */
        Compressor(const CompressionParameters& compressionParams, size_t numThreads = 1) noexcept;

        Compressor(const Compressor&) = delete;
        Compressor& operator=(const Compressor&) = delete;
        Compressor(Compressor&&) = default;
        Compressor& operator=(Compressor&&) = delete;

        /*!
         * Calculate the bytes required to allocate for compression.
         * There is no way to know exactly how many bytes are needed
         * before the compression actually happens. This is because
         * there are pathological cases where compression actually
         * results in a larger image. Therefore, we demand twice
         * the size of the input image, and hopefully this is enough.
         *
         * \return Max bytes required to compress the entire image
         */
        size_t getMaxBytesRequiredToCompress() const noexcept;

        /*!
         * Calculate how many bytes caller should supply to compress
         * a given number of tiles
         * \param numTiles How many tiles do you want to compress?
         * \return Bytes required to compress number of tiles
         */
        size_t getMaxBytesRequiredToCompress(size_t numTiles) const noexcept;

        /*!
         * Accessor to return the number of layers.
         * At present, we only ever write one
         * \return How many layers are in compressed image
         */
        size_t getNumLayers() const noexcept { return 1; }

        /*!
         * Accessor to get the bit rate.
         * This is equivalent to the compression ratio
         * \return Bit rate
         */
        size_t getCompressionRatio() const noexcept
        {
            return gsl::narrow_cast<size_t>(mCompressionParams.getCompressionRatio());
        }

        /*!
         * Compresses a raw image. This overload will take a vector,
         * resize it appropriately, and write the compressed image to it
         *
         * \param rawImageData The raw image to compress.
         * \param numThreads Number of threads to use when compressing.
         * \param[out] compressedData The compressed image data.
         * \param[out] bytesPerTile Number of bytes in each compressed tile.
         */
        void compress(std::span<const std::byte> rawImageData,
            std::vector<std::byte>& compressedData,
            std::vector<size_t>& bytesPerTile) const;

        /*!
         * Compresses a raw image. This overload lets you pass in your own
         * pre-allocated memory.
         *
         * \param rawImageData The raw image to compress.
         * \param numThreads Number of threads to use when compressing.
         * \param[in,out] compressedData The pre-allocated compressed image data.
         * Must be at least getMaxBytesRequiredToCompress() bytes.  On output, will
         * contain the compressed image.
         * \param[out] bytesPerTile Number of bytes in each compressed tile.
         */
        std::span<std::byte> compress(std::span<const std::byte> rawImageData,
            std::span<std::byte> compressedData,
            std::vector<size_t>& bytesPerTile) const;

        /*!
         * Compresses a raw image tile.  This will include the image header and
         * footer in the output if it is the first or last file, as appropriate.
         *
         * \param rawImageData The raw image to compress.
         * \param tileIndex Index of tile.
         * \param[out] compressedTile The compressed tile.
         */
        void compressTile(std::span<const std::byte> rawImageData,
            size_t tileIndex,
            std::vector<std::byte>& compressedTile) const;

        /*!
         * Compresses a raw image tile.  This will include the image header and
         * footer in the output if it is the first or last file, as appropriate.
         *
         * \param rawImageData The raw image to compress.
         * \param tileIndex Index of tile.
         * \param[in,out] compressedTile The compressed tile. Data should be
         *  allocated before calling, with compressedTile.size set to the number of
         *  bytes available (which must be at least
         *  getMaxBytesRequiredToCompress(1).  After compression, size will be set
         *  to the number of compressed bytes actually written to the tile, and
         *  data will point to the compressed tile.
         */
        std::span<std::byte> compressTile(std::span<const std::byte> rawImageData,
            size_t tileIndex,
            std::span<std::byte> compressedTile) const;

        /*!
         * Compresses the local portion of the image, taking into account that it
         * is a slice of a larger global image (the tile numbering stored in the
         * J2K-compressed data reflects the global tile numbers).
         *
        * \param rawImageData The local raw image to compress.
        * \param globalStartRow Global start row corresponding to the 0th row of
        * 'rawImageData'.  Must be a multiple of the # of rows in a tile.
        * \param numLocalRows Number of rows in 'rawImageData'.  Must be either
        * a multiple of the # of rows in a tile or this AOI must encompass the
        * bottom of the image.
        * \param numThreads Number of threads to use when compressing.
        * \param[in,out] compressedData The pre-allocated compressed image data.
        * Must be at least getMaxBytesRequiredToCompress(numTilesInThisRowRange)
        * bytes.  On output, will contain the compressed image.
        * \param[out] tileRange Global tile range that corresponds to the
        * rows that were compressed
        * \param[out] bytesPerTile Number of bytes in each compressed tile.  This
        * is tileRange.mNumElements long, where index 0 corresponds to global tile
        * tileRange.mStartElement.
         */
        std::span<std::byte> compressRowSubrange(std::span<const std::byte> rawImageData,
            size_t globalStartRow,
            size_t numLocalRows,
            std::span<std::byte> compressedData,
            types::Range& tileRange,
            std::vector<size_t>& bytesPerTile) const;

        /*!
         * Compresses the local portion of the image, taking into account that it
         * is a slice of a larger global image (the tile numbering stored in the
         * J2K-compressed data reflects the global tile numbers).
         *
        * \param rawImageData The local raw image to compress.
        * \param tileRange Global tile range corresponding to 'rawImageData'.
        * \param numThreads Number of threads to use when compressing.
        * \param[in,out] compressedData The pre-allocated compressed image data.
        * Must be at least
        * getMaxBytesRequiredToCompress(tileRange.mNumElements * numColsOfTiles)
        * bytes.  On output, will contain the compressed image.
        * \param[out] bytesPerTile Number of bytes in each compressed tile.  This
        * is tileRange.mNumElements long, where index 0 corresponds to global tile
        * tileRange.mStartElement.
         */
        std::span<std::byte> compressTileSubrange(std::span<const std::byte> rawImageData,
            const types::Range& tileRange,
            std::span<std::byte> compressedData,
            std::vector<size_t>& bytesPerTile) const;
    };
}

#endif // NITF_J2KCompressor_hpp_INCLUDED_
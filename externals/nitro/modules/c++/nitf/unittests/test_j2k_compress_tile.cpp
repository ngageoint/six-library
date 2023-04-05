/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2017, MDA Information Systems LLC
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
 * License along with this program;
 * If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <cstring>
#include <iostream>
#include <numeric>
#include <std/cstddef>

#include <sio/lite/FileReader.h>
#include <sio/lite/ReadUtils.h>
#include <types/Range.h>
#include <io/ReadUtils.h>
#include <io/TempFile.h>
#include <io/FileOutputStream.h>
#include <sys/OS.h>
#include <gsl/gsl.h>

#include <import/nrt.h>
#include <nitf/ImageBlocker.hpp>
#include <nitf/J2KCompressor.hpp>
#include <nitf/UnitTests.hpp>

#include "TestCase.h"

struct Image final
{
    types::RowCol<size_t> dims;
    std::vector<std::byte> pixels;
};

static void generateTestImage(Image& image)
{
    image.dims.row = 2048;
    image.dims.col = 1024;
    image.pixels.resize(image.dims.area());

    for (size_t ii = 0; ii < image.pixels.size(); ++ii)
    {
        // Let's write this out as some sort of pattern so
        // J2K compression has something to work with
        image.pixels[ii] = static_cast<std::byte>((ii / 100) % 128);
    }
}

static void compressEntireImage(const j2k::Compressor& compressor,
    const Image& inputImage,
    std::vector<std::byte>& outputImage)
{
    std::vector<size_t> bytesPerBlock;
    const std::span<const std::byte> pixels(inputImage.pixels.data(), inputImage.pixels.size());
    compressor.compress(pixels, outputImage, bytesPerBlock);
}

static void compressTileSubrange(const j2k::Compressor& compressor,
    const j2k::CompressionParameters& params,
    const Image& inputImage,
    size_t numSubsets,
    std::vector<std::byte>& outputImage)
{
    const auto tileDims = params.getTileDims();
    const types::RowCol<size_t> numTiles(params.getNumRowsOfTiles(), params.getNumColsOfTiles());

    const auto rowsOfTilesPerSubset = numTiles.row / numSubsets;

    std::vector<std::vector<std::byte>> compressedImages(numSubsets);
    std::vector<size_t> numBytesCompressed(numSubsets);

    for (size_t subset = 0; subset < numSubsets; ++subset)
    {
        const auto tileRow = rowsOfTilesPerSubset * subset;
        const auto numRowsOfTiles = (subset == numSubsets - 1) ? numTiles.row - tileRow : rowsOfTilesPerSubset;
        const auto subsetNumTiles = numRowsOfTiles * numTiles.col;

        auto& compressedImage = compressedImages[subset];
        compressedImage.resize(compressor.getMaxBytesRequiredToCompress(subsetNumTiles));

        const std::span<std::byte> compressedTiles(compressedImage.data(), compressedImage.size());
        std::vector<size_t> compressedBytesPerBlock;

        const auto offset = (tileRow * tileDims.row) * inputImage.dims.col;
        const auto& pixels = inputImage.pixels;
        const std::span<const std::byte> uncompressed(pixels.data() + offset, pixels.size() - offset);

        const auto result = compressor.compressTileSubrange(
            uncompressed, types::Range(tileRow * numTiles.col, subsetNumTiles),
            compressedTiles, compressedBytesPerBlock);
        numBytesCompressed[subset] = result.size();
    }

    const auto numTotalBytesCompressed = std::accumulate(numBytesCompressed.begin(), numBytesCompressed.end(), static_cast<size_t>(0));
    outputImage.resize(numTotalBytesCompressed);
    auto outputPtr = outputImage.data();
    for (size_t ii = 0; ii < numSubsets; ++ii)
    {
        ::memcpy(outputPtr, compressedImages[ii].data(), numBytesCompressed[ii]);
        outputPtr += numBytesCompressed[ii];
    }
}

static void compressByTile(const j2k::Compressor& compressor,
    const j2k::CompressionParameters& params,
    const Image& inputImage,
    std::vector<std::byte>& outputImage)
{
    const auto tileDims = params.getTileDims();
    const types::RowCol<size_t> numTiles(params.getNumRowsOfTiles(), params.getNumColsOfTiles());

    outputImage.resize(compressor.getMaxBytesRequiredToCompress());

    // Compress and write a block/tile at a time
    size_t bytesWritten = 0;

    for (size_t blockRow = 0, blockNum = 0; blockRow < numTiles.row; ++blockRow)
    {
        for (size_t blockCol = 0; blockCol < numTiles.col; ++blockCol, ++blockNum)
        {
            const auto offset = blockRow * tileDims.row * inputImage.dims.col + blockCol * tileDims.col;
            const auto& pixels = inputImage.pixels;
            const std::span<const std::byte> uncompressed(pixels.data() + offset, pixels.size() - offset);
            const std::span<std::byte> compressedTile(&outputImage[bytesWritten], outputImage.size() - bytesWritten);

            const auto result = compressor.compressTile(uncompressed, blockNum, compressedTile);
            bytesWritten += result.size();
        }
    }

    outputImage.resize(bytesWritten);
}

static bool equals(const std::vector<std::byte>& lhs, const std::vector<std::byte>& rhs)
{
    if (lhs == rhs)
    {
        return true;
    }

    if (lhs.size() != rhs.size())
    {
        std::cerr << "Image sizes do not match: " << lhs.size() << " vs. " << rhs.size() << "\n";
        return false;
    }

    for (size_t ii = 0; ii < lhs.size(); ++ii)
    {
        if (lhs[ii] != rhs[ii])
        {
            std::cerr << "Data mismatch from index: " << ii << "\n";
            for (size_t errorIndex = ii; errorIndex < ii + 15; ++errorIndex)
            {
                if (errorIndex >= lhs.size())
                {
                    break;
                }
                std::cerr << errorIndex << ": " << static_cast<int>(lhs[errorIndex]) << " vs. " << static_cast<int>(rhs[errorIndex]) << "\n";
            }
            break;
        }
    }
    return false;
}

TEST_CASE(j2k_compress_tile)
{
    nitf::Test::setNitfPluginPath();

    const size_t numThreads = sys::OS().getNumCPUs() - 1;

    {
      // be sure equals() is used to avoid compiler warnings
      static const std::vector<std::byte> lhs, rhs;
      TEST_ASSERT_TRUE(equals(lhs, rhs));
    }

    Image source;
    generateTestImage(source);

    for (size_t tilesPerDim = 1; tilesPerDim <= 5; ++tilesPerDim)
    {
        std::vector<std::byte> imageCompressedWhole;
        std::vector<std::byte> imageCompressedByTile;

        const types::RowCol<size_t> tileDims(source.dims.row / tilesPerDim, source.dims.col / tilesPerDim);
        const j2k::CompressionParameters params(source.dims, tileDims, 1, 2);
        j2k::Compressor compressor(params, numThreads);

        compressEntireImage(compressor, source, imageCompressedWhole);
        compressByTile(compressor, params, source, imageCompressedByTile);

        //TEST_ASSERT(equals(imageCompressedWhole, imageCompressedByTile));
    }

    for (size_t numSubsets = 1; numSubsets <= 5; ++numSubsets)
    {
        std::vector<std::byte> imageCompressedWhole;
        std::vector<std::byte> imageCompressedBySubset;
        constexpr size_t TILES_PER_DIM = 10;

        const types::RowCol<size_t> tileDims(source.dims.row / TILES_PER_DIM, source.dims.col / TILES_PER_DIM);
        const j2k::CompressionParameters params(source.dims, tileDims, 1, 2);
        const j2k::Compressor compressor(params, numThreads);

        compressEntireImage(compressor, source, imageCompressedWhole);
        compressTileSubrange(compressor, params, source, numSubsets, imageCompressedBySubset);

        //TEST_ASSERT(equals(imageCompressedWhole, imageCompressedBySubset));
    }
}

TEST_MAIN((void)argc; (void)argv;
    TEST_CHECK(j2k_compress_tile);
    )


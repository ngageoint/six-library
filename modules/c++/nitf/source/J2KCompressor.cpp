/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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

#include "nitf/J2KCompressor.hpp"

#include <string.h>

#include "j2k/TileWriter.h"

#include <numeric>
#include <stdexcept>
#include <limits>
#include <sstream>
#include <vector>
#include <algorithm>
#include <std/span>
#include <iterator>
#include <std/memory>
#include <std/cstddef> // std::byte
#include <stdexcept>
#include <limits>
#include <sstream>
#include <vector>
#include <algorithm>
#include <std/span>
#include <iterator>

#include <gsl/gsl.h>
#include <except/Exception.h>
#include <sys/Conf.h>
#include <io/ByteStream.h>
#include <math/Round.h>
#include <nitf/ImageBlocker.hpp>
#include <mt/WorkSharingBalancedRunnable1D.h>
#include <io/BufferViewStream.h>
#include <io/NullStreams.h>
#include <io/SeekableStreams.h>
#include <sys/Conf.h>
#include <types/RowCol.h>
#include <gsl/gsl.h>
#include <except/Exception.h>
#include <sys/Conf.h>
#include <mem/ScopedArray.h>
#include <io/ByteStream.h>

#include "nitf/J2KStream.hpp"
#include "nitf/J2KImage.hpp"
#include "nitf/J2KEncoder.hpp"
#include "nitf/J2KCompressionParameters.hpp"

#undef min
#undef max

static size_t writeImpl(void* buffer, size_t numBytes, void* data)
{
    auto compressedOutputStream = static_cast<::io::SeekableOutputStream*>(data);
    if (compressedOutputStream != nullptr)
    {
        try
        {
            compressedOutputStream->write(buffer, numBytes);
            return numBytes;
        }
        catch (const except::Exception&) {}
    }

    // Openjpeg expects (OPJ_SIZE_T)-1 as the result of a failed
    // call to a user provided write.
    return static_cast<size_t>(-1);
}

static int64_t skipImpl(sys::Off_T bytesToSkip, void* data)
{
    auto compressedOutputStream = static_cast<::io::SeekableOutputStream*>(data);
    if (compressedOutputStream != nullptr)
    {
        try
        {
            compressedOutputStream->seek(bytesToSkip, ::io::Seekable::CURRENT);
            return bytesToSkip;
        }
        catch (const except::Exception&) {}
    }

    // Openjpeg expects -1 as the result of a failed call to a user provided skip()
    return -1;
}

static bool seekImpl(int64_t numBytes, void* data)
{
    auto compressedOutputStream = static_cast<::io::SeekableOutputStream*>(data);
    if (compressedOutputStream != nullptr)
    {
        try
        {
            compressedOutputStream->seek(numBytes, ::io::Seekable::START);
            return true;
        }
        catch (const except::Exception&) {}
    }

    // Openjpeg expects 0 (OPJ_FALSE) as the result of a failed call to a user provided seek()
    return false;
}

/*!
    * \class OPJTileWriter
    * \desc Implementation class for writing compressed tiles to an output stream.
    * This class is used by OPJCompressor to do thread based tile compression.
    */
class TileWriter final
{
    ::io::SeekableOutputStream* mOutputStream = nullptr;
    j2k::CompressionParameters mCompressionParams;

    j2k::Stream mStream;             //! The openjpeg stream.
    j2k::Image mImage;             //! The openjpeg image.
    j2k::Encoder mEncoder;             //! The openjpeg encoder.
    bool mIsCompressing = false;             //! Whether we are currently compressing or not.

    void resizeTile(types::RowCol<size_t>& tile, size_t tileIndex) noexcept
    {
        const auto tileDims = mCompressionParams.getTileDims();
        const auto rawImageDims = mCompressionParams.getRawImageDims();
        const auto numColsOfTiles = mCompressionParams.getNumColsOfTiles();
        const auto numRowsOfTiles = mCompressionParams.getNumRowsOfTiles();

        const auto tileRow = tileIndex / numColsOfTiles;
        if ((tileRow == numRowsOfTiles - 1) && (rawImageDims.row % tileDims.row != 0))
        {
            tile.row = rawImageDims.row % tileDims.row;
        }

        const auto tileCol = tileIndex - (tileRow * numColsOfTiles);
        if ((tileCol == numColsOfTiles - 1) && (rawImageDims.col % tileDims.col != 0))
        {
            tile.col = rawImageDims.col % tileDims.col;
        }
    }

public:
    /*!
        * Constructor
        *
        * \param outputStream The output stream to write the J2K codestream to.
        *
        * \param compressionParams The J2K compression parameters.
        */
    TileWriter(::io::SeekableOutputStream& outputStream, const j2k::CompressionParameters& compressionParams) :
        mCompressionParams(compressionParams),
        mStream(j2k::StreamType::OUTPUT),
        mImage(mCompressionParams.getRawImageDims()),
        mEncoder(mImage, mCompressionParams)
    {
        setOutputStream(outputStream);
        j2k_stream_set_write_function(mStream.getNative(), writeImpl);
        j2k_stream_set_seek_function(mStream.getNative(), seekImpl);
        j2k_stream_set_skip_function(mStream.getNative(), skipImpl);
    }
    TileWriter(const TileWriter&) = delete;
    TileWriter& operator=(const TileWriter&) = delete;
    TileWriter(TileWriter&&) = delete;
    TileWriter& operator=(TileWriter&&) = delete;

    /*!
        * Destructor - calls end().
        */
    ~TileWriter() noexcept
    {
        try
        {
            end();
        }
        catch (...)
        {
        }
    }

    /*!
        * Starts the J2K compression. The first call to flush() after this
        * is invoked will write the J2K header to the output stream.
        */
    void start()
    {
        if (mIsCompressing)
        {
            return;
        }

        const auto startCompressSuccess = j2k_start_compress(mEncoder.getNative(), mImage.getNative(), mStream.getNative());
        if (!startCompressSuccess)
        {
            if (mEncoder.errorOccurred())
            {
                const std::string opjErrorMsg = mEncoder.getErrorMessage();
                mEncoder.clearError();

                throw except::Exception(Ctxt("Error starting compression with openjpeg error: " + opjErrorMsg));
            }

            throw except::Exception(Ctxt("Error starting compression."));
        }
        mIsCompressing = true;
    }

    /*!
        * Ends the J2K compression. This will flush the J2K footer to the output stream.
        */
    void end()
    {
        if (!mIsCompressing)
        {
            return;
        }

        const auto endCompressSuccess = j2k_end_compress(mEncoder.getNative(), mStream.getNative());
        if (!endCompressSuccess)
        {
            if (mEncoder.errorOccurred())
            {
                const std::string opjErrorMsg = mEncoder.getErrorMessage();
                mEncoder.clearError();

                throw except::Exception(Ctxt("Error ending compression with openjpeg error: " + opjErrorMsg));
            }

            throw except::Exception(Ctxt("Error ending compression."));
        }
        mIsCompressing = false;
    }

    /*!
        * Writes any J2K codestream data currently in the internal buffer used by
        * openjpeg to the output stream.
        */
    void flush()
    {
        if (!mIsCompressing)
        {
            throw except::Exception(Ctxt("Cannot flush data to output stream: compression has not been started."));
        }

        const auto flushSuccess = j2k_flush(mEncoder.getNative(), mStream.getNative());
        if (!flushSuccess)
        {
            if (mEncoder.errorOccurred())
            {
                const std::string opjErrorMsg = mEncoder.getErrorMessage();
                mEncoder.clearError();

                throw except::Exception(Ctxt("Failed to flush J2K codestream data with openjpeg error: " + opjErrorMsg));
            }

            throw except::Exception(Ctxt("Failed to flush J2K codestream data."));
        }
    }

    /*!
        *  Calls opj_write_tile. Tiles should be structured as contiguous,
        *  increasing rows with a fixed column width i.e given 2x6 image
        *  with 2x3 tiles:
        *
        *            0  1  2  3  4  5
        *            6  7  8  9  10 11
        *
        *  The 0th tile:
        *
        *            0  1  2
        *            6  7  8
        *
        *  should be laid out in contiguous memory as: 0 1 2 6 7 8.
        */

    void writeTile(const std::byte* tileData, size_t tileIndex)
    {
        start();

        const auto tileDims(mCompressionParams.getTileDims());

        // Resize of the dimensions of this tile if it is a partial tile
        types::RowCol<size_t> resizedTileDims(tileDims);
        resizeTile(resizedTileDims, tileIndex);

        // Create a smaller buffer for our partial tile
        std::vector<std::byte> partialTileBuffer;
        if ((resizedTileDims.col < tileDims.col) || (resizedTileDims.row < tileDims.row))
        {
            partialTileBuffer.resize(resizedTileDims.area());
            for (size_t row = 0; row < resizedTileDims.row; ++row)
            {
                const auto srcTileRowStart = tileData + row * tileDims.col;
                const auto src = sys::make_span(srcTileRowStart, resizedTileDims.col);

                // partialTileBuffer.data() + row * resizedTileDims.col
                auto dest = partialTileBuffer.begin();
                std::advance(dest, gsl::narrow<ptrdiff_t>(row * resizedTileDims.col));

                std::copy(src.begin(), src.end(), dest);
            }
        }

        const auto imageData = partialTileBuffer.empty() ? tileData : partialTileBuffer.data();
        const void* imageData_ = imageData;

        // Compress the tile - if an I/O error occurs in our write handler,
        // the OPJEncoder error handler will get called.
        const auto writeSuccess = j2k_write_tile(mEncoder.getNative(),
            gsl::narrow<uint32_t>(tileIndex),
            static_cast<const uint8_t*>(imageData_), gsl::narrow<uint32_t>(resizedTileDims.area()),
            mStream.getNative());
        if (!writeSuccess)
        {
            std::ostringstream os;
            os << "Failed to compress tile " << tileIndex << " (rows: " << resizedTileDims.row << ", cols: " << resizedTileDims.col << ")";
            if (mEncoder.errorOccurred())
            {
                const std::string opjErrorMsg = mEncoder.getErrorMessage();
                mEncoder.clearError();

                os << " with openjpeg error: " << opjErrorMsg;
                throw except::Exception(Ctxt(os.str()));
            }

            throw except::Exception(Ctxt(os.str()));
        }
    }

    /*!
        * Updates the output stream that openjpeg will write J2K codestream
        * data to when flush() is called.
        *
        * \param outputStream The stream to write to.
        */
    void setOutputStream(::io::SeekableOutputStream& outputStream) noexcept
    {
        mOutputStream = &outputStream;
        j2k_stream_set_user_data(mStream.getNative(), mOutputStream, nullptr);
    }
};

using BufferViewStream = io::BufferViewStream<std::byte> ;

class CodestreamOp final
{
    types::RowCol<size_t> getRowColIndices(size_t tileIndex) const noexcept
    {
        return types::RowCol<size_t>(
            tileIndex / mCompressionParams.getNumColsOfTiles(),
            tileIndex % mCompressionParams.getNumColsOfTiles());
    }

    const size_t mStartTile;
    std::shared_ptr<BufferViewStream>* const mTileStreams;
    const std::span<const std::byte> mUncompressedImage;
    j2k::CompressionParameters mCompressionParams;

    mutable std::unique_ptr<TileWriter> mWriter;
    std::vector<std::byte> mImageBlock;
    std::span<std::byte> mpImageBlock;

public:
    CodestreamOp(
        size_t startTile,
        std::shared_ptr<BufferViewStream>* tileStreams,
        std::span<const std::byte> uncompressedImage,
        const j2k::CompressionParameters& compressionParams) :
        mStartTile(startTile),
        mTileStreams(tileStreams),
        mUncompressedImage(uncompressedImage),
        mCompressionParams(compressionParams)
    {
        mImageBlock.resize(mCompressionParams.getTileDims().area());
        mpImageBlock = sys::make_span(mImageBlock);
    }
    CodestreamOp(const CodestreamOp&) = delete;
    CodestreamOp& operator=(const CodestreamOp&) = delete;
    CodestreamOp(CodestreamOp&&) = default;
    CodestreamOp& operator=(CodestreamOp&&) = delete;

    void operator()(size_t localTileIndex) const
    {
        const auto tileDims = mCompressionParams.getTileDims();
        const auto globalTileIndex = localTileIndex + mStartTile;
        const auto fullDims = mCompressionParams.getRawImageDims();

        // Need local indices to offset into the uncompressed image properly
        const auto localTileIndices = getRowColIndices(localTileIndex);

        const types::RowCol<size_t> localStart(localTileIndices.row * tileDims.row, localTileIndices.col * tileDims.col);

        const size_t offset = localStart.row * fullDims.col + localStart.col;
        const std::span<const std::byte> uncompressedImage(mUncompressedImage.data() + offset, mUncompressedImage.size_bytes() - offset);

        // Need global indices to determine if we're on the edge of the global image or not
        const auto globalTileIndices = getRowColIndices(globalTileIndex);
        const types::RowCol<size_t> globalStart(globalTileIndices.row * tileDims.row, globalTileIndices.col * tileDims.col);
        const types::RowCol<size_t> globalEnd(std::min(globalStart.row + tileDims.row, fullDims.row), std::min(globalStart.col + tileDims.col, fullDims.col));

        // Block it
        const types::RowCol<size_t> validInBlock(globalEnd.row - globalStart.row, globalEnd.col - globalStart.col);
        nitf::ImageBlocker::block(uncompressedImage,
            sizeof(std::byte), fullDims.col,
            tileDims, validInBlock,
            mpImageBlock);

        auto tileStream = mTileStreams[localTileIndex];
        if (!mWriter)
        {
            mWriter = std::make_unique<TileWriter>(*tileStream, mCompressionParams);

            // Write out the header
            // OpenJPEG makes us write the header, but we only want to keep it if we're tile 0
            mWriter->start();
            mWriter->flush();

            if (globalTileIndex != 0)
            {
                tileStream->seek(0, io::Seekable::START);
            }
        }
        else
        {
            mWriter->setOutputStream(*tileStream);
        }

        // Write out the tile
        mWriter->writeTile(mpImageBlock.data(), globalTileIndex);
        mWriter->flush();
    }

    void finalize(bool keepFooter)
    {
        if (!mWriter)
        {
            return;
        }
            
        // Writer::end() is required to clean up OpenJPEG objects
        // This step also writes out the footer, which we may or may not
        // actually want to keep
        if (keepFooter)
        {
            // tileIdx is guaranteed to be in-bounds if keepFooter is true
            const size_t lastTile = mCompressionParams.getNumTiles() - 1;
            const size_t tileIdx = lastTile - mStartTile;
            mWriter->setOutputStream(*mTileStreams[tileIdx]);

            // Write out the footer
            mWriter->end();
        }
        else
        {
            io::SeekableNullOutputStream outStream;
            mWriter->setOutputStream(outStream);

            // Write out the footer
            mWriter->end();
        }
    }        
};

j2k::Compressor::Compressor(const CompressionParameters& compressionParams, size_t numThreads) noexcept :
    mCompressionParams(compressionParams), mNumThreads(numThreads)
{
}

size_t j2k::Compressor::getMaxBytesRequiredToCompress() const noexcept
{
    return getMaxBytesRequiredToCompress(mCompressionParams.getNumTiles());
}

/*!
 * In rare cases, the "compressed" image will actually be slightly larger
 * than the uncompressed image.  This is a presumably worst case number
 * here - it's probably much larger than it needs to be.
 */
constexpr long double POOR_COMPRESSION_SCALE_FACTOR = 2.0;
size_t j2k::Compressor::getMaxBytesRequiredToCompress(size_t numTiles) const noexcept
{
    const auto bytesPerTile = mCompressionParams.getTileDims().area();
    const auto maxBytes_ = gsl::narrow_cast<long double>(bytesPerTile * numTiles) * POOR_COMPRESSION_SCALE_FACTOR;
    const auto maxBytes = gsl::narrow_cast<size_t>(std::ceil(maxBytes_));
    return maxBytes;
}

void j2k::Compressor::compress(
    std::span<const std::byte> rawImageData,
    std::vector<std::byte>& compressedData,
    std::vector<size_t>& bytesPerTile) const
{
    compressedData.resize(getMaxBytesRequiredToCompress());
    const auto compressedDataView = compress(rawImageData, sys::make_span(compressedData), bytesPerTile);
    compressedData.resize(compressedDataView.size());
}

std::span<std::byte> j2k::Compressor::compress(std::span<const std::byte> rawImageData,
    std::span<std::byte> compressedData,
    std::vector<size_t>& bytesPerTile) const
{
    return compressTileSubrange(rawImageData,
        types::Range(0, mCompressionParams.getNumTiles()),
        compressedData,
        bytesPerTile);
}

void j2k::Compressor::compressTile(
    std::span<const std::byte> rawImageData,
    size_t tileIndex,
    std::vector<std::byte>& compressedTile) const
{
    compressedTile.resize(getMaxBytesRequiredToCompress(1));
    std::vector<size_t> bytesPerTile;
    auto compressedView = sys::make_span(compressedTile);
    compressedView = compressTileSubrange(rawImageData, types::Range(tileIndex, 1), 
        compressedView, bytesPerTile);
    compressedTile.resize(compressedView.size());
}

std::span<std::byte> j2k::Compressor::compressTile(
    std::span<const std::byte> rawImageData,
    size_t tileIndex,
    std::span<std::byte> compressedTile) const
{
    std::vector<size_t> bytesPerTile;
    return compressTileSubrange(rawImageData, types::Range(tileIndex, 1), 
        compressedTile, bytesPerTile);
}

std::span<std::byte> j2k::Compressor::compressRowSubrange(
    std::span<const std::byte> rawImageData,
    size_t globalStartRow,
    size_t numLocalRows,
    std::span<std::byte> compressedData,
    types::Range& tileRange,
    std::vector<size_t>& bytesPerTile) const
{
    // Sanity checks
    const size_t numRowsInTile = mCompressionParams.getTileDims().row;
    if (globalStartRow % numRowsInTile != 0)
    {
        std::ostringstream ostr;
        ostr << "Global start row = " << globalStartRow << " must be a multiple of number of rows in tile = " << numRowsInTile;
        throw except::Exception(Ctxt(ostr.str()));
    }

    if ((numLocalRows % numRowsInTile != 0) &&
        (globalStartRow + numLocalRows != mCompressionParams.getRawImageDims().row))
    {
        std::ostringstream ostr;
        ostr << "Number of local rows = " << numLocalRows << " must be a multiple of number of rows in tile = " << numRowsInTile;
        throw except::Exception(Ctxt(ostr.str()));
    }

    const auto startRowOfTiles = globalStartRow / numRowsInTile;
    const auto numRowsOfTiles = math::ceilingDivide(numLocalRows, numRowsInTile);

    const auto numColsOfTiles = mCompressionParams.getNumColsOfTiles();
    tileRange.mStartElement = startRowOfTiles * numColsOfTiles;
    tileRange.mNumElements = numRowsOfTiles * numColsOfTiles;

    return compressTileSubrange(rawImageData,
        tileRange,
        compressedData,
        bytesPerTile);
}

std::span<std::byte> j2k::Compressor::compressTileSubrange(
    std::span<const std::byte> rawImageData,
    const types::Range& tileRange,
    std::span<std::byte> compressedData,
    std::vector<size_t>& bytesPerTile) const
{
    // We write initially directly into 'compressedData', reserving the max
    // expected # of bytes/tile
    const auto numTiles = tileRange.mNumElements;
    const auto maxNumBytesPerTile = getMaxBytesRequiredToCompress(1);
    const auto numBytesNeeded = maxNumBytesPerTile * numTiles;
    if (compressedData.size() < numBytesNeeded)
    {
        std::ostringstream ostr;
        ostr << "Require " << numBytesNeeded << " bytes for compression of "
            << numTiles << " tiles but only received " << compressedData.size() << " bytes";
        throw except::Exception(Ctxt(ostr.str()));
    }

    auto compressedPtr = compressedData.data();
    std::vector<std::shared_ptr<BufferViewStream>> tileStreams(numTiles);
    for (size_t tile = 0; tile < numTiles; ++tile, compressedPtr += maxNumBytesPerTile)
    {
        auto bufferStream = std::make_shared<BufferViewStream>(mem::BufferView<std::byte>(compressedPtr, maxNumBytesPerTile));
        tileStreams[tile] = bufferStream;
    }

    std::vector<CodestreamOp> ops;
    ops.reserve(mNumThreads);
    for (size_t ii = 0; ii < mNumThreads; ++ii)
    {
        ops.emplace_back(tileRange.mStartElement, tileStreams.data(), rawImageData, mCompressionParams);
    }

    // Compress the image
    mt::runWorkSharingBalanced1D(numTiles, mNumThreads, ops);

    // End compression for each thread
    // If the last tile is in 'tileRange', we need to ensure that we write the
    // footer exactly once
    bool keepFooter = (tileRange.endElement() == mCompressionParams.getNumTiles());
    for (auto& op : ops)
    {
        op.finalize(keepFooter);
        keepFooter = false;
    }

    // At this point the tiles are all compressed into 'compressedData' but
    // they're not contiguous.  Shift memory around to get a contiguous buffer.
    size_t numBytesWritten = 0;
    bytesPerTile.resize(numTiles);

    auto dest = compressedData.data();
    for (size_t tileNum = 0; tileNum < numTiles; ++tileNum)
    {
        BufferViewStream& tileStream = *tileStreams[tileNum];

        // This shouldn't be possible because if a tile was too big, it would
        // have thrown when compressing
        const auto numBytesThisTile = gsl::narrow<size_t>(tileStream.tell());
        if (numBytesWritten + numBytesThisTile > compressedData.size())
        {
            std::ostringstream os;
            os << "Cannot write " << numBytesThisTile << " bytes for tile " << tileNum << " at byte offset " << numBytesWritten
                << " - exceeds maximum compressed image size (" << compressedData.size() << " bytes)!";
            throw except::Exception(Ctxt(os.str()));
        }

        const auto src = tileStream.get();

        // Copy the tile to the output buffer
        // Since we're reusing the same buffer for the contiguous output,
        // memory addresses may overlap so we need to use memmove()
        if (src != dest)
        {
            ::memmove(dest, src, numBytesThisTile);
        }

        numBytesWritten += numBytesThisTile;
        bytesPerTile[tileNum] = numBytesThisTile;
        dest += numBytesThisTile;
    }

    return sys::make_span(compressedData.data(), numBytesWritten);
}



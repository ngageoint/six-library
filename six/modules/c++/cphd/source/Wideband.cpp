/* =========================================================================
 * This file is part of cphd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2020, MDA Information Systems LLC
 *
 * cphd-c++ is free software; you can redistribute it and/or modify
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

#include <limits>
#include <sstream>
#include <thread>
#include <string>
#include <std/memory>

#include <nitf/coda-oss.hpp>
#include <except/Exception.h>
#include <io/FileInputStream.h>
#include <mt/ThreadGroup.h>
#include <mt/ThreadPlanner.h>

#include <six/Init.h>
#include <cphd/ByteSwap.h>
#include <cphd/Wideband.h>
#include <cphd/FileHeader.h>

#undef min
#undef max

namespace
{
template <typename InT>
class PromoteRunnable : public sys::Runnable
{
public:
    PromoteRunnable(const std::complex<InT>* input,
                    size_t startRow,
                    size_t numRows,
                    size_t numCols,
                    std::complex<float>* output) :
        mInput(input + startRow * numCols),
        mDims(numRows, numCols),
        mOutput(output + startRow * numCols)
    {
    }

    virtual void run()
    {
        for (size_t row = 0, idx = 0; row < mDims.row; ++row)
        {
            for (size_t col = 0; col < mDims.col; ++col, ++idx)
            {
                const std::complex<InT>& input(mInput[idx]);
                mOutput[idx] = std::complex<float>(input.real(), input.imag());
            }
        }
    }

private:
    const std::complex<InT>* const mInput;
    const types::RowCol<size_t> mDims;
    std::complex<float>* const mOutput;
};

template <typename InT>
class ScaleRunnable : public sys::Runnable
{
public:
    ScaleRunnable(const std::complex<InT>* input,
                  size_t startRow,
                  size_t numRows,
                  size_t numCols,
                  const double* scaleFactors,
                  std::complex<float>* output) :
        mInput(input + startRow * numCols),
        mDims(numRows, numCols),
        mScaleFactors(scaleFactors + startRow),
        mOutput(output + startRow * numCols)
    {
    }

    virtual void run()
    {
        for (size_t row = 0, idx = 0; row < mDims.row; ++row)
        {
            const double scaleFactor(mScaleFactors[row]);
            for (size_t col = 0; col < mDims.col; ++col, ++idx)
            {
                const std::complex<InT>& input(mInput[idx]);
                mOutput[idx] = std::complex<float>(static_cast<float>(input.real() * scaleFactor),
                                                   static_cast<float>(input.imag() * scaleFactor));
            }
        }
    }

private:
    const std::complex<InT>* const mInput;
    const types::RowCol<size_t> mDims;
    const double* const mScaleFactors;
    std::complex<float>* const mOutput;
};

template <typename InT>
void promote(const void* input,
             const types::RowCol<size_t>& dims,
             size_t numThreads,
             std::complex<float>* output)
{
    if (numThreads <= 1)
    {
        PromoteRunnable<InT>(static_cast<const std::complex<InT>*>(input),
                             0,
                             dims.row,
                             dims.col,
                             output)
                .run();
    }
    else
    {
        mt::ThreadGroup threads;
        const mt::ThreadPlanner planner(dims.row, numThreads);

        size_t threadNum(0);
        size_t startRow(0);
        size_t numRowsThisThread(0);
        while (planner.getThreadInfo(threadNum++, startRow, numRowsThisThread))
        {
           auto scaler = std::make_unique<PromoteRunnable<InT>>(
                    static_cast<const std::complex<InT>*>(input),
                    startRow,
                    numRowsThisThread,
                    dims.col,
                    output);
            threads.createThread(std::move(scaler));
        }

        threads.joinAll();
    }
}

void promote(const void* input,
             size_t elementSize,
             const types::RowCol<size_t>& dims,
             size_t numThreads,
             std::complex<float>* output)
{
    switch (elementSize)
    {
    case 2:
        promote<int8_t>(input, dims, numThreads, output);
        break;
    case 4:
        promote<int16_t>(input, dims, numThreads, output);
        break;
    case 8:
        promote<float>(input, dims, numThreads, output);
        break;
    default:
        throw except::Exception(
                Ctxt("Unexpected element size " + std::to_string(elementSize)));
    }
}
template <typename InT>
void scale(const void* input,
           const types::RowCol<size_t>& dims,
           const double* scaleFactors,
           size_t numThreads,
           std::complex<float>* output)
{
    if (numThreads <= 1)
    {
        ScaleRunnable<InT>(static_cast<const std::complex<InT>*>(input),
                           0,
                           dims.row,
                           dims.col,
                           scaleFactors,
                           output)
                .run();
    }
    else
    {
        mt::ThreadGroup threads;
        const mt::ThreadPlanner planner(dims.row, numThreads);

        size_t threadNum(0);
        size_t startRow(0);
        size_t numRowsThisThread(0);
        while (planner.getThreadInfo(threadNum++, startRow, numRowsThisThread))
        {
           auto scaler = std::make_unique<ScaleRunnable<InT>>(
                    static_cast<const std::complex<InT>*>(input),
                    startRow,
                    numRowsThisThread,
                    dims.col,
                    scaleFactors,
                    output);
            threads.createThread(std::move(scaler));
        }

        threads.joinAll();
    }
}

void scale(const void* input,
           size_t elementSize,
           const types::RowCol<size_t>& dims,
           const double* scaleFactors,
           size_t numThreads,
           std::complex<float>* output)
{
    switch (elementSize)
    {
    case 2:
        scale<int8_t>(input, dims, scaleFactors, numThreads, output);
        break;
    case 4:
        scale<int16_t>(input, dims, scaleFactors, numThreads, output);
        break;
    case 8:
        scale<float>(input, dims, scaleFactors, numThreads, output);
        break;
    default:
        throw except::Exception(
                Ctxt("Unexpected element size " + std::to_string(elementSize)));
    }
}
}

namespace cphd
{
const size_t Wideband::ALL = std::numeric_limits<size_t>::max();

Wideband::Wideband(const std::string& pathname,
                   const cphd::MetadataBase& metadata,
                   int64_t startWB,
                   int64_t sizeWB) :
    mInStream(std::make_shared<io::FileInputStream>(pathname)),
    mMetadata(metadata),
    mWBOffset(startWB),
    mWBSize(sizeWB),
    mElementSize(mMetadata.getNumBytesPerSample()),
    mOffsets(mMetadata.getNumChannels())
{
    initialize();
}

Wideband::Wideband(std::shared_ptr<io::SeekableInputStream> inStream,
                   const cphd::MetadataBase& metadata,
                   int64_t startWB,
                   int64_t sizeWB) :
    mInStream(inStream),
    mMetadata(metadata),
    mWBOffset(startWB),
    mWBSize(sizeWB),
    mElementSize(mMetadata.getNumBytesPerSample()),
    mOffsets(mMetadata.getNumChannels())
{
    initialize();
}

void Wideband::initialize()
{
    mOffsets[0] = mWBOffset;
    if (!mMetadata.isCompressed())
    {
        // No Signal Array Compression
        for (size_t ii = 1; ii < mMetadata.getNumChannels(); ++ii)
        {
            const int64_t offset =
                    static_cast<int64_t>(mMetadata.getNumSamples(ii - 1)) *
                    mMetadata.getNumVectors(ii - 1) * mElementSize;

            mOffsets[ii] = mOffsets[ii - 1] + offset;
        }
    }
    else
    {
        // Signal Array is Compressed
        for (size_t ii = 1; ii < mMetadata.getNumChannels(); ++ii)
        {
            mOffsets[ii] =
                    mOffsets[ii - 1] + mMetadata.getCompressedSignalSize(ii);
        }
    }
}

int64_t Wideband::getFileOffset(size_t channel,
                                   size_t vector,
                                   size_t sample) const
{
    if (channel >= mOffsets.size())
    {
        throw except::Exception(Ctxt("Invalid channel number"));
    }

    if (vector >= mMetadata.getNumVectors(channel))
    {
        throw except::Exception(Ctxt("Invalid vector"));
    }

    if (sample >= mMetadata.getNumSamples(channel))
    {
        throw except::Exception(Ctxt("Invalid sample"));
    }

    const int64_t bytesPerVectorFile =
            mMetadata.getNumSamples(channel) * mElementSize;

    const int64_t offset = mOffsets[channel] + bytesPerVectorFile * vector +
            sample * mElementSize;
    return offset;
}

int64_t Wideband::getFileOffset(size_t channel) const
{
    if (channel >= mOffsets.size())
    {
        throw except::Exception(Ctxt("Invalid channel number"));
    }

    const int64_t offset = mOffsets[channel];
    return offset;
}

void Wideband::checkReadInputs(size_t channel,
                               size_t firstVector,
                               size_t& lastVector,
                               size_t firstSample,
                               size_t& lastSample,
                               types::RowCol<size_t>& dims) const
{
    checkChannelInput(channel);

    const size_t maxVector = mMetadata.getNumVectors(channel) - 1;
    if (firstVector > maxVector)
    {
        throw except::Exception(Ctxt("Invalid first vector"));
    }

    if (lastVector == ALL)
    {
        lastVector = maxVector;
    }
    else if (lastVector < firstVector || lastVector > maxVector)
    {
        throw except::Exception(Ctxt("Invalid last vector"));
    }

    const size_t maxSample = mMetadata.getNumSamples(channel) - 1;
    if (firstSample > maxSample)
    {
        throw except::Exception(Ctxt("Invalid first sample"));
    }

    if (lastSample == ALL)
    {
        lastSample = maxSample;
    }
    else if (lastSample < firstSample || lastSample > maxSample)
    {
        throw except::Exception(Ctxt("Invalid last sample"));
    }

    dims.row = lastVector - firstVector + 1;
    dims.col = lastSample - firstSample + 1;

    if (isPartialRead(channel, dims) && mMetadata.isCompressed())
    {
        throw except::Exception(
                Ctxt("Cannot do partial read of compressed channel"));
    }
}

bool Wideband::isPartialRead(size_t channel,
                             const types::RowCol<size_t>& dims) const
{
    return dims.row != mMetadata.getNumVectors(channel) ||
            dims.col != mMetadata.getNumSamples(channel);
}

void Wideband::checkChannelInput(size_t channel) const
{
    if (channel >= mMetadata.getNumChannels())
    {
        throw except::Exception(Ctxt("Invalid channel"));
    }
}

void Wideband::readImpl(size_t channel,
                        size_t firstVector,
                        size_t lastVector,
                        size_t firstSample,
                        size_t lastSample,
                        void* data) const
{
    types::RowCol<size_t> dims;
    checkReadInputs(
            channel, firstVector, lastVector, firstSample, lastSample, dims);

    // Compute the byte offset into this channel's wideband in the CPHD file
    // First to the start of the first pulse we're going to read
    int64_t inOffset = getFileOffset(channel, firstVector, firstSample);

    auto dataPtr = static_cast<std::byte*>(data);
    if (dims.col == mMetadata.getNumSamples(channel))
    {
        // Life is easy - can do a single seek and read
        mInStream->seek(inOffset, io::FileInputStream::START);
        mInStream->read(dataPtr, dims.row * dims.col * mElementSize);
    }
    else
    {
        // We're stuck reading a row at a time since we're only reading some
        // of the columns
        const size_t bytesPerVectorAOI = dims.col * mElementSize;
        const size_t bytesPerVectorFile =
                mMetadata.getNumSamples(channel) * mElementSize;

        for (size_t row = 0; row < dims.row; ++row)
        {
            mInStream->seek(inOffset, io::FileInputStream::START);
            mInStream->read(dataPtr, bytesPerVectorAOI);
            dataPtr += bytesPerVectorAOI;
            inOffset += bytesPerVectorFile;
        }
    }
}

void Wideband::readImpl(size_t channel, void* data) const
{
    // Compute the byte offset into this channel's wideband in the CPHD file
    // First to the start of the first pulse we're going to read
    int64_t inOffset = getFileOffset(channel);

    auto dataPtr = static_cast<std::byte*>(data);
    mInStream->seek(inOffset, io::FileInputStream::START);
    mInStream->read(dataPtr, getBytesRequiredForRead(channel));
}

void Wideband::read(size_t channel,
                    size_t firstVector,
                    size_t lastVector,
                    size_t firstSample,
                    size_t lastSample,
                    size_t numThreads,
                    const mem::BufferView<sys::ubyte>& data) const
{
    // Sanity checks
    types::RowCol<size_t> dims;
    checkReadInputs(
            channel, firstVector, lastVector, firstSample, lastSample, dims);

    const size_t numPixels(dims.row * dims.col);
    const size_t minSize = numPixels * mElementSize;
    if (data.size < minSize)
    {
        std::ostringstream ostr;
        ostr << "Need at least " << minSize << " bytes but only got "
             << data.size;
        throw except::Exception(Ctxt(ostr.str()));
    }

    // Perform the read
    readImpl(channel,
             firstVector,
             lastVector,
             firstSample,
             lastSample,
             data.data);

    // Byte swap to little endian if necessary
    // Element size is half mElementSize because it's complex
    if (shouldByteSwap())
    {
        cphd::byteSwap(data.data, mElementSize / 2, numPixels * 2, numThreads);
    }
}

size_t Wideband::getBytesRequiredForRead(size_t channel) const
{
    if (mMetadata.isCompressed())
    {
        return mMetadata.getCompressedSignalSize(channel);
    }
    else
    {
        return mMetadata.getNumBytesPerSample() *
                mMetadata.getNumSamples(channel) *
                mMetadata.getNumVectors(channel);
    }
}

size_t Wideband::getBytesRequiredForRead(size_t channel,
                                         size_t firstVector,
                                         size_t lastVector,
                                         size_t firstSample,
                                         size_t lastSample) const
{
    const auto dims = getBufferDims(
            channel, firstVector, lastVector, firstSample, lastSample);
    return dims.area() * mMetadata.getNumBytesPerSample();
}

void Wideband::read(size_t channel,
                    const mem::BufferView<sys::ubyte>& data) const
{
    // Sanity checks
    checkChannelInput(channel);

    const size_t minSize = getBytesRequiredForRead(channel);
    if (data.size < minSize)
    {
        std::ostringstream ostr;
        ostr << "Need at least " << minSize << " bytes but only got "
             << data.size;
        throw except::Exception(Ctxt(ostr.str()));
    }

    // Perform the read
    readImpl(channel, data.data);

    if (shouldByteSwap())
    {
        // TODO: Would be nice to have a way to test this without
        // logging onto Solaris...
        const size_t numPixels = getBufferDims(0, 0, ALL, 0, ALL).area();
        cphd::byteSwap(data.data,
                       mElementSize / 2,
                       numPixels * 2,
                       std::thread::hardware_concurrency());
    }
}

bool Wideband::shouldByteSwap() const
{
    return (std::endian::native == std::endian::little) && !mMetadata.isCompressed() &&
            mElementSize > 2;
}

void Wideband::read(size_t channel,
                    size_t firstVector,
                    size_t lastVector,
                    size_t firstSample,
                    size_t lastSample,
                    size_t numThreads,
                    mem::ScopedArray<sys::ubyte>& data) const
{
    types::RowCol<size_t> dims;
    checkReadInputs(
            channel, firstVector, lastVector, firstSample, lastSample, dims);

    const size_t bufSize = dims.row * dims.col * mElementSize;
    data = std::make_unique<sys::ubyte[]>(bufSize);

    read(channel,
         firstVector,
         lastVector,
         firstSample,
         lastSample,
         numThreads,
         mem::BufferView<sys::ubyte>(data.get(), bufSize));
}

void Wideband::read(size_t channel, mem::ScopedArray<sys::ubyte>& data) const
{
    const size_t bufSize = getBytesRequiredForRead(channel);
    data = std::make_unique<sys::ubyte[]>(bufSize);

    read(channel, mem::BufferView<sys::ubyte>(data.get(), bufSize));
}

bool Wideband::allOnes(const std::vector<double>& vectorScaleFactors)
{
    for (size_t ii = 0; ii < vectorScaleFactors.size(); ++ii)
    {
        if (vectorScaleFactors[ii] != 1.0)
        {
            return false;
        }
    }
    return true;
}

void Wideband::read(size_t channel,
                    size_t firstVector,
                    size_t lastVector,
                    size_t firstSample,
                    size_t lastSample,
                    const std::vector<double>& vectorScaleFactors,
                    size_t numThreads,
                    const mem::BufferView<sys::ubyte>& scratch,
                    const mem::BufferView<std::complex<float>>& data) const
{
    // Sanity checks
    types::RowCol<size_t> dims;
    checkReadInputs(
            channel, firstVector, lastVector, firstSample, lastSample, dims);

    if (vectorScaleFactors.size() != dims.row)
    {
        std::ostringstream ostr;
        ostr << "Expected " << dims.row << " vector scale factors but got "
             << vectorScaleFactors.size();
        throw except::Exception(Ctxt(ostr.str()));
    }

    if (dims.row == 0)
    {
        return;
    }

    // If the caller provides per-vector scale factors, but they're all 1's,
    // we don't need to actually apply anything
    const bool needToScale(!allOnes(vectorScaleFactors));

    const size_t numPixels(dims.row * dims.col);

    if (data.size < numPixels)
    {
        std::ostringstream ostr;
        ostr << "Need at least " << numPixels << " pixels but only got "
             << data.size;
        throw except::Exception(Ctxt(ostr.str()));
    }

    if (needToScale)
    {
        const size_t minScratchSize = numPixels * mElementSize;
        if (scratch.size < minScratchSize)
        {
            std::ostringstream ostr;
            ostr << "Need at least " << minScratchSize << " bytes but only got "
                 << scratch.size;
            throw except::Exception(Ctxt(ostr.str()));
        }

        // Perform the read into the scratch buffer
        readImpl(channel,
                 firstVector,
                 lastVector,
                 firstSample,
                 lastSample,
                 scratch.data);

        // Byte swap to little endian if necessary
        if ((std::endian::native == std::endian::little) && mElementSize > 2)
        {
            // Need to endian swap and then scale
            cphd::byteSwapAndScale(scratch.data,
                                   mElementSize,
                                   dims,
                                   vectorScaleFactors.data(),
                                   numThreads,
                                   data.data);
        }
        else
        {
            // Just need to scale
            scale(scratch.data,
                  mElementSize,
                  dims,
                  vectorScaleFactors.data(),
                  numThreads,
                  data.data);
        }
    }
    // We need to convert the output to floating-point data
    else if (mElementSize != 8)
    {
        // Perform the read into the scratch buffer
        readImpl(channel,
                 firstVector,
                 lastVector,
                 firstSample,
                 lastSample,
                 scratch.data);

        if ((std::endian::native == std::endian::little) && mElementSize > 2)
        {
            cphd::byteSwapAndPromote(
                    scratch.data, mElementSize, dims, numThreads, data.data);
        }
        else
        {
            promote(scratch.data, mElementSize, dims, numThreads, data.data);
        }
    }
    else
    {
        // Perform the read directly into the output buffer
        readImpl(channel,
                 firstVector,
                 lastVector,
                 firstSample,
                 lastSample,
                 data.data);

        // Byte swap to little endian if necessary
        // Element size is half mElementSize because it's complex
        if (shouldByteSwap())
        {
            cphd::byteSwap(data.data,
                           mElementSize / 2,
                           numPixels * 2,
                           numThreads);
        }
    }
}

std::ostream& operator<<(std::ostream& os, const Wideband& d)
{
    os << "Wideband::\n"
       << "  mWBOffset: " << d.mWBOffset << "\n"
       << "  mWBSize: " << d.mWBSize << "\n"
       << "  mElementSize: " << d.mElementSize << "\n";

    if (d.mOffsets.empty())
    {
        os << "   mOffsets: (empty)"
           << "\n";
    }
    else
    {
        for (size_t ii = 0; ii < d.mOffsets.size(); ++ii)
        {
            os << "[" << ii << "] mOffsets: " << d.mOffsets[ii] << "\n";
        }
    }
    return os;
}
}

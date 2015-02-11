/******************************************************************************
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * Developed in the performance of one or more U.S. Government contracts.
 * The U.S. Government has Unlimited Rights in this computer software as set
 * forth in the Rights in Technical Data and Compute Software: Noncommercial
 * Items clauses.
 ******************************************************************************/

#include <limits>
#include <sstream>

#include <sys/Conf.h>
#include <mt/ThreadGroup.h>
#include <mt/ThreadPlanner.h>
#include <except/Exception.h>
#include <io/FileInputStream.h>
#include <algs/ByteSwap.h>
#include <cphd/Utilities.h>
#include <cphd/Wideband.h>

namespace
{
// TODO: Maybe this should go in sys/Conf.h
//       It's more flexible in that it properly handles float's - you can't
//       just call sys::byteSwap(floatVal) because the compiler may change the
//       byte-swapped float value into a valid IEEE value beforehand.
// TODO: If we're really looking to optimize this, could specialize it for
//       sizes of 2, 4, and 8 to eliminate the for loop
template <typename T>
inline
void byteSwap(const void* in, T& out)
{
    const sys::ubyte* const inPtr = static_cast<const sys::ubyte*>(in);
    sys::ubyte* const outPtr = reinterpret_cast<sys::ubyte*>(&out);

    for (size_t ii = 0, jj = sizeof(T) - 1; ii < jj; ++ii, --jj)
    {
        outPtr[ii] = inPtr[jj];
        outPtr[jj] = inPtr[ii];
    }
}

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
                mOutput[idx] = std::complex<float>(input.real() * scaleFactor,
                                                   input.imag() * scaleFactor);
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
void scale(const void* input,
           const types::RowCol<size_t>& dims,
           const double* scaleFactors,
           size_t numThreads,
           std::complex<float>* output)
{
    if (numThreads <= 1)
    {
        ScaleRunnable<InT>(static_cast<const std::complex<InT>*>(input),
                           0, dims.row, dims.col, scaleFactors, output).run();
    }
    else
    {
        mt::ThreadGroup threads;
        const mt::ThreadPlanner planner(dims.row, numThreads);

        size_t threadNum(0);
        size_t startRow(0);
        size_t numRowsThisThread(0);
        while (planner.getThreadInfo(threadNum++,
                                     startRow,
                                     numRowsThisThread))
        {
            std::auto_ptr<sys::Runnable> scaler(new ScaleRunnable<InT>(
                    static_cast<const std::complex<InT>*>(input),
                    startRow,
                    numRowsThisThread,
                    dims.col,
                    scaleFactors,
                    output));
            threads.createThread(scaler);
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
        scale<sys::Int8_T>(input, dims, scaleFactors, numThreads, output);
        break;
    case 4:
        scale<sys::Int16_T>(input, dims, scaleFactors, numThreads, output);
        break;
    case 8:
        scale<float>(input, dims, scaleFactors, numThreads, output);
        break;
    default:
        throw except::Exception(Ctxt(
                "Unexpected element size " + str::toString(elementSize)));
    }
}

template <typename InT>
class ByteSwapAndScaleRunnable : public sys::Runnable
{
public:
    ByteSwapAndScaleRunnable(const void* input,
                             size_t startRow,
                             size_t numRows,
                             size_t numCols,
                             const double* scaleFactors,
                             std::complex<float>* output) :
        mInput(static_cast<const sys::ubyte*>(input) +
                       startRow * numCols * sizeof(std::complex<InT>)),
        mDims(numRows, numCols),
        mScaleFactors(scaleFactors + startRow),
        mOutput(output + startRow * numCols)
    {
    }

    virtual void run()
    {
        InT real(0);
        InT imag(0);

        for (size_t row = 0, inIdx = 0, outIdx = 0; row < mDims.row; ++row)
        {
            const double scaleFactor(mScaleFactors[row]);

            for (size_t col = 0;
                 col < mDims.col;
                 ++col, inIdx += sizeof(std::complex<InT>), ++outIdx)
            {
                // Have to be careful here - can't treat mInput as a
                // std::complex<InT> directly in case InT is a float (see
                // explanation in byteSwap() comments)
                const sys::ubyte* const input = mInput + inIdx;
                byteSwap(input, real);
                byteSwap(input + sizeof(InT), imag);

                mOutput[outIdx] = std::complex<float>(real * scaleFactor,
                                                      imag * scaleFactor);
            }
        }
    }

private:
    const sys::ubyte* const mInput;
    const types::RowCol<size_t> mDims;
    const double* const mScaleFactors;
    std::complex<float>* const mOutput;
};

template <typename InT>
void byteSwapAndScale(const void* input,
                      const types::RowCol<size_t>& dims,
                      const double* scaleFactors,
                      size_t numThreads,
                      std::complex<float>* output)
{
    if (numThreads <= 1)
    {
        ByteSwapAndScaleRunnable<InT>(input, 0, dims.row, dims.col,
                                      scaleFactors, output).run();
    }
    else
    {
        mt::ThreadGroup threads;
        const mt::ThreadPlanner planner(dims.row, numThreads);

        size_t threadNum(0);
        size_t startRow(0);
        size_t numRowsThisThread(0);
        while (planner.getThreadInfo(threadNum++,
                                     startRow,
                                     numRowsThisThread))
        {
            std::auto_ptr<sys::Runnable> scaler(new ByteSwapAndScaleRunnable<InT>(
                    input,
                    startRow,
                    numRowsThisThread,
                    dims.col,
                    scaleFactors,
                    output));
            threads.createThread(scaler);
        }

        threads.joinAll();
    }
}

void byteSwapAndScale(const void* input,
                      size_t elementSize,
                      const types::RowCol<size_t>& dims,
                      const double* scaleFactors,
                      size_t numThreads,
                      std::complex<float>* output)
{
    switch (elementSize)
    {
    case 2:
        byteSwapAndScale<sys::Int8_T>(input, dims, scaleFactors, numThreads,
                                      output);
        break;
    case 4:
        byteSwapAndScale<sys::Int16_T>(input, dims, scaleFactors, numThreads,
                                       output);
        break;
    case 8:
        byteSwapAndScale<float>(input, dims, scaleFactors, numThreads,
                                output);
        break;
    default:
        throw except::Exception(Ctxt(
                "Unexpected element size " + str::toString(elementSize)));
    }
}
}

namespace cphd
{
const size_t Wideband::ALL = std::numeric_limits<size_t>::max();

Wideband::Wideband(const std::string& pathname,
                   const cphd::Data& data,
                   sys::Off_T startWB,
                   sys::Off_T sizeWB) :
    mInStream(new io::FileInputStream(pathname)),
    mData(data),
    mWBOffset(startWB),
    mWBSize(sizeWB),
    mElementSize(getNumBytesPerSample(mData.sampleType)),
    mOffsets(mData.getNumChannels())
{
    initialize();
}

Wideband::Wideband(mem::SharedPtr<io::SeekableInputStream> inStream,
                   const cphd::Data& data,
                   sys::Off_T startWB,
                   sys::Off_T sizeWB) :
    mInStream(inStream),
    mData(data),
    mWBOffset(startWB),
    mWBSize(sizeWB),
    mElementSize(getNumBytesPerSample(mData.sampleType)),
    mOffsets(mData.getNumChannels())
{
    initialize();
}

void Wideband::initialize()
{
    mOffsets[0] = mWBOffset;
    for (size_t ii = 1; ii < mData.getNumChannels(); ++ii)
    {
        const sys::Off_T offset =
            static_cast<sys::Off_T>(mData.getNumSamples(ii - 1)) *
            mData.getNumVectors(ii - 1) *
            mElementSize;

        mOffsets[ii] = mOffsets[ii - 1] + offset;
    }
}

sys::Off_T Wideband::getFileOffset(size_t channel,
                                   size_t vector,
                                   size_t sample) const
{
    if (channel >= mOffsets.size())
    {
        throw(except::Exception(Ctxt("Invalid channel number")));
    }

    if (vector >= mData.getNumVectors(channel))
    {
        throw(except::Exception(Ctxt("Invalid vector")));
    }

    if (sample >= mData.getNumSamples(channel))
    {
        throw(except::Exception(Ctxt("Invalid sample")));
    }

    const sys::Off_T bytesPerVectorFile =
            mData.getNumSamples(channel) * mElementSize;

    const sys::Off_T offset =
            mOffsets[channel] +
            bytesPerVectorFile * vector +
            sample * mElementSize;
    return offset;
}

void Wideband::checkReadInputs(size_t channel,
                               size_t firstVector,
                               size_t& lastVector,
                               size_t firstSample,
                               size_t& lastSample,
                               types::RowCol<size_t>& dims) const
{
    const size_t maxVector = mData.getNumVectors(channel) - 1;
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

    const size_t maxSample = mData.getNumSamples(channel) - 1;
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
}

void Wideband::readImpl(size_t channel,
                        size_t firstVector,
                        size_t lastVector,
                        size_t firstSample,
                        size_t lastSample,
                        void* data)
{
    types::RowCol<size_t> dims;
    checkReadInputs(channel, firstVector, lastVector, firstSample, lastSample,
                    dims);

    // Compute the byte offset into this channel's wideband in the CPHD file
    // First to the start of the first pulse we're going to read
    sys::Off_T inOffset = getFileOffset(channel, firstVector, firstSample);

    sys::byte* dataPtr = static_cast<sys::byte*>(data);
    if (dims.col == mData.getNumSamples(channel))
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
                    mData.getNumSamples(channel) * mElementSize;

        for (size_t row = 0; row < dims.row; ++row)
        {
            mInStream->seek(inOffset, io::FileInputStream::START);
            mInStream->read(dataPtr, bytesPerVectorAOI);
            dataPtr += bytesPerVectorAOI;
            inOffset += bytesPerVectorFile;
        }
    }
}

void Wideband::read(size_t channel,
                    size_t firstVector,
                    size_t lastVector,
                    size_t firstSample,
                    size_t lastSample,
                    size_t numThreads,
                    const algs::BufferView<sys::ubyte>& data)
{
    // Sanity checks
    types::RowCol<size_t> dims;
    checkReadInputs(channel, firstVector, lastVector, firstSample, lastSample,
                    dims);

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
    readImpl(channel, firstVector, lastVector, firstSample, lastSample,
             data.data);

    // Byte swap to little endian if necessary
    // Element size is half mElementSize because it's complex
    if (!sys::isBigEndianSystem() && mElementSize > 2)
    {
        algs::byteSwap(data.data, mElementSize / 2, numPixels * 2, numThreads);
    }
}

void Wideband::read(size_t channel,
                    size_t firstVector,
                    size_t lastVector,
                    size_t firstSample,
                    size_t lastSample,
                    size_t numThreads,
                    mem::ScopedArray<sys::ubyte>& data)
{
    types::RowCol<size_t> dims;
    checkReadInputs(channel, firstVector, lastVector, firstSample, lastSample,
                    dims);

    const size_t bufSize = dims.row * dims.col * mElementSize;
    data.reset(new sys::ubyte[bufSize]);

    read(channel, firstVector, lastVector, firstSample, lastSample, numThreads,
         algs::BufferView<sys::ubyte>(data.get(), bufSize));
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
                    const algs::BufferView<sys::ubyte>& scratch,
                    const algs::BufferView<std::complex<float> >& data)
{
    // Sanity checks
    types::RowCol<size_t> dims;
    checkReadInputs(channel, firstVector, lastVector, firstSample, lastSample,
                    dims);

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

        // TODO: If we wanted to get fancy here, we could do this in blocks so
        //       that we wouldn't need as big of a scratch buffer

        // Perform the read into the scratch buffer
        readImpl(channel, firstVector, lastVector, firstSample, lastSample,
                 scratch.data);

        // Byte swap to little endian if necessary
        if (!sys::isBigEndianSystem() && mElementSize > 2)
        {
            // Need to endian swap and then scale
            byteSwapAndScale(scratch.data, mElementSize, dims,
                             &vectorScaleFactors[0], numThreads, data.data);
        }
        else
        {
            // Just need to scale
            scale(scratch.data, mElementSize, dims, &vectorScaleFactors[0],
                  numThreads, data.data);
        }
    }
    else
    {
        // Perform the read directly into the output buffer
        readImpl(channel, firstVector, lastVector, firstSample, lastSample,
                 data.data);

        // Byte swap to little endian if necessary
        // Element size is half mElementSize because it's complex
        if (!sys::isBigEndianSystem() && mElementSize > 2)
        {
            algs::byteSwap(data.data, mElementSize / 2, numPixels / 2,
                           numThreads);
        }
    }
}

std::ostream& operator<< (std::ostream& os, const Wideband& d)
{
    os << "Wideband::\n"
       << "  mData: " << d.mData << "\n"
       << "  mWBOffset: " << d.mWBOffset << "\n"
       << "  mWBSize: " << d.mWBSize << "\n"
       << "  mElementSize: " << d.mElementSize << "\n";

    if (d.mOffsets.empty())
    {
        os << "   mOffsets: (empty)" << "\n";
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

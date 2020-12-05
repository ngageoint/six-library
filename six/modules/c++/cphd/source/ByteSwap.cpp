/* =========================================================================
 * This file is part of cphd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2019, MDA Information Systems LLC
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
#include <sys/Conf.h>
#include <mt/ThreadPlanner.h>
#include <mt/ThreadGroup.h>
#include <nitf/cstddef.h>
#include <cphd/ByteSwap.h>

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
    const std::byte* const inPtr = static_cast<const std::byte*>(in);
    std::byte* const outPtr = reinterpret_cast<std::byte*>(&out);

    for (size_t ii = 0, jj = sizeof(T) - 1; ii < jj; ++ii, --jj)
    {
        outPtr[ii] = inPtr[jj];
        outPtr[jj] = inPtr[ii];
    }
}

class ByteSwapRunnable : public sys::Runnable
{
public:
    ByteSwapRunnable(void* buffer,
                     size_t elemSize,
                     size_t startElement,
                     size_t numElements) :
        mBuffer(static_cast<std::byte*>(buffer) + startElement * elemSize),
        mElemSize(static_cast<unsigned short>(elemSize)),
        mNumElements(numElements)
    {
    }

    virtual void run()
    {
        sys::byteSwap(mBuffer, mElemSize, mNumElements);
    }

private:
    std::byte* const mBuffer;
    const unsigned short mElemSize;
    const size_t mNumElements;
};

template <typename InT>
class ByteSwapAndPromoteRunnable : public sys::Runnable
{
public:
    ByteSwapAndPromoteRunnable(const void* input,
                             size_t startRow,
                             size_t numRows,
                             size_t numCols,
                             std::complex<float>* output) :
        mInput(static_cast<const std::byte*>(input) +
                       startRow * numCols * sizeof(std::complex<InT>)),
        mDims(numRows, numCols),
        mOutput(output + startRow * numCols)
    {
    }

    virtual void run()
    {
        InT real(0);
        InT imag(0);

        for (size_t row = 0, inIdx = 0, outIdx = 0; row < mDims.row; ++row)
        {
            for (size_t col = 0;
                 col < mDims.col;
                 ++col, inIdx += sizeof(std::complex<InT>), ++outIdx)
            {
                // Have to be careful here - can't treat mInput as a
                // std::complex<InT> directly in case InT is a float (see
                // explanation in byteSwap() comments)
                const std::byte* const input = mInput + inIdx;
                byteSwap(input, real);
                byteSwap(input + sizeof(InT), imag);

                mOutput[outIdx] = std::complex<float>(real,
                                                      imag);
            }
        }
    }

private:
    const std::byte* const mInput;
    const types::RowCol<size_t> mDims;
    std::complex<float>* const mOutput;
};


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
        mInput(static_cast<const std::byte*>(input) +
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
                const std::byte* const input = mInput + inIdx;
                byteSwap(input, real);
                byteSwap(input + sizeof(InT), imag);

                mOutput[outIdx] = std::complex<float>(
                        static_cast<float>(real * scaleFactor),
                        static_cast<float>(imag * scaleFactor));
            }
        }
    }

private:
    const std::byte* const mInput;
    const types::RowCol<size_t> mDims;
    const double* const mScaleFactors;
    std::complex<float>* const mOutput;
};

template <typename InT>
void byteSwapAndPromote(const void* input,
                      const types::RowCol<size_t>& dims,
                      size_t numThreads,
                      std::complex<float>* output)
{
    if (numThreads <= 1)
    {
        ByteSwapAndPromoteRunnable<InT>(input, 0, dims.row, dims.col,output).run();
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
            std::unique_ptr<sys::Runnable> scaler(
                new ByteSwapAndPromoteRunnable<InT>(
                    input,
                    startRow,
                    numRowsThisThread,
                    dims.col,
                    output));
            threads.createThread(std::move(scaler));
        }

        threads.joinAll();
    }
}

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
            std::unique_ptr<sys::Runnable> scaler(new ByteSwapAndScaleRunnable<InT>(
                    input,
                    startRow,
                    numRowsThisThread,
                    dims.col,
                    scaleFactors,
                    output));
            threads.createThread(std::move(scaler));
        }

        threads.joinAll();
    }
}
}

namespace cphd
{
void byteSwap(void* buffer,
              size_t elemSize,
              size_t numElements,
              size_t numThreads)
{
    if (numThreads <= 1)
    {
        sys::byteSwap(buffer,
                      static_cast<unsigned short>(elemSize),
                      numElements);
    }
    else
    {
        mt::ThreadGroup threads;
        const mt::ThreadPlanner planner(numElements, numThreads);

        size_t threadNum(0);
        size_t startElement(0);
        size_t numElementsThisThread(0);
        while (planner.getThreadInfo(threadNum++,
                                     startElement,
                                     numElementsThisThread))
        {
            std::unique_ptr<sys::Runnable> thread(new ByteSwapRunnable(
                    buffer,
                    elemSize,
                    startElement,
                    numElementsThisThread));

            threads.createThread(std::move(thread));
        }
        threads.joinAll();
    }
}

void byteSwapAndPromote(const void* input,
                      size_t elementSize,
                      const types::RowCol<size_t>& dims,
                      size_t numThreads,
                      std::complex<float>* output)
{
    switch (elementSize)
    {
    case 2:
        ::byteSwapAndPromote<int8_t>(input, dims, numThreads, output);
        break;
    case 4:
        ::byteSwapAndPromote<int16_t>(input, dims, numThreads, output);
        break;
    case 8:
        ::byteSwapAndPromote<float>(input, dims, numThreads, output);
        break;
    default:
        throw except::Exception(Ctxt(
                "Unexpected element size " + str::toString(elementSize)));
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
        ::byteSwapAndScale<int8_t>(input, dims, scaleFactors, numThreads,
                                        output);
        break;
    case 4:
        ::byteSwapAndScale<int16_t>(input, dims, scaleFactors, numThreads,
                                         output);
        break;
    case 8:
        ::byteSwapAndScale<float>(input, dims, scaleFactors, numThreads,
                                  output);
        break;
    default:
        throw except::Exception(Ctxt(
                "Unexpected element size " + str::toString(elementSize)));
    }
}
}

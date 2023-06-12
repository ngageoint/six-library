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
#include <cphd/ByteSwap.h>

#include <stdint.h>

#include <string>
#include <memory>
#include <type_traits>
#include <std/span>
#include <std/cstddef>
#include <tuple>

#include <sys/Conf.h>
#include <sys/ByteSwap.h>
#include <sys/ByteSwapValue.h>
#include <sys/Span.h>
#include <sys/Runnable.h>
#include <mt/ThreadPlanner.h>
#include <mt/ThreadGroup.h>
#include <mt/ThreadedByteSwap.h>
#include <nitf/coda-oss.hpp>

namespace
{
template <typename T>
inline void byteSwap(const void* in, T& out)
{
    auto const inBytes = sys::make_span<std::byte>(in, sizeof(T));
    out = sys::byteSwapValue<T>(inBytes);
}

inline const std::byte* calc_offset(const void* input_, size_t offset)
{
    auto input = static_cast<const std::byte*>(input_);
    return input + offset;
}

template <typename InT>
struct ByteSwapAndPromoteRunnable final : public sys::Runnable
{
    ByteSwapAndPromoteRunnable(const void* input,
                             size_t startRow,
                             size_t numRows,
                             size_t numCols,
                             std::complex<float>* output) :
        mInput(calc_offset(input, startRow * numCols * sizeof(std::complex<InT>))),
        mDims(numRows, numCols),
        mOutput(output + startRow * numCols)
    {
    }

    void run() override
    {
        InT real(0);
        InT imag(0);

        for (size_t row = 0, inIdx = 0, outIdx = 0; row < mDims.row; ++row)
        {
            for (size_t col = 0; col < mDims.col; ++col, inIdx += sizeof(std::complex<InT>), ++outIdx)
            {
                // Have to be careful here - can't treat mInput as a
                // std::complex<InT> directly in case InT is a float (see
                // explanation in byteSwap() comments)
                const auto input = calc_offset(mInput, inIdx);
                byteSwap(input, real);
                byteSwap(calc_offset(input, sizeof(InT)), imag);

                mOutput[outIdx] = std::complex<float>(real, imag);
            }
        }
    }

private:
    const std::byte* const mInput;
    const types::RowCol<size_t> mDims;
    std::complex<float>* const mOutput;
};


template <typename InT>
struct ByteSwapAndScaleRunnable final : public sys::Runnable
{
    ByteSwapAndScaleRunnable(const void* input,
                             size_t startRow,
                             size_t numRows,
                             size_t numCols,
                             const double* scaleFactors,
                             std::complex<float>* output) :
        mInput(calc_offset(input, startRow * numCols * sizeof(std::complex<InT>))),
        mDims(numRows, numCols),
        mScaleFactors(scaleFactors + startRow),
        mOutput(output + startRow * numCols)
    {
    }

    void run() override
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
                const auto input = calc_offset(mInput, inIdx);
                byteSwap(input, real);
                byteSwap(calc_offset(input, sizeof(InT)), imag);

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
            auto scaler = std::make_unique<ByteSwapAndPromoteRunnable<InT>>(
                    input,
                    startRow,
                    numRowsThisThread,
                    dims.col,
                    output);
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
            auto scaler = std::make_unique<ByteSwapAndScaleRunnable<InT>>(
                    input,
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
}

namespace cphd
{
void byteSwap(void* buffer, size_t elemSize, size_t numElements, size_t numThreads)
{
    return mt::threadedByteSwap(buffer, elemSize, numElements, numThreads);
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
                "Unexpected element size " + std::to_string(elementSize)));
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
                "Unexpected element size " + std::to_string(elementSize)));
    }
}
}

/* =========================================================================
 * This file is part of sys-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 * (C) Copyright 2023, Maxar Technologies, Inc.
 *
 * sys-c++ is free software; you can redistribute it and/or modify
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

#ifndef CODA_OSS_sys_ByteSwap_h_INCLUDED_
#define CODA_OSS_sys_ByteSwap_h_INCLUDED_
#pragma once

#include <stdint.h>
#include <stdlib.h>

#include <coda_oss/span.h>
#include <coda_oss/cstddef.h>

#include "config/Exports.h"

#include "ByteSwapValue.h"
#include "Runnable.h"

namespace sys
{
/*!
 *  Swap bytes in-place.  Note that a complex pixel
 *  is equivalent to two floats so elemSize and numElems
 *  must be adjusted accordingly.
 *
 *  \param [inout] buffer to transform
 *  \param elemSize
 *  \param numElems
 */
coda_oss::span<const coda_oss::byte> CODA_OSS_API byteSwap(coda_oss::span<coda_oss::byte>buffer, size_t elemSize);
void CODA_OSS_API byteSwap(void* buffer, size_t elemSize, size_t numElems);

/*!
 *  Swap bytes into output buffer.  Note that a complex pixel
 *  is equivalent to two floats so elemSize and numElems
 *  must be adjusted accordingly.
 *
 *  \param buffer to transform
 *  \param elemSize
 *  \param numElems
 *  \param[out] outputBuffer buffer to write swapped elements to
 */
coda_oss::span<const coda_oss::byte> CODA_OSS_API byteSwap(coda_oss::span<const coda_oss::byte> buffer,
         size_t elemSize, coda_oss::span<coda_oss::byte> outputBuffer);
void CODA_OSS_API byteSwap(const void* buffer, size_t elemSize, size_t numElems, void* outputBuffer);

struct ByteSwapRunnable final : public sys::Runnable
{
    ByteSwapRunnable(void* buffer, size_t elemSize, size_t startElement, size_t numElements) :
        mBuffer(static_cast<coda_oss::byte*>(buffer) + startElement * elemSize),
        mElemSize(elemSize), mNumElements(numElements)
    {
    }
    void run() override
    {
        byteSwap(mBuffer, mElemSize, mNumElements);
    }

    virtual ~ByteSwapRunnable() = default;
    ByteSwapRunnable(const ByteSwapRunnable&) = delete;
    ByteSwapRunnable& operator=(const ByteSwapRunnable&) = delete;
    ByteSwapRunnable(ByteSwapRunnable&&) = delete;
    ByteSwapRunnable& operator=(ByteSwapRunnable&&) = delete;

private:
    void* const mBuffer;
    const size_t mElemSize;
    const size_t mNumElements;
};

struct ByteSwapCopyRunnable final : public sys::Runnable
{
    ByteSwapCopyRunnable(const void* buffer, size_t elemSize, size_t startElement, size_t numElements, void* outputBuffer) :
        mBuffer(static_cast<const coda_oss::byte*>(buffer) + startElement * elemSize),
        mElemSize(elemSize), mNumElements(numElements),
         mOutputBuffer(static_cast<coda_oss::byte*>(outputBuffer) + startElement * elemSize)
    {
    }
    void run() override
    {
        byteSwap(mBuffer, mElemSize, mNumElements, mOutputBuffer);
    }

    virtual ~ByteSwapCopyRunnable() = default;
    ByteSwapCopyRunnable(const ByteSwapCopyRunnable&) = delete;
    ByteSwapCopyRunnable& operator=(const ByteSwapCopyRunnable&) = delete;
    ByteSwapCopyRunnable(ByteSwapCopyRunnable&&) = delete;
    ByteSwapCopyRunnable& operator=(ByteSwapCopyRunnable&&) = delete;

private:
    const void* const mBuffer;
    const size_t mElemSize;
    const size_t mNumElements;
    void* const mOutputBuffer;
};

}
#endif  // CODA_OSS_sys_ByteSwap_h_INCLUDED_

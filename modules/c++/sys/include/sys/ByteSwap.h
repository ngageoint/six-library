#ifndef CODA_OSS_sys_ByteSwap_h_INCLUDED_
#define CODA_OSS_sys_ByteSwap_h_INCLUDED_
#pragma once

#include "coda_oss/span.h"

#include "Runnable.h"

namespace sys
{
template <typename T, typename U = T>
struct ByteSwapCopyRunnable final : public sys::Runnable
{
    ByteSwapCopyRunnable(coda_oss::span<const T> buffer, coda_oss::span<U> outputBuffer) :
        mBuffer(buffer), mOutputBuffer(outputBuffer)
    {
    }

    virtual ~ByteSwapCopyRunnable() = default;
    ByteSwapCopyRunnable(const ByteSwapCopyRunnable&) = delete;
    ByteSwapCopyRunnable& operator=(const ByteSwapCopyRunnable&) = delete;
    ByteSwapCopyRunnable(ByteSwapCopyRunnable&&) = default;
    ByteSwapCopyRunnable& operator=(ByteSwapCopyRunnable&&) = default;

    void run() override
    {
        sys::byteSwap(mBuffer, mOutputBuffer);
    }

private:
    const coda_oss::span<const T> mBuffer;
    const coda_oss::span<U> mOutputBuffer;
};

}

#endif  // CODA_OSS_sys_ByteSwap_h_INCLUDED_

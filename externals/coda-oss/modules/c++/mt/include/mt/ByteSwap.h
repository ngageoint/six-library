#ifndef CODA_OSS_mt_ByteSwap_h_INCLUDED_
#define CODA_OSS_mt_ByteSwap_h_INCLUDED_
#pragma once

#include <stdint.h>
#include <assert.h>
#include <memory>
#include <stdexcept>
#include <tuple>

#include "coda_oss/span.h"
#include "sys/Conf.h"
#include "sys/ByteSwap.h"

#include "ThreadPlanner.h"
#include "ThreadGroup.h"

namespace mt
{
/*
 * Threaded byte-swapping and copy
 *
 * \param buffer Buffer to swap
 * \param numElements Number of elements in 'buffer'
 * \param numThreads Number of threads to use for byte-swapping
 * \param outputBuffer buffer to write into
 */
template<typename T, typename U = T>
inline void threadedByteSwap(const T* buffer,
              size_t numElements,
              size_t numThreads,
              U* outputBuffer)
{
    if (numThreads <= 1)
    {
        const coda_oss::span<const T> buffer_(buffer, numElements);
        const coda_oss::span<U> outputBuffer_(outputBuffer, numElements);
        sys::byteSwap(buffer_, outputBuffer_);
        return;
    }

    mt::ThreadGroup threads;
    const mt::ThreadPlanner planner(numElements, numThreads);

    size_t threadNum(0);
    size_t startElement(0);
    size_t numElementsThisThread(0);
    while (planner.getThreadInfo(threadNum++, startElement, numElementsThisThread))
    {
        const coda_oss::span<const T> buffer_(buffer+startElement, numElementsThisThread);
        const coda_oss::span<U> outputBuffer_(outputBuffer+startElement, numElementsThisThread);
        auto thread = std::make_unique<sys::ByteSwapCopyRunnable<T, U>>(buffer_, outputBuffer_);
        threads.createThread(thread.release());
    }
    threads.joinAll();
}
template<typename T, typename U = T>
inline void threadedByteSwap(coda_oss::span<const T> buffer, size_t numThreads, coda_oss::span<U> outputBuffer)
{
    // outputBuffer could be std::byte
    if (buffer.size_bytes() != outputBuffer.size_bytes())
    {
        throw std::invalid_argument("buffer.size_bytes() != outputBuffer.size_bytes()");
    }
    threadedByteSwap(buffer.data(), buffer.size(), numThreads, outputBuffer.data());
}

// Drop-in replacement for existing API; prefer routines above instead.
template <typename T>
inline void doThreadedByteSwap_(const void* buffer,
                                size_t elemSize,
                                size_t numElements,
                                size_t numThreads,
                                void* outputBuffer)
{
    assert(sizeof(T) == elemSize);
    std::ignore = elemSize; // avoid compiler warning
    const coda_oss::span<const T> buffer_(static_cast<const T*>(buffer), numElements);
    const coda_oss::span<T> outputBuffer_(static_cast<T*>(outputBuffer), numElements);
    threadedByteSwap(buffer_, numThreads, outputBuffer_);
}
inline void threadedByteSwap_(const void* buffer,
                              size_t elemSize,
                              size_t numElements,
                              size_t numThreads,
                              void* outputBuffer)
{
    if (elemSize == 2)
    {
        static_assert(sizeof(uint16_t) == 2, "sizeof(uint16_t) should be 2");
        return doThreadedByteSwap_<uint16_t>(buffer, elemSize, numElements, numThreads, outputBuffer);
    }
    if (elemSize == 4)
    {
        static_assert(sizeof(uint32_t) == 4, "sizeof(uint32_t) should be 4");
        return doThreadedByteSwap_<uint32_t>(buffer, elemSize, numElements, numThreads, outputBuffer);
    }
    if (elemSize == 8)
    {
        static_assert(sizeof(uint64_t) == 8, "sizeof(uint64_t) should be 8");
        return doThreadedByteSwap_<uint64_t>(buffer, elemSize, numElements, numThreads, outputBuffer);
    }
    throw std::invalid_argument("''elemSize' must be 2, 4, or 8.");
}

/*
 * Threaded byte-swapping
 *
 * \param buffer Buffer to swap (contents will be overridden)
 * \param numElements Number of elements in 'buffer'
 * \param numThreads Number of threads to use for byte-swapping
 */
template <typename T>
inline void threadedByteSwap(T* buffer,
              size_t numElements,
              size_t numThreads)
{
    threadedByteSwap(buffer, numElements, numThreads, buffer);
}
template <typename T>
inline void threadedByteSwap(coda_oss::span<T> buffer, size_t numThreads)
{
    threadedByteSwap(buffer.data(), buffer.size(), numThreads);
}

// Drop-in replacement for existing API; prefer routines above instead.
inline void threadedByteSwap_(void* buffer,
                              size_t elemSize,
                              size_t numElements,
                              size_t numThreads)
{
    threadedByteSwap_(buffer, elemSize, numElements, numThreads, buffer);
}

}

#endif  // CODA_OSS_mt_ByteSwap_h_INCLUDED_

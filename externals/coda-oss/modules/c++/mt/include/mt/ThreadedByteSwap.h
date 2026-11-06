/* =========================================================================
 * This file is part of mt-c++ 
 * =========================================================================
 * 
 * © 2023 Maxar Technologies, Inc.
 *
 * mt-c++ is free software; you can redistribute it and/or modify
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
#ifndef CODA_OSS_mt_ThreadedByteSwap_h_INCLUDED_
#define CODA_OSS_mt_ThreadedByteSwap_h_INCLUDED_

#include <memory>

#include "sys/ByteSwap.h"

#include "ThreadPlanner.h"
#include "ThreadGroup.h"

namespace mt
{
/*
 * Threaded byte-swapping
 *
 * \param buffer Buffer to swap (contents will be overridden)
 * \param elemSize Size of each element in 'buffer'
 * \param numElements Number of elements in 'buffer'
 * \param numThreads Number of threads to use for byte-swapping
 */
inline void threadedByteSwap(void* buffer, size_t elemSize, size_t numElements, size_t numThreads)
{
    if (numThreads <= 1)
    {
        sys::byteSwap(buffer, elemSize, numElements);
    }
    else
    {
        mt::ThreadGroup threads;
        const mt::ThreadPlanner planner(numElements, numThreads);

        size_t threadNum(0);
        size_t startElement(0);
        size_t numElementsThisThread(0);
        while (planner.getThreadInfo(threadNum++, startElement, numElementsThisThread))
        {
            auto thread = std::make_unique<sys::ByteSwapRunnable>(
                    buffer,
                    elemSize,
                    startElement,
                    numElementsThisThread);

            threads.createThread(thread.release());
        }
        threads.joinAll();
    }
}

/*
 * Threaded byte-swapping and copy
 *
 * \param buffer Buffer to swap
 * \param elemSize Size of each element in 'buffer'
 * \param numElements Number of elements in 'buffer'
 * \param numThreads Number of threads to use for byte-swapping
 * \param outputBuffer buffer to write into
 */
inline void threadedByteSwap(const void* buffer, size_t elemSize, size_t numElements, size_t numThreads, void* outputBuffer)
{
    if (numThreads <= 1)
    {
        sys::byteSwap(buffer, elemSize, numElements, outputBuffer);
    }
    else
    {
        mt::ThreadGroup threads;
        const mt::ThreadPlanner planner(numElements, numThreads);

        size_t threadNum(0);
        size_t startElement(0);
        size_t numElementsThisThread(0);
        while (planner.getThreadInfo(threadNum++, startElement, numElementsThisThread))
        {
            auto thread = std::make_unique<sys::ByteSwapCopyRunnable>(
                    buffer,
                    elemSize,
                    startElement,
                    numElementsThisThread,
                    outputBuffer);

            threads.createThread(thread.release());
        }
        threads.joinAll();

    }
}
}

#endif  // CODA_OSS_mt_ThreadedByteSwap_h_INCLUDED_

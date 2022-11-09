/* =========================================================================
 * This file is part of mt-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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

#ifndef __MT_BASIC_THREAD_POOL_H__
#define __MT_BASIC_THREAD_POOL_H__

#include <vector>
#include <memory>

#include "except/Exception.h"
#include "sys/Mutex.h"
#include "sys/Thread.h"
#include "mt/RequestQueue.h"
#include "mt/GenericRequestHandler.h"
#include "mt/ThreadPoolException.h"
#include "mem/SharedPtr.h"

namespace mt
{
template<typename RequestHandler_T>
struct BasicThreadPool
{
    /*! Constructor.  Set up the thread pool.
     *  \param numThreads the number of threads
     */
    BasicThreadPool() = default;
    BasicThreadPool(size_t numThreads) :
        mNumThreads(numThreads)
    {
    }

    //! Destructor
    virtual ~BasicThreadPool()
    {
        //destroy(static_cast<unsigned short>(mPool.size()));
        shutdown();
    }

    BasicThreadPool(const BasicThreadPool&) = delete;
    BasicThreadPool& operator=(const BasicThreadPool&) = delete;

    void start()
    {
        if (mStarted)
        {
            throw ThreadPoolException("The thread pool is already started.");
        }

        mStarted = true;
        addThreads(mNumThreads);
    }

    void join()
    {
        for (size_t i = 0; i < mPool.size(); i++)
        {
            dbg_printf("mPool[%ju]->join()\n", i);
            mPool[i]->join();
        }
        destroy();
        mStarted = false;
    }

    // This adds new threads immediately and is safe to call even after
    // start() has been called
    void grow(size_t bySize)
    {
        mNumThreads += bySize;

        if (mStarted)
        {
            // It's safe to add new threads on the fly - they'll all get a
            // pointer to the same thread-safe request queue and start pulling
            // off requests once they start
            // This does assume that newRequestHandler() itself is safe to
            // call once the pool has been started
            addThreads(bySize);
        }
    }

    // This stops the pool to shrink the threads but does NOT restart the
    // pool - start() must be called again
    void shrink(size_t bySize)
    {
        if (mStarted)
        {
            join();
        }

        mNumThreads = (bySize > mNumThreads) ? 0 : mNumThreads - bySize;
    }

    void addRequest(sys::Runnable *handler)
    {
        mHandlerQueue.enqueue(handler);
    }

    size_t getSize() const
    {
        return mPool.size();
    }

    void shutdown()
    {
        // Add requests that signal the thread should stop
        static sys::Runnable* stopSignal = nullptr;
        for (size_t i = 0; i < mPool.size(); ++i)
        {
            addRequest(stopSignal);
        }
        // Join all threads
        join();
        // Clear the request queue - mainly just cleanup in case we reuse
        mHandlerQueue.clear();
    }

protected:

    // Derive this to use a new kind of request handler
    // For instance, you may want an IterativeRequestHandler
    virtual RequestHandler_T *newRequestHandler()
    {
        return std::make_unique<RequestHandler_T>(&mHandlerQueue).release();
    }

    void destroy()
    {
        mPool.clear();
    }

    bool mStarted = false;
    size_t mNumThreads = 0;
    std::vector<std::shared_ptr<sys::Thread>> mPool;
    mt::RunnableRequestQueue mHandlerQueue;

private:
    void addThread()
    {
        auto thread(std::make_shared<sys::Thread>(newRequestHandler()));
        mPool.push_back(thread);
        thread->start();
    }

    void addThreads(size_t numThreads)
    {
        for (size_t ii = 0; ii < numThreads; ++ii)
        {
            addThread();
        }
    }
};
}

#endif

/* =========================================================================
 * This file is part of mt-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
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
#include "except/Exception.h"
#include "sys/Mutex.h"
#include "sys/Thread.h"
#include "mt/RequestQueue.h"
#include "mt/GenericRequestHandler.h"
#include "mt/ThreadPoolException.h"

namespace mt
{
template<typename RequestHandler_T>
class BasicThreadPool
{
public:

    /*! Constructor.  Set up the thread pool.
     *  \param numThreads the number of threads
     */
    BasicThreadPool(unsigned short numThreads = 0) :
        mStarted(false)
    {
        mNumThreads = numThreads;
    }

    //! Deconstructor
    virtual ~BasicThreadPool()
    {
        destroy(static_cast<unsigned short>(mPool.size()));
    }

    virtual void start()
    {
        if (mStarted)
            throw(ThreadPoolException("The thread pool is already started."));

        mStarted = true;

        for (unsigned short i = 0; i < mNumThreads; i++)
        {
            sys::Thread *thread = new sys::Thread(newRequestHandler());
            mPool.push_back(thread);
            mPool[i]->start();
        }
    }

    virtual void join()
    {
        for (size_t i = 0; i < mPool.size(); i++)
        {
            dbg_printf("mPool[%d]->join()\n", i);
            mPool[i]->join();
        }
        destroy(static_cast<unsigned short>(mPool.size()));
        mStarted = false;
    }

    virtual void grow(unsigned short bySize)
    {
        mNumThreads += bySize;
    }

    virtual void shrink(unsigned short bySize)
    {
        if (mStarted)
            join();

        mNumThreads = (bySize > mNumThreads) ? 0 : mNumThreads - bySize;
    }

    void addRequest(sys::Runnable *handler)
    {
        mHandlerQueue.enqueue(handler);
    }

    virtual size_t getSize() const
    {
        return mPool.size();
    }

    virtual void shutdown()
    {
        // Add requests that signal the thread should stop
        static sys::Runnable *stopSignal = NULL;
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
        return new RequestHandler_T(&mHandlerQueue);
    }

    virtual void destroy(unsigned short bySize)
    {
        if (bySize > mPool.size())
            bySize = static_cast<unsigned short>(mPool.size());

        for (unsigned short i = 0; i < bySize; i++)
        {
            sys::Thread *t = mPool.back();
            mPool.pop_back();
            delete t;
        }
    }

    bool mStarted;
    unsigned short mNumThreads;
    std::vector<sys::Thread *> mPool;
    mt::RunnableRequestQueue mHandlerQueue;
};
}

#endif

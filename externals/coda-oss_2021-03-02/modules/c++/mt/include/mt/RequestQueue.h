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

#ifndef __MT_REQUEST_QUEUE_H__
#define __MT_REQUEST_QUEUE_H__

#include <queue>
#include "sys/Thread.h"
#include "sys/ConditionVar.h"
#include "sys/Mutex.h"
#include "sys/Dbg.h"

namespace mt
{

/*!
 *
 *  \class RequestQueue
 *  \brief Locked, dual condition request queue
 *
 *  This is a generic class for locked buffers.  Stick
 *  anything in T and it will be protected by a queue lock 
 *  and two condition variables.  When you call dequeue, this
 *  class blocks until there is data (there is a critical section).
 *
 *  This class is the basis for the two provided thread pool APIs,
 *  AbstractThreadPool<Request_T> and BasicThreadPool<RequestHandler_T>
 *
 *
 */

template<typename T>
struct RequestQueue
{
    //! Default constructor
    RequestQueue() :
        mAvailableSpace(&mQueueLock),
        mAvailableItems(&mQueueLock)
    {
    }

    // Put a (copy of, unless T is a pointer) request on the queue
    void enqueue(T request)
    {
#ifdef THREAD_DEBUG
        dbg_printf("Locking (enqueue)\n");
#endif
        mQueueLock.lock();
        mRequestQueue.push(request);
#ifdef THREAD_DEBUG
        dbg_printf("Unlocking (enqueue), new size [%d]\n", mRequestQueue.size());
#endif
        mQueueLock.unlock();

        mAvailableItems.signal();
    }

    // Retrieve (by reference) T from the queue. blocks until ok
    void dequeue(T& request)
    {
#ifdef THREAD_DEBUG
        dbg_printf("Locking (dequeue)\n");
#endif
        mQueueLock.lock();
        while (isEmpty())
        {
            mAvailableItems.wait();
        }

        request = mRequestQueue.front();
        mRequestQueue.pop();

#ifdef THREAD_DEBUG
        dbg_printf("Unlocking (dequeue), new size [%d]\n", mRequestQueue.size());
#endif
        mQueueLock.unlock();
        mAvailableSpace.signal();
    }

    // Check to see if its empty
    inline bool isEmpty()
    {
        return (mRequestQueue.size() == 0);
    }

    // Check the length
    inline int length()
    {
        return mRequestQueue.size();
    }

    void clear()
    {
#ifdef THREAD_DEBUG
        dbg_printf("Locking (dequeue)\n");
#endif
        mQueueLock.lock();
        while (!isEmpty())
        {
            mRequestQueue.pop();
        }

#ifdef THREAD_DEBUG
        dbg_printf("Unlocking (dequeue), new size [%d]\n", mRequestQueue.size());
#endif
        mQueueLock.unlock();
        mAvailableSpace.signal();
    }

    RequestQueue(const RequestQueue&) = delete;
    RequestQueue& operator=(const RequestQueue&) = delete;

private:
    //! The internal data structure
    std::queue<T> mRequestQueue;
    //! The synchronizer
    sys::Mutex mQueueLock;
    //! This condition is "is there space?"
    sys::ConditionVar mAvailableSpace;
    //! This condition is "is there an item?"
    sys::ConditionVar mAvailableItems;
};

typedef RequestQueue<sys::Runnable*> RunnableRequestQueue;
}

#endif // __MT_REQUEST_QUEUE_H__

/* =========================================================================
 * This file is part of mt-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 * (C) Copyright 2025-26 ARKA Group, L.P. All rights reserved
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

#include <deque>
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
public:
    //! Default constructor
    RequestQueue() :
        mAvailableSpace(&mQueueLock),
        mAvailableItems(&mQueueLock)
    {
    }

    //! Puts the request at the front of the queue
    void priorityEnqueue(T request)
    {
#ifdef THREAD_DEBUG
        dbg_printf("Locking (enqueue)\n");
#endif
        mQueueLock.lock();
        mRequestQueue.push_front(request);
#ifdef THREAD_DEBUG
        dbg_printf("Unlocking (enqueue), new size [%d]\n", mRequestQueue.size());
#endif
        mQueueLock.unlock();

        mAvailableItems.signal();
    }

    //! Put a (copy of, unless T is a pointer) request on the queue
    void enqueue(T request)
    {
#ifdef THREAD_DEBUG
        dbg_printf("Locking (enqueue)\n");
#endif
        mQueueLock.lock();
        mRequestQueue.push_back(request);
#ifdef THREAD_DEBUG
        dbg_printf("Unlocking (enqueue), new size [%d]\n", mRequestQueue.size());
#endif
        mQueueLock.unlock();

        mAvailableItems.signal();
    }

    //! Retrieve (by reference) T from the queue. blocks until ok
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
        mRequestQueue.pop_front();

#ifdef THREAD_DEBUG
        dbg_printf("Unlocking (dequeue), new size [%d]\n", mRequestQueue.size());
#endif
        mQueueLock.unlock();
        mAvailableSpace.signal();
    }
    
    //! Retrieves a copy of the n'th item from the front of the queue (0 = first item) without removing it
    T peek(size_t n = 0)
    {
        T request;
#ifdef THREAD_DEBUG
        dbg_printf("Locking (peek)\n");
#endif
        mQueueLock.lock();
        if (mRequestQueue.size() > n)
        {
            request = mRequestQueue[n];
        }
        else
        {
            mQueueLock.unlock();
            throw except::Exception(Ctxt("Request queue cannot peek beyond end of queue"));
        }
        mQueueLock.unlock();
#ifdef THREAD_DEBUG
        dbg_printf("Unlocking (peek)\n");
#endif

        return request;
    }

    //! Lets the n'th request from the front cut in line and dequeue
    //! NOTE: The RequestQueue does not prevent changes to the queue between
    //! when peak() and cutAndDequeue() are called
    void cutAndDequeue(size_t n, T& request)
    {
#ifdef THREAD_DEBUG
        dbg_printf("Locking (peek)\n");
#endif
        mQueueLock.lock();
        if (mRequestQueue.size() > n)
        {
            request = mRequestQueue[n];
            mRequestQueue.erase(mRequestQueue.begin()+n);
        }
        else
        {
            mQueueLock.unlock();
            throw except::Exception(Ctxt("Request queue cannot access beyond end of queue"));
        }
        mQueueLock.unlock();
#ifdef THREAD_DEBUG
        dbg_printf("Unlocking (peek)\n");
#endif
    }

    //! Check to see if its empty
    inline bool isEmpty() const
    {
        return mRequestQueue.empty();
    }

    //! Check the length
    int length() const
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
            mRequestQueue.pop_front();
        }

#ifdef THREAD_DEBUG
        dbg_printf("Unlocking (dequeue), new size [%d]\n", mRequestQueue.size());
#endif
        mQueueLock.unlock();
        mAvailableSpace.signal();
    }

    //! Aggregates ProcFunctor of all of the elements of the queue
    template <typename ProcFunctor, typename AggregateType>
    AggregateType aggregate(const ProcFunctor& aggregate, const AggregateType& initial)
    {
        mQueueLock.lock();
        AggregateType cumulative = initial;
        for (typename std::deque<T>::iterator iter = mRequestQueue.begin();
             iter != mRequestQueue.end();
             ++iter)
        {
            cumulative = aggregate(*iter, cumulative);
        }

        mQueueLock.unlock();
        return cumulative;
    }

    //! Remove the given request from the queue
    // Does nothing if the given request is not in the queue
    // \return true if an item was removed, false otherwise
    template <typename CmpFunctor>
    bool removeRequest(const CmpFunctor& compare)
    {
        mQueueLock.lock();
        for (typename std::deque<T>::iterator iter = mRequestQueue.begin();
             iter != mRequestQueue.end();
             ++iter)
        {
            if (compare(*iter))
            {
                mRequestQueue.erase(iter);
                mQueueLock.unlock();
                mAvailableSpace.signal();
                return true;
            }
        }
        mQueueLock.unlock();
        return false;
    }

private:
    RequestQueue(const RequestQueue&) = delete;
    RequestQueue& operator=(const RequestQueue&) = delete;

private:
    //! The internal data structure
    std::deque<T> mRequestQueue;
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

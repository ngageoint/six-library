/* =========================================================================
 * This file is part of mt-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2026, MDA Information Systems LLC
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

#ifndef __MT_ORDERED_REQUEST_QUEUE_H__
#define __MT_ORDERED_REQUEST_QUEUE_H__

#include <set>
#include "sys/Thread.h"
#include "sys/ConditionVar.h"
#include "sys/Mutex.h"
#include "sys/Dbg.h"

namespace mt
{
template <typename T>
class AbstractComparator
{
 public:
    virtual bool operator()(const T& lhs, const T& rhs) const
    {
        return lhs < rhs;
    }
};

/*!
 *
 *  \class OrderedRequestQueue
 *  \brief Thread-safe altenrative to request queue
 *
 *  std::set inserts an element into a thread lock request queue
 *  that orders its elements based off of the provided operator. 
 *  Dequeue blocks the thread until an element is avaliable. 
 *
 */

template<typename T, typename CmpFtor = AbstractComparator<T>>
class OrderedRequestQueue
{
public:
    //! Default constructor
    OrderedRequestQueue() :
        mAvailableSpace(&mQueueLock),
        mAvailableItems(&mQueueLock)
    {
    }

    OrderedRequestQueue(const CmpFtor f) :
        mRequestQueue(f),
        mAvailableSpace(&mQueueLock),
        mAvailableItems(&mQueueLock)
    {
    }

    //! Put a (copy of, unless T is a pointer) request on the queue
    void enqueue(T request)
    {
#ifdef THREAD_DEBUG
        dbg_printf("Locking (enqueue)\n");
#endif
        mQueueLock.lock();

        mRequestQueue.insert(request);
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

        auto first = mRequestQueue.begin();
        request = *first;
        mRequestQueue.erase(first);

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
            auto iter = mRequestQueue.begin();
            while (n--)
            {
                ++iter;
            }
            request = *iter;
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
    void cutAndDequeue(size_t n, T& request)
    {
#ifdef THREAD_DEBUG
        dbg_printf("Locking (peek)\n");
#endif
        mQueueLock.lock();
        if (mRequestQueue.size() > n)
        {
            auto iter = mRequestQueue.begin();
            while (n--)
            {
                ++iter;
            }
            request = *iter;
            mRequestQueue.erase(iter);
        }
        else
        {
            mQueueLock.unlock();
            throw except::Exception(Ctxt("Request queue cannot access beyond end of queue"));
        }
        mQueueLock.unlock();
        mAvailableSpace.signal();
#ifdef THREAD_DEBUG
        dbg_printf("Unlocking (peek)\n");
#endif
    }

    //! Check to see if its empty
    inline bool isEmpty()
    {
        return (mRequestQueue.size() == 0);
    }

    //! Check the length
    inline size_t length()
    {
        return mRequestQueue.size();
    }

    void clear()
    {
#ifdef THREAD_DEBUG
        dbg_printf("Locking (clear)\n");
#endif
        mQueueLock.lock();
        mRequestQueue.clear();

#ifdef THREAD_DEBUG
        dbg_printf("Unlocking (clear), new size [%d]\n", mRequestQueue.size());
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
        for (typename std::set<T>::iterator iter = mRequestQueue.begin();
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
        for (typename std::set<T>::iterator iter = mRequestQueue.begin();
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

    //! Remove the given request from the queue and return the request object
    // Does nothing if the given request is not in the queue
    // \return true if an item was removed, false otherwise
    // request is set the object of the request in the queue
    template <typename CmpFunctor>
    bool removeAndGetRequest(const CmpFunctor& compare, T& request)
    {
        mQueueLock.lock();
        for (typename std::set<T>::iterator iter = mRequestQueue.begin();
             iter != mRequestQueue.end();
             ++iter)
        {
            if (compare(*iter))
            {
                request = *iter;
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
    // Noncopyable
    OrderedRequestQueue(const OrderedRequestQueue& );
    const OrderedRequestQueue& operator=(const OrderedRequestQueue& );

private:
    //! The internal data structure
    std::set<T, CmpFtor> mRequestQueue;
    //! The synchronizer
    sys::Mutex mQueueLock;
    //! This condition is "is there space?"
    sys::ConditionVar mAvailableSpace;
    //! This condition is "is there an item?"
    sys::ConditionVar mAvailableItems;
};

template <typename OrderingFtor>
using RunnableOrderedRequestQueue = OrderedRequestQueue<sys::Runnable*, OrderingFtor>;
}

#endif // __MT_REQUEST_QUEUE_H__

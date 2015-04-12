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


#ifndef __MT_ABSTRACT_THREAD_POOL_H__
#define __MT_ABSTRACT_THREAD_POOL_H__

#include <vector>

#include "sys/Thread.h"
#include "mt/RequestQueue.h"
#include "mt/ThreadPoolException.h"
#include "mt/WorkerThread.h"

namespace mt
{
/*!
 *  \class AbstractThreadPool
 *  \brief Partially implemented thread pool with a consumer-producer queue
 *
 *  This class is a partial implementation of a thread pool.  
 *  The production of threads has been left up to the derived class.
 *  Additionally the request to be performed/read is not specialized.  
 *  The derived class would presumably also specialize this type 
 *  in order to clarify the derived type of WorkerThread's
 *  performTask() behavior.  
 *
 */
template <typename Request_T> class AbstractThreadPool
{
public:

    /*!
    *  Constructor.  Set up the thread pool.  
    *  \param numThreads the number of threads
    */
    AbstractThreadPool(unsigned short numThreads = 0) :
            mNumThreads(numThreads)
    {}


    //! Destructor
    virtual ~AbstractThreadPool()
    {
        destroy();
    }

    /*!
    *  Intialize and start each thread running.
    *  Typically the caller will either join at this point
    *  or begin another simultaneously running loop task.
    *
    */
    void start()
    {
        for (unsigned short i = 0; i < mNumThreads; i++)
        {
            WorkerThread<Request_T> *thread = newWorker();
            mPool.push_back(thread);
            mPool[i]->start();
        }
    }

    /*!
    *  This function cannot be implemented until the WorkerThread<T>
    *  is overriden with a performTask() function.  At that point, this
    *  function can be derived to produce a pointer to the base class, 
    *  pointing at the newly derived worker thread.
    */
    virtual WorkerThread<Request_T>* newWorker() = 0;

    /*!
    *  Wait on all the threads in a pool.  If the WorkerThread<T>'s run()
    *  behavior is not overriden, this will lock up the thread of control 
    *  until the program is shut down.  
    *
    */
    void join()
    {
        for (unsigned short i = 0; i < mPool.size(); i++)
        {
            dbg_printf("mPool[%d]->join()\n", i);
            mPool[i]->join();

        }
        destroy();


    }

    void setNumThreads(unsigned short numThreads)
    {
        mNumThreads = numThreads;
    }

    void addRequest(Request_T &handler)
    {
        mRequestQueue.enqueue(handler);
    }

    /*!
    *  Get the number of threads in the pool
    *  \return The number of threads in the pool
    */
    size_t getNumThreads() const
    {
        return mPool.size();
    }

protected:

    void destroy()
    {
        size_t size = mPool.size();
        for (unsigned short i = 0; i < size; i++)
        {
            sys::Thread *t = mPool.back();
            mPool.pop_back();
            delete t;
        }
    }

    unsigned short mNumThreads;
    std::vector<sys::Thread *> mPool;
    mt::RequestQueue<Request_T> mRequestQueue;
};
}

#endif

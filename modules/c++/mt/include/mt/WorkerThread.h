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


#ifndef __WORKER_THREAD_H__
#define __WORKER_THREAD_H__


#include "sys/Thread.h"
#include "mt/RequestQueue.h"
#include "str/Convert.h"



namespace mt
{
/*!
 *  \class WorkerThread
 *  \brief Virtual class for thread pool
 *
 *  A WorkerThread object is the basic underlying thread for 
 *  AbstractThreadPool, a partially implemented pool of threads 
 *  operating on a consumer-producer buffer.  This class can be 
 *  implemented by deriving the performTask function.  The thread 
 *  runs until the program is stopped.
 */
template <typename Request_T> class WorkerThread : public sys::Thread
{
public:
    //! Constructor
    WorkerThread(mt::RequestQueue<Request_T>* requestQueue) :
            mRequestQueue(requestQueue), mDone(false)
    {}

    /*!
     *  Virtual destructor
     */
    virtual ~WorkerThread()
    {}

    virtual void initialize()
    {}
    /*!
     *  Run this request
     */
    virtual void run()
    {
        initialize();
        while (!isDone())
        {
            // Pull a runnable off the queue
            Request_T req;
            mRequestQueue->dequeue(req);
            performTask(req);
        }
    }
    virtual bool isDone() const
    {
        return mDone;
    }
    virtual void setDone()
    {
        mDone = true;
    }
    /*!
     *  Virtual function to perform work on a request.
     *  This function must be overriden by the derived class.
     *
     *  \param request A request from the queue
     */
    virtual void performTask(Request_T& request) = 0;

    /*!
     *  Produces the system-named thread ID
     *  \todo Remove?
     *
     */
    std::string getThreadId()
    {
	return str::toString<long>(sys::getThreadID());
    }
protected:

    mt::RequestQueue<Request_T> *mRequestQueue;
    bool mDone;
};
}

#endif

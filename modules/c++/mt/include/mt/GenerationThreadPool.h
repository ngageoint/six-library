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


#ifndef __MT_GENERATION_THREAD_POOL_H__
#define __MT_GENERATION_THREAD_POOL_H__

#if !defined(__APPLE_CC__)

#include <import/sys.h>
#include "mt/BasicThreadPool.h"
#include "mt/CPUAffinityInitializer.h"
#include "mt/CPUAffinityThreadInitializer.h"

namespace mt
{
    class TiedRequestHandler : public sys::Runnable
    {
	RunnableRequestQueue* mRequestQueue;
	sys::Semaphore* mSem;
	CPUAffinityThreadInitializer* mAffinityInit;

    public:
	TiedRequestHandler(RunnableRequestQueue* requestQueue) :
	    mRequestQueue(requestQueue), mAffinityInit(NULL) {}
		
	virtual ~TiedRequestHandler();

	virtual void setSemaphore(sys::Semaphore* sem)
	{
	    mSem = sem;
	}
	virtual void setAffinityInit(CPUAffinityThreadInitializer* affinityInit)
	{
	    mAffinityInit = affinityInit;
	}

	// If we have a thread initializer, tie down our handler to a CPU
	virtual void initialize();

	virtual void run();
    };

    class GenerationThreadPool : public BasicThreadPool<TiedRequestHandler>
    {
	sys::Semaphore mGenerationSync;
	CPUAffinityInitializer* mAffinityInit;
	int mGenSize;
    public:
	GenerationThreadPool(unsigned short numThreads = 0,
			     CPUAffinityInitializer* affinityInit = NULL) 
	    : BasicThreadPool<TiedRequestHandler>(numThreads), 
	    mAffinityInit(affinityInit), mGenSize(0)
	    {
	    }
	virtual ~GenerationThreadPool() {}
	
	virtual TiedRequestHandler *newRequestHandler()
	{
	    TiedRequestHandler* handler = BasicThreadPool<TiedRequestHandler>::newRequestHandler();
	    handler->setSemaphore(&mGenerationSync);
		
	    if (mAffinityInit)
		handler->setAffinityInit(mAffinityInit->newThreadInitializer());
	    
	    
	    return handler;
	}
    
	// Not set up for multiple producers 
	virtual void addGroup(std::vector<sys::Runnable*>& toRun);
	
	// Not set up for multiple producers 
	virtual void waitGroup();
	
	virtual void addAndWaitGroup(std::vector<sys::Runnable*>& toRun)
	{
	    addGroup(toRun);
	    waitGroup();
	}
	virtual void shutdown();

    };
}
#endif
#endif

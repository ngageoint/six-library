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


#ifndef __MT_GENERATION_THREAD_POOL_H__
#define __MT_GENERATION_THREAD_POOL_H__

#include <assert.h>

#include "config/Exports.h"

#if !defined(__APPLE_CC__)

#include <import/sys.h>
#include "mt/BasicThreadPool.h"
#include "mt/CPUAffinityInitializer.h"
#include "mt/CPUAffinityThreadInitializer.h"
#include "mt/Runnable1D.h"


namespace mt
{
    class CODA_OSS_API TiedRequestHandler : public sys::Runnable
    {
	RunnableRequestQueue* mRequestQueue;
	sys::Semaphore* mSem = nullptr;
	CPUAffinityThreadInitializer* mAffinityInit;

    public:
	TiedRequestHandler(RunnableRequestQueue* requestQueue) :
	    mRequestQueue(requestQueue), mAffinityInit(nullptr) {}
		
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

    class CODA_OSS_API GenerationThreadPool : public BasicThreadPool<TiedRequestHandler>
    {
	sys::Semaphore mGenerationSync;
	CPUAffinityInitializer* mAffinityInit = nullptr;
	int mGenSize = 0;
    public:
        GenerationThreadPool() = default;
	GenerationThreadPool(unsigned short numThreads,
			     CPUAffinityInitializer* affinityInit = nullptr) 
	    : BasicThreadPool<TiedRequestHandler>(numThreads), 
	    mAffinityInit(affinityInit)
	    {
	    }
	virtual ~GenerationThreadPool() = default;
	GenerationThreadPool(const GenerationThreadPool&) = delete;
    GenerationThreadPool& operator=(const GenerationThreadPool&) = delete;
	
	virtual TiedRequestHandler *newRequestHandler()
	{
	    TiedRequestHandler* handler = BasicThreadPool<TiedRequestHandler>::newRequestHandler();
        assert(handler != nullptr);
	    handler->setSemaphore(&mGenerationSync);
		
	    if (mAffinityInit)
        {
            handler->setAffinityInit(mAffinityInit->newThreadInitializer().release());
        }

	    return handler;
	}
    
	// Not set up for multiple producers 
	void addGroup(const std::vector<sys::Runnable*>& toRun);
	
	// Not set up for multiple producers 
	void waitGroup();
	
	void addAndWaitGroup(const std::vector<sys::Runnable*>& toRun)
	{
	    addGroup(toRun);
	    waitGroup();
	}

    
    /*!
     *  \brief Runs a given operation on a sequence of numbers in parallel
     *
     *  \param numElements The number of elements to run - op will be called 
     *                     with 0 through numElements-1
     *  \param op          A function-like object taking a parameter of type
     *                     size_t which will be called for each number in the
     *                     given range
     */
    template <typename OpT>
    void run1D(size_t numElements, const OpT& op)
    {
        std::vector<sys::Runnable*> runnables;
        const ThreadPlanner planner(numElements, mNumThreads);
 
        size_t threadNum(0);
        size_t startElement(0);
        size_t numElementsThisThread(0);
        while(planner.getThreadInfo(threadNum++, startElement, numElementsThisThread))
        {
            runnables.push_back(new Runnable1D<OpT>(
                startElement, numElementsThisThread, op));
        }
        addAndWaitGroup(runnables);
        
    }


    };
}
#endif
#endif

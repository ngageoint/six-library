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


#ifndef __MT_ABSTRACT_TIED_THREAD_POOL_H__
#define __MT_ABSTRACT_TIED_THREAD_POOL_H__

#include "mt/AbstractThreadPool.h"
#include "mt/TiedWorkerThread.h"
#include "mt/CPUAffinityInitializer.h"

namespace mt
{
template <typename Request_T>
class AbstractTiedThreadPool : public AbstractThreadPool<Request_T>
{

public:
	AbstractTiedThreadPool(unsigned short numThreads = 0) : AbstractThreadPool<Request_T>(numThreads) {}

	virtual ~AbstractTiedThreadPool(){}

	virtual void initialize(CPUAffinityInitializer* affinityInit = NULL)
	{
	    mAffinityInit = affinityInit;
	}
	virtual CPUAffinityThreadInitializer* getCPUAffinityThreadInitializer()
	{
	    CPUAffinityThreadInitializer* threadInit = NULL;
	    
	    // If we were passed a schematic 
	    // for initializing thread affinity...
	    if (mAffinityInit)
	    {
		threadInit = mAffinityInit->newThreadInitializer();
	    }
	    return threadInit;
	}

	virtual mt::WorkerThread<Request_T>* newWorker()
	{
	    return newTiedWorker(&this->mRequestQueue, 
				 getCPUAffinityThreadInitializer());
	}

 protected:
	virtual mt::TiedWorkerThread<Request_T>* newTiedWorker(mt::RequestQueue<Request_T>* q, 
							       CPUAffinityThreadInitializer* init) = 0;

private:
	CPUAffinityInitializer* mAffinityInit;
};

}
#endif


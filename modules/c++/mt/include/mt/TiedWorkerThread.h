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


#ifndef __MT_TIED_WORKER_THREAD_H__
#define __MT_TIED_WORKER_THREAD_H__

#include "mt/CPUAffinityThreadInitializer.h"


namespace mt
{

/**
 * @created 03-Jan-2007 12:51:46
 */
template <typename Request_T>
class TiedWorkerThread : public mt::WorkerThread<Request_T>
{
public:
	TiedWorkerThread(mt::RequestQueue<Request_T>* requestQueue, 
			 CPUAffinityThreadInitializer* cpuAffinityInit = NULL):
	    mt::WorkerThread<Request_T>(requestQueue), 
	    mCPUAffinityInit(cpuAffinityInit){}

	virtual ~TiedWorkerThread()
	{
	    if (mCPUAffinityInit)
	    {
		delete mCPUAffinityInit;
		mCPUAffinityInit = NULL;
	    }
	}
	
	virtual void initialize()
	{
	    if (mCPUAffinityInit)
	    {
		mCPUAffinityInit->initialize();
	    }
	}

	virtual void performTask(Request_T& request) = 0;

private:
	TiedWorkerThread();
	CPUAffinityThreadInitializer* mCPUAffinityInit;
};

}
#endif


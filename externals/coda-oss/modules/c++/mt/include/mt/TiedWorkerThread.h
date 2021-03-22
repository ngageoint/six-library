/* =========================================================================
 * This file is part of mt-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2019, MDA Information Systems LLC
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
#include "mem/SharedPtr.h"


namespace mt
{

/**
 * @created 03-Jan-2007 12:51:46
 */
template <typename Request_T>
class TiedWorkerThread : public mt::WorkerThread<Request_T>
{
public:
    TiedWorkerThread(
            mt::RequestQueue<Request_T>* requestQueue,
            std::unique_ptr<CPUAffinityThreadInitializer>&& cpuAffinityInit =
                    std::unique_ptr<CPUAffinityThreadInitializer>(nullptr)) :
        mt::WorkerThread<Request_T>(requestQueue),
        mCPUAffinityInit(std::move(cpuAffinityInit))
    {
    }
#if !CODA_OSS_cpp17
    TiedWorkerThread(
            mt::RequestQueue<Request_T>* requestQueue,
            std::auto_ptr<CPUAffinityThreadInitializer> cpuAffinityInit =
                    std::auto_ptr<CPUAffinityThreadInitializer>(nullptr)) :
        TiedWorkerThread(requestQueue, std::unique_ptr<CPUAffinityThreadInitializer>(cpuAffinityInit.release()))
    {
    }
#endif

    virtual void initialize()
    {
        if (mCPUAffinityInit.get())
        {
            mCPUAffinityInit->initialize();
        }
    }

    virtual void performTask(Request_T& request) = 0;

private:
    TiedWorkerThread();
    std::unique_ptr<CPUAffinityThreadInitializer> mCPUAffinityInit;
};

}
#endif


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


#ifndef __MT_ABSTRACT_TIED_THREAD_POOL_H__
#define __MT_ABSTRACT_TIED_THREAD_POOL_H__

#include "mt/AbstractThreadPool.h"
#include "mt/TiedWorkerThread.h"
#include "mt/CPUAffinityInitializer.h"
#include "mem/SharedPtr.h"

namespace mt
{
template <typename Request_T>
class AbstractTiedThreadPool : public AbstractThreadPool<Request_T>
{

public:
    AbstractTiedThreadPool(unsigned short numThreads = 0) :
            AbstractThreadPool<Request_T>(numThreads)
    {
    }

    virtual ~AbstractTiedThreadPool(){}

    virtual void initialize(CPUAffinityInitializer* affinityInit = nullptr)
    {
        mAffinityInit = affinityInit;
    }

    virtual mem::auto_ptr<CPUAffinityThreadInitializer>
    getCPUAffinityThreadInitializer()
    {
        mem::auto_ptr<CPUAffinityThreadInitializer> threadInit(nullptr);

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
    virtual mt::TiedWorkerThread<Request_T>*
    newTiedWorker(mt::RequestQueue<Request_T>* q,
#if CODA_OSS_cpp17
                  std::unique_ptr<CPUAffinityThreadInitializer>&& init) = 0;
#else
                  std::auto_ptr<CPUAffinityThreadInitializer> init) = 0;
    virtual mt::TiedWorkerThread<Request_T>*
    newTiedWorker(mt::RequestQueue<Request_T>* q,
                  std::unique_ptr<CPUAffinityThreadInitializer>&& init) {
        std::auto_ptr<CPUAffinityThreadInitializer> init_(init.release());
        return newTiedWorker(q, init_);
    }
#endif


private:
    CPUAffinityInitializer* mAffinityInit;
};

}
#endif


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

#if !defined(__APPLE_CC__) && (defined(__linux) || defined(__linux__))
#include <iostream>

#include <import/sys.h>
#include <import/mt.h>
#include <import/mem.h>
#include <cli/ArgumentParser.h>
#include <sys/ScopedCPUAffinityUnix.h>
using namespace sys;
using namespace mt;

namespace
{
class MyRunTask : public Runnable
{
public:

    MyRunTask(size_t threadNum, sys::AtomicCounter& counter) :
        mThread(threadNum),
        mThreadCounter(counter)
    {
    }

    virtual void run() override
    {
        // Print diagnostics from inside the thread
        while(true)
        {
            if (mThread == mThreadCounter.get())
            {
                mt::CriticalSection<sys::Mutex> obtainLock(&mMutex);
                std::cout << "Thread " << mThread
                          << " " << sys::ScopedCPUAffinityUnix().toString()
                          << "\n";
                mThreadCounter.increment();
                break;
            }
        }

        // Do work, with periodic breaks to give the OS
        // the opportunity to change which CPU the thread is
        // assigned to
        for (size_t trials = 0; trials < 10; ++trials)
        {
            volatile size_t count = 10000000000;
            volatile size_t sum = 0;
            for (size_t ii = 0; ii < count; ++ii)
            {
                sum++;
            }
            sleep(5);
        }
    }

private:
    sys::AtomicCounter::ValueType mThread;
    sys::Mutex mMutex;
    sys::AtomicCounter& mThreadCounter;
};
}

int main(int argc, char** argv)
{
    // This program simulates a workload with periodic breaks
    // in an attempt to illustrate pinning threads spawned
    // by a ThreadGroup to unique CPUs.
    //
    // It is recommended that one runs the following (or equivalent)
    // to view per-process utilization:
    //      mpstat -P ALL 2 1000
    //
    // Running without the --pin option will not perform any pinning.
    // Using mpstat, you should be able to see the assignments
    // drift over time (if numThreads < numCPUsAvailable). If the
    // machine is fairly quiet, drifting may take some time to manifest.
    //
    // With --pin enabled, there should be no drift -- threads will be
    // stuck on the CPUs that are masked in the per-thread diagnostic
    // output.
    try
    {
        const size_t numCPUsAvailable = sys::OS().getNumCPUsAvailable();

        //-----------------------------------------------------
        //  Handle CLI parameters
        //-----------------------------------------------------
        cli::ArgumentParser parser;
        parser.addArgument("--threads",
                           "Number of threads to use",
                           cli::STORE,
                           "threads",
                           "INT")->setDefault(numCPUsAvailable);

        parser.addArgument("--pin",
                           "Enable CPU pinning",
                           cli::STORE_TRUE,
                           "pinToCPU")->setDefault(false);
        const std::unique_ptr<cli::Results> options(parser.parse(argc, argv));

        const bool pinToCPU = options->get<bool>("pinToCPU");
        const size_t numThreads = options->get<size_t>("threads");

        //-----------------------------------------------------
        //  Print diagnostics
        //-----------------------------------------------------
        std::cout << "Num CPUs available: " << numCPUsAvailable << std::endl;
        std::cout << "Num threads requested: " << numThreads << std::endl;
        std::cout << "Use CPU pinning: " << pinToCPU << std::endl;
        std::cout << "Available CPU mask: "
                  << sys::ScopedCPUAffinityUnix().toString() << std::endl;

        if (numThreads > numCPUsAvailable && pinToCPU)
        {
            throw except::Exception(
                Ctxt("Requested more threads than CPUs with pinning enabled"));
        }

        //-----------------------------------------------------
        //  Run the thread group operations,
        //  pinning if requested
        //-----------------------------------------------------
        ThreadGroup threads(pinToCPU);
        const ThreadPlanner planner(numThreads, numThreads);
        size_t threadNum = 0;
        size_t startElement = 0;
        size_t numElementsThisThread = 0;
        sys::AtomicCounter threadCounter;

        while(planner.getThreadInfo(threadNum, startElement, numElementsThisThread))
        {
            threads.createThread(new MyRunTask(threadNum, threadCounter));
            ++threadNum;
        }

        threads.joinAll();
    }

    catch (const except::Throwable& t)
    {
        std::cout << "Exception Caught: " << t.toString() << std::endl;
        return -1;
    }
    catch (...)
    {
        std::cout << "Exception Caught!" << std::endl;
        return -1;
    }

    return 0;
}

#else

#include <iostream>
int main (int, char**)
{
    std::cout << "Usable only on *nix systems" << std::endl;
    return 0;
}

#endif


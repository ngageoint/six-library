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

#if defined(__APPLE_CC__)
#include <iostream>
int main (int argc, char *argv[])
{
    std::cout << "Sorry no semaphores" << std::endl;
    return 0;
}

#else
#include "mt/GenerationThreadPool.h"
#include "mt/CriticalSection.h"

using namespace mt;
using namespace sys;
const int TO_SLEEP = 2;
const int NUM_GENS = 3;
sys::Mutex gLock;

class MyRunTask : public sys::Runnable
{
    int mI;
public:
    MyRunTask(int i) :
        mI(i)
    {
    }
    ~MyRunTask()
    {
    }

    void run()
    {
        sleep(TO_SLEEP);

        // Goes out of scope when we finish printing and return
        mt::CriticalSection < sys::Mutex > cs(&gLock);
        std::cout << "Run " << mI << " completed" << std::endl;

    }
};

void runGeneration(GenerationThreadPool& pool)
{
    static int nRunsInGen = 1;

    std::vector<Runnable*> runs;
    for (int i = 0; i < nRunsInGen; i++)
    {
        runs.push_back(new MyRunTask(i));
    }

    pool.addAndWaitGroup(runs);
    std::cout << "Generation done" << std::endl;
    nRunsInGen++;

}
int main()
{
    try
    {
        // Create a thread pool of size 4
        GenerationThreadPool pool(4);
        // Start it
        pool.start();

        for (int i = 0; i < NUM_GENS; i++)
            runGeneration(pool);

        pool.shutdown();
        pool.join();
    }
    catch (except::Exception& ex)
    {
        std::cout << "Caught exception: " << ex.toString() << std::endl;
        std::cout << "\t" << ex.getTrace() << std::endl;
    }
}
#endif

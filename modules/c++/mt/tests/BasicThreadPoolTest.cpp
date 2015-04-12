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
#include <iostream>
#include <import/sys.h>
#include <import/mt.h>
using namespace sys;
using namespace mt;
using namespace std;

const int NUM_TASKS = 5;
const int TO_SLEEP = 2;

class MyRunTask : public Runnable
{
    Semaphore& mSem;
public:
    MyRunTask(Semaphore& sem) :
        mSem(sem)
    {
    }
    virtual ~MyRunTask()
    {
    }

    virtual void run()
    {
        sleep(TO_SLEEP);
        mSem.signal();
    }
};

int main(int argc, char *argv[])
{
    try
    {
        // Semaphore inited at 0
        Semaphore sem;

        // Create a thread pool of size 2
        BasicThreadPool < GenericRequestHandler > pool(2);

        for (int i = 0; i < NUM_TASKS; i++)
        {
            pool.addRequest(new MyRunTask(sem));
        }
        pool.start();

        for (int i = 0; i < NUM_TASKS; i++)
        {
            sem.wait();
        }

        std::cout << "Finished all" << std::endl;

    }

    catch (except::Throwable& t)
    {
        cout << "Exception Caught: " << t.toString() << endl;
    }
    catch (...)
    {
        cout << "Exception Caught!" << endl;
        return -1;
    }

    return 0;
}
#endif

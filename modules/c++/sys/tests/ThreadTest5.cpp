/* =========================================================================
 * This file is part of sys-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
 *
 * sys-c++ is free software; you can redistribute it and/or modify
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

#include <import/sys.h>
using namespace sys;
using namespace std;

#if defined(_REENTRANT)

class TestThread : public Thread
{
public:
    TestThread(int *val)
    {

        mVal = val;
        std::cout << "Constructing thread with value " << *mVal << std::endl;
    }
    ~TestThread()
    {
        std::cout << "Destructing thread with value " << *mVal << std::endl;
        delete mVal;
    }

    void run()
    {
        cout << "mVal: " << *mVal << endl;
    }
    int *mVal;
};

int main()
{
    std::vector< TestThread *> tAry;
    tAry.resize(5);

    try
    {
        for (int i = 0; i < 5; i++)
        {
            tAry[i] = new TestThread(new int(i + 1));
            tAry[i]->start();
        }
        //         TestThread(new int(1)).start();
        //         TestThread(new int(2)).start();
        //         TestThread(new int(3)).start();
        //         TestThread(new int(4)).start();
        //         TestThread(new int(5)).start();

        for (int i = 0; i < 5; i++)
        {
            tAry[i]->join();
        }
        while (tAry.size())
        {
            TestThread *t = tAry.back();
            tAry.pop_back();
            delete t;
        }

    }
    catch (except::Exception& e)
    {
        cout << e.toString() << endl;
    }
    catch (...)
    {
        cout << "Unknown exception" << endl;
    }
    return 0;
};
#else
int main()
{
    std::cout << "sys is not Multithreaded" << std::endl;
    return 0;
}

#endif

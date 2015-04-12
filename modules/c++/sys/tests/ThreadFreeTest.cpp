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

#include <iostream>
#include <import/sys.h>
using namespace sys;
using namespace std;

class MyRunTask : public Runnable
{
public:
    int result;
    
    MyRunTask()
    {
        result = 0;
    }
    virtual ~MyRunTask()
    {
    }

    virtual void run()
    {
		result = 1;
    }
};

int main(int argc, char *argv[])
{
    Thread *thread;
    MyRunTask *task1;
    MyRunTask *task2;
    MyRunTask *task3;

    try
    {
        task1 = new MyRunTask();
        thread = new Thread(task1);
        thread->start();
        thread->join();
        task2 = new MyRunTask();
        
        if (task1->result != 1)
        {
            cout << "Task1 not run, result: " << task1->result << endl;
            return -1;
        }
        
        delete thread;

        task3 = new MyRunTask();
        
        if (task1 == task3)
            cout << "Task1 freed" << endl;
            
        
        delete task2;
        delete task3;
        
        std::cout << "Finished all" << std::endl;
    }

    catch (except::Throwable& t)
    {
        cout << "Exception Caught: " << t.toString() << endl;
        return -1;
    }
    catch (...)
    {
        cout << "Exception Caught!" << endl;
        return -1;
    }

    return 0;
}

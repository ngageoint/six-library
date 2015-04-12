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

#include "import/sys.h"
#include "import/mt.h"
#include "TestCase.h"

using namespace sys;
using namespace mt;
using namespace std;

class MyRunTask : public Runnable
{
public:
    int result;
    int *state;
    int *num_deleted;
    
    MyRunTask(int *new_state, int *new_num_deleted)
    {
        state = new_state;
        result = *new_state;
        num_deleted = new_num_deleted;
    }
    virtual ~MyRunTask()
    {
        (*num_deleted)++;
    }

    virtual void run()
    {
		while (result == 1)
            result = *state;
    }
};

TEST_CASE(ThreadGroupTest)
{
    ThreadGroup *threads = new ThreadGroup();
    int state = 1, numDeleted = 0;
    MyRunTask *tasks[3];
    
    for (int i = 0; i < 3; i++)
        tasks[i] = new MyRunTask(&state, &numDeleted);
    
    threads->createThread(tasks[0]);
    threads->createThread(tasks[1]);
    state = 2;
    threads->joinAll();
    
    TEST_ASSERT_EQ(tasks[0]->result, 2);
    TEST_ASSERT_EQ(tasks[1]->result, 2);
    
    state = 1;
    threads->createThread(tasks[2]);
    state = 3;

    TEST_ASSERT_EQ(numDeleted, 0);
    
    delete threads;
    TEST_ASSERT_EQ(numDeleted, 3);
}

int main(int argc, char *argv[])
{
    TEST_CHECK(ThreadGroupTest);

    return 0;
}

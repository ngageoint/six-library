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

#include "mt/ThreadGroup.h"

mt::ThreadGroup::ThreadGroup() :
    mLastJoined(0)
{
}

mt::ThreadGroup::~ThreadGroup()
{
    try
    {
        joinAll();
    }
    catch (...)
    {
        // Make sure we don't throw out of the destructor.
    }
}

void mt::ThreadGroup::createThread(sys::Runnable *runnable)
{
    createThread(std::auto_ptr<sys::Runnable>(runnable));
}

void mt::ThreadGroup::createThread(std::auto_ptr<sys::Runnable> runnable)
{
    mem::SharedPtr<sys::Thread> thread(new sys::Thread(runnable.get()));
    runnable.release();
    mThreads.push_back(thread);
    thread->start();
}

void mt::ThreadGroup::joinAll()
{
    bool failed = false;
    // Keep track of which threads we've already joined.
    for (; mLastJoined < mThreads.size(); mLastJoined++)
    {
        try
        {
            mThreads[mLastJoined]->join();
        }
        catch (...)
        {
            failed = true;
        }
    }
    
    if (failed)
        throw except::Error(Ctxt("ThreadGroup could not be joined"));
}

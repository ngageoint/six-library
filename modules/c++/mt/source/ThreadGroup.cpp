/* =========================================================================
 * This file is part of mt-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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

#include <mt/ThreadGroup.h>
#include <mt/CriticalSection.h>

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
    std::auto_ptr<sys::Runnable> internalRunnable(new ThreadGroupRunnable(runnable, *this));
    mem::SharedPtr<sys::Thread> thread(new sys::Thread(internalRunnable.get()));
    internalRunnable.release();
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
    
    if (!mExceptions.empty())
    {
        std::string messageString("Exceptions thrown from ThreadGroup in the following order:\n");
        for (size_t ii=0; ii<mExceptions.size(); ++ii)
        {
            messageString += mExceptions.at(ii).toString();
        }
        throw except::Exception(Ctxt(messageString));
    }

    if (failed)
        throw except::Error(Ctxt("ThreadGroup could not be joined"));
}

void mt::ThreadGroup::addException(const except::Exception& ex)
{
    try
    {
        mt::CriticalSection<sys::Mutex> pushLock(&mMutex);
        mExceptions.push_back(ex);
    }
    catch(...)
    {
        fprintf(stderr, "Error adding exception from a thread to mExceptions.\n");
    }
}

mt::ThreadGroup::ThreadGroupRunnable::ThreadGroupRunnable
    (std::auto_ptr<sys::Runnable> runnable, mt::ThreadGroup& parentThreadGroup):
        mRunnable(runnable), mParentThreadGroup(parentThreadGroup)
{
}

void mt::ThreadGroup::ThreadGroupRunnable::run()
{
    try
    {
        mRunnable->run();
    }
    catch(const except::Exception& ex)
    {
        mParentThreadGroup.addException(ex);
    }
    catch(const std::exception& ex)
    {
        mParentThreadGroup.addException(except::Exception(Ctxt(ex.what())));
    }
    catch(...)
    {
        mParentThreadGroup.addException(
            except::Exception(Ctxt("Unknown ThreadGroup exception.")));
    }
}

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

namespace mt
{
#if defined(MT_DEFAULT_PINNING)
    bool ThreadGroup::DEFAULT_PIN_TO_CPU = true;
#else
    bool ThreadGroup::DEFAULT_PIN_TO_CPU = false;
#endif


ThreadGroup::ThreadGroup(bool pinToCPU) :
    mAffinityInit(pinToCPU ? new CPUAffinityInitializer() : NULL),
    mLastJoined(0)
{
}

ThreadGroup::~ThreadGroup()
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

void ThreadGroup::createThread(sys::Runnable *runnable)
{
    createThread(std::unique_ptr<sys::Runnable>(runnable));
}

void ThreadGroup::createThread(std::unique_ptr<sys::Runnable>&& runnable)
{
    // Note: If getNextInitializer throws, any previously created
    //       threads may never finish if cross-thread communication is used.
    std::unique_ptr<sys::Runnable> internalRunnable(
            new ThreadGroupRunnable(
                    std::move(runnable),
                    *this,
                    getNextInitializer()));

    mem::SharedPtr<sys::Thread> thread(new sys::Thread(internalRunnable.get()));
    internalRunnable.release();
    mThreads.push_back(thread);
    thread->start();
}
#if !CODA_OSS_cpp17
void ThreadGroup::createThread(std::auto_ptr<sys::Runnable> runnable)
{
    createThread(std::unique_ptr<sys::Runnable>(runnable.release()));
}
#endif

void ThreadGroup::joinAll()
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

void ThreadGroup::addException(const except::Exception& ex)
{
    try
    {
        CriticalSection<sys::Mutex> pushLock(&mMutex);
        mExceptions.push_back(ex);
    }
    catch(...)
    {
        fprintf(stderr, "Error adding exception from a thread to mExceptions.\n");
    }
}

mem::auto_ptr<CPUAffinityThreadInitializer> ThreadGroup::getNextInitializer()
{
    mem::auto_ptr<CPUAffinityThreadInitializer> threadInit;
    if (mAffinityInit.get())
    {
        threadInit = mAffinityInit->newThreadInitializer();
    }

    return threadInit;
}

ThreadGroup::ThreadGroupRunnable::ThreadGroupRunnable(
        std::unique_ptr<sys::Runnable>&& runnable,
        ThreadGroup& parentThreadGroup,
        std::unique_ptr<CPUAffinityThreadInitializer>&& threadInit) :
        mRunnable(std::move(runnable)),
        mParentThreadGroup(parentThreadGroup),
        mCPUInit(std::move(threadInit))
{
}
#if !CODA_OSS_cpp17
ThreadGroup::ThreadGroupRunnable::ThreadGroupRunnable(
        std::auto_ptr<sys::Runnable> runnable,
        ThreadGroup& parentThreadGroup,
        std::auto_ptr<CPUAffinityThreadInitializer> threadInit) :
        ThreadGroupRunnable(std::unique_ptr<sys::Runnable>(runnable.release()),
        parentThreadGroup,
         std::unique_ptr<CPUAffinityThreadInitializer>(threadInit.release()))
{
}
#endif

void ThreadGroup::ThreadGroupRunnable::run()
{
    try
    {
        if (mCPUInit.get())
        {
            mCPUInit->initialize();
        }
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

bool ThreadGroup::isPinToCPUEnabled() const
{
    return mAffinityInit.get() != NULL;
}

bool ThreadGroup::getDefaultPinToCPU()
{
    return DEFAULT_PIN_TO_CPU;
}

void ThreadGroup::setDefaultPinToCPU(bool newDefault)
{
    DEFAULT_PIN_TO_CPU = newDefault;
}
}

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

#ifndef __MT_THREAD_GROUP_H__
#define __MT_THREAD_GROUP_H__

#include <vector>
#include <memory>
#include <exception>
#include "sys/Runnable.h"
#include "sys/Thread.h"
#include "mem/SharedPtr.h"
#include "except/Error.h"
#include <sys/Mutex.h>



namespace mt
{

/*!
 * \class ThreadGroup
 *
 * \brief Basic thread group.
 *
 * This class is a basic thread group that can create threads from
 * sys::Runnable objects and wait for all threads to complete.
 *
 */
class ThreadGroup
{
public:

    //! Constructor.
    ThreadGroup();
    
    /*!
    *  Destructor. Attempts to join all threads.
    */
    ~ThreadGroup();
    
    /*!
    *  Creates and starts a thread from a sys::Runnable.
    *  \param runnable pointer to sys::Runnable
    */
    void createThread(sys::Runnable *runnable);
    
    /*!
    *  Creates and starts a thread from a sys::Runnable.
    *  \param runnable auto_ptr to sys::Runnable
    */
    void createThread(std::auto_ptr<sys::Runnable> runnable);
    
    /*!
     * Waits for all threads to complete.
     */
    void joinAll();

private:
    std::vector<mem::SharedPtr<sys::Thread> > mThreads;
    size_t mLastJoined;
    std::vector<except::Exception> mExceptions;
    sys::Mutex mMutex;

    /*!
     * Adds an exception to the mExceptions vector
     */
    void addException(const except::Exception& ex);

    /*!
     * \class ThreadGroupRunnable
     *
     * \brief Internal runnable class to safeguard against running
     * threads who throw exceptions
     */
    class ThreadGroupRunnable : public sys::Runnable
    {
    public:

        //! Constructor.
        ThreadGroupRunnable(std::auto_ptr<sys::Runnable> runnable,
                            mt::ThreadGroup& parentThreadGroup);

        /*!
         *  Call run() on the Runnable passed to createThread
         */
        virtual void run();

    private:
        std::auto_ptr<sys::Runnable> mRunnable;
        mt::ThreadGroup& mParentThreadGroup;

    };
};

}

#endif

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

#include <sys/Conf.h>
#include <except/Error.h>
#include <sys/Runnable.h>
#include <sys/Thread.h>
#include <sys/Mutex.h>
#include <mem/SharedPtr.h>

#include "mt/mt_config.h"
#include <mt/CPUAffinityInitializer.h>
#include <mt/CPUAffinityThreadInitializer.h>

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

    /*!
     * Constructor.
     * \param pinToCPU Optional flag specifying whether CPU pinning
     *                 should occur, using a CPUAffinityInitializer.
     *                 If MT_DEFAULT_PINNING is defined, defaults to true
     *                 (enable affinity-based thread pinning).
     *                 Otherwise, defaults to false (no pinning).
     */
    ThreadGroup(bool pinToCPU = getDefaultPinToCPU());

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
    void createThread(std::unique_ptr<sys::Runnable>&& runnable);
    #if !CODA_OSS_cpp17
    void createThread(std::auto_ptr<sys::Runnable> runnable);
    #endif

    /*!
     * Waits for all threads to complete.
     */
    void joinAll();

    /*!
     * Checks whether CPU pinning is set (i.e. whether mAffinityInit is NULL)
     *
     * \return true if CPU pinning is set, false otherwise
     */
    bool isPinToCPUEnabled() const;

    /*!
     * Gets the current default value for CPU pinning
     *
     * \return true if CPU pinning is enabled by default, false otherwise
     */
    static bool getDefaultPinToCPU();

    /*!
     * Sets the default value for CPU pinning
     *
     * \param newDefault the new default value for CPU pinning
     */
    static void setDefaultPinToCPU(bool newDefault);

private:
    std::unique_ptr<CPUAffinityInitializer> mAffinityInit;
    size_t mLastJoined;
    std::vector<mem::SharedPtr<sys::Thread> > mThreads;
    std::vector<except::Exception> mExceptions;
    sys::Mutex mMutex;

    /*!
     * The default setting for pinToCPU, used in the ThreadGroup constructor
     * Can't set non-const static values in the header, so set at the top of
     * the implementation
     */
    static bool DEFAULT_PIN_TO_CPU;

    /*!
     * Adds an exception to the mExceptions vector
     */
    void addException(const except::Exception& ex);

    /*!
     * \returns the next available thread initializer provided by
     *          the internal CPUAffinityInitializer. If no initializer
     *          was created, will return NULL.
     */
    mem::auto_ptr<CPUAffinityThreadInitializer> getNextInitializer();

    /*!
     * \class ThreadGroupRunnable
     *
     * \brief Internal runnable class to safeguard against running
     * threads who throw exceptions
     */
    class ThreadGroupRunnable : public sys::Runnable
    {
    public:

        /*!
         * Constructor.
         * \param runnable sys::Runnable object that will be executed by
         *                 the current thread
         * \param parentThreadGroup ThreadGroup object that spawned
         *                          this ThreadGroupRunnable
         * \param threadInit Affinity-based initializer for the thread
         *                   that controls which CPUs the runnable is allowed
         *                   to execute on. If NULL, no affinity preferences
         *                   will be enforced.
         */
        ThreadGroupRunnable(
                std::unique_ptr<sys::Runnable>&& runnable,
                mt::ThreadGroup& parentThreadGroup,
                std::unique_ptr<CPUAffinityThreadInitializer>&& threadInit =
                        std::unique_ptr<CPUAffinityThreadInitializer>(nullptr));
        #if !CODA_OSS_cpp17
        ThreadGroupRunnable(
                std::auto_ptr<sys::Runnable> runnable,
                mt::ThreadGroup& parentThreadGroup,
                std::auto_ptr<CPUAffinityThreadInitializer> threadInit =
                        std::auto_ptr<CPUAffinityThreadInitializer>(nullptr));
        #endif

        /*!
         *  Call run() on the Runnable passed to createThread
         */
        virtual void run();

    private:
        std::unique_ptr<sys::Runnable> mRunnable;
        mt::ThreadGroup& mParentThreadGroup;
        std::unique_ptr<CPUAffinityThreadInitializer> mCPUInit;
    };
};

}

#endif

/* =========================================================================
 * This file is part of sys-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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


#ifndef __SYS_SYNC_FACTORY_IRIX_H__
#define __SYS_SYNC_FACTORY_IRIX_H__

#include <mem/SharedPtr.h>

#  if defined(__sgi)
#include <iostream>
#include "sys/Dbg.h"
#include <memory>
#include <unistd.h>
#include <signal.h>
#include <ulocks.h>
#include <sys/types.h>
#include <sys/timers.h>
#include <sys/procset.h>
#include <sys/prctl.h>

namespace sys
{
class SemaphoreIrix;
class MutexIrix;
class ThreadIrix;
class ConditionVarIrix;


class SyncInterface
{
public:
    SyncInterface()
    {}

    virtual ~SyncInterface()
    {}

    virtual bool waitSemaphore(SemaphoreIrix& sema) = 0;

    virtual bool signalSemaphore(SemaphoreIrix& sema) = 0;

    virtual bool setLock(MutexIrix& mutex) = 0;
    virtual bool unsetLock(MutexIrix& mutex) = 0;
    virtual bool startThread(ThreadIrix& t) = 0;
    virtual bool killThread(ThreadIrix& t) = 0;
    virtual bool createLock(MutexIrix& mutex) = 0;
    virtual bool destroyLock(MutexIrix& mutex) = 0;
    virtual bool createSemaphore(SemaphoreIrix& sema,
                                 unsigned int count) = 0;
    virtual bool destroySemaphore(SemaphoreIrix& sema) = 0;

};

class SyncFactoryIrix : public SyncInterface
{
protected:
class SyncImplIrix : public SyncInterface
    {
    public:
        SyncImplIrix();

        virtual ~SyncImplIrix();

        bool createLock(MutexIrix& mutex);

        bool destroyLock(MutexIrix& mutex);

        bool setLock(MutexIrix& mutex);

        bool unsetLock(MutexIrix& mutex);

        bool waitSemaphore(SemaphoreIrix& sema);

        bool signalSemaphore(SemaphoreIrix& sema);

        bool createSemaphore(SemaphoreIrix& sema, unsigned int count);

        bool destroySemaphore(SemaphoreIrix& sema);

        bool startThread(ThreadIrix& t);

        bool killThread(ThreadIrix& t);

        usptr_t* getArena()
        {
            return mArena;
        }
        int mRef;

    private:
        usptr_t* mArena;
        ulock_t* mGuard;
    };

    static mem::auto_ptr<ulock_t> cs;
    static SyncFactoryIrix::SyncImplIrix* createImpl()
    {

        if (!mImpl)
        {
            dbg_ln("Creating Impl");
            mImpl = new SyncFactoryIrix::SyncImplIrix();

        }
        return mImpl;
    }
    static void destroyImpl()
    {

        if (mImpl != nullptr)
        {
            dbg_ln("Destroying Impl");
            delete mImpl;
            mImpl = nullptr;
        }

    }

private:

    static SyncImplIrix* mImpl;
public:
    SyncFactoryIrix()
    {}

    ~SyncFactoryIrix()
    {
        // Deletes lock as well
        if (mImpl->mRef == 0)
        {
            dbg_ln("There are no more references to the implementation.  Deleting...");
            destroyImpl();
        }
    }

    bool startThread(ThreadIrix& t)
    {
        EVAL(&t);
        return createImpl()->startThread(t);
    }

    bool killThread(ThreadIrix& t)
    {
        return createImpl()->killThread(t);
    }
    // SyncFactoryIrix().createLock(mutex);
    // SyncFactoryIrix().destroyLock(mutex);
    bool createLock(MutexIrix& mutex)
    {
        return createImpl()->createLock(mutex);
    }

    bool destroyLock(MutexIrix& mutex)
    {

        return createImpl()->destroyLock(mutex);
    }

    bool waitSemaphore(SemaphoreIrix& sema)
    {
        return createImpl()->waitSemaphore(sema);
    }

    bool signalSemaphore(SemaphoreIrix& sema)
    {
        return createImpl()->signalSemaphore(sema);
    }

    bool setLock(MutexIrix& mutex)
    {
        return createImpl()->setLock(mutex);
    }
    bool unsetLock(MutexIrix& mutex)
    {
        return createImpl()->unsetLock(mutex);
    }


    bool createSemaphore(SemaphoreIrix& sema, unsigned int count)
    {
        // This functions 1) Locks (mULock)
        //                2) Acts
        //                3) Increments mRef
        //                4) Unlocks
        return createImpl()->createSemaphore(sema, count);
    }


    bool destroySemaphore(SemaphoreIrix& sema)
    {
        // This functions 1) Locks
        //                2) Acts
        //                3) Decrements mRef
        //                4) Unlocks
        return createImpl()->destroySemaphore(sema);
        //if (mImpl->mRef == 0) delete mImpl;

    }
};
}
#  endif
#endif

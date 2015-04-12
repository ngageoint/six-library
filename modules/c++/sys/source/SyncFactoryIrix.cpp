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


#  if defined(__sgi) && defined(_REENTRANT) && !defined(__POSIX)
#include <ulocks.h>

#include "sys/SyncFactoryIrix.h"
#include "sys/MutexIrix.h"
#include "sys/SemaphoreIrix.h"
#include "sys/ThreadIrix.h"
#include "sys/ConditionVarIrix.h"

sys::SyncFactoryIrix::SyncImplIrix* sys::SyncFactoryIrix::mImpl = NULL;
sys::SyncFactoryIrix::SyncImplIrix::SyncImplIrix() : mRef(0), mArena(NULL)
{
    // NOTE: SIGUSR1 is used in the Irix conditional
    // variable, and needs to be blocked by the process.
    // If it isn't blocked, then the threads may terminate
    // prematurely if they are not waiting on the signal
    // when the signal occurs.
    HERE();
    sigset_t lSignalSet;
    //sigset_t   lSignalInfo;

    sigemptyset(&lSignalSet);
    sigaddset(&lSignalSet, SIGUSR1);
    sigprocmask(SIG_BLOCK, &lSignalSet, NULL);


    // Set maximum number of sharing processes, default=8
    usconfig(CONF_INITUSERS, 128);

    // Enable autogrow options for arena
    usconfig(CONF_AUTOGROW, 1); // Default=enabled, but set anyways
    usconfig(CONF_AUTORESV, 1); // Only valid if using /dev/zero

    // Create shared memory arena using logical swap pool
    mArena = usinit( "/dev/zero" );

    assert( mArena );
    EVAL( mArena);

    mGuard = new ulock_t;
    *mGuard = usnewlock(mArena);
    if (*mGuard == NULL)
    {
        delete mGuard;
        mGuard = NULL;
    }


}

sys::SyncFactoryIrix::SyncImplIrix::~SyncImplIrix()
{
    dbg_ln("Destructing the SyncImpl");

    dbg_ln("Removing the guard from the arena");
    usfreelock(*mGuard, mArena);
    dbg_ln("Deleting the guard");
    delete mGuard;
    dbg_ln("Detaching the arena");
    usdetach(mArena);

    mArena = NULL;
    dbg_ln("Done destructing the SyncImpl");
}

bool sys::SyncFactoryIrix::SyncImplIrix::startThread(sys::ThreadIrix& t)
{
    dbg_ln("Starting thread");
    t.getNative() = sproc(sys::ThreadIrix::__start,
                          PR_SADDR | PR_SFDS | PR_SDIR | PR_SUMASK,
                          &t);
    EVAL(t.getNative());
    if (t.getNative() <= 0)
    {
        dbg_ln("sproc() call failed!");
        return false;

    }
    return true;
}
bool sys::SyncFactoryIrix::SyncImplIrix::killThread(sys::ThreadIrix& t)
{
    dbg_ln("Killing thread");
    if ( ::kill(t.getNative(), SIGKILL) == 0 )
    {
        t.setIsRunning(false);
        return true;
    }
    dbg_ln("Thread kill() failed");
    return false;
}
bool sys::SyncFactoryIrix::SyncImplIrix::createLock(sys::MutexIrix& mutex)
{
    dbg_ln("Creating the lock");
    ussetlock(*mGuard);
    mutex.getNative() = new ulock_t;
    *(mutex.getNative()) = usnewlock(mArena);
    if (*(mutex.getNative()) == NULL)
    {
        dbg_ln("Lock creation failed");
        delete mutex.getNative();
        mutex.getNative() = NULL;
        return false;
    }
    TRACE(mRef++);
    EVAL(mRef);
    usunsetlock(*mGuard);
    dbg_ln("Successfully created lock");
    return true;
}

bool sys::SyncFactoryIrix::SyncImplIrix::destroyLock(sys::MutexIrix& mutex)
{
    dbg_ln("Destroying the lock");
    ussetlock(*mGuard);
    EVAL(mArena);

    EVAL( &mutex );
    EVAL( mutex.getNative() );
    TRACE( usfreelock(*(mutex.getNative()), mArena) );
    TRACE( delete mutex.getNative() );
    mutex.getNative() = NULL;

    TRACE(mRef--);
    EVAL(mRef);
    usunsetlock(*mGuard);
    return true;

}

bool sys::SyncFactoryIrix::SyncImplIrix::setLock(sys::MutexIrix& mutex)
{
    dbg_ln("Setting the lock");
    return ( ussetlock( *(mutex.getNative() ) ) == 1 );
}

bool sys::SyncFactoryIrix::SyncImplIrix::unsetLock(sys::MutexIrix& mutex)
{
    dbg_ln("Unsetting the lock");

    return ( usunsetlock( *(mutex.getNative() ) ) == 0 );
}


bool sys::SyncFactoryIrix::SyncImplIrix::waitSemaphore(sys::SemaphoreIrix& sema)
{
    dbg_ln("Waiting for semaphore");
    return ( uspsema( sema.getNative() ) == 1 );
}

bool sys::SyncFactoryIrix::SyncImplIrix::signalSemaphore(sys::SemaphoreIrix& sema)
{
    dbg_ln("Signalling semaphore");
    return ( usvsema( sema.getNative() ) == 0 );
}

bool sys::SyncFactoryIrix::SyncImplIrix::createSemaphore(sys::SemaphoreIrix& sema,
        unsigned int count)
{
    dbg_ln("Creating semaphore");
    ussetlock(*mGuard);
    sema.getNative() = usnewsema(mArena, count);
    if (!sema.getNative())
    {
        dbg_ln("Semaphore creation failed!");
        return false;
    }
    TRACE(mRef++);
    EVAL(mRef);
    usunsetlock(*mGuard);
    dbg_ln("Successfully created semaphore");
    return true;
}

bool sys::SyncFactoryIrix::SyncImplIrix::destroySemaphore(sys::SemaphoreIrix& sema)
{
    dbg_ln("Destroying semaphore");
    ussetlock(*mGuard);
    usfreesema(sema.getNative(), mArena);

    TRACE(mRef--);
    EVAL(mRef);
    usunsetlock(*mGuard);
    return true;
}
#endif


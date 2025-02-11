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


#ifndef __SYS_NSPR_MUTEX_H__
#define __SYS_NSPR_MUTEX_H__

#if defined(USE_NSPR_THREADS)

#include "sys/MutexInterface.h"
#include <prlock.h>
#include <prcvar.h>
#include <prthread.h>


namespace sys
{
/*!
 *  \class MutexNSPR
 *  \brief This is a Netscape Portable Runtime mutex 
 *  implementation
 *
 *  Use NSPR thread package to make platform independent 
 *  threading
 */
class MutexNSPR : public MutexInterface<PRLock *>
{
public:
    //!  Constructor.  Create a new mutex
    MutexNSPR();

    //!  Destroy the native mutex

    ~MutexNSPR();
    /*!
     *  Lock a NSPR mutex
     */
    void lock();

    /*!
     *  Unlock NSPR mutex.
     */
    virtual void unlock();
    
    /*!
     *  Returns the native type.
     */
    PRLock*& getNative();
    
private:
    PRLock* mNative;
};

}
#  endif // NSPR
#endif // Header

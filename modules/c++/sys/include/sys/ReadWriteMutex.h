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


#ifndef __SYS_READ_WRITE_MUTEX_INTERFACE_H__
#define __SYS_READ_WRITE_MUTEX_INTERFACE_H__



#if defined(_REENTRANT) && !defined(__APPLE_CC__)

#include "sys/Dbg.h"
#include "sys/Mutex.h"
#include "sys/Semaphore.h"

namespace sys
{
    
    /*!
     *  \class ReadWriteMutex
     *  \brief Locks resources exclusively during writes while allowing
     *  simultaneous reads
     *
     */
    class ReadWriteMutex
    {
    public:
	//!  Constructor
	ReadWriteMutex(int maxReaders) : mSem(maxReaders)
        {
	    mMaxReaders = maxReaders;
	    dbg_printf("Creating a read/write mutex\n");
	}
	
	//!  Destructor
	virtual ~ReadWriteMutex() 
	{
	    dbg_printf("Destroying a read/write mutex\n");
	}

	/*!
	 *  Lock for reading (no writes allowed)
	 *  \return true upon success
	 */
	virtual bool lockRead();
	
	/*!
	 *  Unlock for reading (writes allowed)
	 *  \return true upon success
	 */
	virtual bool unlockRead();

        /*!
         *  Lock for writing (no reads/other writes allowed)
         */
        virtual void lockWrite();

        /*!
         *  Unlock for writing (reads allowed)
         */
	virtual void unlockWrite();

    protected:
	sys::Semaphore mSem;
	sys::Mutex mMutex;
        int mMaxReaders;
    };
    
}

#endif // Are we reentrant?

#endif


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


#if !defined(__APPLE_CC__)
#include "sys/ReadWriteMutex.h"

void sys::ReadWriteMutex::lockRead()
{
    // Count up one reader
	mSem.wait();
}
void sys::ReadWriteMutex::unlockRead()
{
    // Count down one reader
	mSem.signal();
}

void sys::ReadWriteMutex::lockWrite()
{
        // Need to lock so other writers cannot try
        // waiting
	mMutex.lock();
        // Count the semaphore all the way up so we 
        // know that any call to lockRead will have to
        // wait for a signal
	for(int i=0; i < mMaxReaders; ++i)
	{
		mSem.wait();
	}
	mMutex.unlock();
}

void sys::ReadWriteMutex::unlockWrite()
{
        // Signal so that readers can resume reading
	for(int i=0; i < mMaxReaders; ++i)
	{
		mSem.signal();
	}
}

#endif

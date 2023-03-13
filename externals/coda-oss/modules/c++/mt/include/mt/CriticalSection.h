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


#ifndef __MT_CRITICAL_SECTION_H__
#define __MT_CRITICAL_SECTION_H__

/*!
 *  \file
 *  \brief Scope-based critical sections
 *
 *  This object locks when it is created and unlocks
 *  as soon as it goes out of scope.  This should be used
 *  You should never new allocate this object, lest you
 *  should forget to delete it.  You should also never modify the state of
 *  the mutex it owns while this object exists.
 *
 */
namespace mt
{
/*!
 *  \class CriticalSection
 *  \brief Scope-based critical section object
 *  This object locks when it is created and unlocks
 *  as soon as it goes out of scope.
 *
 *  \code
 *  void performCSOp(Info& specialInfo)
 *  {
 *     CriticalSection cs(&globalLock); // We now have the lock
 *     GlobalBuffer.remove(specialInfo);
 *  }
 *  \endcode
 */
template <typename T> struct CriticalSection
{
    //!  Constructor.  Lock the mutex.
    CriticalSection(T* mutex) :
        mMutex(mutex),
        mIsLocked(false)
    {
        manualLock();
    }

    //!  Destructor.  Unlock the mutex (if necessary).
    ~CriticalSection()
    {
        if (mIsLocked)
        {
            try
            {
                manualUnlock();
            }
            catch (...)
            {
                // Don't throw out of the destructor.
            }
        }
    }

    //!  Manual unlock the CS.  You probably don't want to use this.
    void manualUnlock()
    {
        mMutex->unlock();
        mIsLocked = false;
    }

    //!  Manual lock the CS.  You probably don't want to use this.
    void manualLock()
    {
        mMutex->lock();
        mIsLocked = true;
    }

    CriticalSection(const CriticalSection&) = delete;
    CriticalSection& operator=(const CriticalSection&) = delete;

private:
    T* const mMutex;
    bool mIsLocked;

};
}
#endif

/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2010, General Dynamics - Advanced Information Systems
 *
 * NITRO is free software; you can redistribute it and/or modify
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
 * License along with this program; if not, If not,
 * see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef __NITF_HANDLE_MANAGER_HPP__
#define __NITF_HANDLE_MANAGER_HPP__

#include <string>
#include <map>
#include <import/sys.h>
#include <import/mt.h>
#include "nitf/Handle.hpp"

namespace nitf
{
class HandleManager
{
private:
typedef void* CAddress;

    std::map<CAddress, Handle*> mHandleMap; //! map for storing the handles
    sys::Mutex mMutex; //! mutex used for locking the map

public:
    HandleManager() {}
    virtual ~HandleManager() {}

    template <typename T>
    bool hasHandle(T* object)
    {
        if (!object) return false;
        mt::CriticalSection<sys::Mutex> obtainLock(&mMutex);
        return mHandleMap.find(object) != mHandleMap.end();
    }

    template <typename T, typename DestructFunctor_T>
    BoundHandle<T, DestructFunctor_T>* acquireHandle(T* object)
    {
        if (!object) return NULL;
        mt::CriticalSection<sys::Mutex> obtainLock(&mMutex);
        if (mHandleMap.find(object) == mHandleMap.end())
        {
            BoundHandle<T, DestructFunctor_T>* handle =
                new BoundHandle<T, DestructFunctor_T>(object);
            mHandleMap[object] = handle;
        }
        BoundHandle<T, DestructFunctor_T>* handle =
            (BoundHandle<T, DestructFunctor_T>*)mHandleMap[object];
        obtainLock.manualUnlock();

        handle->incRef();
        return handle;
    }

    template <typename T>
    void releaseHandle(T* object)
    {
        mt::CriticalSection<sys::Mutex> obtainLock(&mMutex);
        std::map<CAddress, Handle*>::iterator it = mHandleMap.find(object);
        if (it != mHandleMap.end())
        {
            Handle* handle = (Handle*)it->second;
            if (handle->decRef() <= 0)
            {
                mHandleMap.erase(it);
                obtainLock.manualUnlock();
                delete handle;
            }
        }
    }
};

/*!
 * Create a Singleton registry for managing the Handles
 *
 * Note that this will NOT get deleted at exit, so there will be a memory loss
 * the size of a HandleManager object (about 50 bytes). We can't let the singleton
 * be deleted at exit, in case other singletons contain references to these
 * handles.
 */
typedef mt::Singleton<HandleManager, false> HandleRegistry;

}
#endif

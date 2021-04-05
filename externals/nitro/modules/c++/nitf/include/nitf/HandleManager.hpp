/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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
#pragma once

#include <string>
#include <map>
#include <mutex>

#include "nitf/coda-oss.hpp"
#include "nitf/Handle.hpp"

namespace nitf
{
class HandleManager
{
    using CAddress = void*;

    std::map<CAddress, Handle*> mHandleMap; //! map for storing the handles
    mutable std::mutex mMutex; //! mutex used for locking the map

    template <typename T, typename DestructFunctor_T>
    BoundHandle<T, DestructFunctor_T>* acquireHandle_(T* object)
    {
        using retval_t = BoundHandle<T, DestructFunctor_T>;

        std::lock_guard<std::mutex> obtainLock(mMutex);
        if (mHandleMap.find(object) == mHandleMap.end())
        {
            mHandleMap[object] = new retval_t(object);
        }
        return static_cast<retval_t*>(mHandleMap[object]);
    }

public:
    HandleManager() = default;
    virtual ~HandleManager() = default;

    HandleManager(const HandleManager&) = delete;
    HandleManager(HandleManager&&) = delete;
    HandleManager& operator=(const HandleManager&) = delete;
    HandleManager& operator=(HandleManager&&) = delete;

    template <typename T>
    bool hasHandle(T* object) const
    {
        if (!object) return false;
        std::lock_guard<std::mutex> obtainLock(mMutex);
        return mHandleMap.find(object) != mHandleMap.end();
    }

    template <typename T, typename DestructFunctor_T>
    BoundHandle<T, DestructFunctor_T>* acquireHandle(T* object)
    {
        if (!object) return nullptr;
        auto handle = acquireHandle_<T, DestructFunctor_T>(object);
        if (handle != nullptr)
        {
            handle->incRef();
        }
        return handle;
    }

    template <typename T>
    void releaseHandle(T* object)
    {
        Handle* handle = nullptr;
        {
            std::lock_guard<std::mutex> obtainLock(mMutex);
            std::map<CAddress, Handle*>::iterator it = mHandleMap.find(object);
            if (it != mHandleMap.end())
            {
                handle = it->second;
                if (handle != nullptr)
                {
                    if (handle->decRef() <= 0)
                    {
                        mHandleMap.erase(it);
                    }
                    else
                    {
                        handle = nullptr; // don't actually "delete"
                    }
                }
            }
        }

        delete handle;
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
using HandleRegistry =  mt::Singleton<HandleManager, false>;

}
#endif

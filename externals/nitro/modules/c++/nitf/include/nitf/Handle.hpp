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

#ifndef __NITF_HANDLE_HPP__
#define __NITF_HANDLE_HPP__
#pragma once

#include <iostream>
#include <mutex>

/*!
 *  \file Handle.hpp
 *  \brief Contains handle wrapper to manage shared native objects
 */
#include "nitf/System.hpp"

namespace nitf
{

/*!
 *  \class Handle
 *  \brief  This class is the base definition of a Handle
 */
struct Handle
{
    Handle() = default;
    virtual ~Handle() {}

    //! Get the ref count
    int getRef() const { return refCount; }

    //! Increment the ref count
    int incRef()
    {
        std::lock_guard<std::mutex> lock(mutex);
        if (refCount < 0) refCount = 0;
        refCount++;
        return refCount;
    }

    //! Decrement the ref count
    int decRef()
    {
        std::lock_guard<std::mutex> lock(mutex);
        refCount--;
        if (refCount < 0) refCount = 0;
        return refCount;
    }

protected:
    std::mutex mutex;
    int refCount{ 0 };
};


/*!
 *  \struct MemoryDestructor
 *  \brief  A functor that is used to destruct underlying memory contained
 *          in handles. Extend this class to custom-destruct objects.
 */
template <typename T>
struct MemoryDestructor
{
    virtual void operator()(T* /*nativeObject*/) {}
    virtual ~MemoryDestructor() {}
};


/*!
 *  \class Handle
 *  \brief  This class wraps a pointer and keeps track of
 *  the number of times it is referenced through the incRef
 *  and decRef functions.
 */
template <typename Class_T, typename DestructFunctor_T = MemoryDestructor<Class_T> >
class BoundHandle : public Handle
{
private:
    Class_T* handle;
    int managed;

public:
    //! Create handle from native object
    BoundHandle(Class_T* h = NULL) : handle(h), managed(1) {}

    //! Destructor
    virtual ~BoundHandle()
    {
        //call the destructor, to destroy the object
        if(handle && managed <= 0)
        {
            DestructFunctor_T functor;
            functor(handle);
        }
    }

    //! Assign from native object
    Handle& operator=(Class_T* h)
    {
        if (h != handle)
            handle = h;
        return *this;
    }

    //! Get the native object
    Class_T* get() { return handle; }

    //! Get the address of then native object
    Class_T** getAddress() { return &handle; }

    /*!
     * Sets whether or not the native object is "managed" by the underlying
     * C library. Most objects are in turn managed, but some (such as Records)
     * are not. By setting the object to be un-managed, the native object will
     * be passed to the DestructFunctor_T functor, and most likely destroyed,
     * depending on what the functor does.
     */
    void setManaged(bool flag) { managed += flag ? 1 : (managed == 0 ? 0 : -1); }

    //! Is the native object managed?
    bool isManaged() { return managed > 0; }

};

}
#endif

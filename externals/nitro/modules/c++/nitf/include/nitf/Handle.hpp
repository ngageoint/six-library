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

#ifndef NITRO_nitf_Handle_hpp_INCLUDED_
#define NITRO_nitf_Handle_hpp_INCLUDED_
#pragma once

#include <iostream>
#include <mutex>
#include <memory>

/*!
 *  \file Handle.hpp
 *  \brief Contains handle wrapper to manage shared native objects
 */
#include "nitf/System.hpp"
#include "nitf/exports.hpp"

namespace nitf
{

/*!
 *  \class Handle
 *  \brief  This class is the base definition of a Handle
 */
class NITRO_NITFCPP_API Handle
{
    class Impl;
    std::unique_ptr<Impl> mPimpl;

public:
    Handle() noexcept(false);
    virtual ~Handle()  /*noexcept(false)*/;

    Handle(const Handle&) = delete;
    Handle& operator=(const Handle&) = delete;
    Handle(Handle&&) noexcept;
    Handle& operator=(Handle&&) noexcept;

    //! Get the ref count
    int getRef() const noexcept;

    //! Increment the ref count
    int incRef();

    //! Decrement the ref count
    int decRef();
};


/*!
 *  \struct MemoryDestructor
 *  \brief  A functor that is used to destruct underlying memory contained
 *          in handles. Extend this class to custom-destruct objects.
 */
template <typename T>
struct NITRO_NITFCPP_API MemoryDestructor
{
    virtual void operator() (T* /*nativeObject*/) noexcept(false) {}
    virtual ~MemoryDestructor() {}
};


/*!
 *  \class Handle
 *  \brief  This class wraps a pointer and keeps track of
 *  the number of times it is referenced through the incRef
 *  and decRef functions.
 */
template <typename Class_T, typename DestructFunctor_T = MemoryDestructor<Class_T> >
class NITRO_NITFCPP_API BoundHandle : public Handle  // no "final", SWIG doesn't like it
{
    Class_T* handle = nullptr;
    int managed = 1;

public:
    //! Create handle from native object
    BoundHandle() = delete;
    BoundHandle(Class_T* h) : handle(h) {}

    ~BoundHandle() /*noexcept(false)*/
    {
        //call the destructor, to destroy the object
        if(handle && !isManaged())
        {
            DestructFunctor_T functor;
            functor(handle);
        }
    }    
    BoundHandle(const BoundHandle&) = delete;
    BoundHandle(BoundHandle&&) = default;
    BoundHandle& operator=(const BoundHandle&) = delete;
    BoundHandle& operator=(BoundHandle&&) = default;

    //! Assign from native object
    Handle& operator=(Class_T* h) noexcept
    {
        if (h != handle)
            handle = h;
        return *this;
    }

    //! Get the native object
    Class_T* get() noexcept { return handle; }
    const Class_T* get() const noexcept { return handle; }

    //! Get the address of then native object
    Class_T** getAddress() noexcept { return &handle; }

    /*!
     * Sets whether or not the native object is "managed" by the underlying
     * C library. Most objects are in turn managed, but some (such as Records)
     * are not. By setting the object to be un-managed, the native object will
     * be passed to the DestructFunctor_T functor, and most likely destroyed,
     * depending on what the functor does.
     */
    void setManaged(bool flag) noexcept { managed += flag ? 1 : (managed == 0 ? 0 : -1); }

    //! Is the native object managed?
    bool isManaged() const noexcept { return managed > 0; }

};

}
#endif // NITRO_nitf_Handle_hpp_INCLUDED_

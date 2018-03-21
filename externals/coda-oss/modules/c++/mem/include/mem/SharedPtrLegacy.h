/* =========================================================================
 * This file is part of mem-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * mem-c++ is free software; you can redistribute it and/or modify
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

#ifndef __MEM_SHARED_PTR_LEGACY_H__
#define __MEM_SHARED_PTR_LEGACY_H__

#include <memory>
#include <cstddef>

#include <sys/AtomicCounter.h>

namespace mem
{
/*!
 *  \class SharedPtr
 *  \brief This class provides RAII for object allocations via new.
 *         Additionally, it uses thread-safe reference counting so that the
 *         underlying pointer can be shared among multiple objects.  When the
 *         last SharedPtr goes out of scope, the underlying pointer is
 *         deleted.
 */
template <class T>
class SharedPtr
{
public:
    explicit SharedPtr(T* ptr = NULL) :
        mPtr(ptr)
    {
        // Initially we have a reference count of 1
        // In the constructor, we take ownership of the pointer no matter
        // what, so temporarily wrap it in an auto_ptr in case creating the
        // atomic counter throws
        std::auto_ptr<T> scopedPtr(ptr);
        mRefCtr = new sys::AtomicCounter(1);
        scopedPtr.release();
    }

    explicit SharedPtr(std::auto_ptr<T> ptr) :
        mPtr(ptr.get())
    {
        // Initially we have a reference count of 1
        // If this throws, the auto_ptr will clean up the input pointer
        // for us
        mRefCtr = new sys::AtomicCounter(1);

        // We now own the pointer
        ptr.release();
    }

    template <typename OtherT>
    explicit SharedPtr(std::auto_ptr<OtherT> ptr) :
        mPtr(ptr.get())
    {
        // Initially we have a reference count of 1
        // If this throws, the auto_ptr will clean up the input pointer
        // for us
        mRefCtr = new sys::AtomicCounter(1);

        // We now own the pointer
        ptr.release();
    }

    SharedPtr(const SharedPtr& rhs) :
        mRefCtr(rhs.mRefCtr),
        mPtr(rhs.mPtr)
    {
        mRefCtr->increment();
    }

    template <typename OtherT>
    SharedPtr(const SharedPtr<OtherT>& rhs) :
        mRefCtr(rhs.mRefCtr),
        mPtr(rhs.mPtr)
    {
        mRefCtr->increment();
    }

    ~SharedPtr()
    {
        if (mRefCtr->decrementThenGet() == 0)
        {
            delete mRefCtr;
            delete mPtr;
        }
    }

    const SharedPtr&
    operator=(const SharedPtr& rhs)
    {
        if (this != &rhs)
        {
            if (mRefCtr->decrementThenGet() == 0)
            {
                // We were holding the last copy of this data prior to this
                // assignment - need to clean it up
                delete mRefCtr;
                delete mPtr;
            }

            mRefCtr = rhs.mRefCtr;
            mPtr = rhs.mPtr;
            mRefCtr->increment();
        }

        return *this;
    }

    T* get() const
    {
        return mPtr;
    }

    T& operator*() const
    {
        return *mPtr;
    }

    T* operator->() const
    {
        return mPtr;
    }

    sys::AtomicCounter::ValueType getCount() const
    {
        return mRefCtr->get();
    }

    void reset(std::auto_ptr<T> scopedPtr)
    {
        // NOTE: We need to create newRefCtr on the side before decrementing
        //       mRefCtr. This way, we can provide the strong exception
        //       guarantee (i.e. the operation either succeeds or throws - the
        //       underlying object is always in a good state).
        sys::AtomicCounter* const newRefCtr = new sys::AtomicCounter(1);

        if (mRefCtr->decrementThenGet() == 0)
        {
            // We were holding the last copy of this data prior to this
            // reset - need to clean up
            delete mRefCtr;
            delete mPtr;
        }

        mRefCtr = newRefCtr;
        mPtr = scopedPtr.release();
    }

    void reset(T* ptr = NULL)
    {
        // We take ownership of the pointer no matter what, so
        // temporarily wrap it in an auto_ptr in case creating
        // the atomic counter throws in the underlying method.
        reset(std::auto_ptr<T>(ptr));
    }

    // This allows derived classes to be used for construction/assignment
    template <class OtherT> friend class SharedPtr;

private:
    sys::AtomicCounter* mRefCtr;
    T*                  mPtr;
};
}

#endif

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

#ifndef __NITF_OBJECT_HPP__
#define __NITF_OBJECT_HPP__
#pragma once

#include "nitf/coda-oss.hpp"
#include "nitf/Handle.hpp"
#include "nitf/HandleManager.hpp"
#include "nitf/NITFException.hpp"

namespace nitf
{
// Forward declarations so we can say we are friends
class List;
class HashTable;
/*!
 * \class Object
 * \brief  This class keeps a pointer to an underlying C object
 * The Object class does not destroy or clone any memory. The sole purpose is
 * to act as a non-invasive wrapper on top of objects internal to the NITF
 * C core.
 */
template <typename T, typename DestructFunctor_T = MemoryDestructor<T> >
class Object
{
protected:
    //make the nitf containers friends
    friend class List;
    friend class HashTable;

    //! The handle to the underlying memory
    BoundHandle<T, DestructFunctor_T>* mHandle = nullptr;

    //! Release this object's hold on the handle
    void releaseHandle()
    {
        if (mHandle && mHandle->get())
            HandleRegistry::getInstance().releaseHandle(mHandle->get());
        mHandle = nullptr;
    }

    //! Set native object
    virtual void setNative(T* nativeObj)
    {
        //only modify if it is a different native object
        if (!isValid() || mHandle->get() != nativeObj)
        {
            releaseHandle();
            mHandle = HandleRegistry::getInstance().template acquireHandle<T, DestructFunctor_T>(nativeObj);
        }
    }

    /*!
     * Attempt to set native object, or throw if it is null
     * Convenience method to help check errors from C "constructors"
     * \param nativeObj Native C object to set
     * \param error The error object used in "construction" of native C object
     *              This allows the resulting exception to easily propogate
     *              the correct error information
     * \throws NITFException if `nativeObj` is NULL
     *
     * Usage:
     *    auto* native = nitf_DESegment_construct(&error);
     *    setNativeOrThrow(native, &error);
     */
    virtual void setNativeOrThrow(T* nativeObj, nitf_Error* error)
    {
        if (nativeObj == nullptr)
        {
            throw nitf::NITFException(error);
        }
        setNative(nativeObj);
    }

public:

    //! Destructor
    virtual ~Object() { releaseHandle(); }

    //! Is the object valid (native object not null)?
    virtual bool isValid() const noexcept
    {
        return getNative() != nullptr;
    }

    //! Equality, based on handle
    bool operator==(const Object& obj) const noexcept
    {
        return mHandle == obj.mHandle;
    }

    //! Inequality, based on handle
    bool operator!=(const Object& obj) const noexcept
    {
        return !(operator==(obj));
    }

    //! Get native object
    virtual T * getNative() const noexcept
    {
        return mHandle ? mHandle->get() : nullptr;
    }

    //! Get native object
    virtual T * getNativeOrThrow() const
    {
        T* val = getNative();
        if (val)
            return val;
        throw nitf::NITFException(Ctxt("Invalid handle"));
    }

    /*!
     * Returns an ID that represents this particular object.
     * Currently, the ID is the stringized memory address of the underlying
     * memory.
     *
     * This *could* be used for equality purposes, however be warned, because
     * as objects die, etc. the memory addresses will be reused.
     */
    std::string getObjectID() const
    {
        return FmtX("%p", getNative());
    }

    bool isManaged() const noexcept { return isValid() && mHandle->isManaged(); }

    /*!
     * Set the management of the underlying memory
     *
     * \param flag  if flag is true, the underlying library will adopt and manage the memory
     *              if flag is false, the memory can be freed when refcount == 0
     */
    void setManaged(bool flag) noexcept
    {
        if (isValid())
            mHandle->setManaged(flag);
    }

    std::string toString() const
    {
        return getObjectID();
    }

    void incRef()
    {
        mHandle->incRef();
    }

    void decRef() 
    {
        mHandle->decRef();
    }

};


}

/*!
 * Helpful macro that can be used to define a Class with name _Name.
 * The macro will also create a sub-class of MemoryDestructor which knows
 * how to destroy the underlying memory for the given object. The _Name
 * gets stringified/concat'ed in the macro. An example for 'Record' would
 * look like this:
 *
 *  struct RecordDestructor : public MemoryDestructor<nitf_Record> \
 *  { \
 *      ~RecordDestructor() = default; \
 *      virtual void operator()(nitf_Record *nativeObject) \
 *      { nitf_Record_destruct(&nativeObject); } \
 *  }; \
 *  \
 *  class Record : public Object<nitf_Record, RecordDestructor>
 *
 * This works for all nitf objects that *can* be destroyed, and who have a
 * corresponding nitf_##_destruct method.
 */

#define DECLARE_CLASS_IN_operator_function_(Name_, Package_) \
void operator()(Package_##_##Name_ * nativeObject) override \
      { Package_##_##Name_##_destruct(&nativeObject); }
 
#ifdef _MSC_VER
#define DECLARE_CLASS_IN_operator_function(Name_, Package_) \
    __pragma(warning(push)) \
    __pragma(warning(disable: 26440)) /* Function '...' can be declared '...' (f.6). */ \
    DECLARE_CLASS_IN_operator_function_(Name_, Package_) \
    __pragma(warning(pop))
#else
#define DECLARE_CLASS_IN_operator_function(Name_, Package_) \
    DECLARE_CLASS_IN_operator_function_(Name_, Package_)
#endif

// SWIG doesn't like "final"
#define DECLARE_CLASS_IN(_Name, _Package) \
    struct _Name##Destructor /*final*/ : public nitf::MemoryDestructor<_Package##_##_Name> \
    { DECLARE_CLASS_IN_operator_function(_Name, _Package) }; \
    class _Name : public nitf::Object<_Package##_##_Name, _Name##Destructor>

#define DECLARE_CLASS(_Name) DECLARE_CLASS_IN(_Name, nitf)

#endif

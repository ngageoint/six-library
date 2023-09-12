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

#pragma once
#ifndef NITRO_nitf_Object_hpp_INCLUDED_
#define NITRO_nitf_Object_hpp_INCLUDED_

#include <assert.h>

#include <type_traits>
#include <std/cstddef> // std::byte

#include "nitf/coda-oss.hpp"
#include "nitf/Handle.hpp"
#include "nitf/exports.hpp"
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
class NITRO_NITFCPP_API Object
{
protected:
    //make the nitf containers friends
    friend class List;
    friend class HashTable;

    //! The handle to the underlying memory
    BoundHandle<T, DestructFunctor_T>* mHandle = nullptr;
private:
    bool isValidHandle() const noexcept
    {
        return mHandle && mHandle->get();
    }
protected:
    //! Release this object's hold on the handle
    void releaseHandle()
    {
        if (isValidHandle())
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
            assert(error != nullptr);
            throw nitf::NITFException(error);
        }
        setNative(nativeObj);
    }

public:
    virtual ~Object() /*noexcept(noexcept(releaseHandle()))*/ { releaseHandle(); }

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
    virtual T * getNativeOrThrow() const noexcept(false)
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

// Don't need both "override" and "final": https://learn.microsoft.com/en-us/cpp/code-quality/c26435?view=msvc-170 
#define DECLARE_CLASS_IN_operator_function_(Name_, Package_) \
void operator()(Package_##_##Name_ * nativeObject) NITRO_nitf_MemoryDestructor_noexcept_false_ final \
      { Package_##_##Name_##_destruct(&nativeObject); }
 
#ifdef _MSC_VER
// https://stackoverflow.com/questions/599035/force-visual-studio-to-link-all-symbols-in-a-lib-file
#define DECLARE_CLASS_IN_operator_function(Name_, Package_) \
    __pragma(comment(linker,"/export:" #Package_ "_" #Name_ "_destruct")); \
    __pragma(warning(push)) __pragma(warning(disable: 26440)) /* Function '...' can be declared '...' (f.6). */ \
    DECLARE_CLASS_IN_operator_function_(Name_, Package_) \
    __pragma(warning(pop))
#else
#define DECLARE_CLASS_IN_operator_function(Name_, Package_) \
    DECLARE_CLASS_IN_operator_function_(Name_, Package_)
#endif

// SWIG doesn't like "final"
#define DECLARE_CLASS_IN(_Name, _Package) \
    struct NITRO_NITFCPP_API _Name##Destructor /*final*/ : public nitf::MemoryDestructor<_Package##_##_Name> \
    { DECLARE_CLASS_IN_operator_function(_Name, _Package) }; \
    class NITRO_NITFCPP_API _Name : public nitf::Object<_Package##_##_Name, _Name##Destructor>

#define NITRO_DECLARE_CLASS_NRT(_Name) DECLARE_CLASS_IN(_Name, nrt)
#define NITRO_DECLARE_CLASS_NITF(_Name) DECLARE_CLASS_IN(_Name, nitf)
#define NITRO_DECLARE_CLASS_J2K(Name_) DECLARE_CLASS_IN(Name_, j2k)
#define DECLARE_CLASS(_Name) NITRO_DECLARE_CLASS_NITF(_Name)

namespace nitf
{
    // Refer to a field in a native structure without have to explicitly name it; rather
    // use the offset and pointer math.  Besides making it easier to refer to fields
    // without naming them, it makes it easier to iterate over all of the fields in
    // a struct.

    // fieldOffset is  offsetof(TNative, <field>), e.g., offsetof(nitf_TextSubheader, filePartType)
    template<typename TReturn, typename TNative>
    inline TReturn fromNativeOffset_(const TNative& native, size_t fieldOffset) noexcept
    {
        // This should be a C struct
        static_assert(std::is_standard_layout<TNative>::value, "!std::is_standard_layout<>");

        const void* const pNative_ = &native;
        auto pNativeBytes = static_cast<const std::byte*>(pNative_); // for pointer math
        pNativeBytes += fieldOffset;
        const void* const pAddressOfField_ = pNativeBytes;

        auto pAddressOfField = static_cast<const TReturn*>(pAddressOfField_);
        if (pAddressOfField != nullptr) // code-analysis diagnostic
        {
            return *pAddressOfField;
        }
        return nullptr;
    }
    template<typename TReturn, typename TObject>
    inline TReturn fromNativeOffset(const TObject& object, size_t fieldOffset) noexcept(false)
    {
        auto& native = *(object.getNativeOrThrow()); // e.g., nitf_testing_Test1a&

        // Be sure this is one of our wrapper objects; if it is, it will have:
        //    using native_t = nitf_<C type>;
        using native_object_t = typename TObject::native_t; // e.g., nitf_testing_Test1a
        using get_native_t = typename std::remove_reference<decltype(native)>::type;
        static_assert(std::is_same<native_object_t, get_native_t>::value, "!std::is_same<>");

        using native_t = typename TReturn::native_t; // e.g., nitf_Field
        auto const pField = fromNativeOffset_<native_t*>(native, fieldOffset);
        return TReturn(pField); // e.g., nitf::Field(pNativeField)
    }
    #define nitf_offsetof(name) offsetof(native_t, name)
}

#endif // NITRO_nitf_Object_hpp_INCLUDED_

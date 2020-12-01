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

#ifndef __NITF_EXTENSIONS_HPP__
#define __NITF_EXTENSIONS_HPP__

#include "nitf/Extensions.h"
#include "nitf/System.hpp"
#include "nitf/NITFException.hpp"
#include "nitf/TRE.hpp"
#include "nitf/Object.hpp"
#include <string>

/*!
 *  \file Extensions.hpp
 *  \brief  Contains wrapper implementation for Extensions
 */

namespace nitf
{
/*!
 *  \class ExtensionsIterator
 *  \brief  The C++ wrapper for the nitf_ExtensionsIterator
 */
class ExtensionsIterator
{
public:
    //! Constructor
    ExtensionsIterator() = default;

    //! Copy constructor
    ExtensionsIterator(const ExtensionsIterator & x) noexcept
    {
        *this = x;
    }

    //! Assignment Operator
    ExtensionsIterator & operator=(const ExtensionsIterator & x) noexcept
    {
        if (&x != this)
            handle = x.handle;
        return *this;
    }

    /*!
     *  Set native object
     */
    ExtensionsIterator(nitf_ExtensionsIterator x) noexcept
    {
        setHandle(x);
    }

    /*!
     *  Get native object
     */
    nitf_ExtensionsIterator & getHandle() noexcept
    {
        return handle;
    }

    /*!
     *  Set native object
     */
    void setHandle(nitf_ExtensionsIterator x) noexcept
    {
        handle = x;
    }

    /*!
     *  Checks to see if this iterator is equal to another
     *  \param it2  The iterator to compare with
     *  \return  True if so, and False otherwise
     */
    bool equals(nitf::ExtensionsIterator & it2) const noexcept
    {
        const NITF_BOOL x = nitf_ExtensionsIterator_equals(&handle, &it2.getHandle());
        if (!x) return false;
        return true;
    }

    bool operator==(const nitf::ExtensionsIterator& it2) const noexcept
    {
        return this->equals((nitf::ExtensionsIterator&)it2);
    }

    /*!
     *  Checks to see if this iterator is not equal to another
     *  \param it2  The iterator to compare with
     *  \return  True if so, and False otherwise
     */
    bool notEqualTo(nitf::ExtensionsIterator & it2) const noexcept
    {
        const NITF_BOOL x = nitf_ExtensionsIterator_notEqualTo(&handle, &it2.getHandle());
        if (!x) return false;
        return true;
    }

    bool operator!=(const nitf::ExtensionsIterator& it2) const noexcept
    {
        return this->notEqualTo((nitf::ExtensionsIterator&)it2);
    }

    /*!
     *  Increment the iterator
     */
    void increment() noexcept
    {
        nitf_ExtensionsIterator_increment(&handle);
    }

    //! Increment the iterator (postfix)
    void operator++(int ) noexcept
    {
        increment();
    }

    //! Increment the iterator (prefix)
    void operator++() noexcept
    {
        increment();
    }

    //! Get the TRE from the iterator
    nitf::TRE operator*()
    {
        return get();
    }

    /*!
     *  Get the TRE from the iterator
     */
    nitf::TRE get() const
    {
        nitf_TRE * x = nitf_ExtensionsIterator_get(&handle);
        return nitf::TRE(x);
    }

private:
    mutable nitf_ExtensionsIterator handle;
    nitf_Error error;
};


/*!
 *  \class Extensions
 *  \brief  The C++ wrapper for the nitf_Extensions
 *
 *  The Extensions may exist in almost any part of the
 *  NITF file.  In essence, this object is a data store
 *  of information that was provided in data extensions.
 */
DECLARE_CLASS(Extensions)
{
public:

typedef nitf::ExtensionsIterator Iterator;

    //! Copy constructor
    Extensions(const Extensions & x)
    {
        setNative(x.getNative());
    }

    //! Assignment Operator
    Extensions & operator=(const Extensions & x)
    {
        if (&x != this)
            setNative(x.getNative());
        return *this;
    }

    //! Set native object
    Extensions(nitf_Extensions * x)
    {
        setNative(x);
        getNativeOrThrow();
    }

    //! Constructor
    Extensions() noexcept(false)
    {
        setNative(nitf_Extensions_construct(&error));
        getNativeOrThrow();
        setManaged(false);
    }

    //! Clone
    nitf::Extensions clone() const
    {
        nitf::Extensions dolly(nitf_Extensions_clone(getNativeOrThrow(), &error));
        dolly.setManaged(false);
        return dolly;
    }

    ~Extensions() = default;

    /*!
     *  Insert a TRE into the extensions section
     *  \param name  The name of the TRE
     *  \param tre  The TRE itself
     */
    void appendTRE(nitf::TRE & tre)
    {
        //if the TRE is already managed, we throw an Exception
        if (tre.isManaged())
            throw nitf::NITFException(Ctxt("The given TRE is already managed by the library. Try cloning it first."));

        const NITF_BOOL x = nitf_Extensions_appendTRE(getNative(),
            tre.getNativeOrThrow(), &error);
        if (!x)
            throw nitf::NITFException(&error);
        tre.setManaged(true); //The TRE is now owned by the underlying extensions object
    }

    /*!
     *  Get the TREs by the name given
     *  \param  The name of the TRE to get
     *  \return  A List of TREs matching the specified name
     */
    nitf::List getTREsByName(const std::string& name) const
    {
        nitf_List* x = nitf_Extensions_getTREsByName(getNative(), name.c_str());
        if (!x)
            throw except::NoSuchKeyException();
        return nitf::List(x);
    }

    /*!
     *  Sometimes we're simply not interested in a TRE section
     *  at all.  We can kill the entire list of TREs using
     *  this function.
     *  \param name  The name of the TREs to erase
     */
    void removeTREsByName(const std::string& name) noexcept
    {
        nitf_Extensions_removeTREsByName(getNative(), name.c_str());
    }

    /*!
     * Remove the TRE at the given iterator position
     * \param iter  The ExtensionsIterator to erase
     */
    void remove(Iterator& iter) noexcept
    {
        nitf_TRE* tre = nitf_Extensions_remove(
                            getNative(), &iter.getHandle(), &error);
        if (tre) delete tre;
    }


    //! Get the hash
    nitf::HashTable getHash() const
    {
        return nitf::HashTable(getNativeOrThrow()->hash);
    }

    //! Set the hash
    void setHash(nitf::HashTable & value)
    {
        //release the currently "owned" one
        nitf::HashTable ht = nitf::HashTable(getNativeOrThrow()->hash);
        ht.setManaged(false);

        //have the library manage the "new" one
        getNativeOrThrow()->hash = value.getNative();
        value.setManaged(true);
    }

    /*!
     *  Checks if the TRE exists
     *  \param  The name of the TRE
     */
    bool exists(const std::string& key) const noexcept
    {
        const NITF_BOOL x = nitf_Extensions_exists(getNative(), key.c_str());
        return x ? true : false;
    }

    /*!
     *  Get the begin iterator
     *  \return The iterator pointing to the first TRE
     */
    Iterator begin() const noexcept
    {
        const nitf_ExtensionsIterator x = nitf_Extensions_begin(getNative());
        return nitf::ExtensionsIterator(x);
    }

    /*!
     *  Get the end iterator
     *  \return  The iterator pointing PAST the last TRE (null)
     */
    Iterator end() const noexcept
    {
        const nitf_ExtensionsIterator x = nitf_Extensions_end(getNative());
        return nitf::ExtensionsIterator(x);
    }

    uint64_t computeLength(nitf::Version version) const noexcept
    {
        return nitf_Extensions_computeLength(getNative(), version, &error);
    }

private:
    mutable nitf_Error error{};
};
}
#endif

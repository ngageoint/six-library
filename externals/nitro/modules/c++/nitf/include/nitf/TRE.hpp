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

#ifndef __NITF_TRE_HPP__
#define __NITF_TRE_HPP__

#include <string>
#include "nitf/Field.hpp"
#include "nitf/Object.hpp"
#include "nitf/Pair.hpp"
#include "nitf/System.hpp"
#include "nitf/TRE.h"

/*!
 *  \file TRE.hpp
 *  \brief  Contains wrapper implementation for TRE
 */
namespace nitf
{
/*!
 *  \class FieldIterator
 *  \brief  The C++ wrapper for the nitf_TREEnumerator
 */
struct TREFieldIterator : public nitf::Object<nitf_TREEnumerator> // no "final", SWIG doesn't like it
{
    TREFieldIterator() noexcept(false)
    {
        setNative(nullptr);
    }

    ~TREFieldIterator() = default;


    //! Copy constructor
    TREFieldIterator(const TREFieldIterator& x)
    {
        *this = x;
    }

    //! Assignment Operator
    TREFieldIterator& operator=(const TREFieldIterator& x)
    {
        if (&x != this)
        {
            setNative(x.getNative());
            mPair = x.mPair;
        }
        return *this;
    }

    //! Set native object
    TREFieldIterator(nitf_TREEnumerator* x) : mPair(nullptr)
    {
        setNative(x);
        getNativeOrThrow();
        increment();
    }

    TREFieldIterator(NITF_DATA* x)
    {
        *this = x;
    }
    TREFieldIterator& operator=(NITF_DATA* x)
    {
        setNative(static_cast<nitf_TREEnumerator*>(x));
        getNativeOrThrow();
        increment();
        return *this;
    }

    /*!
     *  Checks to see if this iterator is equal to another
     *  \param it2  The iterator to compare with
     *  \return  True if so, and False otherwise
     */
    bool operator==(const nitf::TREFieldIterator& it2) const noexcept
    {
        // need to do this double-check so that the last iteration of an
        // iterator doesn't get skipped
        if (getNative() == it2.getNative())
            return mPair == it2.mPair;
        return false;
    }

    bool operator!=(const nitf::TREFieldIterator& it2) const noexcept
    {
        return !this->operator==((nitf::TREFieldIterator&)it2);
    }

    /*!
     *  Increment the iterator
     */
    void increment()
    {
        nitf_TREEnumerator* enumerator = getNative();
        if (enumerator != nullptr)
        {
            if (isValid() && enumerator->hasNext(&enumerator))
                mPair = enumerator->next(enumerator, &error);
            else
                mPair = nullptr;
        }
        setNative(enumerator);  // always reset, in case it got destroyed
    }

    //! Increment the iterator (postfix)
    void operator++(int)
    {
        increment();
    }

    //! Increment the iterator (prefix)
    void operator++()
    {
        increment();
    }

    //! Get the TRE from the iterator
    nitf::Pair operator*()
    {
        if (!mPair)
            throw except::NullPointerReference();
        return nitf::Pair(mPair);
    }

    std::string getFieldDescription() const
    {
        nitf_TREEnumerator* enumerator = getNative();
        if (enumerator && isValid())
        {
            const char* desc =
                    enumerator->getFieldDescription(enumerator, &error);
            if (desc)
                return desc;
        }
        return "";
    }

    private:
    mutable nitf_Error error{};
    nitf_Pair* mPair = nullptr;
};

/*!
 *  \class TRE
 *  \brief  The C++ wrapper for the nitf_TRE
 */
DECLARE_CLASS(TRE)
{
    public:
    typedef nitf::TREFieldIterator Iterator;

    //! Copy constructor
    TRE(const TRE& x);

    //! Assignment Operator
    TRE& operator=(const TRE& x);

    //! Set native object
    TRE(nitf_TRE * x);

    TRE(NITF_DATA * x);

    TRE& operator=(NITF_DATA* x);

    //! Without the const char* constructors, in VS if you do something like
    //  TRE("my_tre_tag")
    //  You get the NITF_DATA* constructor rather than the std::string one
    TRE(const char* tag);

    TRE(const char* tag, const char* id);

    TRE(const std::string& tag);

    TRE(const std::string& tag, const std::string& id);

    //! Clone
    nitf::TRE clone() const;

    ~TRE();

    /*!
     *  Get a begin TRE field iterator
     *  \return  A field iterator pointing at the first field in the TRE
     */
    Iterator begin() const;

    /*!
     *  Get an end TRE field iterator
     *  \return  A field iterator pointing PAST the last field in the TRE
     */
    Iterator end() const noexcept;

    /*!
     * Get the field specified by the key. Throws an exception if the field
     * does not exist.
     */
    nitf::Field getField(const std::string& key) const;

    nitf::Field operator[](const std::string& key);

    /*!
     * Returns a List of Fields that match the given pattern.
     */
    nitf::List find(const std::string& pattern) const;

    /*!
     * Recalculate the field counts and positions for the TRE.
     * This might be necessary if, for example, you need to
     * change the number of iterations in a loop.
     *
     * setField does not do this by default, because some
     * applications want to set each field all at once.
     * If the TRE is large, recalculating the metadata each time
     * you change something becomes expensive.
     *
     * If you find yourself writing code that looks like this:
     *
     *  tre.setField("key", value);
     *  tre.updateFields();
     *  tre.setField("key2", value);
     *  tre.updateFields();
     *  tre.setField("key3", value);
     *  tre.updateFields();
     *
     * You probably want to run an iterator over the TRE instead.
     * The incrementing will automatically figure out any changed
     * positioning due to loops or conditionals as it goes, and it
     * will be faster.
     *
     * In short, this is here if you need it, but if you're needing
     * it frequently you should open an issue/look for a better solution
     */
    void updateFields();

    /*!
     * Sets the field with the given value. The input value
     * is converted to a std::string, and the string-ized value
     * is then used as the value input for the TRE field.
     * \param key The name of the field to set
     * \param value New value for specified field
     * \param forceUpdate If true, recalculate the number and positions
     *                    of the TRE fields. See `updateFields()`
     */
    template <typename T>
    void setField(std::string key, T value, bool forceUpdate = false)
    {
        nitf_Field* field = nitf_TRE_getField(getNative(), key.c_str());
        if (!field)
        {
            std::ostringstream msg;
            msg << key << " is not a recognized field for this TRE";
            throw except::Exception(Ctxt(msg.str()));
        }
        if (field->type == NITF_BINARY)
        {
            if (!nitf_TRE_setField(getNative(),
                                   key.c_str(),
                                   &value,
                                   sizeof(value),
                                   &error))
            {
                throw NITFException(&error);
            }
        }
        else
        {
            std::string s = truncate(str::toString(value), field->length);
            if (!nitf_TRE_setField(getNative(),
                                   key.c_str(),
                                   (NITF_DATA*)s.c_str(),
                                   s.size(),
                                   &error))
            {
                throw NITFException(&error);
            }
        }

        if (forceUpdate)
        {
            updateFields();
        }
    }

    /*!
     *  Does the field exist?
     *  \param key  The field name in which to check
     */
    bool exists(const std::string& key) const;

    //! Get the total length of the TRE data
    size_t getCurrentSize() const;

    //! Get the tag
    std::string getTag() const;

    //! Set the tag
    void setTag(const std::string& value);

    /**
     * Get the TRE identifier. This is NOT the tag, however it may be the
     * same value as the tag. The ID is used to identify a specific
     * version/incarnation of the TRE, if multiple are possible. For most TREs,
     * this value will be the same as the tag.
     */
    std::string getID() const;

    private:
    std::string truncate(const std::string& value, size_t maxDigits) const;

    mutable nitf_Error error{};
};
}
#endif

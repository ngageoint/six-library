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

#ifndef __NITF_TRE_HPP__
#define __NITF_TRE_HPP__

#include "nitf/TRE.h"
#include "nitf/System.hpp"
#include "nitf/Field.hpp"
#include "nitf/Pair.hpp"
#include "nitf/Object.hpp"
#include <string>

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
class TREFieldIterator : public nitf::Object<nitf_TREEnumerator>
{
public:
    TREFieldIterator() : mPair(NULL)
    {
        setNative(NULL);
    }

    ~TREFieldIterator(){}

    //! Copy constructor
    TREFieldIterator(const TREFieldIterator & x)
    {
        setNative(x.getNative());
    }

    //! Assignment Operator
    TREFieldIterator & operator=(const TREFieldIterator & x)
    {
        if (&x != this)
        {
            setNative(x.getNative());
            mPair = x.mPair;
        }
        return *this;
    }

    //! Set native object
    TREFieldIterator(nitf_TREEnumerator * x) : mPair(NULL)
    {
        setNative(x);
        getNativeOrThrow();
        increment();
    }

    TREFieldIterator(NITF_DATA * x)
    {
        setNative((nitf_TREEnumerator*)x);
        getNativeOrThrow();
        increment();
    }

    TREFieldIterator & operator=(NITF_DATA * x)
    {
        setNative((nitf_TREEnumerator*)x);
        getNativeOrThrow();
        increment();
        return *this;
    }

    /*!
     *  Checks to see if this iterator is equal to another
     *  \param it2  The iterator to compare with
     *  \return  True if so, and False otherwise
     */
    bool operator==(const nitf::TREFieldIterator& it2)
    {
        //need to do this double-check so that the last iteration of an iterator
        //doesn't get skipped
        if (getNative() == it2.getNative())
            return mPair == it2.mPair;
        return false;
    }

    bool operator!=(const nitf::TREFieldIterator& it2)
    {
        return !this->operator==((nitf::TREFieldIterator&)it2);
    }

    /*!
     *  Increment the iterator
     */
    void increment()
    {
        nitf_TREEnumerator* enumerator = getNative();
        if (isValid() && enumerator->hasNext(&enumerator))
            mPair = enumerator->next(enumerator, &error);
        else
            mPair = NULL;
        setNative(enumerator); //always reset, in case it got destroyed
    }

    //! Increment the iterator (postfix)
    void operator++(int x)
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

    std::string getFieldDescription()
    {
        nitf_TREEnumerator* enumerator = getNative();
        if (enumerator && isValid())
        {
            const char* desc = enumerator->getFieldDescription(enumerator,
                                                               &error);
            if (desc)
                return std::string(desc);
        }
        return "";
    }

private:
    nitf_Error error;
    nitf_Pair *mPair;
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
    TRE(const TRE & x);

    //! Assignment Operator
    TRE & operator=(const TRE & x);

    //! Set native object
    TRE(nitf_TRE * x);

    TRE(NITF_DATA * x);

    TRE & operator=(NITF_DATA * x);

    //! Without the const char* constructors, in VS if you do something like
    //  TRE("my_tre_tag")
    //  You get the NITF_DATA* constructor rather than the std::string one
    TRE(const char* tag) throw(nitf::NITFException);

    TRE(const char* tag, const char* id) throw(nitf::NITFException);

    TRE(const std::string& tag) throw(nitf::NITFException);

    TRE(const std::string& tag, const std::string& id)
        throw(nitf::NITFException);

    //! Clone
    nitf::TRE clone() throw(nitf::NITFException);

    ~TRE();

    /*!
     *  Get a begin TRE field iterator
     *  \return  A field iterator pointing at the first field in the TRE
     */
    Iterator begin();

    /*!
     *  Get an end TRE field iterator
     *  \return  A field iterator pointing PAST the last field in the TRE
     */
    Iterator end() throw (nitf::NITFException);

    /*!
     * Get the field specified by the key. Throws an exception if the field
     * does not exist.
     */
    nitf::Field getField(const std::string& key)
        throw(except::NoSuchKeyException);

    nitf::Field operator[] (const std::string& key)
        throw(except::NoSuchKeyException);

    /*!
     * Returns a List of Fields that match the given pattern.
     */
    nitf::List find(const std::string& pattern);

	/*!
	 * Sets the field with the given value. The input value
	 * is converted to a std::string, and the string-ized value
	 * is then used as the value input for the TRE field.
	 */
    template <typename T> void setField(std::string key, T value)
    {
    	std::string s = str::toString<T>(value);
	    if (!nitf_TRE_setField(getNative(),
	                            key.c_str(),
	                            (NITF_DATA*)s.c_str(),
	                            s.size(),
	                            &error) )
	        throw nitf::NITFException(&error);
    }

    /*!
     *  Does the field exist?
     *  \param key  The field name in which to check
     */
    bool exists(const std::string& key);

    //! Get the total length of the TRE data
    size_t getCurrentSize();

    //! Get the tag
    std::string getTag() const;

    //! Set the tag
    void setTag(const std::string & value);

    /**
     * Get the TRE identifier. This is NOT the tag, however it may be the
     * same value as the tag. The ID is used to identify a specific
     * version/incarnation of the TRE, if multiple are possible. For most TREs,
     * this value will be the same as the tag.
     */
    std::string getID() const;


private:
    nitf_Error error;
};

}
#endif

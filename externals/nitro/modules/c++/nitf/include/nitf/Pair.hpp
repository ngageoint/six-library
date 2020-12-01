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

#ifndef __NITF_PAIR_HPP__
#define __NITF_PAIR_HPP__
#pragma once

#include "nitf/System.hpp"
#include "nitf/Object.hpp"
#include <string>

/*!
 *  \file Pair.hpp
 *  \brief  Contains wrapper implementation for Pair
 */
namespace nitf
{

/*!
 *  \class Pair
 *  \brief  The C++ wrapper for the nitf_Pair
 */
struct Pair final : public nitf::Object<nitf_Pair>
{
    ~Pair() = default;
    Pair(Pair&&) = default;
    Pair& operator=(Pair&&) = default;

    //! Copy constructor
    Pair(const Pair & x)
    {
        *this = x;
    }

    //! Assignment Operator
    Pair & operator=(const Pair & x)
    {
        if (&x != this)
            setNative(x.getNative());
        return *this;
    }

    //! Set native object
    Pair(nitf_Pair * x)
    {
        setNative(x);
        getNativeOrThrow();
    }

    Pair(NITF_DATA * x)
    {
        *this = x;
    }

    Pair & operator=(NITF_DATA * x)
    {
        setNative(static_cast<nitf_Pair*>(x));
        getNativeOrThrow();
        return *this;
    }

    /*!
     *  Copy the key, maintain a pointer to the data
     *  \param key  The key in the pair (is copied)
     *  \param data  The data in the pair (not a copy)
     */
    void init(const std::string& key, NITF_DATA* data) noexcept
    {
        nitf_Pair_init(getNative(), key.c_str(), data);
    }

    /*!
     *  Simply calls the init method
     *  \param src  The source Pair
     */
    void copy(const nitf::Pair & src) noexcept
    {
        nitf_Pair_copy(getNative(), src.getNative());
    }

    //! Get the key
    char * getKey() const
    {
        return getNativeOrThrow()->key;
    }
    //! Set the key
    void setKey(char * value)
    {
        getNativeOrThrow()->key = value;
    }
    //! Get the data
    NITF_DATA * getData() const
    {
        return getNativeOrThrow()->data;
    }
    //! Set the data
    void setData(NITF_DATA * value)
    {
        getNativeOrThrow()->data = value;
    }

    //! Get the first component (key)
    char * first() const
    {
        return getKey();
    }
    //! Get the second component (data)
    NITF_DATA * second() const
    {
        return getData();
    }

private:
    Pair() = default;
    nitf_Error error{};
};

}
#endif

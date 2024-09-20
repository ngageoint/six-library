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

#ifndef __NITF_FIELD_HPP__
#define __NITF_FIELD_HPP__
#pragma once

#include <string>
#include <limits>
#include <type_traits>
#include <std/cstddef> // std::byte

#include <nitf/Field.h>

#include <nitf/coda-oss.hpp>
#include <nitf/System.hpp>
#include <nitf/HashTable.hpp>
#include <nitf/List.hpp>
#include <nitf/DateTime.hpp>
#include <nitf/Object.hpp>
#include <nitf/NITFException.hpp>
#include <nitf/exports.hpp>

/*!
 *  \file Field.hpp
 *  \brief  Contains wrapper implementation for Field
 */

namespace nitf
{
namespace detail
{
// Some template metaprogramming to allow us to provide a templated
// operator T() below - this'll get us the right numeric type and then we
// have an overloading for std::string below
// Note that this has to be at the namespace level - we can't define this class
// inside Field due to the explicit specializations
template <bool IsIntegerT, bool IsSignedT>
struct GetConvType
{
};

template <>
struct GetConvType<true, true>
{
    static const nitf_ConvType CONV_TYPE = NITF_CONV_INT;
};

template <>
struct GetConvType<true, false>
{
    static const nitf_ConvType CONV_TYPE = NITF_CONV_UINT;
};

template <bool IsSignedT>
struct GetConvType<false, IsSignedT>
{
    static const nitf_ConvType CONV_TYPE = NITF_CONV_REAL;
};
}

/*!
 *  \class Field
 *  \brief  The C++ wrapper for the nitf_Field
 *
 *  The Field is a generic type object that allows storage
 *  and casting of data amongst disparate data types.
 */
class NITRO_NITFCPP_API Field /*final*/ : public nitf::Object<nitf_Field> // no "final", SWIG doesn't like it
{
    void setU_(uint32_t data);
    void set_(int32_t data);
    void set_(double data);

public:
    using FieldType = nitf::FieldType;
    static const auto BCS_A = FieldType::NITF_BCS_A;
    static const auto BCS_N = FieldType::NITF_BCS_N;
    static const auto BINARY = FieldType::NITF_BINARY;

    Field & operator=(const char * value)
    {
        set(value);
        return *this;
    }

    Field & operator=(const std::string& value)
    {
        set(value);
        return *this;
    }

    Field & operator=(int8_t value)
    {
        set(value);
        return *this;
    }

    Field & operator=(int16_t value)
    {
        set(value);
        return *this;
    }

    Field & operator=(int32_t value)
    {
        set(value);
        return *this;
    }

    Field & operator=(int64_t value)
    {
        set(value);
        return *this;
    }

    Field & operator=(uint8_t value)
    {
        set(value);
        return *this;
    }

    Field & operator=(uint16_t value)
    {
        set(value);
        return *this;
    }

    Field & operator=(uint32_t value)
    {
        set(value);
        return *this;
    }

    Field & operator=(uint64_t value)
    {
        set(value);
        return *this;
    }

    Field & operator=(float value)
    {
        set(value);
        return *this;
    }

    Field & operator=(double value)
    {
        set(value);
        return *this;
    }

    //! Copy constructor
    Field(const Field & x)
    {
        *this = x;
    }

    //! Assignment Operator
    Field & operator=(const Field & x)
    {
        if (&x != this)
            setNative(x.getNative());
        return *this;
    }

    Field() = delete; // does not make sense to construct a Field from scratch
    Field(size_t length, FieldType type);

    //! Set native object
    using native_t = nitf_Field;
    Field(nitf_Field * field)
    {
        setNative(field);
        getNativeOrThrow();
    }

    Field(NITF_DATA * x)
    {
        *this = x;
    }

    Field & operator=(NITF_DATA * x)
    {
        setNative(static_cast<nitf_Field*>(x));
        getNativeOrThrow();
        return *this;
    }

    ~Field() = default;

    void set(uint8_t data)
    {
        setU_(data);
    }
    void set(uint16_t data)
    {
        setU_(data);
    }
    void set(uint32_t data)
    {
        setU_(data);
    }
    void set(uint64_t data);
    void set(int8_t data)
    {
        set_(data);
    }
    void set(int16_t data)
    {
        set_(data);
    }
    void set(int32_t data)
    {
        set_(data);
    }
    void set(int64_t data);
    void set(float data)
    {
        set_(data);
    }
    void set(double data)
    {
        set_(data);
    }
    void set(const char* data);
    void set(const std::string& data)
    {
        set(data.c_str());
    }
    void set(const nitf::DateTime& dateTime, const std::string& format = NITF_DATE_FORMAT_21);

    nitf::DateTime asDateTime(const std::string& format = NITF_DATE_FORMAT_21) const;

    //! Get the type
    FieldType getType() const
    {
        return static_cast<FieldType>(getNativeOrThrow()->type);
    }

    //! Set the type
    void setType(FieldType type)
    {
        getNativeOrThrow()->type = static_cast<nitf_FieldType>(type);
    }

    //! Get the data
    char * getRawData() const
    {
        return getNativeOrThrow()->raw;
    }
    //! Set the data
    void setRawData(char * raw, size_t length)
    {
        set(raw, length);
    }

    //! Get the data length
    size_t getLength() const
    {
        return getNativeOrThrow()->length;
    }

    /**
     * Resizes the field to the given length.
     * Use this method with caution as it can resize a field to be larger than
     * it should be, according to specs.
     */
    void resize(size_t length);

    //! Returns the field as any numeric type T
    template <typename T>
    operator T() const
    {
        T retval;
        get(&retval,
            detail::GetConvType<std::numeric_limits<T>::is_integer,
                                std::numeric_limits<T>::is_signed>::CONV_TYPE,
            sizeof(T));
        return retval;
    }

    //! Returns the field as a string
    operator std::string() const
    {
        const auto pNative = getNativeOrThrow();
        return std::string(pNative->raw, pNative->length);
    }
    std::string toString(bool trim=false) const
    {
        return trim ? toTrimString() : *this;
    }
    std::string toTrimString() const
    {
        std::string retval = *this; // implicitly converted to std::string
        str::trim(retval);
        return retval;
    }

    operator char* () const = delete; // Don't allow this cast ever.

private:
    //! get the value
    void get_(NITF_DATA* outval, nitf_ConvType vtype, size_t length) const;
    void get(NITF_DATA* outval, nitf::ConvType vtype, size_t length) const
    {
        get_(outval, vtype, length);
    }

    //! set the value
    void set(NITF_DATA* inval, size_t length);

    mutable nitf_Error error{};
};

}
#endif

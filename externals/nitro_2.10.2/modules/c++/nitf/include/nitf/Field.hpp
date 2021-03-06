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

#include <nitf/Field.h>

#include <nitf/coda-oss.hpp>
#include <nitf/System.hpp>
#include <nitf/HashTable.hpp>
#include <nitf/List.hpp>
#include <nitf/DateTime.hpp>
#include <nitf/Object.hpp>
#include <nitf/NITFException.hpp>

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
class Field /*final*/ : public nitf::Object<nitf_Field> // no "final", SWIG doesn't like it
{
    void setU_(uint32_t data)
    {
        const NITF_BOOL x = nitf_Field_setUint32(getNativeOrThrow(), data, &error);
        if (!x)
            throw nitf::NITFException(&error);
    }
    void set_(int32_t data)
    {
        if (!nitf_Field_setInt32(getNativeOrThrow(), data, &error))
            throw nitf::NITFException(&error);
    }
    void set_(double data)
    {
        if (!nitf_Field_setReal(getNativeOrThrow(),
            "f", false, data, &error))
            throw nitf::NITFException(&error);
    }

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
        setNative(x.getNative());
    }

    //! Assignment Operator
    Field & operator=(const Field & x)
    {
        if (&x != this)
            setNative(x.getNative());
        return *this;
    }

    //! Set native object
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
    void set(uint64_t data)
    {
        const NITF_BOOL x = nitf_Field_setUint64(getNativeOrThrow(), data, &error);
        if (!x)
            throw nitf::NITFException(&error);
    }

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
    void set(int64_t data)
    {
        if (!nitf_Field_setInt64(getNativeOrThrow(), data, &error))
            throw nitf::NITFException(&error);
    }

    void set(float data)
    {
        set_(data);
    }
    void set(double data)
    {
        set_(data);
    }

    void set(const char * data)
    {
        const NITF_BOOL x = nitf_Field_setString(getNativeOrThrow(), data, &error);
        if (!x)
            throw nitf::NITFException(&error);
    }
    void set(const std::string& data)
    {
        set(data.c_str());
    }

    void set(const nitf::DateTime& dateTime,
             const std::string& format = NITF_DATE_FORMAT_21)
    {
        const NITF_BOOL x = nitf_Field_setDateTime(getNativeOrThrow(),
                dateTime.getNative(), format.c_str(), &error);
        if (!x)
            throw nitf::NITFException(&error);
    }

    nitf::DateTime asDateTime(const std::string& format = NITF_DATE_FORMAT_21)
    {
        nitf_DateTime* const dateTime =
                nitf_Field_asDateTime(getNativeOrThrow(), format.c_str(),
                                      &error);
        if (!dateTime)
        {
            throw nitf::NITFException(&error);
        }
        return nitf::DateTime(dateTime);
    }

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
    void resize(size_t length)
    {
        nitf_Field *field = getNativeOrThrow();
        const NITF_BOOL resizable = field->resizable;
        field->resizable = 1;

        if (!nitf_Field_resizeField(field, length, &error))
            throw nitf::NITFException(&error);
        field->resizable = resizable;
    }

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
        return std::string(getNativeOrThrow()->raw,
            getNativeOrThrow()->length);
    }
    std::string toString() const
    {
        return *this;
    }
    std::string toTrimString() const
    {
        std::string retval = *this; // implicitly converted to std::string
        str::trim(retval);
        return retval;
    }

    Field() = delete; // does not make sense to construct a Field from scratch
    operator char* () const = delete; // Don't allow this cast ever.

private:
    //! get the value
    void get(NITF_DATA* outval, nitf::ConvType vtype, size_t length) const
    {
        nitf_Error e;
        const NITF_BOOL x = nitf_Field_get(getNativeOrThrow(), outval, vtype, length, &e);
        if (!x)
            throw nitf::NITFException(&e);
    }

    //! set the value
    void set(NITF_DATA* inval, size_t length)
    {
        const NITF_BOOL x = nitf_Field_setRawData(getNativeOrThrow(), inval, length, &error);
        if (!x)
            throw nitf::NITFException(&error);
    }

    nitf_Error error{};
};

}
#endif

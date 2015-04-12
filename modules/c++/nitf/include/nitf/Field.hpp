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

#ifndef __NITF_FIELD_HPP__
#define __NITF_FIELD_HPP__

#include "nitf/Field.h"
#include "nitf/System.hpp"
#include "nitf/NITFException.hpp"
#include "nitf/Object.hpp"
#include "nitf/HashTable.hpp"
#include "nitf/List.hpp"
#include "nitf/DateTime.hpp"
#include <import/str.h>
#include <string>

/*!
 *  \file Field.hpp
 *  \brief  Contains wrapper implementation for Field
 */

namespace nitf
{
/*!
 *  \class Field
 *  \brief  The C++ wrapper for the nitf_Field
 *
 *  The Field is a generic type object that allows storage
 *  and casting of data amongst disparate data types.
 */
class Field : public nitf::Object<nitf_Field>
{

public:
enum FieldType
    {
        BCS_A = NITF_BCS_A,
        BCS_N = NITF_BCS_N,
        BINARY = NITF_BINARY
    };

    Field & operator=(const char * value) throw(nitf::NITFException)
    {
        set(value);
        return *this;
    }

    Field & operator=(std::string value)
    {
        set(value);
        return *this;
    }

    Field & operator=(nitf::Int8 value)
    {
        set(value);
        return *this;
    }

    Field & operator=(nitf::Int16 value)
    {
        set(value);
        return *this;
    }

    Field & operator=(nitf::Int32 value)
    {
        set(value);
        return *this;
    }

    Field & operator=(nitf::Int64 value)
    {
        set(value);
        return *this;
    }

    Field & operator=(nitf::Uint8 value)
    {
        set(value);
        return *this;
    }

    Field & operator=(nitf::Uint16 value)
    {
        set(value);
        return *this;
    }

    Field & operator=(nitf::Uint32 value)
    {
        set(value);
        return *this;
    }

    Field & operator=(nitf::Uint64 value)
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
        setNative((nitf_Field*)x);
        getNativeOrThrow();
    }

    Field & operator=(NITF_DATA * x)
    {
        setNative((nitf_Field*)x);
        getNativeOrThrow();
        return *this;
    }

    //! Destructor
    ~Field() {}

    void set(nitf::Uint8 data)
    {
        if (!nitf_Field_setUint32(getNativeOrThrow(),
                                  nitf::Uint32(data), &error))
            throw nitf::NITFException(&error);
    }

    void set(nitf::Uint16 data)
    {
        if (!nitf_Field_setUint32(getNativeOrThrow(),
                                  nitf::Uint32(data), &error))
            throw nitf::NITFException(&error);
    }

    void set(nitf::Uint32 data) throw(nitf::NITFException)
    {
        NITF_BOOL x = nitf_Field_setUint32(getNativeOrThrow(), data, &error);
        if (!x)
            throw nitf::NITFException(&error);
    }

    void set(nitf::Uint64 data) throw(nitf::NITFException)
    {
        NITF_BOOL x = nitf_Field_setUint64(getNativeOrThrow(), data, &error);
        if (!x)
            throw nitf::NITFException(&error);
    }

    void set(nitf::Int8 data)
    {
        if (!nitf_Field_setInt32(getNativeOrThrow(),
                                 nitf::Uint32(data), &error))
            throw nitf::NITFException(&error);
    }

    void set(nitf::Int16 data)
    {
        if (!nitf_Field_setInt32(getNativeOrThrow(),
                                 nitf::Uint32(data), &error))
            throw nitf::NITFException(&error);
    }

    void set(nitf::Int32 data)
    {
        if (!nitf_Field_setInt32(getNativeOrThrow(),
                                 nitf::Uint32(data), &error))
            throw nitf::NITFException(&error);
    }

    void set(nitf::Int64 data)
    {
        if (!nitf_Field_setInt64(getNativeOrThrow(),
                                 nitf::Uint32(data), &error))
            throw nitf::NITFException(&error);
    }

    void set(float data)
    {
        if (!nitf_Field_setReal(getNativeOrThrow(),
                                "f", false, double(data), &error))
            throw nitf::NITFException(&error);
    }

    void set(double data)
    {
        if (!nitf_Field_setReal(getNativeOrThrow(),
                                "f", false, data, &error))
            throw nitf::NITFException(&error);
    }

    void set(const char * data) throw(nitf::NITFException)
    {
        NITF_BOOL x = nitf_Field_setString(getNativeOrThrow(), (char*)data, &error);
        if (!x)
            throw nitf::NITFException(&error);
    }

    void set(std::string data) throw(nitf::NITFException)
    {
        NITF_BOOL x = nitf_Field_setString(getNativeOrThrow(), (char*)data.c_str(), &error);
        if (!x)
            throw nitf::NITFException(&error);
    }

    void set(nitf::DateTime dateTime, std::string format = NITF_DATE_FORMAT_21) throw(nitf::NITFException)
    {
        NITF_BOOL x = nitf_Field_setDateTime(getNativeOrThrow(),
                dateTime.getNative(), (char*)format.c_str(), &error);
        if (!x)
            throw nitf::NITFException(&error);
    }

    nitf::DateTime asDateTime(std::string format = NITF_DATE_FORMAT_21) throw(nitf::NITFException)
    {
        nitf_DateTime *x = nitf_Field_asDateTime(getNativeOrThrow(),
                (char*)format.c_str(), &error);
        if (!x)
            throw nitf::NITFException(&error);
        nitf::DateTime d(x);
        d.setManaged(false);
        return d;
    }

    //! Get the type
    FieldType getType() const
    {
        return (FieldType)getNativeOrThrow()->type;
    }

    //! Set the type
    void setType(FieldType type)
    {
        getNativeOrThrow()->type = (nitf_FieldType)type;
    }

    //! Get the data
    char * getRawData() const
    {
        return getNativeOrThrow()->raw;
    }
    //! Set the data
    void setRawData(char * raw, size_t length) throw(nitf::NITFException)
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
        NITF_BOOL resizable = field->resizable;
        field->resizable = 1;

        if (!nitf_Field_resizeField(field, length, &error))
            throw nitf::NITFException(&error);
        field->resizable = resizable;
    }

    operator nitf::Uint8() const
    {
        nitf::Uint8 data;
        get(&data, NITF_CONV_UINT, sizeof(nitf::Uint8));
        return data;
    }

    operator nitf::Uint16() const
    {
        nitf::Uint16 data;
        get(&data, NITF_CONV_UINT, sizeof(nitf::Uint16));
        return data;
    }

    operator nitf::Uint32() const
    {
        nitf::Uint32 data;
        get(&data, NITF_CONV_UINT, sizeof(nitf::Uint32));
        return data;
    }

    operator nitf::Uint64() const
    {
        nitf::Uint64 data;
        get(&data, NITF_CONV_UINT, sizeof(nitf::Uint64));
        return data;
    }

    operator nitf::Int8() const
    {
        nitf::Int8 data;
        get(&data, NITF_CONV_INT, sizeof(nitf::Int8));
        return data;
    }

    operator nitf::Int16() const
    {
        nitf::Int16 data;
        get(&data, NITF_CONV_INT, sizeof(nitf::Int16));
        return data;
    }

    operator nitf::Int32() const
    {
        nitf::Int32 data;
        get(&data, NITF_CONV_INT, sizeof(nitf::Int32));
        return data;
    }

    operator nitf::Int64() const
    {
        nitf::Int64 data;
        get(&data, NITF_CONV_INT, sizeof(nitf::Int64));
        return data;
    }

    operator float() const
    {
        float data;
        get(&data, NITF_CONV_REAL, sizeof(float));
        return data;
    }

    operator double() const
    {
        double data;
        get(&data, NITF_CONV_REAL, sizeof(double));
        return data;
    }

    operator std::string() const
    {
        return toString();
    }

    std::string toString() const
    {
        return std::string(getNativeOrThrow()->raw,
                           getNativeOrThrow()->length );
    }

private:
    Field(){} //private -- does not make sense to construct a Field from scratch

    //! get the value
    void get(NITF_DATA* outval, nitf::ConvType vtype, size_t length) const
    {
        nitf_Error e;
        NITF_BOOL x = nitf_Field_get(getNativeOrThrow(), outval, vtype, length, &e);
        if (!x)
            throw nitf::NITFException(&e);
    }

    //! set the value
    void set(NITF_DATA* inval, size_t length) throw(nitf::NITFException)
    {
        NITF_BOOL x = nitf_Field_setRawData(getNativeOrThrow(), inval, length, &error);
        if (!x)
            throw nitf::NITFException(&error);
    }

    nitf_Error error;

    operator char*() const; // Don't allow this cast ever.
};

}

#endif

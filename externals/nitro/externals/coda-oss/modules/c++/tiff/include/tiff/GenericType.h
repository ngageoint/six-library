/* =========================================================================
 * This file is part of tiff-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * tiff-c++ is free software; you can redistribute it and/or modify
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

#ifndef __TIFF_GENERIC_TYPE_H__
#define __TIFF_GENERIC_TYPE_H__

#include <string>
#include <import/io.h>

#include "tiff/Common.h"

namespace tiff
{

/**
 *********************************************************************
 * @class TypeInterface
 * @brief The interface class that all TIFF types inherit from.
 *********************************************************************/
class TypeInterface : public io::Serializable
{
public:
    //! Default constructor
    TypeInterface()
    {
    }

    //! Deconstructor
    virtual ~TypeInterface()
    {
    }

    /**
     *****************************************************************
     * Returns the data of the TIFF type in byte format.
     *
     * @return
     *   the data in byte format
     *****************************************************************/
    virtual unsigned char *data() const = 0;

    /**
     *****************************************************************
     * Returns the size of the TIFF type.
     *
     * @return
     *   the size of the TIFF type
     *****************************************************************/
    virtual unsigned short size() const = 0;

    /**
     *****************************************************************
     * Converts the data to a string representation of it.
     *
     * @return
     *   the string representation of the data
     *****************************************************************/
    virtual std::string toString() const = 0;
};

/**
 *********************************************************************
 * @class GenericType
 * @brief A templated TIFF type for abstracting data types.
 *
 * Stores data as the templatized type.  All TIFF types can then
 * be treated as a single type with a different data member.  Contains
 * functions for reading and writing the data to a stream, converting
 * the data to a string for printing, and retrieving the data in
 * byte form.
 *********************************************************************/
template <typename Data_T, typename Strategy_T = tiff::PrintStrategy> class GenericType :
    public TypeInterface
{
public:
    //! Default constructor
    GenericType() :
        mData(0)
    {
    }

    explicit GenericType(std::string s) :
        mData(str::toType<Data_T>(s))
    {
    }

    /**
     *****************************************************************
     * Constructor, initializes the object with the specified data.
     *
     * @param data
     *   the data to initialize the object with
     *****************************************************************/
    GenericType(const unsigned char *data)
    {
        const void* const pData = data;
        mData = *(static_cast<const Data_T*>(pData));
    }

    //! Deconstructor
    virtual ~GenericType()
    {
    }

    /**
     *****************************************************************
     * Writes the data member to the specified stream.
     *
     * @param output
     *   the output stream to write the member to
     *****************************************************************/
    virtual void serialize(io::OutputStream& output) override
    {
        output.write((char *)&mData, sizeof(Data_T));
    }

    /**
     *****************************************************************
     * Reads the data member from the specified stream.
     *
     * @param input
     *   the input stream to read the member from
     *****************************************************************/
    virtual void deserialize(io::InputStream& input) override
    {
        input.read((char *)&mData, sizeof(Data_T));
    }

    virtual unsigned char *data() const override
    {
        return (unsigned char *)&mData;
    }

    virtual unsigned short size() const override
    {
        return sizeof(mData);
    }

    virtual std::string toString() const override
    {
        return Strategy_T::toString(mData);
    }

    /**
     *****************************************************************
     * Casting operator.  Returns the data in its native type.
     *
     * @return
     *   the member data in the native type
     *****************************************************************/
    operator Data_T() const
    {
        return mData;
    }

protected:

    //! The member data
    Data_T mData;
};

} // End namespace.

#endif // __TIFF_GENERIC_TYPE_H__

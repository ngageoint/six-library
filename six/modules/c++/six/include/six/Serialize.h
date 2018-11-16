/* =========================================================================
 * This file is part of six-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2018, MDA Information Systems LLC
 *
 * six-c++ is free software; you can redistribute it and/or modify
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
#ifndef __SIX_SERIALIZE_H__
#define __SIX_SERIALIZE_H__

#include <vector>
#include <algorithm>
#include <iterator>

#include <sys/Conf.h>

namespace six
{
/*!
 * \struct Serializer
 * \tparam T Scalar type
 * \brief Implements serialization and deserialization for scalar
 *  types
 */
template<typename T>
struct Serializer
{
    /*!
     * Serialize a value into a byte buffer.
     * \param val The value to serialize.
     * \param swapBytes Should byte-swapping be applied?
     * \param[out] values The serialized data.
     */
    static void serializeImpl(const T& val,
                              bool swapBytes,
                              std::vector<sys::byte>& buffer)
    {
        const size_t length = sizeof(T);
        const sys::byte* data = reinterpret_cast<const sys::byte*>(&val);

        if (swapBytes)
        {
            const size_t prevLength = buffer.size();
            buffer.resize(prevLength + length);
            sys::byteSwap(data,
                          static_cast<unsigned short>(length),
                          1,
                          &buffer[prevLength]);
        }
        else
        {
            std::copy(data, data + length, std::back_inserter(buffer));
        }
    }

    /*!
     * Deserialize a byte array into a scalar value.
     * \param buffer The data to deserialize. Pointer is incremented
     *  by sizeof(T) after calling this function.
     * \param swapBytes Should byte-swapping be applied?
     * \param[out] val The value to deserialize into.
     */
    static void deserializeImpl(const sys::byte*& buffer, bool swapBytes, T& val)
    {
        const size_t length = sizeof(T);
        sys::byte* data = reinterpret_cast<sys::byte*>(&val);
        std::copy(buffer, buffer + length, data);
        if (swapBytes)
        {
            sys::byteSwap(data, static_cast<unsigned short>(length), 1);
        }

        buffer += length;
    }
};

/*!
 * \struct Serializer
 * \tparam T Scalar type
 * \brief Implements serialization and deserialization for vectors of
 *  scalar types
 */
template<typename T>
struct Serializer<std::vector<T> >
{
    /*!
     * Serialize a vector of values into a byte buffer.
     * \param val The vector of values to serialize.
     * \param swapBytes Should byte-swapping be applied?
     * \param[out] values The serialized data.
     */
    static void serializeImpl(const std::vector<T>& val,
                              bool swapBytes,
                              std::vector<sys::byte>& buffer)
    {
        const size_t length = val.size();

        Serializer<size_t>::serializeImpl(length, swapBytes, buffer);
        for (size_t ii = 0; ii < length; ++ii)
        {
            Serializer<T>::serializeImpl(val[ii], swapBytes, buffer);
        }
    }

    /*!
     * Deserialize a byte array into a vector of values
     * \param buffer The data to deserialize. Pointer is incremented
     *  by sizeof(size_t) + vector_length * sizeof(T) after calling
     *  this function.
     * \param swapBytes Should byte-swapping be applied?
     * \param[out] val The vector of values to deserialize into.
     */
    static void deserializeImpl(const sys::byte*& buffer,
                                bool swapBytes,
                                std::vector<T>& val)
    {
        const size_t currentVectorLength = val.size();
        size_t length;
        Serializer<size_t>::deserializeImpl(buffer, swapBytes, length);
        val.resize(currentVectorLength + length);

        for (size_t ii = 0; ii < length; ++ii)
        {
            Serializer<T>::deserializeImpl(buffer, swapBytes,
                val[currentVectorLength + ii]);
        }
    }
};

/*!
 * Function interface to serialize.
 * \tparam T Data type to serialize. Argument determines which
 *  Serializer functor's implementation to use.
 * \param val Value(s) to serialize
 * \param swapBytes Should the bytes be swapped?
 * \param[out] buffer Byte array to serialize into
 */
template<typename T>
void serialize(const T& val, bool swapBytes, std::vector<sys::byte>& buffer)
{
    Serializer<T>::serializeImpl(val, swapBytes, buffer);
}

/*!
 * Function interface to deserialize
 * \tparam T Data type to deserialize. Argument determines which
 *  Serializer functor's implementation to use.
 * \param buffer Address from which to begin deserialization. Pointer
 *  is incremented by the total serialized storage size of T after
 *  this calling this function.
 * \param swapBytes Should bytes be swapped?
 * \param[out] Value(s) to deserialize into.
 */
template<typename T>
void deserialize(const sys::byte*& buffer, bool swapBytes, T& val)
{
    Serializer<T>::deserializeImpl(buffer, swapBytes, val);
}
}
#endif

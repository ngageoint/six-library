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
#pragma once

#include <vector>
#include <algorithm>
#include <iterator>

#include <sys/Conf.h>
#include <nitf/coda-oss.hpp>

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
    template<typename U>
    static void serializeImpl_(const T& val,
                              bool swapBytes,
                              std::vector<U>& buffer)
    {
        const size_t length = sizeof(T);
        auto data = reinterpret_cast<typename std::vector<U>::const_pointer>(&val);

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
    static void serializeImpl(const T& val,
                              bool swapBytes,
                              std::vector<sys::byte>& buffer)
    {
        serializeImpl_(val, swapBytes, buffer);
    }
    static void serializeImpl(const T& val,
                              bool swapBytes,
                              std::vector<std::byte>& buffer)
    {
        serializeImpl_(val, swapBytes, buffer);
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
        auto data = reinterpret_cast<sys::byte*>(&val);
        std::copy(buffer, buffer + length, data);
        if (swapBytes)
        {
            sys::byteSwap(data, static_cast<unsigned short>(length), 1);
        }

        buffer += length;
    }
    static void deserializeImpl(const std::byte*& buffer, bool swapBytes, T& val)
    {
        auto& buffer_ = reinterpret_cast<const sys::byte*&>(buffer);
        deserializeImpl(buffer_, swapBytes, val);
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
    template<typename U>
    static void serializeImpl_(const std::vector<T>& val,
                              bool swapBytes,
                              std::vector<U>& buffer)
    {
        const size_t length = val.size();

        Serializer<size_t>::serializeImpl(length, swapBytes, buffer);
        for (size_t ii = 0; ii < length; ++ii)
        {
            Serializer<T>::serializeImpl(val[ii], swapBytes, buffer);
        }
    }
    static void serializeImpl(const std::vector<T>& val,
                              bool swapBytes,
                              std::vector<sys::byte>& buffer)
    {
        serializeImpl_(val, swapBytes, buffer);
    }
    static void serializeImpl(const std::vector<T>& val,
                              bool swapBytes,
                              std::vector<std::byte>& buffer)
    {
        serializeImpl_(val, swapBytes, buffer);
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
    static void deserializeImpl(const std::byte*& buffer,
                                bool swapBytes,
                                std::vector<T>& val)
    {
        auto& buffer_ = reinterpret_cast<const sys::byte*&>(buffer);
        deserializeImpl(buffer_, swapBytes, val);
    }
};

/*!
 * \struct Serializer
 * \brief Implements serialization and deserialization for std::strings
 */
template <>
struct Serializer<std::string>
{
    /*!
     * Serialize a std::string into the byte buffer.
     *
     * \param val The std::string to serialize.
     * \param swapBytes Should byte-swapping be applied?
     * \param[out] buffer The serialized data.
     */
    template<typename T>
    static void serializeImpl_(const std::string& val,
                              bool swapBytes,
                              std::vector<T>& buffer)
    {
        const size_t length = val.size();
        Serializer<size_t>::serializeImpl(length, swapBytes, buffer);
        const auto begin = reinterpret_cast<typename std::vector<T>::const_pointer>(val.c_str());
        const auto end = begin + val.size();
        std::copy(begin, end, std::back_inserter(buffer));
    }
    static void serializeImpl(const std::string& val,
                              bool swapBytes,
                              std::vector<sys::byte>& buffer)
    {
        serializeImpl_(val, swapBytes, buffer);
    }
    static void serializeImpl(const std::string& val,
                              bool swapBytes,
                              std::vector<std::byte>& buffer)
    {
        serializeImpl_(val, swapBytes, buffer);
    }

    /*!
     * Deserialize a byte array into a std::string.
     *
     * \param buffer The data to deserialize. Pointer is incremented
     *        by sizeof(size_t) + string_length.
     * \param swapBytes Should byte-swapping be applied?
     * \param[out] val The std::string to deserialize into.
     */
    static void deserializeImpl(const sys::byte*& buffer,
                                bool swapBytes,
                                std::string& val)
    {
        size_t length;
        Serializer<size_t>::deserializeImpl(buffer, swapBytes, length);

        const char* charPtr = reinterpret_cast<const char*>(buffer);
        val.assign(charPtr, charPtr + length);
        buffer += length;
    }
    static void deserializeImpl(const std::byte*& buffer,
                                bool swapBytes,
                                std::string& val)
    {
        auto& buffer_ = reinterpret_cast<const sys::byte*&>(buffer);
        deserializeImpl(buffer_, swapBytes, val);
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
template<typename T, typename U>
void serialize_(const T& val, bool swapBytes, std::vector<U>& buffer)
{
    Serializer<T>::serializeImpl(val, swapBytes, buffer);
}
template<typename T>
void serialize(const T& val, bool swapBytes, std::vector<sys::byte>& buffer)
{
    serialize_(val, swapBytes, buffer);
}
template<typename T>
void serialize(const T& val, bool swapBytes, std::vector<std::byte>& buffer)
{
    serialize_(val, swapBytes, buffer);
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
template<typename T>
void deserialize(const std::byte*& buffer, bool swapBytes, T& val)
{
    auto& buffer_ = reinterpret_cast<const sys::byte*&>(buffer);
    deserialize(buffer_, swapBytes, val);
}
}
#endif

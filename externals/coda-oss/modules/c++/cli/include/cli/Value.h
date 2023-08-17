/* =========================================================================
 * This file is part of cli-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * cli-c++ is free software; you can redistribute it and/or modify
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

#pragma once
#ifndef CODA_OSS_cli_Value_h_INCLUDED_
#define CODA_OSS_cli_Value_h_INCLUDED_

#include <string>
#include <iterator>

#include <import/str.h>
#include "sys/Conf.h"
#include "config/Exports.h"

namespace cli
{

/**
 * The Value class provides access to one or more actual values.
 * It provides index-based access to parameters.
 */
class CODA_OSS_API Value
{
public:
    Value()
    {
    }

    template<typename T>
    explicit Value(std::vector<T> value)
    {
        setContainer<T>(value);
    }

    template<typename T>
    Value(T value)
    {
        set<T>(value);
    }

    template<typename T>
    Value(T* value, size_t size, bool own = false)
    {
        set<T>(value, size, own);
    }

    ~Value()
    {
        cleanup();
    }

    template<typename T>
    void set(T value)
    {
        cleanup();
        mValues.push_back(str::toString(value));
    }

    template<typename T>
    void set(T* value, size_t size, bool own = false)
    {
        cleanup();
        mValues.reserve(size);
        for (size_t i = 0; i < size; ++i)
            add(value[i]);
        if (own)
            delete[] value;
    }

    template<typename T>
    void setContainer(const std::vector<T>& c)
    {
        cleanup();
        mValues.reserve(c.size());
        for (size_t i = 0; i < c.size(); ++i)
            add(c[i]);
    }

    template<typename T>
    T operator [](size_t index) const
    {
        return at<T>(index);
    }

    template<typename T>
    T at(size_t index = 0) const
    {
        if (index >= mValues.size())
            throw except::IndexOutOfRangeException(
                                                   Ctxt(
                                                        FmtX(
                                                             "Invalid index: %d",
                                                             index)));
        return str::toType<T>(mValues[index]);
    }

    template<typename T>
    T get(size_t index = 0) const
    {
        return at<T>(index);
    }

    template<typename T>
    void add(T val)
    {
        mValues.push_back(str::toString(val));
    }

    /**
     * Returns the size of the value.
     */
    size_t size() const
    {
        return mValues.size();
    }

    Value* clone() const
    {
        return new Value(mValues);
    }

    std::string toString() const
    {
        if (mValues.size() == 1)
            return mValues[0];
        std::ostringstream s;
        s << "[" << str::join(mValues, ", ") << "]";
        return s.str();
    }

    typedef std::vector<std::string>::iterator iterator;
    typedef std::vector<std::string>::const_iterator const_iterator;

    iterator begin() { return mValues.begin(); }
    const_iterator begin() const { return mValues.begin(); }
    iterator end() { return mValues.end(); }
    const_iterator end() const { return mValues.end(); }

protected:
    std::vector<std::string> mValues;

    void cleanup()
    {
        mValues.clear();
    }
};

}
#endif  // CODA_OSS_cli_Value_h_INCLUDED_

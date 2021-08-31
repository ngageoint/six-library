/* =========================================================================
 * This file is part of mem-c++
 * =========================================================================
 *
 * (C) Copyright 2013 - 2014, MDA Information Systems LLC
 *
 * mem-c++ is free software; you can redistribute it and/or modify
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

#ifndef __MEM_VECTOR_OF_POINTERS_H__
#define __MEM_VECTOR_OF_POINTERS_H__
#pragma once

#include <cstddef>
#include <vector>
#include <memory>

#include "mem/SharedPtr.h"

namespace mem
{
/*!
 *  \class VectorOfPointers
 *  \brief This class provides safe cleanup for vectors of pointers
 */
template <typename T>
struct VectorOfPointers
{
    VectorOfPointers() = default;

    ~VectorOfPointers()
    {
        clear();
    }

    void clear()
    {
        erase(begin(), end());
        mValues.clear();
    }

    const std::vector<T*>& get() const
    {
        return mValues;
    }

    size_t size() const
    {
        return mValues.size();
    }

    bool empty() const
    {
        return mValues.empty();
    }

    T* operator[](std::ptrdiff_t idx) const
    {
        return mValues[idx];
    }

    T* back() const
    {
        return mValues.back();
    }

    template <typename OtherT>
        void push_back(OtherT* value)
    {
        std::unique_ptr<OtherT> scopedValue(value);
        push_back(std::move(scopedValue));
    }

    template <typename OtherT>
    void push_back(std::unique_ptr<OtherT>&& value)
    {
        mValues.resize(mValues.size() + 1);
        mValues.back() = value.release();
    }
    #if !CODA_OSS_cpp17  // std::auto_ptr removed in C++17
    template <typename OtherT>
        void push_back(mem::auto_ptr<OtherT> value)
    {
        std::unique_ptr<OtherT> scopedValue(value.release());
        push_back(std::move(scopedValue));
    }
    #endif

    typedef typename std::vector<T*>::iterator iterator;
    typedef typename std::vector<T*>::const_iterator const_iterator;

    iterator begin() { return mValues.begin(); }
    const_iterator begin() const { return mValues.begin(); }
    iterator end() { return mValues.end(); }
    const_iterator end() const { return mValues.end(); }

    iterator erase(iterator pos)
    {
        delete *pos;
        return mValues.erase(pos);
    }

    iterator erase(iterator first, iterator last)
    {
        iterator iter = first;
        while (iter != last)
        {
            delete *iter;
            ++iter;
        }

        return mValues.erase(first, last);
    }

    VectorOfPointers(const VectorOfPointers&) = delete;
    VectorOfPointers& operator=(const VectorOfPointers&) = delete;

private:
    std::vector<T*> mValues;
};

template <typename T>
    struct VectorOfSharedPointers
{
    VectorOfSharedPointers() = default;
    ~VectorOfSharedPointers() = default;
    VectorOfSharedPointers(const VectorOfSharedPointers&) = default;
    VectorOfSharedPointers(VectorOfSharedPointers&&) = default;
    VectorOfSharedPointers& operator=(const VectorOfSharedPointers&) = default;
    VectorOfSharedPointers& operator=(VectorOfSharedPointers&&) = default;

    VectorOfSharedPointers(const std::vector<std::shared_ptr<T>>& values) : mValues(values) { }
    VectorOfSharedPointers& operator=(const std::vector<std::shared_ptr<T>>& values)
    {
        mValues = values;
        return *this;
    }

    operator std::vector<std::shared_ptr<T>>&() { return mValues; }
    operator const std::vector<std::shared_ptr<T>>&() const { return mValues; }

    void clear()
    {
        mValues.clear();
    }

    std::vector<T*> get() const
    {
        std::vector<T*> values(size());
        for (size_t ii = 0; ii < size(); ++ii)
        {
            values[ii] = mValues[ii].get();
        }
        return values;
    }

    size_t size() const
    {
        return mValues.size();
    }

    bool empty() const
    {
        return mValues.empty();
    }

    const std::shared_ptr<T>& operator[](std::ptrdiff_t idx) const
    {
        return mValues[idx];
    }
    std::shared_ptr<T>& operator[](std::ptrdiff_t idx)
    {
        return mValues[idx];
    }

    template <typename OtherT>
        void push_back(OtherT* value)
    {
        std::unique_ptr<OtherT> scopedValue(value);
        push_back(std::move(scopedValue));
    }

    template <typename OtherT>
    void push_back(std::unique_ptr<OtherT>&& value)
    {
        mValues.resize(mValues.size() + 1);
        mValues.back().reset(value.release());
    }
    #if !CODA_OSS_cpp17  // std::auto_ptr removed in C++17
    template <typename OtherT>
        void push_back(mem::auto_ptr<OtherT> value)
    {
        std::unique_ptr<OtherT> scopedValue(value.release());
        push_back(std::move(scopedValue));
    }
    #endif

    template <typename OtherT>
        void push_back(std::shared_ptr<OtherT> value)
    {
        mValues.push_back(value);
    }

    typedef typename std::vector<std::shared_ptr<T>>::iterator iterator;
    typedef typename std::vector<std::shared_ptr<T> >::const_iterator const_iterator;

    iterator begin() { return mValues.begin(); }
    const_iterator begin() const { return mValues.begin(); }
    iterator end() { return mValues.end(); }
    const_iterator end() const { return mValues.end(); }

    iterator erase(iterator pos)
    {
        return mValues.erase(pos);
    }

    iterator erase(iterator first, iterator last)
    {
        return mValues.erase(first, last);
    }

private:
    std::vector<std::shared_ptr<T>> mValues;
};
}

#endif

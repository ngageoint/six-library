/* =========================================================================
 * This file is part of mem-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2018, MDA Information Systems LLC
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

#ifndef __MEM_SHARED_PTR_CPP_11_H__
#define __MEM_SHARED_PTR_CPP_11_H__

#include <memory>

namespace mem
{
/*!
 *  \class SharedPtr
 *  \brief This is a derived class of std::shared_ptr. The purpose of this
 *         class is to provide backwards compatibility in systems that do
 *         not have C++11 support.
 *         Because this inherits from std::shared_ptr it can be directly
 *         passed into interfaces requiring std::shared_ptr or legacy
 *         interfaces.
 *         For future work, prefer std::shared_ptr when possible.
 *
 *         WARNING: std::shared_ptr<T>* foo = new SharedPtr<T> will leak!
 */
template <class T>
class SharedPtr : public std::shared_ptr<T>
{
public:
    SharedPtr() : std::shared_ptr<T>()
    {
    }

    using std::shared_ptr<T>::shared_ptr;

    using std::shared_ptr<T>::reset;

    // The base class only handles unique_ptr<T>&&
    explicit SharedPtr(std::unique_ptr<T>&& ptr) :
        std::shared_ptr<T>(ptr.release())
    {
    }

    // The base class only handles unique_ptr<T>&&
    template <typename OtherT>
    explicit SharedPtr(std::unique_ptr<OtherT>&& ptr) :
        std::shared_ptr<T>(ptr.release())
    {
    }

    void reset(std::unique_ptr<T>&& scopedPtr)
    {
        std::shared_ptr<T>::reset(scopedPtr.release());
    }

    // Implemented to support the legacy SharedPtr. Prefer use_count.
    long getCount() const
    {
        return std::shared_ptr<T>::use_count();
    }
};
}

#endif

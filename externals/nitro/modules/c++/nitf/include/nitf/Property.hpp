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

#ifndef NITRO_Property_hpp_INCLUDED_
#define NITRO_Property_hpp_INCLUDED_
#pragma once

#include <functional>

#include <nitf/Field.hpp>

 /*!
 *  \file Field.hpp
 *  \brief  Contains wrapper implementation for Property
 */

namespace nitf
{
    // https://stackoverflow.com/a/61433507/8877
    template<class T>
    class PropertyGet /*final*/
    {
        std::function<T(void)> get_;
    public:
        PropertyGet(const std::function<T(void)>& get) : get_(get) {}

        T get() const { return get_(); }
        operator T() const { return get(); }
    };

    template<class T>
    class Property /*final*/
    {
        PropertyGet<T> get_;
        std::function<void(const T&)> set_;
    public:
        Property(const std::function<T(void)>& get, const std::function<void(const T&)>& set) : get_(get), set_(set) { }

        T get() const { return get_.get()(); }
        operator T() const { return get(); }

        void set(const T& t) { set_(t); }
        void operator=(const T& t) { set(t); }
    };
}
#endif // NITRO_Property_hpp_INCLUDED_

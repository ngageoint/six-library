/* =========================================================================
 * This file is part of sys-c++
 * =========================================================================
 *
 * (C) Copyright 2020, Maxar Technologies, Inc.
 *
 * sys-c++ is free software; you can redistribute it and/or modify
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
 * License along with this program; If not, http://www.gnu.org/licenses/.
 *
 */
#ifndef CODA_OSS_sys_Optional_h_INCLUDED_
#define CODA_OSS_sys_Optional_h_INCLUDED_
#pragma once

#include <assert.h>

#include <stdexcept>
#include <utility>

#include "CPlusPlus.h"

// Simple version of std::Optional since that doesn't exist until C++17.
//
// This doesn't even TRY to match the actual C++17 specification,
// it only tries to minimize code changes.
//
// http://en.cppreference.com/w/cpp/utility/Optional
namespace sys
{
namespace details
{
    inline void throw_bad_optional_access()
    {
        throw std::logic_error("No value for Optional<>.");  // TODO: std::bad_optional_access
    }
}

template <typename T>
class Optional  // no "final" as SWIG doesn't like it
{
    T value_;
    bool has_value_ = false;

    inline void check_has_value() const
    {
        if (!has_value())
        {
            details::throw_bad_optional_access();
        }
    }

public:
    using value_type = T;

    #if defined(_MSC_VER) && _PREFAST_ // Visual Studio /analyze
    __pragma(warning(push))
    __pragma(warning(disable: 26495)) // Variable '...' is uninitialized. Always initialize a member variable(type.6).
    #endif
    Optional() noexcept
    {
    }
    #if defined(_MSC_VER) && _PREFAST_
    __pragma(warning(pop))
    #endif
    Optional(const value_type& v) : value_(v), has_value_(true)
    {
    }
    #if defined(_MSC_VER) && _PREFAST_ // Visual Studio /analyze
    __pragma(warning(push))
    __pragma(warning(disable: 26495)) // Variable '...' is uninitialized. Always initialize a member variable(type.6).
    #endif
    Optional(const Optional& other) : has_value_(other.has_value_)
    {
        if (has_value())
        {
            value_ = other.value_;
        }
    }
    #if defined(_MSC_VER) && _PREFAST_
    __pragma(warning(pop))
    #endif

    template <typename... Args>  // https://en.cppreference.com/w/cpp/utility/Optional/emplace
    T& emplace(Args&&... args)
    {
        value_ = value_type(std::forward<Args>(args)...);
        has_value_ = true;
        return value_;
    }

    template<typename U = T> // https://en.cppreference.com/w/cpp/utility/optional/operator%3D
    Optional& operator=(U&& value)
    {
        value_ = std::forward<U>(value);
        has_value_ = true;
        return *this;
    }

    bool has_value() const noexcept
    {
        return has_value_;
    }
    explicit operator bool() const noexcept
    {
        return has_value();
    }

    void reset() noexcept
    {
        has_value_ = false;
    }

    // https://en.cppreference.com/w/cpp/utility/optional/value
    T& value() &
    {
        check_has_value();
        return value_;
    }
    const T& value() const&
    {
        check_has_value();
        return value_;
    }
    T&& value() &&
    {
        check_has_value();
        return value_;
    }
    const T&& value() const&&
    {
        check_has_value();
        return value_;
    }

    // https://en.cppreference.com/w/cpp/utility/optional/operator*
    const T* operator->() const
    {
        assert(has_value());
        return &value_; // "This operator does not check whether the optional contains a value!"
    }
    T* operator->()
    {
        assert(has_value());
        return &value_;  // "This operator does not check whether the optional contains a value!"
    }

    const T& operator*() const&
    {
        assert(has_value());
        return value_;  // "This operator does not check whether the optional contains a value!"
    }
    T& operator*() &
    {
        assert(has_value());
        return value_;  // "This operator does not check whether the optional contains a value!"
    }
    const T&& operator*() const&&
    {
        assert(has_value());
        return value_;  // "This operator does not check whether the optional contains a value!"
    }
    T&& operator*() &&
    {
        assert(has_value());
        return value_;  // "This operator does not check whether the optional contains a value!"
    }

    // https://en.cppreference.com/w/cpp/utility/optional/value_or
    template<typename U>
    T value_or(U&& default_value) const&
    {
        // Equivalent to bool(*this) ? **this : static_cast<T>(std::forward<U>(default_value))
        return has_value() ? **this : static_cast<T>(std::forward<U>(default_value));
    }
    template <typename U>
    T value_or(U&& default_value) &&
    {
        // Equivalent to bool(*this) ? std::move(**this) : static_cast<T>(std::forward<U>(default_value))
        return has_value() ? std::move(**this) : static_cast<T>(std::forward<U>(default_value));
    }
};

// https://en.cppreference.com/w/cpp/utility/optional/make_optional
template <typename T, typename... TArgs>
inline Optional<T> make_Optional(TArgs&&... args)
{
    return Optional<T>(T(std::forward<TArgs>(args)...));
}

// https://en.cppreference.com/w/cpp/utility/optional/operator_cmp
// Compares two optional objects, lhs and rhs. The contained values are compared
// (using the corresponding operator of T) only if both lhs and rhs contain values. Otherwise,
// * lhs is considered equal to rhs if, and only if, both lhs and rhs do not contain a value.
// * lhs is considered less than rhs if, and only if, rhs contains a value and lhs does not.
template <typename T, typename U>
inline bool operator==(const Optional<T>& lhs, const Optional<U>& rhs)
{
    // If bool(lhs) != bool(rhs), returns false
    if (bool(lhs) != bool(rhs))
    {
        return false;
    }
    // Otherwise, if bool(lhs) == false (and so bool(rhs) == false as well), returns true
    if (!lhs.has_value())
    {
        assert(!rhs.has_value());
        return true;
    }
    // Otherwise, returns *lhs == *rhs.
    return *lhs == *rhs;
}
template <typename T, typename U>
inline bool operator==(const T& value, const Optional<U>& opt)
{
    return make_Optional<T>(value) == opt;
}
template <typename T, typename U>
inline bool operator==(const Optional<T>& opt, const U& value)
{
    return opt == make_Optional<U>(value);
}

template <typename T, typename U>
inline bool operator!=(const Optional<T>& lhs, const Optional<U>& rhs)
{
    return !(lhs == rhs);
}
template <typename T, typename U>
inline bool operator!=(const T& value, const Optional<U>& opt)
{
    return !(value == opt);
}
template <typename T, typename U>
inline bool operator!=(const Optional<T>& opt, const U& value)
{
    return !(opt == value);
}

template <typename T, typename U>
inline bool operator<(const Optional<T>& lhs, const Optional<U>& rhs)
{
    // If bool(rhs) == false returns false
    if (!rhs.has_value())
    {
        return false;
    }
    // Otherwise, if bool(lhs) == false, returns true
    if (!lhs.has_value())
    {
        return true;
    }
    // Otherwise returns *lhs < *rhs
    return *lhs < *rhs;
}
template <typename T, typename U>
inline bool operator<(const T& value, const Optional<U>& opt)
{
    return make_Optional<T>(value) < opt;
}
template <typename T, typename U>
inline bool operator<(const Optional<T>& opt, const U& value)
{
    return opt < make_Optional<U>(value);
}

template <typename T, typename U>
inline bool operator<=(const Optional<T>& lhs, const Optional<U>& rhs)
{
    // If bool(lhs) == false returns true
    if (!lhs.has_value())
    {
        return true;
    }
    // Otherwise, if bool(rhs) == false, returns false
    if (!rhs.has_value())
    {
        return false;
    }
    // Otherwise returns *lhs <= *rhs
    return *lhs <= *rhs;
}
template <typename T, typename U>
inline bool operator<=(const T& value, const Optional<U>& opt)
{
    return make_Optional<T>(value) <= opt;
}
template <typename T, typename U>
inline bool operator<=(const Optional<T>& opt, const U& value)
{
    return opt <= make_Optional<U>(value);
}

template <typename T, typename U>
inline bool operator>(const Optional<T>& lhs, const Optional<U>& rhs)
{
    // If bool(lhs) == false returns false
    if (!lhs.has_value())
    {
        return false;
    }
    // Otherwise, if bool(rhs) == false, returns true
    if (!rhs.has_value())
    {
        return true;
    }
    // Otherwise returns *lhs > *rhs
    return *lhs > *rhs;
}
template <typename T, typename U>
inline bool operator>(const T& value, const Optional<U>& opt)
{
    return make_Optional<T>(value) > opt;
}
template <typename T, typename U>
inline bool operator>(const Optional<T>& opt, const U& value)
{
    return opt > make_Optional<U>(value);
}

template <typename T, typename U>
inline bool operator>=(const Optional<T>& lhs, const Optional<U>& rhs)
{
    // If bool(rhs) == false returns true
    if (!rhs.has_value())
    {
        return true;
    }
    // Otherwise, if bool(lhs) == false, returns false
    if (!lhs.has_value())
    {
        return false;
    }
    // Otherwise returns *lhs >= *rhs
    return *lhs >= *rhs;
}
template <typename T, typename U>
inline bool operator>=(const T& value, const Optional<U>& opt)
{
    return make_Optional<T>(value) >= opt;
}
template <typename T, typename U>
inline bool operator>=(const Optional<T>& opt, const U& value)
{
    return opt >= make_Optional<U>(value);
}

#define CODA_OSS_sys_Optional 201606L // c.f., __cpp_lib_optional
}

#include "str/Convert.h"
namespace str
{
template <typename T>
std::string toString(const sys::Optional<T>& value)
{
    return toString(value.value());
}
}

#endif  // CODA_OSS_sys_Optional_h_INCLUDED_

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

#include "Conf.h"

// Simple version of std::Optional since that doesn't exist until C++17.
//
// This doesn't even TRY to match the actual C++17 specification,
// it only tries to minimize code changes.
//
// http://en.cppreference.com/w/cpp/utility/Optional
namespace sys
{
template <typename T>
class Optional  // no "final" as SWIG doesn't like it
{
    T value_;
    bool has_value_ = false;

public:
    using value_type = T;

    constexpr Optional() noexcept
    {
    }
    constexpr Optional(const value_type& v) : value_(v), has_value_(true)
    {
    }
    constexpr Optional(const Optional& other) :
        value_(other.value_), has_value_(other.has_value_)
    {
    }

    template <typename... Args>  // https://en.cppreference.com/w/cpp/utility/Optional/emplace
    T& emplace(Args&&... args)
    {
        value_ = value_type(std::forward<Args>(args)...);
        has_value_ = true;
        return value_;
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
        if (!has_value())
        {
            throw std::logic_error("No value for Optional<>."); // TODO: std::bad_optional_access
        }
        return value_;
    }
    const T& value() const&
    {
        if (!has_value())
        {
            throw std::logic_error("No value for Optional<>."); // TODO: std::bad_optional_access
        }
        return value_;
    }
    T&& value() &&
    {
        if (!has_value())
        {
            throw std::logic_error("No value for Optional<>."); // TODO: std::bad_optional_access
        }
        return value_;
    }
    const T&& value() const&&
    {
        if (!has_value())
        {
            throw std::logic_error("No value for Optional<>."); // TODO: std::bad_optional_access
        }
        return value_;
    }

    // https://en.cppreference.com/w/cpp/utility/optional/operator*
    const T* operator->() const
    {
        return &value_; // "This operator does not check whether the optional contains a value!"
    }
    T* operator->()
    {
        return &value_; // "This operator does not check whether the optional contains a value!"
    }

    const T& operator*() const&
    {
        return value_; // "This operator does not check whether the optional contains a value!"
    }
    T& operator*() &
    {
        return value_; // "This operator does not check whether the optional contains a value!"
    }
    const T&& operator*() const&&
    {
        return value_; // "This operator does not check whether the optional contains a value!"
    }
    T&& operator*() &&
    {
        return value_; // "This operator does not check whether the optional contains a value!"
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

}

#ifndef CODA_OSS_DEFINE_std_optional_
    #if CODA_OSS_cpp17 && __has_include(<optional>)  // __has_include is C++17
        #define CODA_OSS_DEFINE_std_optional_ -1  // OK to #include <>, below
    #else
        #define CODA_OSS_DEFINE_std_optional_ CODA_OSS_AUGMENT_std_namespace // maybe use our own
    #endif  // CODA_OSS_cpp17
#endif  // CODA_OSS_DEFINE_std_optional_

#if CODA_OSS_DEFINE_std_optional_ == 1
    namespace std // This is slightly uncouth: we're not supposed to augment "std".
    {
        template<typename T>
        using optional = sys::Optional<T>;
    }
#elif CODA_OSS_DEFINE_std_optional_ == -1  // set above
    #include <optional>
#endif // CODA_OSS_DEFINE_std_optional_

#endif  // CODA_OSS_sys_Optional_h_INCLUDED_

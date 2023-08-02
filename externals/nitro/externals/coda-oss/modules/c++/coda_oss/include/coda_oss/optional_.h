/* =========================================================================
 * This file is part of coda_oss-c++
 * =========================================================================
 *
 * (C) Copyright 2020, Maxar Technologies, Inc.
 *
 * coda_oss-c++ is free software; you can redistribute it and/or modify
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

#pragma once
#ifndef CODA_OSS_coda_oss_optional__h_INCLUDED_
#define CODA_OSS_coda_oss_optional__h_INCLUDED_

#include <assert.h>

#include <utility>
#include <stdexcept>

// Simple version of std::optional since that doesn't exist until C++17.
// http://en.cppreference.com/w/cpp/utility/Optional

namespace coda_oss
{
namespace details
{
inline void throw_bad_optional_access()
{
    throw std::logic_error("No value for optional<>.");  // TODO: std::bad_optional_access
}

template <typename T>
class optional final
{
    T value_{};
    bool has_value_ = false;

    void check_has_value() const
    {
        if (!has_value())
        {
            details::throw_bad_optional_access();
        }
    }

public:
    using value_type = T;

    optional() noexcept
    {
    }
    optional(const value_type& v) : value_(v), has_value_(true)
    {
    }
    optional(value_type&& v) : value_(std::move(v)), has_value_(true)
    {
    }
    optional(const optional& other)
    {
        *this = other;
    }
    optional(optional&& other) noexcept
    {
        *this = std::move(other);
    }
    template <typename U>
    explicit optional(const optional<U>& other)
    {
        *this = other;
    }
    template <typename U>
    explicit optional(optional<U>&& other)
    {
        *this = std::forward<optional<U>>(other);
    }
    template <typename U = T>
    explicit optional(U&& value)
    {
        *this = std::forward<U>(value);
    }

    // https://en.cppreference.com/w/cpp/utility/optional/operator%3D
    optional& operator=(const optional& other) 
    {
        value_ = other.value_;
        has_value_ = other.has_value_;
        return *this;
    }
    optional& operator=(optional&& other) noexcept 
    {
        value_ = std::move(other.value_);
        has_value_ = std::move(other.has_value_);
        return *this;
    }
    template <typename U = T> 
    optional& operator=(U&& value) noexcept
    {
        value_ = std::forward<U>(value);
        has_value_ = true;
        return *this;
    }
    template <typename U>
    optional& operator=(const optional<U>& other)
    {
        value_ = other.value_;
        has_value_ = other.has_value_;
        return *this;
    }
    template <typename U>
    optional& operator=(optional<U>&& other)
    {
        value_ = std::forward<optional<U>>(other.value_);
        has_value_ = other.has_value_;
        return *this;
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
        value_ = T{};
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
        return &value_;  // "This operator does not check whether the optional contains a value!"
    }
    T* operator->() noexcept
    {
        assert(has_value());
        return &value_;  // "This operator does not check whether the optional contains a value!"
    }
    const T& operator*() const& noexcept
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
    template <typename U>
    T value_or(U&& default_value) const&
    {
        // Equivalent to bool(*this) ? **this :
        // static_cast<T>(std::forward<U>(default_value))
        return has_value() ? **this
                           : static_cast<T>(std::forward<U>(default_value));
    }
    template <typename U>
    T value_or(U&& default_value) &&
    {
        // Equivalent to bool(*this) ? std::move(**this) :
        // static_cast<T>(std::forward<U>(default_value))
        return has_value() ? std::move(**this)
                           : static_cast<T>(std::forward<U>(default_value));
    }
};

// https://en.cppreference.com/w/cpp/utility/optional/make_optional
template <typename T, typename... TArgs>
inline optional<T> make_optional(TArgs&&... args)
{
    return optional<T>(T(std::forward<TArgs>(args)...));
}

// https://en.cppreference.com/w/cpp/utility/optional/operator_cmp
// Compares two optional objects, lhs and rhs. The contained values are compared
// (using the corresponding operator of T) only if both lhs and rhs contain
// values. Otherwise,
// * lhs is considered equal to rhs if, and only if, both lhs and rhs do not contain a value.
// * lhs is considered less than rhs if, and only if, rhs contains a value and lhs does not.
template <typename T, typename U>
inline bool operator==(const optional<T>& lhs, const optional<U>& rhs)
{
    // "If bool(lhs) != bool(rhs), returns false."
    if (lhs.has_value() != rhs.has_value())
    {
        return false;
    }
    // "Otherwise, if bool(lhs) == false (and so bool(rhs) == false as well), returns true."
    if (!lhs.has_value())
    {
        assert(!rhs.has_value());
        return true;
    }
    // "Otherwise, returns *lhs == *rhs."
    return *lhs == *rhs;
}
template <typename T, typename U>
inline bool operator==(const T& value, const optional<U>& opt)
{
    return make_optional<T>(value) == opt;
}
template <typename T, typename U>
inline bool operator==(const optional<T>& opt, const U& value)
{
    return opt == make_optional<U>(value);
}

template <typename T, typename U>
inline bool operator!=(const optional<T>& lhs, const optional<U>& rhs)
{
    return !(lhs == rhs);
}
template <typename T, typename U>
inline bool operator!=(const T& value, const optional<U>& opt)
{
    return !(value == opt);
}
template <typename T, typename U>
inline bool operator!=(const optional<T>& opt, const U& value)
{
    return !(opt == value);
}

template <typename T, typename U>
inline bool operator<(const optional<T>& lhs, const optional<U>& rhs)
{
    // "If bool(rhs) == false returns false."
    if (!rhs.has_value())
    {
        return false;
    }
    // "Otherwise, if bool(lhs) == false, returns true."
    if (!lhs.has_value())
    {
        return true;
    }
    // "Otherwise returns *lhs < *rhs."
    return *lhs < *rhs;
}
template <typename T, typename U>
inline bool operator<(const T& value, const optional<U>& opt)
{
    return make_optional<T>(value) < opt;
}
template <typename T, typename U>
inline bool operator<(const optional<T>& opt, const U& value)
{
    return opt < make_optional<U>(value);
}

template <typename T, typename U>
inline bool operator<=(const optional<T>& lhs, const optional<U>& rhs)
{
    // "If bool(lhs) == false returns true."
    if (!lhs.has_value())
    {
        return true;
    }
    // "Otherwise, if bool(rhs) == false, returns false."
    if (!rhs.has_value())
    {
        return false;
    }
    // "Otherwise returns *lhs <= *rhs."
    return *lhs <= *rhs;
}
template <typename T, typename U>
inline bool operator<=(const T& value, const optional<U>& opt)
{
    return make_optional<T>(value) <= opt;
}
template <typename T, typename U>
inline bool operator<=(const optional<T>& opt, const U& value)
{
    return opt <= make_optional<U>(value);
}

template <typename T, typename U>
inline bool operator>(const optional<T>& lhs, const optional<U>& rhs)
{
    // "If bool(lhs) == false returns false."
    if (!lhs.has_value())
    {
        return false;
    }
    // "Otherwise, if bool(rhs) == false, returns true."
    if (!rhs.has_value())
    {
        return true;
    }
    // "Otherwise returns *lhs > *rhs."
    return *lhs > *rhs;
}
template <typename T, typename U>
inline bool operator>(const T& value, const optional<U>& opt)
{
    return make_optional<T>(value) > opt;
}
template <typename T, typename U>
inline bool operator>(const optional<T>& opt, const U& value)
{
    return opt > make_optional<U>(value);
}

template <typename T, typename U>
inline bool operator>=(const optional<T>& lhs, const optional<U>& rhs)
{
    // "If bool(rhs) == false returns true."
    if (!rhs.has_value())
    {
        return true;
    }
    // "Otherwise, if bool(lhs) == false, returns false."
    if (!lhs.has_value())
    {
        return false;
    }
    // "Otherwise returns *lhs >= *rhs."
    return *lhs >= *rhs;
}
template <typename T, typename U>
inline bool operator>=(const T& value, const optional<U>& opt)
{
    return make_optional<T>(value) >= opt;
}
template <typename T, typename U>
inline bool operator>=(const optional<T>& opt, const U& value)
{
    return opt >= make_optional<U>(value);
}
}
}

#endif  // CODA_OSS_coda_oss_optional__h_INCLUDED_

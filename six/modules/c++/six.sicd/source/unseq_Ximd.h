/* =========================================================================
 * This file is part of sys-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 * � Copyright 2024, Maxar Technologies, Inc.
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
 * License along with this program; If not, 
 * see <http://www.gnu.org/licenses/>.
 *
 */

#pragma once 

/*!
 *  \brief A simple (poor man's) "implemtation" of std::experimental::simd
 * for development/testing purposes.
 *
 * `std::experimental::simd` needs G++11 and C++20 and the "vectorclass"
 * library needs C++17.  "Rolling our own" using `std::array` lets developers
 * try out some things without needing actual SIMD code.
 *
 */

#include "sys/Dbg.h"

// This is intended for development/testing purposes, so enable by default only for debug.
#ifndef SIX_Ximd_ENABLED
#define SIX_Ximd_ENABLED CODA_OSS_DEBUG
#endif

#if SIX_Ximd_ENABLED

#include <array>
#include <type_traits>
#include <functional>
#include <cmath>
#include <memory>

namespace six
{
namespace ximd
{

// Need a class for the "broadcast" constructor.
// Also helps to avoid overloading `std::array`.
template <typename T, int N = 4>
struct Ximd final
{
    static_assert(std::is_arithmetic<T>::value, "T must be arithmetic");
    // static_assert(std::is_same<T, std::remove_cv<T>::type>::value, "no `const` for T");

    using value_type = T;
    using reference = T&;

    Ximd() = default;
    // This is the same as the "generater" overload below ... avoid enable_if gunk for now.
     template<typename U>
     Ximd(U v) noexcept
    {
        *this = Ximd([&](size_t) { return v; }, nullptr);
    }
    template <typename U>
    Ximd(const Ximd<U>& other) noexcept
    {
        *this = other;
    }
    template <typename U>
    Ximd(const U* mem)
    {
        copy_from(mem);
    }

    // https://en.cppreference.com/w/cpp/experimental/simd/simd/simd
    // this is the same as `U&& v` above; avoid enable_if gunk for now.
    template <typename G>
    explicit Ximd(G&& gen, std::nullptr_t) noexcept
    {
        // This is where all the "magic" (would) happen.
        for (size_t i = 0; i < size(); i++)
        {
            (*this)[i] = gen(i);
        }
    }

    reference operator[](size_t pos) noexcept
    {
        return value[pos];
    }
    value_type operator[](size_t pos) const noexcept
    {
        return value[pos];
    }

    static constexpr size_t size() noexcept
    {
        return N;
    }

    template <typename U>
    void copy_from(const U* mem)
    {
        *this = Ximd([&](size_t i) { return mem[i]; }, nullptr);
    }
    template <typename U>
    void copy_to(U* mem) const
    {
        for (size_t i = 0; i < size(); i++)
        {
            mem[i] = (*this)[i];
        }
    }

    Ximd& operator++() noexcept
    {
        *this = Ximd([&](size_t i) { return ++value[i]; }, nullptr);
        return *this;
    }
    Ximd operator++(int) noexcept
    {
        auto tmp = *this;
        ++(*this);
        return tmp;
    }

private:
    std::array<value_type, N> value{};
};

// template<typename T, int N>
// using fixed_size_ximd = Ximd<T, N>;
//
// template<typename T, int N>
// using native_ximd = Ximd<T>;

using ximd_mask = Ximd<bool>;

template <typename T>
inline auto operator+(const Ximd<T>& lhs, const Ximd<T>& rhs) noexcept
{
    return Ximd<T>([&](size_t i) { return lhs[i] + rhs[i]; }, nullptr);
}
template <typename T>
inline auto operator+(const Ximd<T>& lhs, typename Ximd<T>::value_type rhs) noexcept
{
    return lhs + Ximd<T>(rhs);
}
template <typename T>
inline auto operator-(const Ximd<T>& lhs, const Ximd<T>& rhs) noexcept
{
    return Ximd<T>([&](size_t i) { return lhs[i] - rhs[i]; }, nullptr);
}
template <typename T>
inline auto operator-(const Ximd<T>& lhs, typename Ximd<T>::value_type rhs) noexcept
{
    return lhs - Ximd<T>(rhs);
}
template <typename T>
inline auto operator*(const Ximd<T>& lhs, const Ximd<T>& rhs) noexcept
{
    return Ximd<T>([&](size_t i) { return lhs[i] * rhs[i]; }, nullptr);
}
template <typename T>
inline auto operator/(const Ximd<T>& lhs, typename Ximd<T>::value_type rhs) noexcept
{
    return lhs / Ximd<T>(rhs);
}
template <typename T>
inline auto operator/(const Ximd<T>& lhs, const Ximd<T>& rhs) noexcept
{
    return Ximd<T>([&](size_t i) { return lhs[i] / rhs[i]; }, nullptr);
}

template <typename T>
inline auto& operator+=(Ximd<T>& lhs, const Ximd<T>& rhs) noexcept
{
    lhs = lhs + rhs;
    return lhs;
}
template <typename T>
inline auto& operator-=(Ximd<T>& lhs, const Ximd<T>& rhs) noexcept
{
    lhs = lhs - rhs;
    return lhs;
}

template <typename T>
inline auto operator==(const Ximd<T>& lhs, const Ximd<T>& rhs) noexcept
{
    return ximd_mask([&](size_t i) { return lhs[i] == rhs[i]; }, nullptr);
}
template <typename T>
inline auto operator==(const Ximd<T>& lhs, typename Ximd<T>::value_type rhs) noexcept
{
    return lhs == Ximd<T>(rhs);
}
template <typename T>
inline auto operator!=(const Ximd<T>& lhs, const Ximd<T>& rhs) noexcept
{
    return ximd_mask([&](size_t i) { return lhs[i] != rhs[i]; }, nullptr);
}
template <typename T>
inline auto operator!=(const Ximd<T>& lhs, typename Ximd<T>::value_type rhs) noexcept
{
    return lhs != Ximd<T>(rhs);
}
template <typename T>
inline auto operator<(const Ximd<T>& lhs, const Ximd<T>& rhs) noexcept
{
    return ximd_mask([&](size_t i) { return lhs[i] < rhs[i]; }, nullptr);
}
template <typename T>
inline auto operator<(const Ximd<T>& lhs, typename Ximd<T>::value_type rhs) noexcept
{
    return lhs < Ximd<T>(rhs);
}
template <typename T>
inline auto operator<=(const Ximd<T>& lhs, const Ximd<T>& rhs) noexcept
{
    return ximd_mask([&](size_t i) { return lhs[i] <= rhs[i]; }, nullptr);
}
template <typename T>
inline auto operator>(const Ximd<T>& lhs, const Ximd<T>& rhs) noexcept
{
    return ximd_mask([&](size_t i) { return lhs[i] > rhs[i]; }, nullptr);
}
template <typename T>
inline auto operator>(const Ximd<T>& lhs, typename Ximd<T>::value_type rhs) noexcept
{
    return lhs > Ximd<T>(rhs);
}

inline bool any_of(const ximd_mask& m)
{
    for (size_t i = 0; i < m.size(); i++)
    {
        if (m[i])
        {
            return true;
        }
    }
    return false;
}

inline bool all_of(const ximd_mask& m)
{
    for (size_t i = 0; i < m.size(); i++)
    {
        if (!m[i])
        {
            return false;
        }
    }
    return true;
}

template <typename T>
inline auto atan2(const Ximd<T>& real, const Ximd<T>& imag)
{
    return Ximd<T>([&](size_t i) { return std::atan2(real[i], imag[i]); }, nullptr);
}
template <typename T>
inline auto round(const Ximd<T>& v)
{
    return Ximd<T>([&](size_t i) { return std::round(v[i]); }, nullptr);
}
template <typename T>
inline auto lround(const Ximd<T>& v)
{
    return Ximd<int>([&](size_t i) { return std::lround(v[i]); }, nullptr);
}

template<typename T>
static auto simd_select(const ximd_mask& test, const Ximd<T>& t, const Ximd<T>& f)
{
    Ximd<T> retval;
    for (size_t i = 0; i < test.size(); i++)
    {
        retval[i] = test[i] ? t[i] : f[i];
    }
    return retval;
}

inline auto operator&&(const ximd_mask& lhs, const ximd_mask& rhs) noexcept
{
    return ximd_mask([&](size_t i) { return lhs[i] && rhs[i]; }, nullptr);
}

} // ximd
} // sys

#endif
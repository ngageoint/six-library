/* =========================================================================
 * This file is part of math-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2016, MDA Information Systems LLC
 *
 * math-c++ is free software; you can redistribute it and/or modify
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

#ifndef __MATH_UTILITIES_H__
#define __MATH_UTILITIES_H__

#include <stdlib.h>
#include <math.h>
#include <math/math_config.h>
#include <sys/Conf.h>
#include <cmath>

namespace math
{
/*!
 * Find sign of input, expressed as -1, 0, or 1
 * \param val A signed number
 * \return 1 if val is positive, -1 if negative, 0 otherwise
 */
template <typename T>
inline constexpr int sign(T val) noexcept
{
    return val < 0 ? -1 : val > 0 ? 1 : 0;
}

inline constexpr double square(double val) noexcept
{
    return val * val;
}

inline constexpr double cube(double val) noexcept
{
    return square(val) * val;
}

/*!
 * Return true if argument is NaN
 *
 * \param value Argument to be checked for NaN
 * \return true if value is NaN
 */
template <typename T> inline bool isNaN(T value) noexcept
{
    return std::isnan(value);
}

// https://man7.org/linux/man-pages/man3/sincos.3.html
void SinCos(float angle, float& sin, float& cos) noexcept;
void SinCos(double angle, double& sin, double& cos) noexcept;
void SinCos(long double angle, long double& sin, long double& cos) noexcept;

/*
 * Calculate the binomial coefficient
 * Be wary of the possibility of overflow from integer arithmetic.
 *
 * \param n number of possibilities
 * \param k number of outcomes
 * \return n choose k
 */
sys::Uint64_T nChooseK(size_t n, size_t k);
}

#endif


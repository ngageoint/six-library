/* =========================================================================
 * This file is part of math-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2015, MDA Information Systems LLC
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

#ifndef __MATH_ROUND_H__
#define __MATH_ROUND_H__

#include <cmath>
#include <stddef.h>

#include "config/Exports.h"

namespace math
{
/*!
 *  Returns the nearest integral value towards zero.
 *
 *  \param value A number to evaluate
 *  \return The 'fixed' number
 */
template<typename T> inline T fix(T value_)
{
    const double value = value_;
    const auto result = value > 0.0 ? std::floor(value) : std::ceil(value);
    return static_cast<T>(result);
}

/*!
 *  Returns the integral value that is nearest to value, with halfway cases
 *  rounded away from zero.
 *
 *  \param value A number to evaluate
 *  \return The rounded number
 */
template<typename T> inline T round(T value_)
{
    const double value = value_;
    const auto result = value > 0.0 ? std::floor(value + 0.5) : std::ceil(value - 0.5);
    return static_cast<T>(result);
}

/*!
 *  Rounds a floating point number to given number of fractional digits.
 *
 *  \param value A number to evaluate
 *  \param fractionalDigits Number of fractional digits to round to
 *  \return The rounded number
 */
template<typename T> inline T round(T value_, size_t fractionalDigits)
{
    double power10 = 1.0;
    for (size_t i = 0; i < fractionalDigits; ++i)
    {
        power10 *= 10.0;
    }

    const double value = value_;
    const auto result = value > 0.0 ? std::floor(value * power10 + 0.5) / power10
                        : std::ceil(value * power10 - 0.5) / power10;
    return static_cast<T>(result);
}

/*!
 * Equivalent to ceil((float)numerator / denominator)
 *
 * \param numerator Number to divide
 * \param denominator Non-zero number to divide by
 * \return Result of division, rounded up
 * \throw if denominator is 0
 */
CODA_OSS_API size_t ceilingDivide(size_t numerator, size_t denominator);
}

#endif

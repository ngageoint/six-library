/* =========================================================================
 * This file is part of math-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2017, MDA Information Systems LLC
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
#include "math/Utilities.h"

// https://man7.org/linux/man-pages/man3/sincos.3.html
#define _GNU_SOURCE         /* See feature_test_macros(7) */
#include <math.h>

#include <except/Exception.h>
#include <str/Convert.h>

namespace math
{
sys::Uint64_T nChooseK(size_t n, size_t k)
{
    if (n < k)
    {
        throw except::Exception(Ctxt("n Choose k undefined for n < k.\n"
                "n: " + str::toString(n) + " k: " + str::toString(k)));
    }

    // Algorithm to compute n Choose k without using factorials found here:
    // http://csharphelper.com/blog/2014/08/
    //         calculate-the-binomial-coefficient-n-choose-k-efficiently-in-c
    sys::Uint64_T coefficient = 1;
    for (size_t ii = 1; ii <= k; ++ii)
    {
        coefficient *= (n - (k - ii));
        coefficient /= ii;
    }
    return coefficient;
}
}

static inline void sincosf_(float x, float& sin, float& cos)
{
    #if !_WIN32 // TODO: what about Apple?
    sincosf(x, &sin, &cos);
    #else
    sin = std::sin(x);
    cos = std::cos(x);
    #endif
}
static inline void sincos_(double x, double& sin, double& cos)
{
#if !_WIN32  // TODO: what about Apple?
    sincos(x, &sin, &cos);
#else
    sin = std::sin(x);
    cos = std::cos(x);
#endif
}
void math::SinCos(float x, float& sin, float& cos)
{
    sincosf_(x, sin, cos);
}
void math::SinCos(double x, double& sin, double& cos)
{
    sincos_(x, sin, cos);
}
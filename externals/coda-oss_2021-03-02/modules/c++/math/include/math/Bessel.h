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

#ifndef __MATH_BESSEL_H
#define __MATH_BESSEL_H

#include <cstddef>
namespace math
{
/*!
 * Modified Bessel function of the first kind
 * This function wraps the following order-specific functions
 */
double besselI(size_t order, double x);

/*!
 * Modified Bessel function of the first kind, order 0
 */
double besselIOrderZero(double x);

/*!
 * Modified Bessel function of the first kind, order 1
 */
double besselIOrderOne(double x);

/*!
 * Modified Bessel function of the first kind, order n > 1
 */
double besselIOrderN(size_t order, double x);
}

#endif


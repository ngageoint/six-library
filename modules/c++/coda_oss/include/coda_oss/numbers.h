/* =========================================================================
 * This file is part of coda_oss-c++
 * =========================================================================
 *
 * (C) Copyright 2020-2022, Maxar Technologies, Inc.
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
#ifndef CODA_OSS_coda_oss_numbers_h_INCLUDED_
#define CODA_OSS_coda_oss_numbers_h_INCLUDED_

#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES // get M_PI, etc.
#endif
#include <math.h>
/*
#define M_E        2.71828182845904523536   // e
#define M_LOG2E    1.44269504088896340736   // log2(e)
#define M_LOG10E   0.434294481903251827651  // log10(e)
#define M_LN2      0.693147180559945309417  // ln(2)
#define M_LN10     2.30258509299404568402   // ln(10)
#define M_PI       3.14159265358979323846   // pi
#define M_PI_2     1.57079632679489661923   // pi/2
#define M_PI_4     0.785398163397448309616  // pi/4
#define M_1_PI     0.318309886183790671538  // 1/pi
#define M_2_PI     0.636619772367581343076  // 2/pi
#define M_2_SQRTPI 1.12837916709551257390   // 2/sqrt(pi)
#define M_SQRT2    1.41421356237309504880   // sqrt(2)
#define M_SQRT1_2  0.707106781186547524401  // 1/sqrt(2)
*/

namespace coda_oss
{

namespace numbers
{
// https://en.cppreference.com/w/cpp/header/numbers

template <typename T> constexpr T e_v = static_cast<T>(M_E);
template <typename T> constexpr T log2e_v = static_cast<T>(M_LOG2E);
template <typename T> constexpr T log10e_v = static_cast<T>(M_LOG10E);
template <typename T> constexpr T pi_v = static_cast<T>(M_PI);
template <typename T> constexpr T inv_pi_v = static_cast<T>(M_1_PI);
//template <typename T> constexpr T inv_sqrtpi_v = static_cast<T>();
template <typename T> constexpr T ln2_v = static_cast<T>(M_LN2);
template <typename T> constexpr T ln10_v = static_cast<T>(M_LN10);
template <typename T> constexpr T sqrt2_v = static_cast<T>(M_SQRT2);
//template <typename T> constexpr T sqrt3_v = static_cast<T>();
//template <typename T> constexpr T inv_sqrt3_v = static_cast<T>();
//template <typename T> constexpr T egamma_v = static_cast<T>();
//template <typename T> constexpr T phi_v  = static_cast<T>();

constexpr double e = e_v<double>;
constexpr double log2e = log2e_v<double>;
constexpr double log10e = log10e_v<double>;
constexpr double pi = pi_v<double>;
constexpr double inv_pi = inv_pi_v<double>;
//constexpr double inv_sqrtpi = inv_sqrtpi_v<double>;
constexpr double ln2 = ln2_v<double>;
constexpr double ln10 = ln10_v<double>;
constexpr double sqrt2 = sqrt2_v<double>;
//constexpr double sqrt3 = sqrt3_v<double>;
//constexpr double inv_sqrt3 = inv_sqrt3_v<double>;
//constexpr double egamma = egamma_v<double>;
//constexpr double phi = phi_v<double>;
}

}

#endif  // CODA_OSS_coda_oss_numbers_h_INCLUDED_

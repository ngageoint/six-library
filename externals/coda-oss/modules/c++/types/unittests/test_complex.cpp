/* =========================================================================
 * This file is part of types-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2017, MDA Information Systems LLC
 *
 * types-c++ is free software; you can redistribute it and/or modify
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

#include "TestCase.h"

#include <types/complex.h>

TEST_CASE(TestCxShort_abs)
{
    constexpr auto real = 123;
    constexpr auto imag = -321;

    CODA_OSS_disable_warning_push
    #ifdef _MSC_VER
    #pragma warning(disable: 4996) // '...': warning STL4037: The effect of instantiating the template std::complex for any type other than float, double, or long double is unspecified. You can define _SILENCE_NONFLOATING_COMPLEX_DEPRECATION_WARNING to suppress this warning.
    #endif
    const std::complex<short> cx_short(real, imag);
    CODA_OSS_disable_warning_pop
    const auto expected = abs(cx_short);

    const types::zint16_t types_zint16(real, imag);
    const auto actual = abs(types_zint16);
    TEST_ASSERT_EQ(actual, expected);
}

TEST_MAIN(
    TEST_CHECK(TestCxShort_abs);
    )

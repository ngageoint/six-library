/* =========================================================================
 * This file is part of sys-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2017, MDA Information Systems LLC
 *
 * sys-c++ is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.

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

#include <std/cstddef>

#include <sys/Conf.h>
#include <avx/extractf.h>

TEST_CASE(extractf)
{
  /*
    #if defined(__GNUC__)
        // direct_m256_extract_str
        __m256 ymm = _mm256_setzero_ps();
        float val = ymm[7];
    #elif defined(_MSC_VER)
        // member_m256_extract_str
        __m256 ymm = _mm256_setzero_ps();
        float val = ymm.m256_f32[7];
    #else
        float ymm = 0.0;
        float val = sys::mm256_extractf(ymm, 7);
        val = sys::mm256_extractf_(ymm, 7);
#endif

    val = avx::mm256_extractf(ymm, 7);
    if (val) {} // suppress compiler warning about unused "val"
  */
  TEST_ASSERT_TRUE(true);
}
    

TEST_MAIN(
    TEST_CHECK(extractf);
)

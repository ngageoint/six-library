/* =========================================================================
 * This file is part of avx-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2019, MDA Information Systems LLC
 * (C) Copyright 2021, Maxar Technologies, Inc.
 *
 * config-c++ is free software; you can redistribute it and/or modify
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
#ifndef CODA_OSS_avx_extractf_h_INCLUDED_
#define CODA_OSS_avx_extractf_h_INCLUDED_

#include <config/compiler_extensions.h>

#ifndef CODA_OSS_mm256_extractf_DEFINED_
    #define CODA_OSS_mm256_extractf_DEFINED_ 1

    #include <immintrin.h>
    namespace avx
    {
        // Extract the i'th 32-bit float lane from a 256-bit AVX register.
        // AVX intrinsics such as _mm256_extract_epi32() require a compile-time
        // constant index, which doesn't work for a run-time "i" parameter; a
        // simple reinterpret_cast avoids that restriction and generates
        // equivalent code on GCC, Clang and MSVC.
        template <typename T>
        inline float& mm256_extractf_(T& ymm, int i)
        {
            return reinterpret_cast<float*>(&ymm)[i];
        }
        template <typename T>
        inline float& mm256_extractf(T& ymm, int i)
        {
            return mm256_extractf_(ymm, i);
        }
    }

#endif

#endif  // CODA_OSS_avx_extractf_h_INCLUDED_

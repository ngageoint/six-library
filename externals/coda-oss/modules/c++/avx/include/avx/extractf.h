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
#pragma once

#include <config/compiler_extensions.h>

#ifndef CODA_OSS_mm256_extractf_DEFINED_
    #define CODA_OSS_mm256_extractf_DEFINED_ 1

    #include <immintrin.h>
    namespace avx
    {
        template <typename T>
        inline T& mm256_extractf_(T& ymm, int i)
        {
            //This looks awful, but almost all of these intrinsics simply reinterpret bits and generate no actual instructions.
            #define CODA_OSS_sys_MM256_EXTRACTF_(ymm_,i_) _mm256_cvtss_f32(_mm256_castsi256_ps(_mm256_set1_epi32(_mm256_extract_epi32(_mm256_castps_si256(ymm_),i_))))
            return CODA_OSS_sys_MM256_EXTRACTF_(ymm, i);
        }
        template <typename T>
        inline float& mm256_extractf(T& ymm, int i)
        {
            #if defined(__GNUC__)
                return ymm[i];
            #elif defined(_MSC_VER)
                return ymm.m256_f32[i];
            #else
                return mm256_extractf_(ymm, i);
            #endif
        }
    }

#endif

#endif  // CODA_OSS_avx_extractf_h_INCLUDED_

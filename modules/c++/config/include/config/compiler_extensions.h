/* =========================================================================
 * This file is part of config-c++
 * =========================================================================
 *
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
#ifndef CODA_OSS_config_compiler_extentions_h_INCLUDED_
#define CODA_OSS_config_compiler_extentions_h_INCLUDED_
#pragma once

#ifndef CODA_OSS_attribute_noinline_DEFINED_
    #define CODA_OSS_attribute_noinline_DEFINED_ 1

    // https://stackoverflow.com/a/49468469/8877
    #if defined(__GNUC__)
        #define ATTRIBUTE_NOINLINE __attribute__((noinline))
        #define DECLSPEC_NOINLINE
        #define NOINLINE(RETURN_TYPE_) RETURN_TYPE_ ATTRIBUTE_NOINLINE
        #ifndef HAVE_ATTRIBUTE_NOINLINE
            #define HAVE_ATTRIBUTE_NOINLINE 1
        #endif
    #elif defined(_MSC_VER)
        #define ATTRIBUTE_NOINLINE
        #define DECLSPEC_NOINLINE __declspec(noinline)
        #define NOINLINE(RETURN_TYPE_) DECLSPEC_NOINLINE RETURN_TYPE_
        #ifndef HAVE_DECLSPEC_NOINLINE
            #define HAVE_DECLSPEC_NOINLINE 1
        #endif
    #else
        #define ATTRIBUTE_NOINLINE
        #define DECLSPEC_NOINLINE
        #define NOINLINE(RETURN_TYPE_) RETURN_TYPE_
    #endif
#endif // CODA_OSS_attribute_noinline_DEFINED_

#ifndef CODA_OSS_attribute_aligned_DEFINED_
    #define CODA_OSS_attribute_aligned_DEFINED_ 1

    #if defined(__GNUC__)
        // https://gcc.gnu.org/onlinedocs/gcc-3.2/gcc/Variable-Attributes.html
        #define ATTRIBUTE_ALIGNED(x) __attribute__((aligned(x)))
        #define DECLSPEC_ALIGN(x)
        #define ALIGNED(declaration, x) declaration ATTRIBUTE_ALIGNED(x)
        #ifndef HAVE_ATTRIBUTE_ALIGNED
            #define HAVE_ATTRIBUTE_ALIGNED 1
        #endif
    #elif defined(_MSC_VER)
        #define ATTRIBUTE_ALIGNED(x)
        #define DECLSPEC_ALIGN(x) __declspec(align(x))
        #define ALIGNED(declaration, x) DECLSPEC_ALIGN(x) declaration
        #ifndef HAVE_DECLSPEC_ALIGN
            #define HAVE_DECLSPEC_ALIGN 1
        #endif
    #else
        #define ATTRIBUTE_ALIGNED(X)
        #define DECLSPEC_ALIGN(x)
        #define ALIGNED(declaration, x) declaration
    #endif
#endif // CODA_OSS_attribute_aligned_DEFINED_

#ifndef _MM256_EXTRACTF
    #ifdef HAVE_M256_DIRECT_INDEX
        #define _MM256_EXTRACTF(ymm_,i_) ymm_[i_]
    #elif defined HAVE_M256_MEMBER_INDEX
        #define _MM256_EXTRACTF(ymm_,i_) ymm_.m256_f32[i_]
    #else
        //This looks awful, but almost all of these intrinsics simply reinterpret bits and generate no actual instructions.
        #define _MM256_EXTRACTF(ymm_,i_) _mm256_cvtss_f32(_mm256_castsi256_ps(_mm256_set1_epi32(_mm256_extract_epi32(_mm256_castps_si256(ymm_),i_))))
    #endif
#endif  // _MM256_EXTRACTF


#ifndef CODA_OSS_HAVE_DISABLE_WARNINGS
    #define CODA_OSS_HAVE_DISABLE_WARNINGS 1

    // Adapted from
    // https://www.fluentcpp.com/2019/08/30/how-to-disable-a-warning-in-cpp/
    #if defined(_MSC_VER)
        #define CODA_OSS_disable_warning_push           __pragma(warning( push ))
        #define CODA_OSS_disable_warning_pop            __pragma(warning( pop ))
        #define CODA_OSS_disable_warning(warningNumber) __pragma(warning( disable : warningNumber ))

        #define CODA_OSS_disable_warning_system_header_push  __pragma(warning(push, 0))

        // 4100 => 'unreferenced formal parameter'
        #define CODA_OSS_UNREFERENCED_FORMAL_PARAMETER CODA_OSS_disable_warning(4100)

        // 4551 => 'function call missing argument list'
        #define CODA_OSS_FUNCTION_CALL_MISSING_ARG_LIST CODA_OSS_disable_warning(4551)

        // 4702 Unreachable Code Warning
        #define CODA_OSS_DISABLE_UNREACHABLE_CODE CODA_OSS_disable_warning(4702)
    #elif defined(__GNUC__) || defined(__clang__)
        #define CODA_OSS_do_pragma(X) _Pragma(#X)
        #define CODA_OSS_disable_warning_push           CODA_OSS_do_pragma(GCC diagnostic push)
        #define CODA_OSS_disable_warning_pop            CODA_OSS_do_pragma(GCC diagnostic pop)
        #define CODA_OSS_disable_warning(warningName)   CODA_OSS_do_pragma(GCC diagnostic ignored #warningName)

        #define CODA_OSS_disable_warning_system_header_push \
            CODA_OSS_disable_warning_push \
            CODA_OSS_disable_warning(-Wall) \
            CODA_OSS_disable_warning(-Wextra)

        // no such thing
        #define CODA_OSS_UNREFERENCED_FORMAL_PARAMETER
        #define CODA_OSS_FUNCTION_CALL_MISSING_ARG_LIST
        #define CODA_OSS_DISABLE_UNREACHABLE_CODE
    #else
        #define CODA_OSS_disable_warning_push
        #define CODA_OSS_disable_warning_pop
        #define CODA_OSS_disable_warning(warningName)
        #define CODA_OSS_disable_warning_system_header_push
        #define CODA_OSS_FUNCTION_CALL_MISSING_ARG_LIST
        #define CODA_OSS_DISABLE_UNREACHABLE_CODE
    #endif
#endif // CODA_OSS_HAVE_DISABLE_WARNINGS

#ifndef CODA_OSS_mark_symbol_unused
    // Fix unused symbol warnings that crash Release build on -Werror
    // (won't work without C-style cast)
    // https://stackoverflow.com/a/777359/5401366
    #define CODA_OSS_mark_symbol_unused(x) do { \
        CODA_OSS_disable_warning_push \
        CODA_OSS_FUNCTION_CALL_MISSING_ARG_LIST \
        ((void)x); \
        CODA_OSS_disable_warning_pop \
    } while (0);
#endif

#if !defined(CODA_OSS_library_export) && !defined(CODA_OSS_library_import)

    #if defined(__GNUC__) // && HAVE_VISIBILITY 
        // https://www.gnu.org/software/gnulib/manual/html_node/Exported-Symbols-of-Shared-Libraries.html
        #define CODA_OSS_library_export __attribute__((visibility("default")))

        // For GCC, there's no difference in consuming ("importing") an .a or .so
        #define CODA_OSS_library_import /* no __declspec(dllimport) for GCC */

    #elif defined(_MSC_VER) // && (defined(_WINDLL) && !defined(_LIB))
        #define CODA_OSS_library_export __declspec(dllexport)

        // Actually, it seems that the linker is able to figure this out from the .LIB,
        // so there doesn't seem to be a need for __declspec(dllimport).  Clients don't
        // need to #define NITRO_NITFCPP_DLL ... ?  Well, almost ... it looks
        // like __declspec(dllimport) is needed to get virtual "inline"s (e.g.,
        // destructors) correct.
        #define CODA_OSS_library_import __declspec(dllimport)

    #else
        // https://stackoverflow.com/a/2164853/8877
        #define CODA_OSS_library_export /* do nothing and hope for the best? */
        #define CODA_OSS_library_import /* do nothing and hope for the best? */
        #pragma warning Unknown dynamic link import semantics.
    #endif

#endif

#endif  // CODA_OSS_config_compiler_extentions_h_INCLUDED_

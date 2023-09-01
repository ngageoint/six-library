/* =========================================================================
 * This file is part of six-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * six-c++ is free software; you can redistribute it and/or modify
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
#pragma once
#ifndef SIX_six_Complex_h_INCLUDED_
#define SIX_six_Complex_h_INCLUDED_

#include <stdint.h>

#include <types/Complex.h>
#include "scene/Types.h"

namespace six
{
    using zfloat = scene::zfloat;
    using zdouble = scene::zdouble;

    // `std::complex<short>` is (no longer) valid C++, some compilers issue warnings.
    // Since SIX is the primary "culprit," let it sort things out using infrastrcture from CODA-OSS.

    #ifndef SIX_six_unique_ComplexInteger
    // Preserve existing behavior where `std::complex<short>` is used.
    #define SIX_six_unique_ComplexInteger 0 // TODO: set to 1
    #endif
    template<typename T>
    #if SIX_six_unique_ComplexInteger
    using ComplexInteger = types::Complex<T>;
    #else
    using ComplexInteger = std::complex<T>;
    #endif

    using zint16_t = ComplexInteger<int16_t>;
}

#endif // SIX_six_Complex_h_INCLUDED_

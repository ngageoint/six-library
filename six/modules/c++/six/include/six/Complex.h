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
    #if CODA_OSS_types_unique_ComplexInteger
    using zint16_t = std::complex<int16_t>; // TODO: types::zint16_t;
    #else
    using zint16_t = types::zint16_t;
    #endif
}

#endif // SIX_six_Complex_h_INCLUDED_

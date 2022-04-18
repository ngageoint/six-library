/* =========================================================================
 * This file is part of gsl-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 * (C) Copyright 2021, Maxar Technologies, Inc.
 *
 * gsl-c++ is free software; you can redistribute it and/or modify
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
#ifndef CODA_OSS_gsl_Gsl__h_INCLUDED_
#define CODA_OSS_gsl_Gsl__h_INCLUDED_
#pragma once

#include <utility>

#include "gsl/Gsl_narrow.h"

#include "gsl/use_gsl.h" // Can't compile all of GSL with older versions of GCC/MSVC
#if !CODA_OSS_gsl_use_real_gsl_
// Add to "gsl" if we're not using the real thing
namespace gsl
{
    template <class T, class U>
    constexpr T narrow_cast(U&& u) noexcept
    {
        return Gsl::narrow_cast<T>(std::forward<U>(u));
    }

   template <class T, class U>
   constexpr T narrow(U u) noexcept(false)
    {
        return Gsl::narrow<T>(u);
    }
 }
#endif // CODA_OSS_gsl_use_real_gsl_

#endif  // CODA_OSS_gsl_Gsl__h_INCLUDED_

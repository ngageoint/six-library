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
#ifndef CODA_OSS_gsl_gsl_span__h_INCLUDED_
#define CODA_OSS_gsl_gsl_span__h_INCLUDED_
#pragma once

#include "mem/Span_.h"

// Need a fairly decent C++ compiler to use the real GSL
#include "gsl/use_gsl.h"

#if CODA_OSS_use_real_gsl
#include "gsl/span"				// span
#else
 namespace gsl
 {
     template <typename T>
     using span = mem::Span<T>;

     template <typename T>
     inline span<T> make_span(T* d, size_t sz)
     {
         return mem::make_Span<T>(d, sz);
     }

     template <typename TContainer>
     inline span<typename TContainer::value_type> make_span(TContainer& c)
     {
         return mem::make_Span(c);
     }
     template <typename TContainer>
     inline span<typename TContainer::value_type> make_span(const TContainer& c)
     {
         return make_span(const_cast<TContainer&>(c));
     }
 }
#endif // CODA_OSS_use_real_gsl

#endif  // CODA_OSS_gsl_gsl_span__h_INCLUDED_

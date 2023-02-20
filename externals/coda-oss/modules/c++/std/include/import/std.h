/* =========================================================================
 * This file is part of std-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 * (C) Copyright 2023, Maxar Technologies, Inc.
 *
 * std-c++ is free software; you can redistribute it and/or modify
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


#ifndef CODA_OSS_import_std_h_INCLUDED_
#define CODA_OSS_import_std_h_INCLUDED_
#pragma once

#include "coda_oss/CPlusPlus.h"
#include "config/disable_compiler_warnings.h"

CODA_OSS_disable_warning_push
#if _MSC_VER
#pragma warning(disable: 4619) // #pragma warning: there is no warning number '...'

#pragma warning(disable: 4710) // '...': function not inlined
#pragma warning(disable: 5027) // '...': move assignment operator was implicitly defined as deleted
#pragma warning(disable: 5026) // '...': move constructor was implicitly defined as deleted
#pragma warning(disable: 4626) // '...': assignment operator was implicitly defined as deleted
#pragma warning(disable: 4625) // '...': copy constructor was implicitly defined as deleted
#pragma warning(disable: 4355) // '...': used in base member initializer list
#pragma warning(disable: 5220) // '...': a non-static data member with a volatile qualified type no longer implies that compiler generated copy/move constructors and copy/move assignment operators are not trivial
#pragma warning(disable: 5204) // '...': class has virtual functions, but its trivial destructor is not virtual; instances of objects derived from this class may not be destructed correctly
#pragma warning(disable: 5264)  // '...': '...' variable is not used
#endif // _MSC_VER

// Common C++14 headers
#include "cpp14.h"

#if CODA_OSS_cpp17
#include "cpp17.h"
#endif

CODA_OSS_disable_warning_pop

#endif // CODA_OSS_import_std_h_INCLUDED_



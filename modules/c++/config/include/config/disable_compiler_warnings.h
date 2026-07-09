/* =========================================================================
 * This file is part of config-c++
 * =========================================================================
 *
 * (C) Copyright 2023, Maxar Technologies, Inc.
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
#ifndef CODA_OSS_config_disable_compiler_warnings_h_INCLUDED_
#define CODA_OSS_config_disable_compiler_warnings_h_INCLUDED_
#pragma once

#include "compiler_extensions.h"

#if defined(_MSC_VER)

// We don't care about any padding added to structs
#pragma warning(disable: 4820) //  '...': '...' bytes padding added after data member '...'

// Assume any unreferenced functions will be used in other code
#pragma warning(disable: 4514) // '...': unreferenced inline function has been removed

// ???
#pragma warning(disable: 4668) // '...' is not defined as a preprocessor macro, replacing with '...' for '...'

// ???
#pragma warning(disable: 5045) // Compiler will insert Spectre mitigation for memory load if /Qspectre switch specified

// Different versions of Visual Studio have different warnings; and almost all of our
// use of them is `disable`.  It not very useful to know we're (attempting) to
// disable a warning that doesn't exist.
#pragma warning(disable: 4619) // #pragma warning: there is no warning number '...'

#elif defined(__GNUC__) || defined(__clang__)

// Don't care about compatibility between different -std=c++nn values.
CODA_OSS_disable_warning(-Wnoexcept-type)

#endif // _MSC_VER

#endif  // CODA_OSS_config_disable_compiler_warnings_h_INCLUDED_

/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * NITRO is free software; you can redistribute it and/or modify
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
 * License along with this program; if not, If not,
 * see <http://www.gnu.org/licenses/>.
 *
 */

#pragma once

#include "config/disable_compiler_warnings.h"

#include "import/std.h"
#include <std/bit>
#include <std/cstddef>
#include <std/filesystem>
#include <std/span>
#include <std/optional>

#include <config/Exports.h> // need #pragma warning(disable) outside push/pop
CODA_OSS_disable_warning_push
#if _MSC_VER
#pragma warning(disable: 26432) // If you define or delete any default operation in the type '...', define or delete them all(c.21).
#pragma warning(disable: 26447) // The function is declared '...' but calls function '..' which may throw exceptions (f.6).
#pragma warning(disable: 26440) // Function '...' can be declared '...' (f.6).
#pragma warning(disable: 26481) // Don't use pointer arithmetic. Use span instead (bounds.1).
#pragma warning(disable: 26455) // Default constructor may not throw. Declare it '...' (f.6).
#pragma warning(disable: 26446) // Prefer to use gsl::at() instead of unchecked subscript operator (bounds.4)
#pragma warning(disable: 26433) // Function '...' should be marked with '...' (c.128).

// TODO: get rid of these someday? ... from Visual Studio code-analysis
#pragma warning(disable: 26434) // Function '...' hides a non-virtual function '...' (c.128).
#pragma warning(disable: 26456) // Operator '...' hides a non-virtual operator '...' (c.128).
#pragma warning(disable: 26487) // Don't return a pointer '...' that may be invalid(lifetime.4).
#endif // _MSC_VER

#include <sys/Conf.h>
#include <except/Throwable.h>
#include <import/str.h>
#include <import/sys.h>
#include <import/mt.h>
#include <import/io.h>
#include <io/ReadUtils.h>
#include <io/TempFile.h>
#include <import/types.h>
#include <import/mem.h>
#include <import/math.h>
#include <gsl/gsl.h>
#include <sys/Span.h>
#include <import/sio/lite.h>

CODA_OSS_disable_warning_pop

/* =========================================================================
 * This file is part of sys-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * sys-c++ is free software; you can redistribute it and/or modify
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

#ifndef CODA_OSS_sys_String_h_INCLUDED_
#define CODA_OSS_sys_String_h_INCLUDED_
#pragma once

#include <string>

#include "str/String_.h"

#if !defined(CODA_OSS_sys_U8string_DEFINED_)
#define CODA_OSS_sys_U8string_DEFINED_ 1
namespace sys
{
	using Char8_T = char8_t;
	using U8string = std::u8string;
}
#endif  // CODA_OSS_sys_U8string_DEFINED_

#endif  // CODA_OSS_sys_String_h_INCLUDED_

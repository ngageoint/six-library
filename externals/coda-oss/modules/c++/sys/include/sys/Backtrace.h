/* =========================================================================
 * This file is part of sys-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 * (C) Copyright 2021, Maxar Technologies, Inc.
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

#ifndef CODA_OSS_sys_Backtrace_h_INCLUDED_
#define CODA_OSS_sys_Backtrace_h_INCLUDED_
#pragma once

#include <string>
#include <vector>

#include "except/Backtrace.h"

#define CODA_OSS_sys_Backtrace CODA_OSS_except_Backtrace
namespace version { namespace sys {
constexpr auto backtrace = version::except::backtrace;
} }

namespace sys
{
/*! 
 * Returns a human-readable string describing the current stack of
 * function calls. Usefulness and format may vary depending on 
 * your platform and what kind of symbols are compiled in.
 *
 * Currently only supported on *nix with glibc and Windows. This function will
 * return with an error message instead of a backtrace if the current
 * configuration is unsupported.
 */
std::string getBacktrace(bool* pSupported = nullptr);
std::string getBacktrace(bool& supported, std::vector<std::string>& frames);
}

#endif // CODA_OSS_sys_Backtrace_h_INCLUDED_

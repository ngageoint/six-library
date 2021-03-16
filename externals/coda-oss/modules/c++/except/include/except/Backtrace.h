/* =========================================================================
 * This file is part of except-c++ 
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

#ifndef CODA_OSS_except_Backtrace_h_INCLUDED_
#define CODA_OSS_except_Backtrace_h_INCLUDED_
#pragma once

#include <string>
#include <vector>

// We know at compile-time whether except::getBacktrace() is supported.
#if defined(__GNUC__)
// https://man7.org/linux/man-pages/man3/backtrace.3.html
// "These functions are GNU extensions."
#define CODA_OSS_except_Backtrace 20210216L
#elif _WIN32
#define CODA_OSS_except_Backtrace 20210216L
#else
#define CODA_OSS_except_Backtrace 0
#endif 

namespace version { namespace except {
constexpr auto backtrace = CODA_OSS_except_Backtrace;
} }

namespace except
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
std::string getBacktrace(bool& supported, std::vector<std::string>& frames);
}

#endif // CODA_OSS_except_Backtrace_h_INCLUDED_

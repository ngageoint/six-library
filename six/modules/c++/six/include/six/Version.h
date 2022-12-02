/* =========================================================================
 * This file is part of six-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 * (C) Copyright 2021, Maxar Technologies, Inc.
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
#ifndef SIX_six_Version_h_INCLUDED_
#define SIX_six_Version_h_INCLUDED_
#pragma once

#include "config/Version.h"
#include "nitf/Version.hpp"

 // SIX 3.2.1	2022-Nov-04
 // SIX 3.2.0	2022-Aug-30 (C++14)
// SIX 3.1.14	2022-Aug-30 
// SIX 3.1.13	2022-Aug-02
// SIX 3.1.12	2022-Jun-29
// SIX 3.1.11	2022-May-13
// SIX 3.1.10	2022-May-03
// SIX 3.1.9	2022-Feb-02
#define SIX_VERSION_MAJOR	3
#define SIX_VERSION_MINOR	2
#define SIX_VERSION_PATCH	1
//#define SIX_VERSION_BUILD	0
//#define SIX_VERSION CODA_OSS_MAKE_VERSION_MMPB(SIX_VERSION_MAJOR, SIX_VERSION_MINOR, SIX_VERSION_PATCH, SIX_VERSION_BUILD)
#define SIX_VERSION CODA_OSS_MAKE_VERSION_MMP(SIX_VERSION_MAJOR, SIX_VERSION_MINOR, SIX_VERSION_PATCH)

namespace six
{
	constexpr auto version = SIX_VERSION;
	constexpr auto version_major = CODA_OSS_GET_VERSION_MAJOR(SIX_VERSION);
	constexpr auto version_minor = CODA_OSS_GET_VERSION_MINOR(SIX_VERSION);
	constexpr auto version_patch = CODA_OSS_GET_VERSION_PATCH(SIX_VERSION);
	//constexpr auto version_build = CODA_OSS_GET_VERSION_BUILD(SIX_VERSION);
}

#endif // SIX_six_Version_h_INCLUDED_

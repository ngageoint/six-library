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
#ifndef SIX_Version_h_INCLUDED_
#define SIX_Version_h_INCLUDED_
#pragma once

#include "config/Version.h"
#include "nitf/Version.hpp"

#define SIX_VERSION_MAJOR	3
#define SIX_VERSION_MINOR	1
#define SIX_VERSION_PATCH	7
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

#endif // SIX_Version_h_INCLUDED_

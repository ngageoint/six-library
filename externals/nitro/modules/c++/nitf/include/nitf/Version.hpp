/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 * (C) Copyright 2021, Maxar Technologies, Inc.
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
 * License along with this program; If not,
 * see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef NITF_Version_hpp_INCLUDED_
#define NITF_Version_hpp_INCLUDED_

#include "config/Version.h"
#include "nitf/Version.h"

// 2.10.7	December 13, 2021
// 2.10.8	February 22, 2022
// 2.10.9	May 3, 2022
// 2.10.10	June 29, 2022
// 2.10.11	August 2, 2022
 // 2.10.12	August 30, 2022
 // 2.11.0	August 30, 2022 (C++14)
 // 2.11.1	November 4, 2022
 // 2.11.2	December 14, 2022
 // 2.11.3	June 5, 2023
 // 2.11.4	August 18, 2023
 // 2.11.5	October 23, 2023
 // 2.11.6	March 18, 2024
#define NITF_VERSION_MAJOR	2
#define NITF_VERSION_MINOR	11
#define NITF_VERSION_PATCH	6  // a.k.a. "point," but too similar to "patch."
#define NITF_VERSION_BUILD		0 // a.k.a. "patch," but too similar to "point."
#define NITF_VERSION CODA_OSS_MAKE_VERSION_MMPB(NITF_VERSION_MAJOR, NITF_VERSION_MINOR, NITF_VERSION_PATCH, NITF_VERSION_BUILD)

namespace nitf
{
	constexpr auto version = NITF_VERSION;
	constexpr auto version_major = CODA_OSS_GET_VERSION_MAJOR(NITF_VERSION);
	constexpr auto version_minor = CODA_OSS_GET_VERSION_MINOR(NITF_VERSION);
	constexpr auto version_patch = CODA_OSS_GET_VERSION_PATCH(NITF_VERSION);
	constexpr auto version_build = CODA_OSS_GET_VERSION_BUILD(NITF_VERSION);
}

#endif // NITF_Version_hpp_INCLUDED_

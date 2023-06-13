/* =========================================================================
 * This file is part of config-c++
 * =========================================================================
 *
 * (C) Copyright 2021, Maxar Technologies, Inc.
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
#ifndef CODA_OSS_config_Version_h_INCLUDED_
#define CODA_OSS_config_Version_h_INCLUDED_
#pragma once

#include <stdint.h>

// Using 1'0000 makes it easy to see the version number in the IDE.
// Note that 1'000 or even 100 won't work of because 2019 (below) ... and it's nice having all fields be four digits
#define CODA_OSS_VERSION_major_ static_cast<uint64_t>(1000000000000) // 1'0000'0000'0000
#define CODA_OSS_VERSION_minor_ static_cast <uint64_t>(100000000) // 1'0000'0000
#define CODA_OSS_VERSION_patch_ static_cast <uint64_t>(10000) // 1'0000
#define CODA_OSS_MAKE_VERSION_MMPB(major, minor, patch, build) (major * CODA_OSS_VERSION_major_) + (minor * CODA_OSS_VERSION_minor_)  + (patch * CODA_OSS_VERSION_patch_) + build
static_assert(CODA_OSS_MAKE_VERSION_MMPB(9999, 9999, 9999, 9999) <= UINT64_MAX, "version calculation is wrong.");
#define CODA_OSS_MAKE_VERSION_MMP(major, minor, patch) CODA_OSS_MAKE_VERSION_MMPB(major, minor, patch, 0 /*build*/)

#define CODA_OSS_GET_VERSION_MAJOR(version) ((version) / CODA_OSS_VERSION_major_)
#define CODA_OSS_GET_VERSION_(version, factor) (((version) / factor) % static_cast<uint64_t>(10000))
#define CODA_OSS_GET_VERSION_MINOR(version) CODA_OSS_GET_VERSION_(version, CODA_OSS_VERSION_minor_)
#define CODA_OSS_GET_VERSION_PATCH(version) CODA_OSS_GET_VERSION_(version, CODA_OSS_VERSION_patch_)
#define CODA_OSS_GET_VERSION_BUILD(version) CODA_OSS_GET_VERSION_(version, 1 /*factor*/)

// Do this ala C++ ... we don't currently have major/minor/patch
//#define CODA_OSS_VERSION_ 20210910L // c.f. __cplusplus
#define CODA_OSS_VERSION_ 2023 ## 0006 ## 0005 ## 0000 ## L

// Use the same macros other projects might want to use; overkill for us.
#define CODA_OSS_VERSION_MAJOR	2023
#define CODA_OSS_VERSION_MINOR	6
#define CODA_OSS_VERSION_PATCH	5
#define CODA_OSS_VERSION_BUILD	0
#define CODA_OSS_VERSION CODA_OSS_MAKE_VERSION_MMPB(CODA_OSS_VERSION_MAJOR, CODA_OSS_VERSION_MINOR, CODA_OSS_VERSION_PATCH, CODA_OSS_VERSION_BUILD)

namespace config
{
constexpr auto coda_oss_version = CODA_OSS_VERSION;
constexpr auto coda_oss_version_major = CODA_OSS_GET_VERSION_MAJOR(CODA_OSS_VERSION);
constexpr auto coda_oss_version_minor = CODA_OSS_GET_VERSION_MINOR(CODA_OSS_VERSION);
constexpr auto coda_oss_version_patch = CODA_OSS_GET_VERSION_PATCH(CODA_OSS_VERSION);
constexpr auto coda_oss_version_build = CODA_OSS_GET_VERSION_BUILD(CODA_OSS_VERSION);

// Be sure the macros above are correct
static_assert(CODA_OSS_VERSION == CODA_OSS_VERSION_, "version calculation is wrong.");
static_assert(CODA_OSS_VERSION == coda_oss_version, "version calculation is wrong.");
static_assert(CODA_OSS_VERSION_MAJOR == coda_oss_version_major, "version calculation is wrong.");
static_assert(CODA_OSS_VERSION_MINOR ==coda_oss_version_minor, "version calculation is wrong.");
static_assert(CODA_OSS_VERSION_PATCH == coda_oss_version_patch, "version calculation is wrong.");
static_assert(CODA_OSS_VERSION_BUILD == coda_oss_version_build, "version calculation is wrong.");
}

#endif  // CODA_OSS_config_Version_h_INCLUDED_
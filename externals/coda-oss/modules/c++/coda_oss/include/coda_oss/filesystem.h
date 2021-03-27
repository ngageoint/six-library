/* =========================================================================
 * This file is part of sys-c++
 * =========================================================================
 *
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
 * License along with this program; If not, http://www.gnu.org/licenses/.
 *
 */
#ifndef CODA_OSS_coda_oss_filesystem_h_INCLUDED_
#define CODA_OSS_coda_oss_filesystem_h_INCLUDED_
#pragma once

#include "sys/CPlusPlus.h"
#if CODA_OSS_cpp17 || (defined(CODA_OSS_AUGMENT_std_filesystem) && (CODA_OSS_AUGMENT_std_filesystem == 1))
#include <std/filesystem>
#define CODA_OSS_coda_oss_std_filesystem_ 1
#endif
#include "sys/Filesystem.h"

namespace coda_oss
{
namespace filesystem
{
#ifdef CODA_OSS_coda_oss_std_filesystem_
using path = std::filesystem::path;
#else
using path = sys::Filesystem::path;
#endif
}
}


#endif  // CODA_OSS_coda_oss_filesystem_h_INCLUDED_
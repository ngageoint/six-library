/* =========================================================================
 * This file is part of sys-c++
 * =========================================================================
 *
 * (C) Copyright 2020, Maxar Technologies, Inc.
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
#ifndef CODA_OSS_sys_filesystem_h_INCLUDED_
#define CODA_OSS_sys_filesystem_h_INCLUDED_
#pragma once

// always implement sys::filesystem::path
#include "sys/sys_filesystem.h"

#include "sys/CPlusPlus.h"
#if CODA_OSS_cpp17
	// Some versions of G++ say they're C++17 but don't have <filesystem>
	#if __has_include(<filesystem>)  // __has_include is C++17
		#include <filesystem>
		#define CODA_OSS_has_std_filesystem_ 1
	#endif
#else
	#define CODA_OSS_has_std_filesystem_ 0
#endif

namespace coda_oss
{
	// Allow coda_oss::filesystem to compile; used in .h files
	namespace filesystem
	{
		#if CODA_OSS_has_std_filesystem_
		#define CODA_OSS_coda_oss_filesystem_impl_ std
		#else
		#define CODA_OSS_coda_oss_filesystem_impl_ sys
		#endif
		using CODA_OSS_coda_oss_filesystem_impl_::filesystem::file_type;
		using CODA_OSS_coda_oss_filesystem_impl_::filesystem::path;

		using CODA_OSS_coda_oss_filesystem_impl_::filesystem::absolute;
		using CODA_OSS_coda_oss_filesystem_impl_::filesystem::create_directory;
		using CODA_OSS_coda_oss_filesystem_impl_::filesystem::current_path;
		using CODA_OSS_coda_oss_filesystem_impl_::filesystem::remove;
		using CODA_OSS_coda_oss_filesystem_impl_::filesystem::temp_directory_path;
		using CODA_OSS_coda_oss_filesystem_impl_::filesystem::is_regular_file;
		using CODA_OSS_coda_oss_filesystem_impl_::filesystem::is_directory;
		using CODA_OSS_coda_oss_filesystem_impl_::filesystem::exists;
		using CODA_OSS_coda_oss_filesystem_impl_::filesystem::file_size;
	}
}

#endif  // CODA_OSS_sys_filesystem_h_INCLUDED_

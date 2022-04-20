/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2022, Maxar Technologies, Inc.
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

#ifndef NITRO_nitf_UnitTests_hpp_INCLUDED_
#define NITRO_nitf_UnitTests_hpp_INCLUDED_
#pragma once

#include <string>
#include <std/filesystem>

#include "nitf/exports.hpp"

namespace nitf
{
	namespace Test
	{
		NITRO_NITFCPP_API std::string buildPluginsDir();
		NITRO_NITFCPP_API std::filesystem::path buildFileDir(const std::filesystem::path& relativePath);
		NITRO_NITFCPP_API std::filesystem::path findInputFile(const std::filesystem::path&);
	}
}

#endif // NITRO_nitf_UnitTests_hpp_INCLUDED_

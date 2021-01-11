/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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

#ifndef __NITF_SYSTEM_HPP__
#define __NITF_SYSTEM_HPP__
#pragma once

/*!
 *  \file System.hpp
 */

#include <stdint.h>

#include "nitf/System.h"
#include "nitf/Field.h"
#include "nitf/Types.h"

#include "sys/CStdDef.h"
#include "sys/Conf.h"

namespace nitf
{
	// Keeping these here so that code using "nitf::Uint64" still compiles;
	// we can't make others change to "uint64_t".
	using Uint64 = uint64_t;
	using Uint32 = uint32_t;
	using Uint16 = uint16_t;
	using Uint8 = uint8_t;
	using Int64 = int64_t;
	using Int32 = int32_t;
	using Int16 = int16_t;
	using Int8 = int8_t;

typedef nitf_Off Off;
typedef nitf_Version Version;
typedef nitf_ConvType ConvType;
typedef nitf_FieldType FieldType;
typedef nitf_AccessFlags AccessFlags;
typedef nitf_CreationFlags CreationFlags;
typedef nitf_CornersType CornersType;

#if CODA_OSS_cpp17
using byte = nitf::byte;
#else
using byte = sys::byte;
#endif

}
#endif

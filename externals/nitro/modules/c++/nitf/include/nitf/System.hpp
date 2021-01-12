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
typedef nitf_AccessFlags AccessFlags;
typedef nitf_CreationFlags CreationFlags;

using Version = nitf_Version;
using FieldType = nitf_FieldType;
using ConvType = nitf_ConvType;
using CornersType = nitf_CornersType;
}

#ifndef SWIGPYTHON
// These are already global via C-style enum for SWIG
constexpr auto NITF_VER_20 = nitf::Version::NITF_VER_20;
constexpr auto NITF_VER_21 = nitf::Version::NITF_VER_21;
constexpr auto NITF_VER_UNKNOWN = nitf::Version::NITF_VER_UNKNOWN;

constexpr auto NITF_BCS_A = nitf::FieldType::NITF_BCS_A;
constexpr auto NITF_BCS_N = nitf::FieldType::NITF_BCS_N;
constexpr auto NITF_BINARY = nitf::FieldType::NITF_BINARY;

constexpr auto NITF_CONV_UINT = nitf::ConvType::NITF_CONV_UINT;
constexpr auto NITF_CONV_INT = nitf::ConvType::NITF_CONV_INT;
constexpr auto NITF_CONV_REAL = nitf::ConvType::NITF_CONV_REAL;
constexpr auto NITF_CONV_STRING = nitf::ConvType::NITF_CONV_STRING;
constexpr auto NITF_CONV_RAW = nitf::ConvType::NITF_CONV_RAW;

constexpr auto NRT_CORNERS_UNKNOWN = nitf::CornersType::NRT_CORNERS_UNKNOWN;
constexpr auto NRT_CORNERS_UTM = nitf::CornersType::NRT_CORNERS_UTM;
constexpr auto NRT_CORNERS_UTM_UPS_S = nitf::CornersType::NRT_CORNERS_UTM_UPS_S;
constexpr auto NRT_CORNERS_UTM_UPS_N = nitf::CornersType::NRT_CORNERS_UTM_UPS_N;
constexpr auto NRT_CORNERS_GEO = nitf::CornersType::NRT_CORNERS_GEO;
constexpr auto NRT_CORNERS_DECIMAL = nitf::CornersType::NRT_CORNERS_DECIMAL;
#endif

#endif

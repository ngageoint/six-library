/* =========================================================================
 * This file is part of str-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 * (C) Copyright 2020, 2021, 2022, 2023, Maxar Technologies, Inc.
 *
 * str-c++ is free software; you can redistribute it and/or modify
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

#pragma once
#ifndef CODA_OSS_str_W1252string_h_INCLUDED_
#define CODA_OSS_str_W1252string_h_INCLUDED_

#include <wchar.h>
#include <stdint.h>

#include <string>
#include <type_traits>

namespace str
{

// This is to make it difficult to get encodings mixed up; it's here (in a .h
// file) as we want to unit-test it. Windows1252_T for Windows-1252 characters
enum class Windows1252_T : unsigned char { };  // https://en.cppreference.com/w/cpp/language/types
using W1252string = std::basic_string<Windows1252_T>;  // https://en.cppreference.com/w/cpp/string

//////////////////////////////////////////////////////////////////////////////////////////

// We'll get strange errors, possibibly at link-time, if wchar_t is not a wchar_t type.
// MSVC has an option to control this: https://docs.microsoft.com/en-us/cpp/build/reference/zc-wchar-t-wchar-t-is-native-type
// https://en.cppreference.com/w/cpp/language/types
// "It has the same size, signedness, and alignment as one of the integer types, but is a distinct type."
static_assert(!std::is_same<wchar_t, uint16_t>::value, "wchar_t should not be the same as uint16_t");
static_assert(!std::is_same<wchar_t, int16_t>::value, "wchar_t should not be the same as int16_t");
static_assert(!std::is_same<wchar_t, uint32_t>::value, "wchar_t should not be the same as uint32_t");
static_assert(!std::is_same<wchar_t, int32_t>::value, "wchar_t should not be the same as int32_t");

}

#endif // CODA_OSS_str_W1252string_h_INCLUDED_

/* =========================================================================
 * This file is part of tiff-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
  * © Copyright 2023, Maxar Technologies, Inc.
 *
 * tiff-c++ is free software; you can redistribute it and/or modify
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

#include <stdarg.h>
#include <iostream>
#include <string>

#include <sys/filesystem.h>
#include <coda_oss/cstddef.h>

using tiff_errorhandler_t = void(*)(const char*, const char*, va_list); // TIFFErrorHandler
tiff_errorhandler_t tiff_setErrorHandler(tiff_errorhandler_t pHandler = nullptr); // TIFFSetWarningHandler()
tiff_errorhandler_t tiff_setWarningHandler(tiff_errorhandler_t pHandler = nullptr); // TIFFSetErrorHandler()

bool tiff_readData(const coda_oss::filesystem::path&, coda_oss::byte* buffer, size_t numElements);

struct tiff_stream_ final { };
using tiff_stream = tiff_stream_*; // TIFF

tiff_stream tiff_streamOpen(const std::string& name, std::istream&); // TIFFStreamOpen()
bool tiff_readData(tiff_stream, coda_oss::byte* buffer, size_t numElements);


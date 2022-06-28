/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2016, MDA Information Systems LLC
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

#include <assert.h>

#include <stdexcept>

#include "nitf/Utils.hpp"

using namespace nitf;

bool Utils::isNumeric(const std::string& str) noexcept
{
    return nitf_Utils_isNumeric(str.c_str()) ? true : false;
}

bool Utils::isAlpha(const std::string& str) noexcept
{
    return nitf_Utils_isAlpha(str.c_str()) ? true : false;
}

void Utils::decimalToGeographic(double decimal, int* degrees, int* minutes,
                                double* seconds) noexcept
{
    nitf_Utils_decimalToGeographic(decimal, degrees, minutes, seconds);
}

double Utils::geographicToDecimal(int degrees, int minutes, double seconds) noexcept
{
    return nitf_Utils_geographicToDecimal(degrees, minutes, seconds);
}

char Utils::cornersTypeAsCoordRep(nitf::CornersType type) noexcept
{
    return nitf_Utils_cornersTypeAsCoordRep(type);
}

void Utils::error_init(nrt_Error& error, const std::string& message,
    const char* file, int line, const char* func, int level) noexcept
{
    nrt_Error_init(&error, message.c_str(), file, line, func, level);
}
void Utils::error_init(nrt_Error* error, const std::string& message,
    const char* file, int line, const char* func, int level)
{
    if (error == nullptr)
    {
        throw std::invalid_argument("'error' is NULL.");
    }
    error_init(*error, message, file, line, func, level);
}
void Utils::error_init(nrt_Error* error, const std::exception& ex,
    const char* file, int line, const char* func, int level)
{
    assert(error != nullptr);
    nrt_Error_init(error, ex.what(), file, line, func, level);
}
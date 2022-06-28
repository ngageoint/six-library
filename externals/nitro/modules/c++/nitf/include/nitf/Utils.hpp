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
 * License along with this program; If not,
 * see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef __NITF_UTILS_HPP__
#define __NITF_UTILS_HPP__

#include <string>
#include <exception>

#include "nitf/System.hpp"
#include "nitf/exports.hpp"

namespace nitf
{
namespace Utils
{
    extern NITRO_NITFCPP_API bool isNumeric(const std::string&) noexcept;

    extern NITRO_NITFCPP_API bool isAlpha(const std::string&) noexcept;

    extern NITRO_NITFCPP_API void decimalToGeographic(double decimal, int* degrees, int* minutes,
                                    double* seconds) noexcept;

    extern NITRO_NITFCPP_API double geographicToDecimal(int degrees, int minutes, double seconds) noexcept;

    extern NITRO_NITFCPP_API char cornersTypeAsCoordRep(nitf::CornersType type) noexcept;

    extern NITRO_NITFCPP_API void error_init(nrt_Error& error, const std::string& message,
        const char* file, int line, const char* func, int level) noexcept;
    extern NITRO_NITFCPP_API void error_init(nrt_Error* error, const std::string& message,
        const char* file, int line, const char* func, int level);
    extern NITRO_NITFCPP_API void error_init(nrt_Error* error, const std::exception&,
        const char* file, int line, const char* func, int level);
};

}

#endif

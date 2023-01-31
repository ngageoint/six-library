/* =========================================================================
 * This file is part of six.sidd30-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * six.sicd-c++ is free software; you can redistribute it and/or modify
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

#ifndef SIX_six_sidd30_CropUtils_h_INCLUDED_
#define SIX_six_sidd30_CropUtils_h_INCLUDED_
#pragma once

#include <std/filesystem>

#include <six/sidd/CropUtils.h>

namespace six
{
    namespace sidd30
    {
        inline void cropSIDD(const std::filesystem::path& inPathname,
            const std::vector<std::string>& schemaPaths,
            const types::RowCol<size_t>& aoiOffset,
            const types::RowCol<size_t>& aoiDims,
            const std::filesystem::path& outPathname)
        {
            return six::sidd::cropSIDD(inPathname.string(), schemaPaths, aoiOffset, aoiDims, outPathname.string());
        }
    }
}

#endif // SIX_six_sidd30_CropUtils_h_INCLUDED_

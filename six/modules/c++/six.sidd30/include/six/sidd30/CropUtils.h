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

#ifndef __SIX_SIDD_CROP_UTILS_H__
#define __SIX_SIDD_CROP_UTILS_H__

#include <string>
#include <vector>

#include <scene/sys_Conf.h>
#include <types/RowCol.h>

namespace six
{
namespace sidd
{
/*
 * Reads in an AOI from a SIDD and creates a cropped SIDD, updating the
 * metadata as appropriate to reflect this
 *
 * TODO: The SIDD standard supports more complicated chipping than this -
 * you can translate, rotate, and/or scale.
 *
 * \param inPathname Input SIDD pathname
 * \param schemaPaths Schema paths to use for reading and writing
 * \param aoiOffset Upper left corner of AOI
 * \param aoiDims Size of AOI
 * \param outPathname Output cropped SIDD pathname
 */
void cropSIDD(const std::string& inPathname,
              const std::vector<std::string>& schemaPaths,
              const types::RowCol<size_t>& aoiOffset,
              const types::RowCol<size_t>& aoiDims,
              const std::string& outPathname);
}
}

#endif

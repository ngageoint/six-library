/* =========================================================================
 * This file is part of six.sicd-c++
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
#pragma once
#ifndef __SIX_SICD_CROP_UTILS_H__
#define __SIX_SICD_CROP_UTILS_H__

#include <string>
#include <vector>

#include <scene/Types.h>
#include <six/NITFReadControl.h>
#include <six/sicd/ComplexData.h>
#include <six/sicd/Exports.h>

namespace six
{
namespace sicd
{
/*
 * Reads in an AOI from a SICD and creates a cropped SICD, updating the
 * metadata as appropriate to reflect this
 *
 * \param inPathname Input SICD pathname
 * \param schemaPaths Schema paths to use for reading and writing
 * \param aoiOffset Upper left corner of AOI
 * \param aoiDims Size of AOI
 * \param outPathname Output cropped SICD pathname
 */
SIX_SICD_API void cropSICD(const std::string& inPathname,
              const std::vector<std::string>& schemaPaths,
              const types::RowCol<size_t>& aoiOffset,
              const types::RowCol<size_t>& aoiDims,
              const std::string& outPathname);

/*
 * Same as above but allow an already-opened reader to be used.
 * NITFReadControl::load() must be called prior to calling this function.
 */
SIX_SICD_API void cropSICD(six::NITFReadControl& reader,
              const std::vector<std::string>& schemaPaths,
              const types::RowCol<size_t>& aoiOffset,
              const types::RowCol<size_t>& aoiDims,
              const std::string& outPathname);

/*
 * Reads in an AOI from a SICD and creates a cropped SICD, updating the
 * metadata as appropriate to reflect this
 *
 * \param inPathname Input SICD pathname
 * \param schemaPaths Schema paths to use for reading and writing
 * \param corners Exactly four corners in ECEF meters.  If the corners are not
 * rectangular in the slant plane, an AOI will be exscribed from these
 * \param outPathname Output cropped SICD pathname
 * \param trimCornersIfNeeded Specifies behavior if one of the AOI corners is
 * outside the image.  If this is true, the corner will be silently trimmed to
 * be in-bounds (and the SICD metadata will reflect this).  If this is false,
 * an exception will be thrown.
 */
SIX_SICD_API void cropSICD(const std::string& inPathname,
              const std::vector<std::string>& schemaPaths,
              const std::vector<scene::Vector3>& corners,
              const std::string& outPathname,
              bool trimCornersIfNeeded = true);

/*
 * Same as above but allow an already-opened reader to be used.
 * NITFReadControl::load() must be called prior to calling this function.
 */
SIX_SICD_API void cropSICD(six::NITFReadControl& reader,
              const std::vector<std::string>& schemaPaths,
              const std::vector<scene::Vector3>& corners,
              const std::string& outPathname,
              bool trimCornersIfNeeded = true);

/*
 * Reads in an AOI from a SICD and creates a cropped SICD, updating the
 * metadata as appropriate to reflect this
 *
 * \param inPathname Input SICD pathname
 * \param schemaPaths Schema paths to use for reading and writing
 * \param corners Exactly four corners in lat/lon.  If the corners are not
 * rectangular in the slant plane, an AOI will be exscribed from these
 * \param outPathname Output cropped SICD pathname
 */
SIX_SICD_API void cropSICD(const std::string& inPathname,
              const std::vector<std::string>& schemaPaths,
              const std::vector<scene::LatLonAlt>& corners,
              const std::string& outPathname,
              bool trimCornersIfNeeded = true);

/*
 * Same as above but allow an already-opened reader to be used.
 * NITFReadControl::load() must be called prior to calling this function.
 */
SIX_SICD_API void cropSICD(six::NITFReadControl& reader,
              const std::vector<std::string>& schemaPaths,
              const std::vector<scene::LatLonAlt>& corners,
              const std::string& outPathname,
              bool trimCornersIfNeeded = true);

/*
 * Given a six::Sicd::ComplexData object and a cropping region,
 * update the metadata in the same way as the functions above.
 * \param complexData the input complexData object
 * \param aoiOffset Upper left corner of AOI
 * \param aoiDims Size of AOI
 *
 * \returns A cloned complex data object where the 
 * meta data has been updated to reflect the cropped aoi.
 */
SIX_SICD_API std::unique_ptr<six::sicd::ComplexData>
cropMetaData(const six::sicd::ComplexData& complexData,
             const types::RowCol<size_t>& aoiOffset,
             const types::RowCol<size_t>& aoiDims);
}
}

#endif

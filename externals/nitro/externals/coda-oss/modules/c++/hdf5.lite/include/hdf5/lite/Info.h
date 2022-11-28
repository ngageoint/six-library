/* =========================================================================
 * This file is part of hdf5.lite-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2022, Maxar Technologies, Inc.
 *
 * hdf5.lite-c++ is free software; you can redistribute it and/or modify
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

#ifndef CODA_OSS_hdf5_lite_Read_h_INCLUDED_
#define CODA_OSS_hdf5_lite_Read_h_INCLUDED_
#pragma once

#include <string>
#include <vector>

#include "config/Exports.h"
#include "sys/filesystem.h"

/*!
 * \file  Read.h
 * \brief HDF File-reading API
 *
 * These are simple routines to read HDF5 files; they're modeled after the MATLab API
 * https://www.mathworks.com/help/matlab/ref/h5info.html
 */
namespace hdf5
{
namespace lite
{
struct GroupInfo final
{
};

struct DatatypeInfo final
{
    std::string name;
    // Class
    // Type
    size_t size = 0;
};

struct DatasetInfo final
{
    std::string name;
    DatatypeInfo datatype;
    // Dataspace
    // ChunkSize
    // FillValue
    // Filter
    // Attributes
};

struct FileInfo final
{
    std::string name;
    std::vector<GroupInfo> groups;
    std::vector<DatasetInfo> datasets;
    std::vector<DatatypeInfo> datatypes;
    // Links
    // Attributes
};


CODA_OSS_API FileInfo fileInfo(const coda_oss::filesystem::path&);
//CODA_OSS_API void locationRead(const std::string& loc, const std::string& datasetName); // e.g, s3://

}
}

#endif // CODA_OSS_hdf5_lite_Read_h_INCLUDED_


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

#ifndef CODA_OSS_hdf5_lite_Info_h_INCLUDED_
#define CODA_OSS_hdf5_lite_Info_h_INCLUDED_
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
enum class Class
{
    NoClass = -1, /**< error                                   */
    Integer = 0, /**< integer types                           */
    Float = 1, /**< floating-point types                    */
    Time = 2, /**< date and time types                     */
    String = 3, /**< character string types                  */
    Bitfield = 4, /**< bit field types                         */
    Opaque = 5, /**< opaque types                            */
    Compound = 6, /**< compound types                          */
    Reference = 7, /**< reference types                         */
    Enum = 8, /**< enumeration types                       */
    Vlen = 9, /**< variable-Length types                   */
    Array = 10, /**< array types                             */
};

// https://docs.hdfgroup.org/hdf5/develop/_h5_d_m__u_g.html
struct NamedObject
{
    std::string filename;  // could be a URL, so not std::filesystem::path
    std::string name;
};

struct DataTypeInfo final : public NamedObject
{
    Class h5Class;
    // Type
    size_t size = 0;
};

struct DataSpaceInfo final
{
};

struct DataSetInfo final : public NamedObject
{
    DataTypeInfo dataType;
    DataSpaceInfo dataSpace;
    // ChunkSize
    // FillValue
    // Filter
    // Attributes
};

struct GroupInfo : public NamedObject
{
    std::vector<GroupInfo> groups;
    std::vector<DataSetInfo> dataSets;
    std::vector<DataTypeInfo> dataTypes;
    // Links
    // Attributes
};

struct FileInfo final : public GroupInfo
{
};


CODA_OSS_API FileInfo fileInfo(const coda_oss::filesystem::path&);
CODA_OSS_API GroupInfo groupInfo(const coda_oss::filesystem::path&, const std::string& loc);
CODA_OSS_API DataSetInfo dataSetInfo(const coda_oss::filesystem::path&, const std::string& loc);

}
}

#endif // CODA_OSS_hdf5_lite_Info_h_INCLUDED_

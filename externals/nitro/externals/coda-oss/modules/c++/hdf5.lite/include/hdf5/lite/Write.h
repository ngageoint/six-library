/* =========================================================================
 * This file is part of hdf5.lite-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2023, Maxar Technologies, Inc.
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

#pragma once
#ifndef CODA_OSS_hdf5_lite_Write_h_INCLUDED_
#define CODA_OSS_hdf5_lite_Write_h_INCLUDED_

 /*!
 * \file  Write.h
 * \brief HDF File-writing API
 *
 * These are simple routines to write HDF5 files; they're loosely modeled after the MATLab API
 * https://www.mathworks.com/help/matlab/ref/h5create.html
 * https://www.mathworks.com/help/matlab/ref/h5write.html
 */

#include <string>
#include <vector>

#include "config/Exports.h"
#include "sys/filesystem.h"
#include "types/RowCol.h"

#include "SpanRC.h"

namespace hdf5
{
namespace lite
{
template<typename TDataSet> // currently implemented for float and double
CODA_OSS_API void createFile(const coda_oss::filesystem::path&, const std::string& ds, const types::RowCol<size_t>&);
template<> CODA_OSS_API void createFile<float>(const coda_oss::filesystem::path&, const std::string& ds, const types::RowCol<size_t>&);
template<> CODA_OSS_API void createFile<double>(const coda_oss::filesystem::path&, const std::string& ds, const types::RowCol<size_t>&);

CODA_OSS_API void createFile(const coda_oss::filesystem::path&, const std::string& ds, SpanRC<const double>);
inline void createFile(const coda_oss::filesystem::path& path, const std::string& ds, SpanRC<double> data_)
{
    SpanRC<const double> data(data_.data(), data_.dims());
    createFile(path, ds, data);
}
CODA_OSS_API void createFile(const coda_oss::filesystem::path&, const std::string& ds, SpanRC<const float>);
inline void createFile(const coda_oss::filesystem::path& path, const std::string& ds, SpanRC<float> data_)
{
    SpanRC<const float> data(data_.data(), data_.dims());
    createFile(path, ds, data);
}

CODA_OSS_API void writeFile(const coda_oss::filesystem::path&, const std::string& loc, SpanRC<const double>);
inline void writeFile(const coda_oss::filesystem::path& path, const std::string& ds, SpanRC<double> data_)
{
    SpanRC<const double> data(data_.data(), data_.dims());
    writeFile(path, ds, data);
}
CODA_OSS_API void writeFile(const coda_oss::filesystem::path&, const std::string& loc, SpanRC<const float>);
inline void writeFile(const coda_oss::filesystem::path& path, const std::string& ds, SpanRC<float> data_)
{
    SpanRC<const float> data(data_.data(), data_.dims());
    writeFile(path, ds, data);
}

}
}

#endif // CODA_OSS_hdf5_lite_Write_h_INCLUDED_

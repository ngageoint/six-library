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

/*!
 * \file  Read.h
 * \brief HDF File-reading API
 *
 * These are simple routines to read HDF5 files; they're loosly modeled after the MATLab API
 * https://www.mathworks.com/help/matlab/import_export/import-hdf5-files.html
 */

#include <string>
#include <vector>

#include "config/Exports.h"
#include "sys/filesystem.h"
#include "types/RowCol.h"

namespace hdf5
{
namespace lite
{

CODA_OSS_API types::RowCol<size_t> readFile(coda_oss::filesystem::path, std::string datasetName, std::vector<double>&);
CODA_OSS_API types::RowCol<size_t> readFile(coda_oss::filesystem::path, std::string datasetName, std::vector<float>&);

}
}

#endif // CODA_OSS_hdf5_lite_Read_h_INCLUDED_


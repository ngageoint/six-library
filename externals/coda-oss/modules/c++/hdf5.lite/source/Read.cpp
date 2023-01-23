/* =========================================================================
 * This file is part of hd5.lite-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2022, Maxar Technologies, Inc.
 *
 * hd5.lite-c++ is free software; you can redistribute it and/or modify
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

#include "hdf5/lite/Read.h"

#include <tuple> // std::ignore
#include <vector>

#include "coda_oss/cstddef.h" // byte

#include "hdf5/lite/HDF5Exception.h"
#include "H5.h"
#include "hdf5.lite.h"

static void read(const H5::DataSet& dataset, std::vector<double>& result)
{
    static_assert(sizeof(double) * 8 == 64, "'double' should be 64-bits"); // IEEE_F64LE

    const auto mem_type = dataset.getDataType();
    if (mem_type != H5::PredType::IEEE_F64LE)
    {
        const except::Context context("getDataType() != IEEE_F64LE", __FILE__, __LINE__, "read");
        throw hdf5::lite::DataSetException(context);
    }
    dataset.read(result.data(), mem_type);
}
static void read(const H5::DataSet& dataset, std::vector<float>& result)
{
    static_assert(sizeof(float) * 8 == 32, "'float' should be 32-bits"); // IEEE_F32LE

    const auto mem_type = dataset.getDataType();
    if (mem_type != H5::PredType::IEEE_F32LE)
    {
        const except::Context context("getDataType() != IEEE_F32LE", __FILE__, __LINE__, "read");
        throw hdf5::lite::DataSetException(context);
    }
    dataset.read(result.data(), mem_type);
}
template<typename T>
static types::RowCol<size_t> readDatasetT(const H5::DataSet& dataset, std::vector<T>& result)
{
    if (dataset.getTypeClass() != H5T_FLOAT)
    {
        const except::Context context("getTypeClass() != H5T_FLOAT", __FILE__, __LINE__, "readDatasetT");
        throw hdf5::lite::DataSetException(context);
    }    

    const auto retval = hdf5::lite::details::getSimpleExtentSize(dataset);
    result.resize(retval.area());
    
    // dataset.read() doesn't care about the buffer type ... that's because H5Dread() also
    // uses void*.  However, the LT API has H5LTread_dataset_double() and  H5LTread_dataset_float(),
    read(dataset, result); // data.read(result.data(), mem_type);

    return retval;
}

inline types::RowCol<size_t> readDataset_(const H5::DataSet& dataset, std::vector<float>& result)
{
    return readDatasetT(dataset, result);
}
inline types::RowCol<size_t> readDataset_(const H5::DataSet& dataset, std::vector<double>& result)
{
    return readDatasetT(dataset, result);
}
template<typename T>
static types::RowCol<size_t> readFile_(const coda_oss::filesystem::path& fileName, const std::string& datasetName,
                                std::vector<T>& result)
{
    /*
     * Open the specified file and the specified dataset in the file.
     */
    H5::H5File file(fileName.string(), H5F_ACC_RDONLY);
    const auto dataset = file.openDataSet(datasetName);
    return readDataset_(dataset, result);
}

types::RowCol<size_t> hdf5::lite::readFile(const coda_oss::filesystem::path& fileName, const std::string& loc,
    std::vector<double>& result)
{
    return details::try_catch_H5Exceptions(readFile_<double>, __FILE__, __LINE__, fileName, loc, result); 
}
types::RowCol<size_t> hdf5::lite::readFile(const coda_oss::filesystem::path& fileName, const std::string& loc,
    std::vector<float>& result)
{
    return details::try_catch_H5Exceptions(readFile_<float>, __FILE__, __LINE__, fileName, loc, result); 
}

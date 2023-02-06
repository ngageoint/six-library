/* =========================================================================
 * This file is part of hd5.lite-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2023, Maxar Technologies, Inc.
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

#include "hdf5/lite/Write.h"

#include <tuple> // std::ignore
#include <vector>

#include "coda_oss/cstddef.h" // byte

#include "hdf5/lite/HDF5Exception.h"
#include "H5.h"
#include "hdf5.lite.h"

// https://raw.githubusercontent.com/HDFGroup/hdf5/develop/c++/examples/h5tutr_rdwt.cpp

template<typename T> static H5::PredType getPredType();
template <>
inline H5::PredType getPredType<float>()
{
    static_assert(sizeof(float) * 8 == 32, "'float' should be 32-bits"); // IEEE_F32LE
    return H5::PredType::IEEE_F32LE;
}
template <>
inline H5::PredType getPredType<double>()
{
    static_assert(sizeof(double) * 8 == 64, "'double' should be 64-bits"); // IEEE_F64LE
    return H5::PredType::IEEE_F64LE;
}

template<typename T>
static void createFile_(const coda_oss::filesystem::path& fileName, const std::string& ds, const types::RowCol<size_t>& sz)
{
    // https://raw.githubusercontent.com/HDFGroup/hdf5/develop/c++/examples/h5tutr_crtdat.cpp
    //
    // Create a new file using the default property lists.
    H5::H5File file(fileName.string(), H5F_ACC_TRUNC);

    // Create the data space for the dataset.
    constexpr int RANK = 2;
    const hsize_t dims[]{sz.row, sz.col};  // dataset dimensions
    H5::DataSpace dataspace(RANK, dims);

    // Create the dataset.
    const auto data_type = getPredType<T>();
    std::ignore = file.createDataSet(ds, data_type, dataspace);
}
template<>
void hdf5::lite::createFile<float>(const coda_oss::filesystem::path& fileName, const std::string& ds, const types::RowCol<size_t>& sz)
{
    details::try_catch_H5ExceptionsV(createFile_<float>, __FILE__, __LINE__, fileName, ds, sz); 
}
template<>
void hdf5::lite::createFile<double>(const coda_oss::filesystem::path& fileName, const std::string& ds, const types::RowCol<size_t>& sz)
{
    details::try_catch_H5ExceptionsV(createFile_<double>, __FILE__, __LINE__, fileName, ds, sz); 
}
void hdf5::lite::createFile(const coda_oss::filesystem::path& fileName, const std::string& ds, hdf5::lite::SpanRC<const float> data)
{
    createFile<float>(fileName, ds, data.dims());
}
void hdf5::lite::createFile(const coda_oss::filesystem::path& fileName, const std::string& ds, hdf5::lite::SpanRC<const double> data)
{
    createFile<double>(fileName, ds, data.dims());
}


template<typename T>
static void writeFile_(const coda_oss::filesystem::path& fileName, const std::string& ds, hdf5::lite::SpanRC<const T> data)
{
    // https://raw.githubusercontent.com/HDFGroup/hdf5/develop/c++/examples/h5tutr_rdwt.cpp
    
    // Open an existing file and dataset.
    H5::H5File file(fileName.string(), H5F_ACC_RDWR);
    auto dataset = file.openDataSet(ds);
    
    const auto dims = hdf5::lite::details::getSimpleExtentSize(dataset);
    if (data.dims() != dims)
    {
        const except::Context ctx("dataSet dimensions do not match data dimensions", __FILE__, __LINE__, "writeFile");
        throw hdf5::lite::DataSetException(ctx);
    }

    // Write the data to the dataset using default memory space, file
    // space, and transfer properties.
    const auto data_type = getPredType<T>();
    dataset.write(data.data(), data_type);
}
void hdf5::lite::writeFile(const coda_oss::filesystem::path& fileName, const std::string& ds, SpanRC<const double> data)
{
    details::try_catch_H5ExceptionsV(writeFile_<double>, __FILE__, __LINE__, fileName, ds, data); 
}
void hdf5::lite::writeFile(const coda_oss::filesystem::path& fileName, const std::string& ds, SpanRC<const float> data)
{
    details::try_catch_H5ExceptionsV(writeFile_<float>, __FILE__, __LINE__, fileName, ds, data); 
}

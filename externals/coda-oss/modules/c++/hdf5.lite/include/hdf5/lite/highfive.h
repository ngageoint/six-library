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

#pragma once
#ifndef CODA_OSS_hdf5_lite_highfive_h_INCLUDED_
#define CODA_OSS_hdf5_lite_highfive_h_INCLUDED_

/*!
 * \file  highfive.h
 * \brief Utility routines for using HighFive
 */

#include <string>
#include <stdexcept>
#include <vector>

#include "types/RowCol.h"

#include "H5_.h"
#include "SpanRC.h"

namespace hdf5
{
namespace lite
{

// Save the trouble of specifying a return type of std::vector<>
template <typename T>
inline auto v_load(const H5Easy::File& file, const std::string& dataset_name)
{
    return H5Easy::load<std::vector<T>>(file, dataset_name);
}
template <typename T>
inline auto vv_load(const H5Easy::File& file, const std::string& dataset_name)
{
    return H5Easy::load<std::vector<std::vector<T>>>(file, dataset_name);
}

template <typename T>
inline HighFive::DataSet writeDataSet(H5Easy::File& file, const std::string& dataset_name, SpanRC<T> data /*, TODO ...*/)
{
    const std::vector<size_t> dims{data.extent(0), data.extent(1)};
    const HighFive::DataSpace dataspace{dims};
    auto retval = file.createDataSet<T>(dataset_name, dataspace);
    retval.write_raw(data.data_handle());
    return retval;
}

template<typename T>
inline HighFive::DataSet writeDataSet(const H5Easy::File& file, const std::string& dataset_name, const T& values /*, TODO ...*/)
{ 
    auto dataset = file.createDataSet<T>(dataset_name, HighFive::DataSpace::From(values));
    dataset.write(values);
    return dataset;
}

// This loads 2D data into one large block of contiguous memory.
// (HighFive::DataSet::read() uses a vector of vectors).
template <typename T>
inline SpanRC<T> readDataSet(const HighFive::DataSet& dataSet, std::vector<T>& result /*, TODO ...*/)
{
    const auto dimensions = dataSet.getSpace().getDimensions();
    if (dimensions.size() > 2)
    {
        throw std::invalid_argument("'dataSet' has unexpected dimensions.");
    }
    const auto col = dimensions.size() == 2 ? dimensions[1] : 1;
    const types::RowCol<size_t> dims(dimensions[0], col);

    result.resize(dims.area());
    dataSet.read(result.data());

    return SpanRC<T>(result.data(), std::array<size_t, 2>{dims.row, dims.col});
}

template <typename T>
inline SpanRC<T> loadDataSet(const H5Easy::File& file, const std::string& dataset_name, std::vector<T>& result /*, TODO ...*/)
{
    auto dataSet = file.getDataSet(dataset_name);
    return readDataSet(dataSet, result);
}

// Wrapper around HighFive::Attribute::read() to fix problems bug with reading strings
template <typename T>
inline void read(const HighFive::Attribute& attribute, T& array)
{
    attribute.read(array);
}
template <>
inline void read(const HighFive::Attribute& attribute, std::string& array)
{
    // Attribute::read() doesn't seem to work for fixed length strings
    const auto dataType = attribute.getDataType();
    if (!dataType.isFixedLenStr())
    {
        return attribute.read(array); // let HighFive deal with it
    }

    // https://stackoverflow.com/questions/31344648/c-c-hdf5-read-string-attribute
    std::vector<char> buf(attribute.getStorageSize() + 1, '\0');
    const auto err = H5Aread(attribute.getId(), dataType.getId(), buf.data());
    if (err < 0)
    {
        throw std::runtime_error("H5Aread() failed.");
    }
    array = buf.data();
}

template<typename T>
inline T read(const HighFive::Attribute& a)
{
    return a.read<T>();
}
template<>
inline std::string read(const HighFive::Attribute& a)
{
    std::string retval;
    read(a, retval);
    return retval;
}

}
}

#endif // CODA_OSS_hdf5_lite_highfive_h_INCLUDED_

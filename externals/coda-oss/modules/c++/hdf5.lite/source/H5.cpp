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

#include "H5.h"

#include <stdexcept>
#include <tuple> // std::ignore

int hdf5::lite::details::getSimpleExtentNdims(const H5::DataSpace& dataspace)
{
    /*
     * Get the number of dimensions in the dataspace.
     */
    const auto rank = dataspace.getSimpleExtentNdims();
    // we only support 1- and 2-D data
    if ((rank != 1) && (rank != 2))
    {
        throw std::invalid_argument("'rank' must be 1 or 2.");
    }
    return rank;
}

std::vector<hsize_t> hdf5::lite::details::getSimpleExtentDims(const H5::DataSpace& dataspace)
{
    /*
     * Get the number of dimensions in the dataspace.
     */
    const auto rank = getSimpleExtentNdims(dataspace);

    /*
     * Get the dimension size of each dimension in the dataspace.
     */
    std::vector<hsize_t> dims_out(rank);
    dims_out.resize(rank);
    const auto ndims = dataspace.getSimpleExtentDims(dims_out.data());
    dims_out.resize(ndims);
    if (dims_out.empty() || (dims_out.size() > 2))
    {
        throw std::invalid_argument("dims_out.size() must be 1 or 2.");
    }

    return dims_out;
}

types::RowCol<size_t> hdf5::lite::details::getSimpleExtentSize(const H5::DataSet& dataset)
{
    /*
     * Get dataspace of the dataset.
     */
    const auto dataspace = dataset.getSpace();

    /*
     * Get the dimension size of each dimension in the dataspace.
     */
    const auto dims_out = getSimpleExtentDims(dataspace);

    // Does it matter whether it's 1-row and n-cols or n-cols and 1-row?
    types::RowCol<size_t> retval;
    retval.row = dims_out[0];
    retval.col = dims_out.size() == 2 ? dims_out[1] : 1;
    return retval;
}

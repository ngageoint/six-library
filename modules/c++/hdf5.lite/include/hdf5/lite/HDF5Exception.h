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

#ifndef CODA_OSS_hdf5_lite_HDF5Exception_h_INCLUDED_
#define CODA_OSS_hdf5_lite_HDF5Exception_h_INCLUDED_

#include "except/Exception.h"

/*!
 *  \file XMLException.h
 *  \brief Contains the exceptions specific to XML
 *
 *  This file contains all of the specialized XML exceptions used by
 *  the xml::lite package
 */
namespace hdf5
{
namespace lite
{

/*!
 *  \class HDF5Exception
 *  \brief The base XML exception class
 *
 *  This is the default XML exception, for when
 *  other, more specialized exception make no sense
 */
DECLARE_EXTENDED_EXCEPTION11(HDF5, except::Exception11);

DECLARE_EXTENDED_EXCEPTION11(DataSet, hdf5::lite::HDF5Exception11)
DECLARE_EXTENDED_EXCEPTION11(DataSpace, hdf5::lite::HDF5Exception11)
DECLARE_EXTENDED_EXCEPTION11(DataType, hdf5::lite::HDF5Exception11)

}
}
#endif  // CODA_OSS_hdf5_lite_HDF5Exception_h_INCLUDED_

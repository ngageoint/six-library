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
#ifndef CODA_OSS_hdf5_lite_HDF5Exception_h_INCLUDED_
#define CODA_OSS_hdf5_lite_HDF5Exception_h_INCLUDED_

#include "except/Exception.h"

/*!
 *  \file HDF5Exception.h
 *  \brief Contains the exceptions specific to HDF5
 *
 *  This file contains all of the specialized HDF5 exceptions used by
 *  the hdf5::lite package
 */
namespace hdf5
{
namespace lite
{

/*!
 *  \class HDF5Exception
 *  \brief The base HDF5 exception class
 *
 *  This is the default HDF5 exception, for when
 *  other, more specialized exception make no sense
 */
CODA_OSS_DECLARE_EXCEPTION(HDF5);

CODA_OSS_DECLARE_EXTENDED_EXCEPTION(DataSet, hdf5::lite::HDF5Exception);
CODA_OSS_DECLARE_EXTENDED_EXCEPTION(DataSpace, hdf5::lite::HDF5Exception);
CODA_OSS_DECLARE_EXTENDED_EXCEPTION(DataType, hdf5::lite::HDF5Exception);

}
}

#endif  // CODA_OSS_hdf5_lite_HDF5Exception_h_INCLUDED_

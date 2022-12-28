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

#include "hdf5.lite.h"

void hdf5::lite::details::try_catch_H5Exceptions_(std::function<void(void*)> f, void* context)
{
    try
    {
        /*
         * Turn off the auto-printing when failure occurs so that we can
         * handle the errors appropriately
         */
        H5::Exception::dontPrint();

        f(context);
    }
    // catch failure caused by the H5File operations
    catch (const H5::FileIException& error)
    {
        const except::Context ctx(error.getDetailMsg(), __FILE__, __LINE__, error.getFuncName());
        throw except::IOException(ctx);
    }
    // catch failure caused by the DataSet operations
    catch (const H5::DataSetIException& error)
    {
        const except::Context ctx(error.getDetailMsg(), __FILE__, __LINE__, error.getFuncName());
        throw hdf5::lite::DataSetException(ctx);
    }
    // catch failure caused by the DataSpace operations
    catch (const H5::DataSpaceIException& error)
    {
        const except::Context ctx(error.getDetailMsg(), __FILE__, __LINE__, error.getFuncName());
        throw hdf5::lite::DataSpaceException(ctx);
    }
    // catch failure caused by the DataType operations
    catch (const H5::DataTypeIException& error)
    {
        const except::Context ctx(error.getDetailMsg(), __FILE__, __LINE__, error.getFuncName());
        throw hdf5::lite::DataTypeException(ctx);
    }
}

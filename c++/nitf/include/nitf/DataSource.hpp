/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2010, General Dynamics - Advanced Information Systems
 *
 * NITRO is free software; you can redistribute it and/or modify
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
 * License along with this program; if not, If not,
 * see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef __NITF_DATASOURCE_HPP__
#define __NITF_DATASOURCE_HPP__

#include "nitf/DataSource.h"
#include "nitf/System.hpp"
#include "nitf/Object.hpp"
#include <string>

/*!
 *  \file DataSource.hpp
 *  \brief  Contains wrapper implementations for DataSources
 */
namespace nitf
{

/*!
 *  \class DataSource
 *  \brief  The C++ wrapper for the nitf_DataSource
 */
DECLARE_CLASS(DataSource)
{
public:
    //! Copy constructor
    DataSource(const DataSource & x)
    {
        setNative(x.getNative());
    }

    //! Assignment Operator
    DataSource & operator=(const DataSource & x)
    {
        if (&x != this)
            setNative(x.getNative());
        return *this;
    }

    // Set native object
    DataSource(nitf_DataSource * x)
    {
        setNative(x);
        getNativeOrThrow();
    }

    DataSource()
    {
    }

    //! Destructor
    virtual ~DataSource()
    {
    }

    // NOTE: The methods below are used just for testing - the underlying C
    //       function pointers are what need to be modified if you want to
    //       have your own source behavior.  If you want your own behavior
    //       for reads, use a RowSource and provide an appropriate
    //       RowSourceCallback.

    /*!
     *  Read from the DataSource into the buffer
     *  \param buf  The buffer
     *  \param size  The size of the buffer
     */
    void read(char * buf, nitf::Off size) throw (nitf::NITFException);

    /*
     * Returns the size of the DataSource, in bytes
     */
    nitf::Off getSize();

    void setSize(nitf::Off size);

protected:
    nitf_Error error;
};

}

#endif

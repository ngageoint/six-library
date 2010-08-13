/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
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


extern "C"
{
    //! Allows the engine to call the read function for this object
    NITF_BOOL __nitf_DataSource_read(NITF_DATA * data,
                                     char * buf,
                                     nitf::Off size,
                                     nitf_Error * error);
    

    //! Needed for the engine interface
    void __nitf_DataSource_destruct(NITF_DATA* data);

    nitf::Off __nitf_DataSource_getSize(NITF_DATA* data);

    void __nitf_DataSource_setSize(NITF_DATA* data, nitf::Off size);
}

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

    //! Destructor
    virtual ~DataSource(){}

    /*!
     *  Read from the DataSource into the buffer
     *  \param buf  The buffer
     *  \param size  The size of the buffer
     */
    virtual void read(char * buf, nitf::Off size) throw (nitf::NITFException){}

    /*
     * Returns the size of the DataSource, in bytes
     */
    virtual nitf::Off getSize() { return 0; }
    virtual void setSize(nitf::Off size) {}
private:


protected:

    //! Get the native data source interface
    nitf_IDataSource getIDataSource()
    {
        // Create a data source interface that
        // the native layer can use
        nitf_IDataSource dataSource =
        {
            &__nitf_DataSource_read,
            &__nitf_DataSource_destruct,
            &__nitf_DataSource_getSize,
            &__nitf_DataSource_setSize
        };
        return dataSource;
    }

    //! Constructor
    DataSource() throw (nitf::NITFException);

    //Here so nothing gets initialized
    DataSource(bool b){}

    nitf_Error error;
};



/*!
 *  \class KnownDataSource
 *  \brief  The base class for known data sources in the engine.
 */
class KnownDataSource : public DataSource
{
public:
    //! Constructor
    KnownDataSource(): DataSource(false), mData(NULL), mIface(NULL){}
    //! Destructor
    virtual ~KnownDataSource() {}

    /*!
     *  Read from the KnownDataSource into the buffer
     *  \param buf  The buffer
     *  \param size  The size of the buffer
     */
    virtual void read(char * buf, nitf::Off size) throw (nitf::NITFException);

    /*
     * Returns the size of the segment
     */
    virtual nitf::Off getSize();
    virtual void setSize(nitf::Off size);
protected:
    NITF_DATA * mData;
    nitf_IDataSource * mIface;
};

}

#endif

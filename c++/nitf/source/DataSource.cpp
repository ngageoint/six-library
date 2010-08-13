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

#include "nitf/DataSource.hpp"

nitf::DataSource::DataSource() throw (nitf::NITFException)
{
    static nitf_IDataSource iDataSource = getIDataSource();

    // Create the dummy handle
    nitf_DataSource * dataSource = (nitf_DataSource*)NITF_MALLOC(sizeof(nitf_DataSource));
    setNative(dataSource);
    if (!isValid())
        throw nitf::NITFException(Ctxt("Could not create data source"));

    // Attach 'this' as the data, which will be the data
    // for the DataSource_read function
    dataSource->data = this;
    dataSource->iface = &iDataSource;

    setManaged(false);
}


extern "C" NITF_BOOL __nitf_DataSource_read(NITF_DATA * data,
                                            char * buf,
                                            nitf::Off size,
                                            nitf_Error * error)
{
    // Get our object from the data and call the read function
    if (!data) throw except::NullPointerReference(Ctxt("DataSource_read"));
    ((nitf::DataSource*)data)->read(buf, size);
    return true;
}

extern "C" void __nitf_DataSource_destruct(NITF_DATA* data){}

extern "C" nitf::Off __nitf_DataSource_getSize(NITF_DATA* data)
{
    // Get our object from the data and call the read function
    if (!data) throw except::NullPointerReference(Ctxt("DataSource_getSize"));
    return ((nitf::DataSource*)data)->getSize();
}

extern "C" void __nitf_DataSource_setSize(NITF_DATA* data, nitf::Off size)
{
    // Get our object from the data and call the read function
    if (!data) throw except::NullPointerReference(Ctxt("DataSource_setSize"));
    ((nitf::DataSource*)data)->setSize(size);
}



void nitf::KnownDataSource::read(char * buf,
                                 nitf::Off size) throw (nitf::NITFException)
{
    if (mIface)
    {
        NITF_BOOL x = mIface->read(mData, buf, size, &error);
        if (!x) throw nitf::NITFException(&error);
    }
    else
        throw except::NullPointerReference(Ctxt("KnownDataSource"));
}

nitf::Off nitf::KnownDataSource::getSize() { return mIface->getSize(mData); }

void nitf::KnownDataSource::setSize(nitf::Off size) { mIface->setSize(mData, size); }

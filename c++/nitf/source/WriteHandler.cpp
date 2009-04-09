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

#include "nitf/WriteHandler.hpp"

nitf::WriteHandler::WriteHandler() throw (nitf::NITFException)
{
    static nitf_IWriteHandler iWriteHandler = getIWriteHandler();

    // Create the dummy handle
    nitf_WriteHandler * writeHandler = (nitf_WriteHandler*)NITF_MALLOC(sizeof(nitf_WriteHandler));
    setNative(writeHandler);
    if (!isValid())
        throw nitf::NITFException(Ctxt("Could not create WriteHandler"));

    // Attach 'this' as the data, which will be the data
    // for the WriteHandler_write function
    writeHandler->data = this;
    writeHandler->iface = &iWriteHandler;

    setManaged(false);
}


NITF_BOOL nitf::WriteHandler::WriteHandler_write(NITF_DATA * data,
        nitf_IOInterface* io,
        nitf_Error * error)
{
    // Get our object from the data and call the read function
    if (!data) throw except::NullPointerReference(Ctxt("WriteHandler_write"));
    nitf::NativeIOInterface interface(io);
    ((nitf::WriteHandler*)data)->write(interface);
    return true;
}

void nitf::WriteHandler::WriteHandler_destruct(NITF_DATA* data){}


void nitf::KnownWriteHandler::write(nitf::IOInterface& handle) throw (nitf::NITFException)
{
    if (mIface)
    {
        NITF_BOOL x = mIface->write(mData, handle.getNative(), &error);
        if (!x) throw nitf::NITFException(&error);
    }
    else
        throw except::NullPointerReference(Ctxt("KnownWriteHandler"));
}

nitf::StreamIOWriteHandler::StreamIOWriteHandler(
        nitf::IOInterface& sourceHandle,
        nitf::Uint64 offset,
        nitf::Uint64 bytes)
{
    setNative(nitf_StreamIOWriteHandler_construct(sourceHandle.getNative(),
            offset, bytes, &error));
    getNativeOrThrow();

    static nitf_IWriteHandler iWriteHandler = getIWriteHandler();

    mData = getNativeOrThrow()->data;
    mIface = getNativeOrThrow()->iface;

    // Attach 'this' as the data, which will be the data
    // for the WriteHandler_write function
    getNativeOrThrow()->data = this;
    getNativeOrThrow()->iface = &iWriteHandler;

    setManaged(false);
}

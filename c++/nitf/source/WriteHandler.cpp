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

nitf::WriteHandler::WriteHandler() throw (nitf::NITFException) :
    cppWriteHandler(NULL)
{
    static nitf_IWriteHandler iWriteHandler = getIWriteHandler();

    // Create the dummy handle
    cppWriteHandler = (nitf_WriteHandler*) NITF_MALLOC(
            sizeof(nitf_WriteHandler));
    if (!cppWriteHandler)
        throw nitf::NITFException(Ctxt("Could not create WriteHandler"));

    // Attach 'this' as the data, which will be the data
    // for the WriteHandler_write function
    cppWriteHandler->data = this;
    cppWriteHandler->iface = &iWriteHandler;
}


extern "C" NITF_BOOL __nitf_WriteHandler_write(NITF_DATA * data,
                                               nitf_IOInterface* io, 
                                               nitf_Error * error)
{
    // Get our object from the data and call the read function
    if (!data)
        throw except::NullPointerReference(Ctxt("WriteHandler_write"));
    nitf::NativeIOInterface interface(io);
    ((nitf::WriteHandler*) data)->write(interface);
    return true;
}

extern "C" void __nitf_WriteHandler_destruct(NITF_DATA* data)
{
}

void nitf::KnownWriteHandler::write(nitf::IOInterface& handle)
        throw (nitf::NITFException)
{
    if (knownHandler && knownHandler->iface)
    {
        NITF_BOOL x = knownHandler->iface->write(knownHandler->data,
                handle.getNative(), &error);
        if (!x)
            throw nitf::NITFException(&error);
    }
    else
        throw except::NullPointerReference(Ctxt("KnownWriteHandler"));
}

void nitf::KnownWriteHandler::setKnownHandler(nitf_WriteHandler *handler)
        throw (nitf::NITFException)
{
    if (!handler)
        throw nitf::NITFException(Ctxt("Could not create WriteHandler"));
    knownHandler = handler;
}

nitf::StreamIOWriteHandler::StreamIOWriteHandler(
        nitf::IOInterface& sourceHandle, nitf::Uint64 offset,
        nitf::Uint64 bytes) :
    nitf::KnownWriteHandler()
{
    setKnownHandler(nitf_StreamIOWriteHandler_construct(
            sourceHandle.getNative(), offset, bytes, &error));
}

void nitf::SegmentWriteHandler::write(nitf::IOInterface& handle)
        throw (nitf::NITFException)
{
    char buf[BLOCK_SIZE];
    nitf::Off numBytes = mReader.getSize();
    size_t readBytes = 0;
    while (numBytes > 0)
    {
        readBytes = numBytes < BLOCK_SIZE ? (size_t) numBytes : BLOCK_SIZE;
        mReader.read((NITF_DATA*) buf, readBytes);
        handle.write(buf, readBytes);
        numBytes -= readBytes;
    }
}

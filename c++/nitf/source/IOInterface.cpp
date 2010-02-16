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

#include "nitf/IOInterface.hpp"

nitf::IOInterface::IOInterface() throw (nitf::NITFException)
{
    static nitf_IIOInterface io = getIOInterfaceImpl();

    // Create the dummy handle
    nitf_IOInterface * interface =
            (nitf_IOInterface*) NITF_MALLOC(sizeof(nitf_IOInterface));
    setNative(interface);
    if (!isValid())
        throw nitf::NITFException(Ctxt("Could not create IOInterface"));

    // Attach 'this' as the data, which will be the data
    // for the WriteHandler_write function
    interface->data = this;
    interface->iface = &io;

    setManaged(false);
}

extern "C" NITF_BOOL __nitf_IOInterfaceImpl_read(NITF_DATA* data,
                                                 char* buf,
                                                 size_t size,
                                                 nitf_Error* error)
{
    if (!data)
        throw except::NullPointerReference(Ctxt("IOInterfaceImpl_read"));
    ((nitf::IOInterface*) data)->read(buf, size);
    return NITF_SUCCESS;
}

extern "C" NITF_BOOL __nitf_IOInterfaceImpl_write(NITF_DATA* data,
                                                  const char* buf,
                                                  size_t size,
                                                  nitf_Error* error)
{
    if (!data)
        throw except::NullPointerReference(Ctxt("IOInterfaceImpl_write"));
    ((nitf::IOInterface*) data)->write(buf, size);
    return NITF_SUCCESS;
}

extern "C" nitf::Off __nitf_IOInterfaceImpl_seek(NITF_DATA* data,
                                                 nitf::Off offset,
                                                 int whence,
                                                 nitf_Error* error)
{
    if (!data)
        throw except::NullPointerReference(Ctxt("IOInterfaceImpl_seek"));
    return ((nitf::IOInterface*) data)->seek(offset, whence);
}

extern "C" nitf::Off __nitf_IOInterfaceImpl_tell(NITF_DATA* data,
                                                 nitf_Error* error)
{
    if (!data)
        throw except::NullPointerReference(Ctxt("IOInterfaceImpl_tell"));
    return ((nitf::IOInterface*) data)->tell();
}

extern "C" nitf::Off __nitf_IOInterfaceImpl_getSize(NITF_DATA* data,
                                                    nitf_Error* error)
{
    if (!data)
        throw except::NullPointerReference(Ctxt("IOInterfaceImpl_getSize"));
    return ((nitf::IOInterface*) data)->getSize();
}

extern "C" NITF_BOOL __nitf_IOInterfaceImpl_close(NITF_DATA* data,
                                                  nitf_Error* error)
{
    if (!data)
        throw except::NullPointerReference(Ctxt("IOInterfaceImpl_tell"));
    ((nitf::IOInterface*) data)->close();
    return NITF_SUCCESS;
}

extern "C" void __nitf_IOInterfaceImpl_destruct(NITF_DATA* data) {}

void nitf::NativeIOInterface::read(char * buf, size_t size)
{
    nitf_IOInterface *io = getNativeOrThrow();
    NITF_BOOL x = io->iface->read(io->data, buf, size, &error);
    if (!x)
        throw nitf::NITFException(&error);
}

void nitf::NativeIOInterface::write(const char * buf, size_t size) throw (nitf::NITFException)
{
    nitf_IOInterface *io = getNativeOrThrow();
    NITF_BOOL x = io->iface->write(io->data, buf, size, &error);
    if (!x)
        throw nitf::NITFException(&error);
}

nitf::Off nitf::NativeIOInterface::seek(nitf::Off offset, int whence) throw (nitf::NITFException)
{
    nitf_IOInterface *io = getNativeOrThrow();
    if (io->iface->seek(io->data, offset, whence, &error))
        return io->iface->tell(io->data, &error);
    throw nitf::NITFException(&error);
}

nitf::Off nitf::NativeIOInterface::tell() throw (nitf::NITFException)
{
    nitf_IOInterface *io = getNativeOrThrow();
    nitf::Off t = io->iface->tell(io->data, &error);
    if (t < 0)
        throw nitf::NITFException(&error);
    return t;
}

nitf::Off nitf::NativeIOInterface::getSize() throw (nitf::NITFException)
{
    nitf_IOInterface *io = getNativeOrThrow();
    nitf::Off size = io->iface->getSize(io->data, &error);
    if (size < 0)
        throw nitf::NITFException(&error);
    return size;
}

void nitf::NativeIOInterface::close()
{
    nitf_IOInterface *io = getNativeOrThrow();
    io->iface->close(io->data, &error);
}

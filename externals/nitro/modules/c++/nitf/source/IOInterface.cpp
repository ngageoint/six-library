/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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

nitf::IOInterface& nitf::IOInterface::operator=(const IOInterface& x)
{
    if (&x != this)
        setNative(x.getNative());
    return *this;
}
nitf::IOInterface::IOInterface(const IOInterface& lhs)
{
    *this = lhs;
}

void nitf::IOInterfaceDestructor::operator()(nitf_IOInterface *io)
{
    if (io)
    {
        nitf_Error error;
        nitf_IOInterface_close(io, &error);
        nitf_IOInterface_destruct(&io);
    }
}

void nitf::IOInterface::read(void* buf, size_t size)
{
    nitf_IOInterface *io = getNativeOrThrow();
    if (io == nullptr)
    {
        throw except::NullPointerReference(Ctxt("IOInterface"));
    }
    const NITF_BOOL x = io->iface->read(io->data, buf, size, &error);
    if (!x)
        throw nitf::NITFException(&error);
}

void nitf::IOInterface::write(const void* buf, size_t size)
{
    nitf_IOInterface *io = getNativeOrThrow();
    if (io != nullptr)
    {
        const NITF_BOOL x = io->iface->write(io->data, buf, size, &error);
        if (!x)
            throw nitf::NITFException(&error);
    }
}

bool nitf::IOInterface::canSeek() const
{
    nitf_IOInterface* io = getNativeOrThrow();
    if (io == nullptr)
    {
        throw except::NullPointerReference(Ctxt("IOInterface"));
    }
    return io->iface->canSeek(io->data, &error) == NRT_SUCCESS;
}

nitf::Off nitf::IOInterface::seek(nitf::Off offset, int whence)
{
    nitf_IOInterface* io = getNativeOrThrow();
    if (io == nullptr)
    {
        throw except::NullPointerReference(Ctxt("IOInterface"));
    }
    if (!NRT_IO_SUCCESS(io->iface->seek(io->data, offset, whence, &error)))
    {
        throw nitf::NITFException(&error);
    }
    return io->iface->tell(io->data, &error);
}

nitf::Off nitf::IOInterface::tell() const
{
    nitf_IOInterface* io = getNativeOrThrow();
    if (io == nullptr)
    {
        throw except::NullPointerReference(Ctxt("IOInterface"));
    }
    const nitf::Off t = io->iface->tell(io->data, &error);
    if (!NRT_IO_SUCCESS(t))
    {
        throw nitf::NITFException(&error);
    }
    return t;
}

nitf::Off nitf::IOInterface::getSize() const
{
    nitf_IOInterface* io = getNativeOrThrow();
    if (io == nullptr)
    {
        throw except::NullPointerReference(Ctxt("IOInterface"));
    }
    const nitf::Off size = io->iface->getSize(io->data, &error);
    if (!NRT_IO_SUCCESS(size))
    {
        throw nitf::NITFException(&error);
    }
    return size;
}

int nitf::IOInterface::getMode() const
{
    nitf_IOInterface *io = getNativeOrThrow();
    if (io == nullptr)
    {
        throw except::NullPointerReference(Ctxt("IOInterface"));
    }
    return io->iface->getMode(io->data, &error);
}

void nitf::IOInterface::close()
{
    nitf_IOInterface *io = getNativeOrThrow();
    if (io != nullptr)
    {
        io->iface->close(io->data, &error);
    }
}

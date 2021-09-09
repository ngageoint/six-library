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

#include "nitf/WriteHandler.hpp"

nitf::WriteHandler::WriteHandler(const WriteHandler& x)
{
    *this = x;
}
nitf::WriteHandler& nitf::WriteHandler::operator=(const WriteHandler& x)
{
    if (&x != this)
        setNative(x.getNative());
    return *this;
}

nitf::WriteHandler::WriteHandler(nitf_WriteHandler* x)
{
    setNative(x);
    getNativeOrThrow();
}

void nitf::WriteHandler::write(nitf::IOInterface& handle)
{
    nitf_WriteHandler *handler = getNativeOrThrow();
    if (handler && handler->iface)
    {
        if (!handler->iface->write(handler->data,
                handle.getNative(), &error))
            throw nitf::NITFException(&error);
    }
    else
        throw except::NullPointerReference(Ctxt("WriteHandler"));
}

nitf::StreamIOWriteHandler::StreamIOWriteHandler(
        nitf::IOInterface& sourceHandle, uint64_t offset,
        uint64_t bytes)
{
    setNative(nitf_StreamIOWriteHandler_construct(
            sourceHandle.getNative(), offset, bytes, &error));
    setManaged(false);
}


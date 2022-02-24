/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 * (C) Copyright 2022, Maxar Technologies, Inc.
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

#include "nitf/J2KWriter.hpp"

#include <gsl/gsl.h>

#include "nitf/J2KContainer.hpp"

j2k::details::Writer::Writer(j2k_Writer* x)
{
    setNative(x);
    getNativeOrThrow();
}
j2k::Writer::Writer(j2k_Writer*&& x) : impl_(x) {}

j2k::Writer::Writer(const Container& container, const WriterOptions& options)
    : Writer(container.createWriter(options)) { }

void j2k::Writer::setTile(uint32_t tileX, uint32_t tileY, std::span<const uint8_t> buf)
{
    impl_.callNativeOrThrowV(j2k_Writer_setTile, tileX, tileY, buf.data(), gsl::narrow<uint32_t>(buf.size()));
}

void j2k::Writer::write(nitf::IOHandle& handle)
{
    impl_.callNativeOrThrowV(j2k_Writer_write, handle.getNativeOrThrow());
}

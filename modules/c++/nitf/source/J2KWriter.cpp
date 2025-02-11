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

#include <stdexcept>

#include <gsl/gsl.h>

#include "nitf/J2KContainer.hpp"

j2k::details::Writer::Writer(j2k_Writer* x)
{
    setNative(x);
    getNativeOrThrow();
}
j2k::Writer::Writer(j2k_Writer*&& x) : impl_(x) {}

j2k::Writer::Writer(const Container& container, const WriterOptions& options)
    : Writer(container.createWriter(options))
{
    pContainer_ = &container;
}

const j2k::Container& j2k::Writer::getContainer() const
{
    auto pContainer = impl_.callNativeOrThrow<j2k_Container*>(j2k_Writer_getContainer);
    if (pContainer != pContainer_->getNativeOrThrow())
    {
        throw std::logic_error("Pointers to native containers don't match!");
    }
    return *pContainer_;
}

void j2k::Writer::setTile(uint32_t tileX, uint32_t tileY, std::span<const uint8_t> buf)
{
    impl_.callNativeOrThrowV(j2k_Writer_setTile, tileX, tileY, buf.data(), gsl::narrow<uint32_t>(buf.size()));
}

void j2k::Writer::write(nitf::IOHandle& handle)
{
    impl_.callNativeOrThrowV(j2k_Writer_write, handle.getNativeOrThrow());
}

j2k::WriteTiler::WriteTiler(Writer& writer, std::span<const uint8_t> buf) : writer_(writer), buf_(buf)
{
    const auto& container = writer_.getContainer();
    this->tileSize_ = container.tileSize();
    this->num_x_tiles_ = container.getTilesX();
    this->numComponents_ = container.getNumComponents();
}
void j2k::WriteTiler::setTile(uint32_t tileX, uint32_t tileY, uint32_t i)
{
    //const auto offset = container.bufferOffset(tileX, tileY, i);
    const auto index = tileY * num_x_tiles_ + tileX % num_x_tiles_;
    const auto bytes = writer_.getContainer().numBytes(i);
    const auto offset_ = index * tileSize_ * bytes * numComponents_;
    const auto offset = gsl::narrow<ptrdiff_t>(offset_);

    const std::span<const uint8_t> buf(buf_.data() + offset, tileSize_);
    writer_.setTile(tileX, tileY, buf);
}
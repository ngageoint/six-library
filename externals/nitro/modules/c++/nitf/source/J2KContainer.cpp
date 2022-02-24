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

#include "nitf/J2KContainer.hpp"

#include <gsl/gsl.h>

j2k::details::Container::Container(j2k_Container* x)
{
    setNative(x);
    getNativeOrThrow();
}
j2k::Container::Container(j2k_Container*&& x) : impl_(x) {}

j2k::Writer j2k::Container::createWriter(const WriterOptions& options) const
{
    return impl_.callNativeOrThrow<j2k_Writer*>(j2k_Writer_construct, options.getNative());
}

j2k::Component j2k::Container::getComponent(uint32_t i) const
{
    return impl_.callNativeOrThrow<j2k_Component*>(j2k_Container_getComponent, i);
}

// a macro to help avoid copy/paste errors
#define NITF_j2k__Container_IMPL(retval, name) retval j2k::Container::name() const { return impl_.callNative<retval>(j2k_Container_ ## name); }

NITF_j2k__Container_IMPL(uint32_t, getNumComponents);

NITF_j2k__Container_IMPL(uint32_t, getWidth);
NITF_j2k__Container_IMPL(uint32_t, getHeight);

NITF_j2k__Container_IMPL(uint32_t, getGridWidth);
NITF_j2k__Container_IMPL(uint32_t, getGridHeight);

NITF_j2k__Container_IMPL(uint32_t, getTileWidth);
NITF_j2k__Container_IMPL(uint32_t, getTileHeight);

NITF_j2k__Container_IMPL(uint32_t, getTilesX);
NITF_j2k__Container_IMPL(uint32_t, getTilesY);

NITF_j2k__Container_IMPL(int, getImageType);

#undef NITF_j2k__Container_IMPL

size_t j2k::Container::tileSize() const
{
    return getTileWidth() * getTileHeight();
}

size_t j2k::Container::numBytes(uint32_t i) const
{
    const auto c = getComponent(i);
    const auto precision = c.getPrecision();
    const auto bytes = (precision - 1) / 8 + 1;
    return bytes;
}

ptrdiff_t j2k::Container::bufferOffset(uint32_t tileX, uint32_t tileY, uint32_t i) const
{
    const auto num_x_tiles = getTilesX();
    const auto index = tileY * num_x_tiles + tileX % num_x_tiles;

    const auto tileSize = this->tileSize();
    const auto bytes = numBytes(i);
    const auto offset = index * tileSize * bytes * getNumComponents();

    return gsl::narrow<ptrdiff_t>(offset);
}
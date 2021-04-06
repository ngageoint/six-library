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

#include "nitf/GraphicSegment.hpp"

using namespace nitf;

GraphicSegment::GraphicSegment(const GraphicSegment & x)
{
    *this = x;
}

GraphicSegment & GraphicSegment::operator=(const GraphicSegment & x)
{
    if (&x != this)
        setNative(x.getNative());
    return *this;
}


GraphicSegment::GraphicSegment(nitf_GraphicSegment * x)
{
    setNative(x);
    getNativeOrThrow();
}


GraphicSegment::GraphicSegment() : GraphicSegment(nitf_GraphicSegment_construct(&error))
{
    setManaged(false);
}

GraphicSegment::GraphicSegment(NITF_DATA * x)
{
    *this = x;
}

GraphicSegment & GraphicSegment::operator=(NITF_DATA * x)
{
    setNative(static_cast<nitf_GraphicSegment*>(x));
    getNativeOrThrow();
    return *this;
}


nitf::GraphicSegment GraphicSegment::clone() const
{
    nitf::GraphicSegment dolly(nitf_GraphicSegment_clone(getNativeOrThrow(), &error));
    dolly.setManaged(false);
    return dolly;
}


nitf::GraphicSubheader GraphicSegment::getSubheader() const
{
    return nitf::GraphicSubheader(getNativeOrThrow()->subheader);
}

void GraphicSegment::setSubheader(nitf::GraphicSubheader & value)
{
    //release the one currently "owned"
    nitf::GraphicSubheader sub = nitf::GraphicSubheader(getNativeOrThrow()->subheader);
    sub.setManaged(false);

    //have the library manage the "new" one
    getNativeOrThrow()->subheader = value.getNative();
    value.setManaged(true);
}

uint64_t GraphicSegment::getOffset() const
{
    return getNativeOrThrow()->offset;
}

void GraphicSegment::setOffset(uint64_t value)
{
    getNativeOrThrow()->offset = value;
}

uint64_t GraphicSegment::getEnd() const
{
    return getNativeOrThrow()->end;
}

void GraphicSegment::setEnd(uint64_t value)
{
    getNativeOrThrow()->end = value;
}

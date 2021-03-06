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

#include "nitf/DESegment.hpp"

using namespace nitf;

DESegment::DESegment(const DESegment & x)
{
    *this = x;
}

DESegment & DESegment::operator=(const DESegment & x)
{
    if (&x != this)
        setNative(x.getNative());
    return *this;
}

DESegment::DESegment(nitf_DESegment * x)
{
    setNative(x);
    getNativeOrThrow();
}

DESegment::DESegment()
{
    auto* deSegment = nitf_DESegment_construct(&error);
    setNativeOrThrow(deSegment, &error);
    setManaged(false);
}

DESegment::DESegment(NITF_DATA * x)
{
    setNative(static_cast<nitf_DESegment*>(x));
    getNativeOrThrow();
}

DESegment & DESegment::operator=(NITF_DATA * x)
{
    setNative(static_cast<nitf_DESegment*>(x));
    getNativeOrThrow();
    return *this;
}

nitf::DESegment DESegment::clone() const
{
    nitf::DESegment dolly(nitf_DESegment_clone(getNativeOrThrow(), &error));
    dolly.setManaged(false);
    return dolly;
}

nitf::DESubheader DESegment::getSubheader() const
{
    return nitf::DESubheader(getNativeOrThrow()->subheader);
}

void DESegment::setSubheader(nitf::DESubheader & value)
{
    //release the one currently "owned"
    nitf::DESubheader sub = nitf::DESubheader(getNativeOrThrow()->subheader);
    sub.setManaged(false);

    //have the library manage the "new" one
    getNativeOrThrow()->subheader = value.getNative();
    value.setManaged(true);
}

uint64_t DESegment::getOffset() const
{
    return getNativeOrThrow()->offset;
}

void DESegment::setOffset(uint64_t value)
{
    getNativeOrThrow()->offset = value;
}

uint64_t DESegment::getEnd() const
{
    return getNativeOrThrow()->end;
}

void DESegment::setEnd(uint64_t value)
{
    getNativeOrThrow()->end = value;
}

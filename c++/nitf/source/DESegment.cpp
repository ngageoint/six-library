/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2010, General Dynamics - Advanced Information Systems
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
    setNative(x.getNative());
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

DESegment::DESegment() throw(nitf::NITFException)
{
    setNative(nitf_DESegment_construct(&error));
    getNativeOrThrow();
    setManaged(false);
}

DESegment::DESegment(NITF_DATA * x)
{
    setNative((nitf_DESegment*)x);
    getNativeOrThrow();
}

DESegment & DESegment::operator=(NITF_DATA * x)
{
    setNative((nitf_DESegment*)x);
    getNativeOrThrow();
    return *this;
}

nitf::DESegment DESegment::clone() throw(nitf::NITFException)
{
    nitf::DESegment dolly(nitf_DESegment_clone(getNativeOrThrow(), &error));
    dolly.setManaged(false);
    return dolly;
}

DESegment::~DESegment(){}

nitf::DESubheader DESegment::getSubheader()
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

nitf::Uint64 DESegment::getOffset() const
{
    return getNativeOrThrow()->offset;
}

void DESegment::setOffset(nitf::Uint64 value)
{
    getNativeOrThrow()->offset = value;
}

nitf::Uint64 DESegment::getEnd() const
{
    return getNativeOrThrow()->end;
}

void DESegment::setEnd(nitf::Uint64 value)
{
    getNativeOrThrow()->end = value;
}

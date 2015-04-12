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

#include "nitf/RESegment.hpp"

using namespace nitf;

RESegment::RESegment(const RESegment & x)
{
    setNative(x.getNative());
}

RESegment & RESegment::operator=(const RESegment & x)
{
    if (&x != this)
        setNative(x.getNative());
    return *this;
}

RESegment::RESegment(nitf_RESegment * x)
{
    setNative(x);
    getNativeOrThrow();
}

RESegment::RESegment() throw(nitf::NITFException)
{
    setNative(nitf_RESegment_construct(&error));
    getNativeOrThrow();
    setManaged(false);
}

RESegment::RESegment(NITF_DATA * x)
{
    setNative((nitf_RESegment*)x);
    getNativeOrThrow();
}

RESegment & RESegment::operator=(NITF_DATA * x)
{
    setNative((nitf_RESegment*)x);
    getNativeOrThrow();
    return *this;
}

nitf::RESegment RESegment::clone() throw(nitf::NITFException)
{
    nitf::RESegment dolly(
        nitf_RESegment_clone(getNativeOrThrow(), &error));
    dolly.setManaged(false);
    return dolly;
}

RESegment::~RESegment(){}

nitf::RESubheader RESegment::getSubheader()
{
    return nitf::RESubheader(getNativeOrThrow()->subheader);
}

void RESegment::setSubheader(nitf::RESubheader & value)
{
    //release the one currently "owned"
    nitf::RESubheader sub = nitf::RESubheader(getNativeOrThrow()->subheader);
    sub.setManaged(false);

    //have the library manage the "new" one
    getNativeOrThrow()->subheader = value.getNative();
    value.setManaged(true);
}

nitf::Uint64 RESegment::getOffset() const
{
    return getNativeOrThrow()->offset;
}

void RESegment::setOffset(nitf::Uint64 value)
{
    getNativeOrThrow()->offset = value;
}

nitf::Uint64 RESegment::getEnd() const
{
    return getNativeOrThrow()->end;
}

void RESegment::setEnd(nitf::Uint64 value)
{
    getNativeOrThrow()->end = value;
}

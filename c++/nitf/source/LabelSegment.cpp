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

#include "nitf/LabelSegment.hpp"

using namespace nitf;

LabelSegment::LabelSegment(const LabelSegment & x)
{
    setNative(x.getNative());
}

LabelSegment & LabelSegment::operator=(const LabelSegment & x)
{
    if (&x != this)
        setNative(x.getNative());
    return *this;
}

LabelSegment::LabelSegment(nitf_LabelSegment * x)
{
    setNative(x);
    getNativeOrThrow();
}

LabelSegment::LabelSegment() throw(nitf::NITFException)
{
    setNative(nitf_LabelSegment_construct(&error));
    getNativeOrThrow();
    setManaged(false);
}

LabelSegment::LabelSegment(NITF_DATA * x)
{
    setNative((nitf_LabelSegment*)x);
    getNativeOrThrow();
}

LabelSegment & LabelSegment::operator=(NITF_DATA * x)
{
    setNative((nitf_LabelSegment*)x);
    getNativeOrThrow();
    return *this;
}


nitf::LabelSegment LabelSegment::clone() throw(nitf::NITFException)
{
    nitf::LabelSegment dolly(
        nitf_LabelSegment_clone(getNativeOrThrow(), &error));
    dolly.setManaged(false);
    return dolly;
}

LabelSegment::~LabelSegment(){}

nitf::LabelSubheader LabelSegment::getSubheader()
{
    return nitf::LabelSubheader(getNativeOrThrow()->subheader);
}

void LabelSegment::setSubheader(nitf::LabelSubheader & value)
{
    //release the one currently "owned"
    nitf::LabelSubheader sub = nitf::LabelSubheader(getNativeOrThrow()->subheader);
    sub.setManaged(false);

    //have the library manage the "new" one
    getNativeOrThrow()->subheader = value.getNative();
    value.setManaged(true);
}

nitf::Uint64 LabelSegment::getOffset() const
{
    return getNativeOrThrow()->offset;
}

void LabelSegment::setOffset(nitf::Uint64 value)
{
    getNativeOrThrow()->offset = value;
}

nitf::Uint64 LabelSegment::getEnd() const
{
    return getNativeOrThrow()->end;
}

void LabelSegment::setEnd(nitf::Uint64 value)
{
    getNativeOrThrow()->end = value;
}


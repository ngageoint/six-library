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

#include "nitf/TextSegment.hpp"

using namespace nitf;

TextSegment::TextSegment(const TextSegment & x)
{
    *this = x;
}

TextSegment & TextSegment::operator=(const TextSegment & x)
{
    if (&x != this)
        setNative(x.getNative());
    return *this;
}

TextSegment::TextSegment(nitf_TextSegment * x)
{
    setNative(x);
    getNativeOrThrow();
}

TextSegment::TextSegment() : TextSegment(nitf_TextSegment_construct(&error))
{
    setManaged(false);
}

TextSegment::TextSegment(NITF_DATA * x) : TextSegment(static_cast<nitf_TextSegment*>(x))
{
}

TextSegment & TextSegment::operator=(NITF_DATA * x)
{
    setNative(static_cast<nitf_TextSegment*>(x));
    getNativeOrThrow();
    return *this;
}

nitf::TextSegment TextSegment::clone() const
{
    nitf::TextSegment dolly(
        nitf_TextSegment_clone(getNativeOrThrow(), &error));
    dolly.setManaged(false);
    return dolly;
}

nitf::TextSubheader TextSegment::getSubheader() const
{
    return nitf::TextSubheader(getNativeOrThrow()->subheader);
}

void TextSegment::setSubheader(nitf::TextSubheader & value)
{
    //release the one currently "owned"
    nitf::TextSubheader sub = nitf::TextSubheader(getNativeOrThrow()->subheader);
    sub.setManaged(false);

    //have the library manage the "new" one
    getNativeOrThrow()->subheader = value.getNative();
    value.setManaged(true);
}

uint64_t TextSegment::getOffset() const
{
    return getNativeOrThrow()->offset;
}

void TextSegment::setOffset(uint64_t value)
{
    getNativeOrThrow()->offset = value;
}

uint64_t TextSegment::getEnd() const
{
    return getNativeOrThrow()->end;
}

void TextSegment::setEnd(uint64_t value)
{
    getNativeOrThrow()->end = value;
}

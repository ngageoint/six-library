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

#include "nitf/ComponentInfo.hpp"

using namespace nitf;

ComponentInfo::ComponentInfo(const ComponentInfo & x)
{
    setNative(x.getNative());
}

ComponentInfo & ComponentInfo::operator=(const ComponentInfo & x)
{
    if (&x != this)
        setNative(x.getNative());
    return *this;
}

ComponentInfo::ComponentInfo(nitf_ComponentInfo * x)
{
    setNative(x);
    getNativeOrThrow();
}

ComponentInfo::ComponentInfo(nitf::Uint32 subHeaderSize, nitf::Uint64 dataSize)
    throw(nitf::NITFException)
{
    setNative(nitf_ComponentInfo_construct(subHeaderSize, dataSize, &error));
    getNativeOrThrow();
    setManaged(false);
}

ComponentInfo ComponentInfo::clone() throw(nitf::NITFException)
{
    nitf::ComponentInfo dolly(nitf_ComponentInfo_clone(getNativeOrThrow(), &error));
    dolly.setManaged(false);
    return dolly;
}

ComponentInfo::~ComponentInfo(){}

nitf::Field ComponentInfo::getLengthSubheader()
{
    return nitf::Field(getNativeOrThrow()->lengthSubheader);
}

nitf::Field ComponentInfo::getLengthData()
{
    return nitf::Field(getNativeOrThrow()->lengthData);
}

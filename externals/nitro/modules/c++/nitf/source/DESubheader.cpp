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

#include "nitf/DESubheader.hpp"

#include "gsl/gsl.h"

using namespace nitf;

DESubheader::DESubheader(const DESubheader & x)
{
    setNative(x.getNative());
}

DESubheader & DESubheader::operator=(const DESubheader & x)
{
    if (&x != this)
        setNative(x.getNative());
    return *this;
}

DESubheader::DESubheader(nitf_DESubheader * x)
{
    setNative(x);
    getNativeOrThrow();
}

DESubheader::DESubheader() : DESubheader(nitf_DESubheader_construct(&error))
{
    setManaged(false);
}

nitf::DESubheader DESubheader::clone() const
{
    nitf::DESubheader dolly(nitf_DESubheader_clone(getNativeOrThrow(), &error));
    dolly.setManaged(false);
    return dolly;
}

nitf::Field DESubheader::getFilePartType() const
{
    return nitf::Field(getNativeOrThrow()->filePartType);
}

nitf::Field DESubheader::getTypeID() const
{
    return nitf::Field(getNativeOrThrow()->typeID);
}

nitf::Field DESubheader::getVersion() const
{
    return nitf::Field(getNativeOrThrow()->version);
}

nitf::Field DESubheader::getSecurityClass() const
{
    return nitf::Field(getNativeOrThrow()->securityClass);
}

nitf::FileSecurity DESubheader::getSecurityGroup() const
{
    return nitf::FileSecurity(getNativeOrThrow()->securityGroup);
}

void DESubheader::setSecurityGroup(nitf::FileSecurity value)
{
    //release the owned security group
    nitf::FileSecurity fs = nitf::FileSecurity(getNativeOrThrow()->securityGroup);
    fs.setManaged(false);

    //have the library manage the new securitygroup
    getNativeOrThrow()->securityGroup = value.getNative();
    value.setManaged(true);
}

nitf::Field DESubheader::getOverflowedHeaderType() const
{
    return nitf::Field(getNativeOrThrow()->overflowedHeaderType);
}

nitf::Field DESubheader::getDataItemOverflowed() const
{
    return nitf::Field(getNativeOrThrow()->dataItemOverflowed);
}

nitf::Field DESubheader::getSubheaderFieldsLength() const
{
    return nitf::Field(getNativeOrThrow()->subheaderFieldsLength);
}

nitf::TRE DESubheader::getSubheaderFields() const
{
    return nitf::TRE(getNativeOrThrow()->subheaderFields);
}

void DESubheader::setSubheaderFields(nitf::TRE fields)
{
    if (getNativeOrThrow()->subheaderFields)
    {
        //release the one currently "owned", if different
        nitf::TRE tre = nitf::TRE(getNativeOrThrow()->subheaderFields);
        if (tre != fields)
            tre.setManaged(false);
    }

    //have the library manage the "new" one
    getNativeOrThrow()->subheaderFields = fields.getNative();
    fields.setManaged(true);
}

uint32_t DESubheader::getDataLength() const
{
    return gsl::narrow<uint32_t>(getNativeOrThrow()->dataLength);
}

void DESubheader::setDataLength(uint32_t value)
{
    getNativeOrThrow()->dataLength = value;
}

nitf::Extensions DESubheader::getUserDefinedSection() const
{
    return nitf::Extensions(getNativeOrThrow()->userDefinedSection);
}

void DESubheader::setUserDefinedSection(nitf::Extensions value)
{
    if (getNativeOrThrow()->userDefinedSection)
    {
        //release the one currently "owned", if different
        nitf::Extensions exts = nitf::Extensions(getNativeOrThrow()->userDefinedSection);
        if (exts != value)
            exts.setManaged(false);
    }

    //have the library manage the "new" one
    getNativeOrThrow()->userDefinedSection = value.getNative();
    value.setManaged(true);
}

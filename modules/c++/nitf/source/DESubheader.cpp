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

#include "nitf/DESubheader.hpp"

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

DESubheader::DESubheader() throw(nitf::NITFException)
{
    setNative(nitf_DESubheader_construct(&error));
    getNativeOrThrow();
    setManaged(false);
}

nitf::DESubheader DESubheader::clone() throw(nitf::NITFException)
{
    nitf::DESubheader dolly(nitf_DESubheader_clone(getNativeOrThrow(), &error));
    dolly.setManaged(false);
    return dolly;
}

DESubheader::~DESubheader(){}

nitf::Field DESubheader::getFilePartType()
{
    return nitf::Field(getNativeOrThrow()->filePartType);
}

nitf::Field DESubheader::getTypeID()
{
    return nitf::Field(getNativeOrThrow()->typeID);
}

nitf::Field DESubheader::getVersion()
{
    return nitf::Field(getNativeOrThrow()->version);
}

nitf::Field DESubheader::getSecurityClass()
{
    return nitf::Field(getNativeOrThrow()->securityClass);
}

nitf::FileSecurity DESubheader::getSecurityGroup()
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

nitf::Field DESubheader::getOverflowedHeaderType()
{
    return nitf::Field(getNativeOrThrow()->overflowedHeaderType);
}

nitf::Field DESubheader::getDataItemOverflowed()
{
    return nitf::Field(getNativeOrThrow()->dataItemOverflowed);
}

nitf::Field DESubheader::getSubheaderFieldsLength()
{
    return nitf::Field(getNativeOrThrow()->subheaderFieldsLength);
}

nitf::TRE DESubheader::getSubheaderFields()
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

nitf::Uint32 DESubheader::getDataLength() const
{
    return getNativeOrThrow()->dataLength;
}

void DESubheader::setDataLength(nitf::Uint32 value)
{
    getNativeOrThrow()->dataLength = value;
}

nitf::Extensions DESubheader::getUserDefinedSection()
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

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

#include "nitf/LabelSubheader.hpp"

using namespace nitf;

LabelSubheader::LabelSubheader(const LabelSubheader & x)
{
    setNative(x.getNative());
}

LabelSubheader & LabelSubheader::operator=(const LabelSubheader & x)
{
    if (&x != this)
        setNative(x.getNative());
    return *this;
}

LabelSubheader::LabelSubheader(nitf_LabelSubheader * x)
{
    setNative(x);
    getNativeOrThrow();
}

LabelSubheader::LabelSubheader()
{
    setNative(nitf_LabelSubheader_construct(&error));
    getNativeOrThrow();
    setManaged(false);
}


LabelSubheader LabelSubheader::clone() const
{
    nitf::LabelSubheader dolly(
        nitf_LabelSubheader_clone(getNativeOrThrow(), &error));
    dolly.setManaged(false);
    return dolly;
}

nitf::Field LabelSubheader::getFilePartType() const
{
    return nitf::Field(getNativeOrThrow()->filePartType);
}

nitf::Field LabelSubheader::getLabelID() const
{
    return nitf::Field(getNativeOrThrow()->labelID);
}

nitf::Field LabelSubheader::getSecurityClass() const
{
    return nitf::Field(getNativeOrThrow()->securityClass);
}

nitf::FileSecurity LabelSubheader::getSecurityGroup() const
{
    return nitf::FileSecurity(getNativeOrThrow()->securityGroup);
}

void LabelSubheader::setSecurityGroup(nitf::FileSecurity value)
{
    //release the owned security group
    nitf::FileSecurity fs = nitf::FileSecurity(getNativeOrThrow()->securityGroup);
    fs.setManaged(false);

    //have the library manage the new securitygroup
    getNativeOrThrow()->securityGroup = value.getNative();
    value.setManaged(true);
}

nitf::Field LabelSubheader::getEncrypted() const
{
    return nitf::Field(getNativeOrThrow()->encrypted);
}

nitf::Field LabelSubheader::getFontStyle() const
{
    return nitf::Field(getNativeOrThrow()->fontStyle);
}

nitf::Field LabelSubheader::getCellWidth() const
{
    return nitf::Field(getNativeOrThrow()->cellWidth);
}

nitf::Field LabelSubheader::getCellHeight() const
{
    return nitf::Field(getNativeOrThrow()->cellHeight);
}

nitf::Field LabelSubheader::getDisplayLevel() const
{
    return nitf::Field(getNativeOrThrow()->displayLevel);
}

nitf::Field LabelSubheader::getAttachmentLevel() const
{
    return nitf::Field(getNativeOrThrow()->attachmentLevel);
}

nitf::Field LabelSubheader::getLocationRow() const
{ 
    return nitf::Field(getNativeOrThrow()->locationRow);
}

nitf::Field LabelSubheader::getLocationColumn() const
{
    return nitf::Field(getNativeOrThrow()->locationColumn);
}

nitf::Field LabelSubheader::getTextColor() const
{
    return nitf::Field(getNativeOrThrow()->textColor);
}

nitf::Field LabelSubheader::getBackgroundColor() const
{
    return nitf::Field(getNativeOrThrow()->backgroundColor);
}

nitf::Field LabelSubheader::getExtendedHeaderLength() const
{
    return nitf::Field(getNativeOrThrow()->extendedHeaderLength);
}

nitf::Field LabelSubheader::getExtendedHeaderOverflow() const
{
    return nitf::Field(getNativeOrThrow()->extendedHeaderOverflow);
}

nitf::Extensions LabelSubheader::getExtendedSection() const
{
    return nitf::Extensions(getNativeOrThrow()->extendedSection);
}

void LabelSubheader::setExtendedSection(nitf::Extensions value)
{
    if (getNativeOrThrow()->extendedSection)
    {
        //release the one currently "owned", if different
        nitf::Extensions exts = nitf::Extensions(getNativeOrThrow()->extendedSection);
        if (exts != value)
            exts.setManaged(false);
    }

    //have the library manage the "new" one
    getNativeOrThrow()->extendedSection = value.getNative();
    value.setManaged(true);
}

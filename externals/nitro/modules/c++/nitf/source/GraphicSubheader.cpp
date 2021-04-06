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

#include "nitf/GraphicSubheader.hpp"

using namespace nitf;

GraphicSubheader::GraphicSubheader(const GraphicSubheader & x)
{
    setNative(x.getNative());
}


GraphicSubheader & GraphicSubheader::operator=(const GraphicSubheader & x)
{
    if (&x != this)
        setNative(x.getNative());
    return *this;
}

GraphicSubheader::GraphicSubheader(nitf_GraphicSubheader * x)
{
    setNative(x);
    getNativeOrThrow();
}

GraphicSubheader::GraphicSubheader() : GraphicSubheader(nitf_GraphicSubheader_construct(&error))
{
    setManaged(false);
}


nitf::GraphicSubheader GraphicSubheader::clone() const
{
    nitf::GraphicSubheader dolly(
        nitf_GraphicSubheader_clone(getNativeOrThrow(), &error));
    dolly.setManaged(false);
    return dolly;
}

nitf::Field GraphicSubheader::getFilePartType() const
{
    return nitf::Field(getNativeOrThrow()->filePartType);
}

nitf::Field GraphicSubheader::getGraphicID() const
{
    return nitf::Field(getNativeOrThrow()->graphicID);
}

nitf::Field GraphicSubheader::getName() const
{
    return nitf::Field(getNativeOrThrow()->name);
}

nitf::Field GraphicSubheader::getSecurityClass() const
{
    return nitf::Field(getNativeOrThrow()->securityClass);
}

nitf::FileSecurity GraphicSubheader::getSecurityGroup() const
{
    return nitf::FileSecurity(getNativeOrThrow()->securityGroup);
}

void GraphicSubheader::setSecurityGroup(nitf::FileSecurity value)
{
    //release the owned security group
    nitf::FileSecurity fs = nitf::FileSecurity(getNativeOrThrow()->securityGroup);
    fs.setManaged(false);

    //have the library manage the new securitygroup
    getNativeOrThrow()->securityGroup = value.getNative();
    value.setManaged(true);
}


nitf::Field GraphicSubheader::getEncrypted() const
{
    return nitf::Field(getNativeOrThrow()->encrypted);
}

nitf::Field GraphicSubheader::getStype() const
{
    return nitf::Field(getNativeOrThrow()->stype);
}

nitf::Field GraphicSubheader::getRes1() const
{
    return nitf::Field(getNativeOrThrow()->res1);
}

nitf::Field GraphicSubheader::getDisplayLevel() const
{
    return nitf::Field(getNativeOrThrow()->displayLevel);
}

nitf::Field GraphicSubheader::getAttachmentLevel() const
{
    return nitf::Field(getNativeOrThrow()->attachmentLevel);
}

nitf::Field GraphicSubheader::getLocation() const
{
    return nitf::Field(getNativeOrThrow()->location);
}

nitf::Field GraphicSubheader::getBound1Loc() const
{
    return nitf::Field(getNativeOrThrow()->bound1Loc);
}

nitf::Field GraphicSubheader::getColor() const
{
    return nitf::Field(getNativeOrThrow()->color);
}

nitf::Field GraphicSubheader::getBound2Loc() const
{
    return nitf::Field(getNativeOrThrow()->bound2Loc);
}

nitf::Field GraphicSubheader::getRes2() const
{
    return nitf::Field(getNativeOrThrow()->res2);
}

nitf::Field GraphicSubheader::getExtendedHeaderLength() const
{
    return nitf::Field(getNativeOrThrow()->extendedHeaderLength);
}

nitf::Field GraphicSubheader::getExtendedHeaderOverflow() const
{
    return nitf::Field(getNativeOrThrow()->extendedHeaderOverflow);
}

nitf::Extensions GraphicSubheader::getExtendedSection() const
{
    return nitf::Extensions(getNativeOrThrow()->extendedSection);
}

void GraphicSubheader::setExtendedSection(nitf::Extensions value)
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

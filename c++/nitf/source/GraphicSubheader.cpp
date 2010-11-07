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

GraphicSubheader::GraphicSubheader() throw(nitf::NITFException)
{
    setNative(nitf_GraphicSubheader_construct(&error));
    getNativeOrThrow();
    setManaged(false);
}


nitf::GraphicSubheader GraphicSubheader::clone() throw(nitf::NITFException)
{
    nitf::GraphicSubheader dolly(
        nitf_GraphicSubheader_clone(getNativeOrThrow(), &error));
    dolly.setManaged(false);
    return dolly;
}

GraphicSubheader::~GraphicSubheader(){}

nitf::Field GraphicSubheader::getFilePartType()
{
    return nitf::Field(getNativeOrThrow()->filePartType);
}

nitf::Field GraphicSubheader::getGraphicID()
{
    return nitf::Field(getNativeOrThrow()->graphicID);
}

nitf::Field GraphicSubheader::getName()
{
    return nitf::Field(getNativeOrThrow()->name);
}

nitf::Field GraphicSubheader::getSecurityClass()
{
    return nitf::Field(getNativeOrThrow()->securityClass);
}

nitf::FileSecurity GraphicSubheader::getSecurityGroup()
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


nitf::Field GraphicSubheader::getEncrypted()
{
    return nitf::Field(getNativeOrThrow()->encrypted);
}

nitf::Field GraphicSubheader::getStype()
{
    return nitf::Field(getNativeOrThrow()->stype);
}

nitf::Field GraphicSubheader::getRes1()
{
    return nitf::Field(getNativeOrThrow()->res1);
}

nitf::Field GraphicSubheader::getDisplayLevel()
{
    return nitf::Field(getNativeOrThrow()->displayLevel);
}

nitf::Field GraphicSubheader::getAttachmentLevel()
{
    return nitf::Field(getNativeOrThrow()->attachmentLevel);
}

nitf::Field GraphicSubheader::getLocation()
{
    return nitf::Field(getNativeOrThrow()->location);
}

nitf::Field GraphicSubheader::getBound1Loc()
{
    return nitf::Field(getNativeOrThrow()->bound1Loc);
}

nitf::Field GraphicSubheader::getColor()
{
    return nitf::Field(getNativeOrThrow()->color);
}

nitf::Field GraphicSubheader::getBound2Loc()
{
    return nitf::Field(getNativeOrThrow()->bound2Loc);
}

nitf::Field GraphicSubheader::getRes2()
{
    return nitf::Field(getNativeOrThrow()->res2);
}

nitf::Field GraphicSubheader::getExtendedHeaderLength()
{
    return nitf::Field(getNativeOrThrow()->extendedHeaderLength);
}

nitf::Field GraphicSubheader::getExtendedHeaderOverflow()
{
    return nitf::Field(getNativeOrThrow()->extendedHeaderOverflow);
}

nitf::Extensions GraphicSubheader::getExtendedSection()
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

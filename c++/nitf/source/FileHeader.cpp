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

#include "nitf/FileHeader.hpp"

using namespace nitf;

FileHeader::FileHeader(const FileHeader & x)
{
    setNative(x.getNative());
}

FileHeader & FileHeader::operator=(const FileHeader & x)
{
    if (&x != this)
        setNative(x.getNative());
    return *this;
}

FileHeader::FileHeader(nitf_FileHeader * x)
{
    setNative(x);
    getNativeOrThrow();
}

FileHeader::FileHeader() throw(nitf::NITFException)
{
    setNative(nitf_FileHeader_construct(&error));
    getNativeOrThrow();
    setManaged(false);
}


nitf::FileHeader FileHeader::clone() throw(nitf::NITFException)
{
    nitf::FileHeader dolly(nitf_FileHeader_clone(getNativeOrThrow(), &error));
    dolly.setManaged(false);
    return dolly;
}

FileHeader::~FileHeader(){}

nitf::Field FileHeader::getFileHeader()
{
    return nitf::Field(getNativeOrThrow()->fileHeader);
}

nitf::Field FileHeader::getFileVersion()
{
    return nitf::Field(getNativeOrThrow()->fileVersion);
}

nitf::Field FileHeader::getComplianceLevel()
{
    return nitf::Field(getNativeOrThrow()->complianceLevel);
}

nitf::Field FileHeader::getSystemType()
{
    return nitf::Field(getNativeOrThrow()->systemType);
}

nitf::Field FileHeader::getOriginStationID()
{
    return nitf::Field(getNativeOrThrow()->originStationID);
}

nitf::Field FileHeader::getFileDateTime()
{
    return nitf::Field(getNativeOrThrow()->fileDateTime);
}

nitf::Field FileHeader::getFileTitle()
{
    return nitf::Field(getNativeOrThrow()->fileTitle);
}

nitf::Field FileHeader::getClassification()
{
    return nitf::Field(getNativeOrThrow()->classification);
}

nitf::FileSecurity FileHeader::getSecurityGroup()
{
    return nitf::FileSecurity(getNativeOrThrow()->securityGroup);
}

void FileHeader::setSecurityGroup(nitf::FileSecurity value)
{
    //release the owned security group
    nitf::FileSecurity fs = nitf::FileSecurity(getNativeOrThrow()->securityGroup);
    fs.setManaged(false);

    //have the library manage the new securitygroup
    getNativeOrThrow()->securityGroup = value.getNative();
    value.setManaged(true);
}

nitf::Field FileHeader::getMessageCopyNum()
{
    return nitf::Field(getNativeOrThrow()->messageCopyNum);
}

nitf::Field FileHeader::getMessageNumCopies()
{
    return nitf::Field(getNativeOrThrow()->messageNumCopies);
}

nitf::Field FileHeader::getEncrypted()
{
    return nitf::Field(getNativeOrThrow()->encrypted);
}

nitf::Field FileHeader::getBackgroundColor()
{
    return nitf::Field(getNativeOrThrow()->backgroundColor);
}

nitf::Field FileHeader::getOriginatorName()
{
    return nitf::Field(getNativeOrThrow()->originatorName);
}

nitf::Field FileHeader::getOriginatorPhone()
{
    return nitf::Field(getNativeOrThrow()->originatorPhone);
}

nitf::Field FileHeader::getFileLength()
{
    return nitf::Field(getNativeOrThrow()->fileLength);
}

nitf::Field FileHeader::getHeaderLength()
{
    return nitf::Field(getNativeOrThrow()->headerLength);
}

nitf::Field FileHeader::getNumImages()
{
    return nitf::Field(getNativeOrThrow()->numImages);
}

nitf::Field FileHeader::getNumGraphics()
{
    return nitf::Field(getNativeOrThrow()->numGraphics);
}

nitf::Field FileHeader::getNumLabels()
{
    return nitf::Field(getNativeOrThrow()->numLabels);
}

nitf::Field FileHeader::getNumTexts()
{
    return nitf::Field(getNativeOrThrow()->numTexts);
}

nitf::Field FileHeader::getNumDataExtensions()
{
    return nitf::Field(getNativeOrThrow()->numDataExtensions);
}

nitf::Field FileHeader::getNumReservedExtensions()
{
    return nitf::Field(getNativeOrThrow()->numReservedExtensions);
}

nitf::ComponentInfo FileHeader::getImageInfo(int i)
    throw(except::IndexOutOfRangeException)
{
    int num = getNumImages();
    if (i < 0 || i >= num)
        throw except::IndexOutOfRangeException(Ctxt(FmtX(
                "Index out of range: (%d <= %d <= %d)", 0, i, num)));
    return nitf::ComponentInfo(getNativeOrThrow()->imageInfo[i]);
}

nitf::ComponentInfo FileHeader::getGraphicInfo(int i)
    throw(except::IndexOutOfRangeException)
{
    int num = getNumGraphics();
    if (i < 0 || i >= num)
        throw except::IndexOutOfRangeException(Ctxt(FmtX(
                "Index out of range: (%d <= %d <= %d)", 0, i, num)));
    return nitf::ComponentInfo(getNativeOrThrow()->graphicInfo[i]);
}

nitf::ComponentInfo FileHeader::getLabelInfo(int i)
    throw(except::IndexOutOfRangeException)
{
    int num = getNumLabels();
    if (i < 0 || i >= num)
        throw except::IndexOutOfRangeException(Ctxt(FmtX(
                "Index out of range: (%d <= %d <= %d)", 0, i, num)));
    return nitf::ComponentInfo(getNativeOrThrow()->labelInfo[i]);
}

nitf::ComponentInfo FileHeader::getTextInfo(int i)
    throw(except::IndexOutOfRangeException)
{
    int num = getNumTexts();
    if (i < 0 || i >= num)
        throw except::IndexOutOfRangeException(Ctxt(FmtX(
                "Index out of range: (%d <= %d <= %d)", 0, i, num)));
    return nitf::ComponentInfo(getNativeOrThrow()->textInfo[i]);
}

nitf::ComponentInfo FileHeader::getDataExtensionInfo(int i)
    throw(except::IndexOutOfRangeException)
{
    int num = getNumDataExtensions();
    if (i < 0 || i >= num)
        throw except::IndexOutOfRangeException(Ctxt(FmtX(
                "Index out of range: (%d <= %d <= %d)", 0, i, num)));
    return nitf::ComponentInfo(getNativeOrThrow()->dataExtensionInfo[i]);
}

nitf::ComponentInfo FileHeader::getReservedExtensionInfo(int i)
    throw(except::IndexOutOfRangeException)
{
    int num = getNumReservedExtensions();
    if (i < 0 || i >= num)
        throw except::IndexOutOfRangeException(Ctxt(FmtX(
                "Index out of range: (%d <= %d <= %d)", 0, i, num)));
    return nitf::ComponentInfo(getNativeOrThrow()->reservedExtensionInfo[i]);
}

nitf::Field FileHeader::getUserDefinedHeaderLength()
{
    return nitf::Field(getNativeOrThrow()->userDefinedHeaderLength);
}

nitf::Field FileHeader::getUserDefinedOverflow()
{
    return nitf::Field(getNativeOrThrow()->userDefinedOverflow);
}

nitf::Field FileHeader::getExtendedHeaderLength()
{
    return nitf::Field(getNativeOrThrow()->extendedHeaderLength);
}

nitf::Field FileHeader::getExtendedHeaderOverflow()
{
    return nitf::Field(getNativeOrThrow()->extendedHeaderOverflow);
}

nitf::Extensions FileHeader::getUserDefinedSection()
{
    return nitf::Extensions(getNativeOrThrow()->userDefinedSection);
}

void FileHeader::setUserDefinedSection(nitf::Extensions value)
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
    nitf::Extensions(value).setManaged(true);
    value.setManaged(true);
}


nitf::Extensions FileHeader::getExtendedSection()
{
    return nitf::Extensions(getNativeOrThrow()->extendedSection);
}

void FileHeader::setExtendedSection(nitf::Extensions value)
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


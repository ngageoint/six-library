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

#include "nitf/FileHeader.hpp"

using namespace nitf;

FileHeader::FileHeader(const FileHeader & x)
{
    *this = x;
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

FileHeader::FileHeader() : FileHeader(nitf_FileHeader_construct(&error))
{
    setManaged(false);
}


nitf::FileHeader FileHeader::clone() const
{
    nitf::FileHeader dolly(nitf_FileHeader_clone(getNativeOrThrow(), &error));
    dolly.setManaged(false);
    return dolly;
}

nitf::Field FileHeader::getFileHeader() const
{
    return nitf::Field(getNativeOrThrow()->fileHeader);
}

nitf::Field FileHeader::getFileVersion() const
{
    return nitf::Field(getNativeOrThrow()->fileVersion);
}

nitf::Field FileHeader::getComplianceLevel() const
{
    return nitf::Field(getNativeOrThrow()->complianceLevel);
}

nitf::Field FileHeader::getSystemType() const
{
    return nitf::Field(getNativeOrThrow()->systemType);
}

nitf::Field FileHeader::getOriginStationID() const
{
    return nitf::Field(getNativeOrThrow()->originStationID);
}

nitf::Field FileHeader::getFileDateTime() const
{
    return nitf::Field(getNativeOrThrow()->fileDateTime);
}

nitf::Field FileHeader::getFileTitle() const
{
    return nitf::Field(getNativeOrThrow()->fileTitle);
}

nitf::Field FileHeader::getClassification() const
{
    return nitf::Field(getNativeOrThrow()->classification);
}

nitf::FileSecurity FileHeader::getSecurityGroup() const
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

nitf::Field FileHeader::getMessageCopyNum() const
{
    return nitf::Field(getNativeOrThrow()->messageCopyNum);
}

nitf::Field FileHeader::getMessageNumCopies() const
{
    return nitf::Field(getNativeOrThrow()->messageNumCopies);
}

nitf::Field FileHeader::getEncrypted() const
{
    return nitf::Field(getNativeOrThrow()->encrypted);
}

nitf::Field FileHeader::getBackgroundColor() const
{
    return nitf::Field(getNativeOrThrow()->backgroundColor);
}

nitf::Field FileHeader::getOriginatorName() const
{
    return nitf::Field(getNativeOrThrow()->originatorName);
}

nitf::Field FileHeader::getOriginatorPhone() const
{
    return nitf::Field(getNativeOrThrow()->originatorPhone);
}

nitf::Field FileHeader::getFileLength() const
{
    return nitf::Field(getNativeOrThrow()->fileLength);
}

nitf::Field FileHeader::getHeaderLength() const
{
    return nitf::Field(getNativeOrThrow()->headerLength);
}

nitf::Field FileHeader::getNumImages() const
{
    return nitf::Field(getNativeOrThrow()->numImages);
}

nitf::Field FileHeader::getNumGraphics() const
{
    return nitf::Field(getNativeOrThrow()->numGraphics);
}

nitf::Field FileHeader::getNumLabels() const
{
    return nitf::Field(getNativeOrThrow()->numLabels);
}

nitf::Field FileHeader::getNumTexts() const
{
    return nitf::Field(getNativeOrThrow()->numTexts);
}

nitf::Field FileHeader::getNumDataExtensions() const
{
    return nitf::Field(getNativeOrThrow()->numDataExtensions);
}

nitf::Field FileHeader::getNumReservedExtensions() const
{
    return nitf::Field(getNativeOrThrow()->numReservedExtensions);
}

static nitf::ComponentInfo make_ComponentInfo(nitf_ComponentInfo** pComponentInfo, int i)
{
    if (pComponentInfo == nullptr)
    {
        throw except::NullPointerReference(Ctxt("FileHeader"));
    }
    return nitf::ComponentInfo(pComponentInfo[i]);
}

nitf::ComponentInfo FileHeader::getImageInfo(int i) const
{
    int num = getNumImages();
    if (i < 0 || i >= num)
        throw except::IndexOutOfRangeException(Ctxt(FmtX(
                "Index out of range: (%d <= %d <= %d)", 0, i, num)));
    return make_ComponentInfo(getNativeOrThrow()->imageInfo, i);
}

nitf::ComponentInfo FileHeader::getGraphicInfo(int i) const
{
    int num = getNumGraphics();
    if (i < 0 || i >= num)
        throw except::IndexOutOfRangeException(Ctxt(FmtX(
                "Index out of range: (%d <= %d <= %d)", 0, i, num)));
    return make_ComponentInfo(getNativeOrThrow()->graphicInfo, i);
}

nitf::ComponentInfo FileHeader::getLabelInfo(int i) const
{
    int num = getNumLabels();
    if (i < 0 || i >= num)
        throw except::IndexOutOfRangeException(Ctxt(FmtX(
                "Index out of range: (%d <= %d <= %d)", 0, i, num)));
    return make_ComponentInfo(getNativeOrThrow()->labelInfo, i);
}

nitf::ComponentInfo FileHeader::getTextInfo(int i) const
{
    int num = getNumTexts();
    if (i < 0 || i >= num)
        throw except::IndexOutOfRangeException(Ctxt(FmtX(
                "Index out of range: (%d <= %d <= %d)", 0, i, num)));
    return make_ComponentInfo(getNativeOrThrow()->textInfo, i);
}

nitf::ComponentInfo FileHeader::getDataExtensionInfo(int i) const
{
    int num = getNumDataExtensions();
    if (i < 0 || i >= num)
        throw except::IndexOutOfRangeException(Ctxt(FmtX(
                "Index out of range: (%d <= %d <= %d)", 0, i, num)));
    return make_ComponentInfo(getNativeOrThrow()->dataExtensionInfo, i);
}

nitf::ComponentInfo FileHeader::getReservedExtensionInfo(int i) const
{
    int num = getNumReservedExtensions();
    if (i < 0 || i >= num)
        throw except::IndexOutOfRangeException(Ctxt(FmtX(
                "Index out of range: (%d <= %d <= %d)", 0, i, num)));
    return make_ComponentInfo(getNativeOrThrow()->reservedExtensionInfo, i);
}

nitf::Field FileHeader::getUserDefinedHeaderLength() const
{
    return nitf::Field(getNativeOrThrow()->userDefinedHeaderLength);
}

nitf::Field FileHeader::getUserDefinedOverflow() const
{
    return nitf::Field(getNativeOrThrow()->userDefinedOverflow);
}

nitf::Field FileHeader::getExtendedHeaderLength() const
{
    return nitf::Field(getNativeOrThrow()->extendedHeaderLength);
}

nitf::Field FileHeader::getExtendedHeaderOverflow() const
{
    return nitf::Field(getNativeOrThrow()->extendedHeaderOverflow);
}

nitf::Extensions FileHeader::getUserDefinedSection() const
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


nitf::Extensions FileHeader::getExtendedSection() const
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


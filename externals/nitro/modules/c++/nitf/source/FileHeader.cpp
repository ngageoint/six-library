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

#include <type_traits>

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

FileHeader::FileHeader() noexcept(false)  : FileHeader(nitf_FileHeader_construct(&error))
{
    setManaged(false);
}


nitf::FileHeader FileHeader::clone() const
{
    nitf::FileHeader dolly(nitf_FileHeader_clone(getNativeOrThrow(), &error));
    dolly.setManaged(false);
    return dolly;
}

#define getField_(name) nitf::Field(getNativeOrThrow()->name)
//#define getField_(name) fromNativeOffset<Field>(*this, nitf_offsetof(name));

nitf::Field FileHeader::getFileHeader() const
{
    return getField_(fileHeader);
}

nitf::Field FileHeader::getFileVersion() const
{
    return getField_(fileVersion);
}

nitf::Field FileHeader::getComplianceLevel() const
{
    return getField_(complianceLevel);
}

nitf::Field FileHeader::getSystemType() const
{
    return getField_(systemType);
}

nitf::Field FileHeader::getOriginStationID() const
{
    return getField_(originStationID);
}

nitf::Field FileHeader::getFileDateTime() const
{
    return getField_(fileDateTime);
}

nitf::Field FileHeader::getFileTitle() const
{
    return getField_(fileTitle);
}

nitf::Field FileHeader::getClassification() const
{
    return getField_(classification);
}

nitf::FileSecurity FileHeader::getSecurityGroup() const
{
    //return fromNativeOffset<FileSecurity>(*this, nitf_offsetof(securityGroup));
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
    return getField_(messageCopyNum);
}

nitf::Field FileHeader::getMessageNumCopies() const
{
    return getField_(messageNumCopies);
}

nitf::Field FileHeader::getEncrypted() const
{
    return getField_(encrypted);
}

nitf::Field FileHeader::getBackgroundColor() const
{
    return getField_(backgroundColor);
}

nitf::Field FileHeader::getOriginatorName() const
{
    return getField_(originatorName);
}

nitf::Field FileHeader::getOriginatorPhone() const
{
    return getField_(originatorPhone);
}

nitf::Field FileHeader::getFileLength() const
{
    return getField_(fileLength);
}

nitf::Field FileHeader::getHeaderLength() const
{
    return getField_(headerLength);
}

nitf::Field FileHeader::getNumImages() const
{
    return getField_(numImages);
}

nitf::Field FileHeader::getNumGraphics() const
{
    return getField_(numGraphics);
}

nitf::Field FileHeader::getNumLabels() const
{
    return getField_(numLabels);
}

nitf::Field FileHeader::getNumTexts() const
{
    return getField_(numTexts);
}

nitf::Field FileHeader::getNumDataExtensions() const
{
    return getField_(numDataExtensions);
}

nitf::Field FileHeader::getNumReservedExtensions() const
{
    return getField_(numReservedExtensions);
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
    return getField_(userDefinedHeaderLength);
}

nitf::Field FileHeader::getUserDefinedOverflow() const
{
    return getField_(userDefinedOverflow);
}

nitf::Field FileHeader::getExtendedHeaderLength() const
{
    return getField_(extendedHeaderLength);
}

nitf::Field FileHeader::getExtendedHeaderOverflow() const
{
    return getField_(extendedHeaderOverflow);
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

/*
std::vector<nitf::Field> FileHeader::getFields() const
{
    std::vector<nitf::Field> retval;

    constexpr auto extent = std::extent<decltype(nitf_FileHeader_fields)>::value;
    for (size_t i = 0; i < extent; i++)
    {
        const auto& desc = nitf_FileHeader_fields[i];
        if (desc.type == NITF_FieldType_Field)
        {
            auto field = fromNativeOffset<Field>(*this, desc.offset);
            retval.push_back(std::move(field));
        }
    }

    return retval;
}
*/
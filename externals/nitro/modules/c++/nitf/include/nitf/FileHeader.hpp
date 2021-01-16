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

#ifndef __NITF_FILEHEADER_HPP__
#define __NITF_FILEHEADER_HPP__
#pragma once

#include "nitf/FileHeader.h"
#include "nitf/System.hpp"
#include "nitf/FileSecurity.hpp"
#include "nitf/Extensions.hpp"
#include "nitf/ComponentInfo.hpp"
#include "nitf/Field.hpp"
#include "nitf/Object.hpp"
#include <string>

/*!
 *  \file FileHeader.hpp
 *  \brief  Contains wrapper implementation for FileHeader
 */

namespace nitf
{

/*!
 *  \class FileHeader
 *  \brief  The C++ wrapper for the nitf_FileHeader
 *
 *  A structure representing the in-memory layout of the NITF file.
 *
 */
DECLARE_CLASS(FileHeader)
{
public:

    //! Copy constructor
    FileHeader(const FileHeader & x);

    //! Assignment Operator
    FileHeader & operator=(const FileHeader & x);

    //! Set native object
    FileHeader(nitf_FileHeader * x);

    //! Constructor
    FileHeader();

    //! Clone
    nitf::FileHeader clone() const;

    ~FileHeader() = default;

    //! Get the fileHeader
    nitf::Field getFileHeader() const;
    std::string fileHeader() const
    {
        return getFileHeader(); // nitf::Field implicitly converts to std::string
    }

    //! Get the fileVersion
    nitf::Field getFileVersion() const;
    std::string fileVersion() const
    {
        return getFileVersion(); // nitf::Field implicitly converts to std::string
    }

    //! Get the complianceLevel
    nitf::Field getComplianceLevel() const;

    //! Get the systemType
    nitf::Field getSystemType() const;

    //! Get the originStationID
    nitf::Field getOriginStationID() const;

    //! Get the fileDateTime
    nitf::Field getFileDateTime() const;

    //! Get the fileTitle
    nitf::Field getFileTitle() const;

    //! Get the classification
    nitf::Field getClassification() const;
    std::string classification() const
    {
        return getClassification(); // nitf::Field implicitly converts to std::string
    }

    //! Get the securityGroup
    nitf::FileSecurity getSecurityGroup() const;

    //! Set the securityGroup
    void setSecurityGroup(nitf::FileSecurity value);

    //! Get the messageCopyNum
    nitf::Field getMessageCopyNum() const;

    //! Get the messageNumCopies
    nitf::Field getMessageNumCopies() const;

    //! Get the encrypted
    nitf::Field getEncrypted() const;

    //! Get the backgroundColor
    nitf::Field getBackgroundColor() const;

    //! Get the originatorName
    nitf::Field getOriginatorName() const;

    //! Get the originatorPhone
    nitf::Field getOriginatorPhone() const;

    //! Get the fileLength
    nitf::Field getFileLength() const;

    //! Get the headerLength
    nitf::Field getHeaderLength() const;

    //! Get the numImages
    nitf::Field getNumImages() const;

    //! Get the numGraphics
    nitf::Field getNumGraphics() const;

    //! Get the numLabels
    nitf::Field getNumLabels() const;

    //! Get the numTexts
    nitf::Field getNumTexts() const;

    //! Get the numDataExtensions
    nitf::Field getNumDataExtensions() const;

    //! Get the numReservedExtensions
    nitf::Field getNumReservedExtensions() const;

    //! Get the imageInfo
    nitf::ComponentInfo getImageInfo(int i) const;

    //! Get the graphicInfo
    nitf::ComponentInfo getGraphicInfo(int i) const;

    //! Get the labelInfo
    nitf::ComponentInfo getLabelInfo(int i) const;

    //! Get the textInfo
    nitf::ComponentInfo getTextInfo(int i) const;

    //! Get the dataExtensionInfo
    nitf::ComponentInfo getDataExtensionInfo(int i) const;

    //! Get the reservedExtensionInfo
    nitf::ComponentInfo getReservedExtensionInfo(int i) const;

    //! Get the userDefinedHeaderLength
    nitf::Field getUserDefinedHeaderLength() const;

    //! Get the userDefinedOverflow
    nitf::Field getUserDefinedOverflow() const;

    //! Get the extendedHeaderLength
    nitf::Field getExtendedHeaderLength() const;

    //! Get the extendedHeaderOverflow
    nitf::Field getExtendedHeaderOverflow() const;

    //! Get the userDefinedSection
    nitf::Extensions getUserDefinedSection() const;

    //! Set the userDefinedSection
    void setUserDefinedSection(nitf::Extensions value);

    //! Get the extendedSection
    nitf::Extensions getExtendedSection() const;

    //! Set the extendedSection
    void setExtendedSection(nitf::Extensions value);

private:
    mutable nitf_Error error{};
};

}
#endif

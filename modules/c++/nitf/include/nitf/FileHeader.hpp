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

#ifndef __NITF_FILEHEADER_HPP__
#define __NITF_FILEHEADER_HPP__

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
    FileHeader() throw(nitf::NITFException);

    //! Clone
    nitf::FileHeader clone() throw(nitf::NITFException);

    ~FileHeader();

    //! Get the fileHeader
    nitf::Field getFileHeader();

    //! Get the fileVersion
    nitf::Field getFileVersion();

    //! Get the complianceLevel
    nitf::Field getComplianceLevel();

    //! Get the systemType
    nitf::Field getSystemType();

    //! Get the originStationID
    nitf::Field getOriginStationID();

    //! Get the fileDateTime
    nitf::Field getFileDateTime();

    //! Get the fileTitle
    nitf::Field getFileTitle();

    //! Get the classification
    nitf::Field getClassification();

    //! Get the securityGroup
    nitf::FileSecurity getSecurityGroup();

    //! Set the securityGroup
    void setSecurityGroup(nitf::FileSecurity value);

    //! Get the messageCopyNum
    nitf::Field getMessageCopyNum();

    //! Get the messageNumCopies
    nitf::Field getMessageNumCopies();

    //! Get the encrypted
    nitf::Field getEncrypted();

    //! Get the backgroundColor
    nitf::Field getBackgroundColor();

    //! Get the originatorName
    nitf::Field getOriginatorName();

    //! Get the originatorPhone
    nitf::Field getOriginatorPhone();

    //! Get the fileLength
    nitf::Field getFileLength();

    //! Get the headerLength
    nitf::Field getHeaderLength();

    //! Get the numImages
    nitf::Field getNumImages();

    //! Get the numGraphics
    nitf::Field getNumGraphics();

    //! Get the numLabels
    nitf::Field getNumLabels();

    //! Get the numTexts
    nitf::Field getNumTexts();

    //! Get the numDataExtensions
    nitf::Field getNumDataExtensions();

    //! Get the numReservedExtensions
    nitf::Field getNumReservedExtensions();

    //! Get the imageInfo
    nitf::ComponentInfo getImageInfo(int i)
        throw(except::IndexOutOfRangeException);

    //! Get the graphicInfo
    nitf::ComponentInfo getGraphicInfo(int i)
        throw(except::IndexOutOfRangeException);

    //! Get the labelInfo
    nitf::ComponentInfo getLabelInfo(int i)
        throw(except::IndexOutOfRangeException);

    //! Get the textInfo
    nitf::ComponentInfo getTextInfo(int i)
        throw(except::IndexOutOfRangeException);

    //! Get the dataExtensionInfo
    nitf::ComponentInfo getDataExtensionInfo(int i)
        throw(except::IndexOutOfRangeException);

    //! Get the reservedExtensionInfo
    nitf::ComponentInfo getReservedExtensionInfo(int i)
        throw(except::IndexOutOfRangeException);

    //! Get the userDefinedHeaderLength
    nitf::Field getUserDefinedHeaderLength();

    //! Get the userDefinedOverflow
    nitf::Field getUserDefinedOverflow();

    //! Get the extendedHeaderLength
    nitf::Field getExtendedHeaderLength();

    //! Get the extendedHeaderOverflow
    nitf::Field getExtendedHeaderOverflow();

    //! Get the userDefinedSection
    nitf::Extensions getUserDefinedSection();

    //! Set the userDefinedSection
    void setUserDefinedSection(nitf::Extensions value);

    //! Get the extendedSection
    nitf::Extensions getExtendedSection();

    //! Set the extendedSection
    void setExtendedSection(nitf::Extensions value);

private:
    nitf_Error error;
};

}
#endif

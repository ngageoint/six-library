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

#include "nitf/ImageSubheader.hpp"

using namespace nitf;

ImageSubheader::ImageSubheader(const ImageSubheader & x)
{
    setNative(x.getNative());
}

ImageSubheader & ImageSubheader::operator=(const ImageSubheader & x)
{
    if (&x != this)
        setNative(x.getNative());
    return *this;
}

ImageSubheader::ImageSubheader(nitf_ImageSubheader * x)
{
    setNative(x);
    getNativeOrThrow();
}

ImageSubheader::ImageSubheader() throw(nitf::NITFException)
{
    setNative(nitf_ImageSubheader_construct(&error));
    getNativeOrThrow();
    setManaged(false);
}


nitf::ImageSubheader ImageSubheader::clone() throw(nitf::NITFException)
{
    nitf::ImageSubheader dolly(nitf_ImageSubheader_clone(getNativeOrThrow(), &error));
    dolly.setManaged(false);
    return dolly;
}

/*!
 *  Destructor
 */
ImageSubheader::~ImageSubheader(){}


void ImageSubheader::setPixelInformation(std::string pvtype,
                         nitf::Uint32 nbpp,
                         nitf::Uint32 abpp,
                         std::string justification,
                         std::string irep, std::string icat,
                         std::vector<nitf::BandInfo>& bands) throw(nitf::NITFException)
{
    nitf::Uint32 bandCount = bands.size();
    nitf_BandInfo ** bandInfo = (nitf_BandInfo **)NITF_MALLOC(
            sizeof(nitf_BandInfo*) * bandCount);
    if (!bandInfo)
    {
        throw nitf::NITFException(Ctxt(FmtX("Out of Memory")));
    }

    for (nitf::Uint32 i = 0; i < bandCount; i++)
    {
        bandInfo[i] = nitf_BandInfo_clone(bands[i].getNative(), &error);
        if (!bandInfo[i])
            throw nitf::NITFException(&error);
    }

    NITF_BOOL x = nitf_ImageSubheader_setPixelInformation(getNativeOrThrow(),
        pvtype.c_str(), nbpp, abpp, justification.c_str(), irep.c_str(),
        icat.c_str(), bandCount, bandInfo, &error);
    if (!x)
        throw nitf::NITFException(&error);
}


void ImageSubheader::setPixelInformation(std::string pvtype,
                         nitf::Uint32 nbpp,
                         nitf::Uint32 abpp,
                         std::string justification,
                         std::string irep, std::string icat,
                         nitf::Uint32 bandCount,
                         std::vector<nitf::BandInfo>& bands) throw(nitf::NITFException)
{
    return setPixelInformation(pvtype, nbpp, abpp, justification, irep, icat,
            bands);
}

void ImageSubheader::setBlocking(nitf::Uint32 numRows,
                     nitf::Uint32 numCols,
                     nitf::Uint32 numRowsPerBlock,
                     nitf::Uint32 numColsPerBlock,
                     const std::string& imode) throw(nitf::NITFException)
{
    NITF_BOOL x = nitf_ImageSubheader_setBlocking(getNativeOrThrow(),
        numRows, numCols, numRowsPerBlock, numColsPerBlock, imode.c_str(),
        &error);
    if (!x)
        throw nitf::NITFException(&error);
}

void ImageSubheader::setDimensions(nitf::Uint32 numRows, nitf::Uint32 numCols)
    throw(nitf::NITFException)
{
    NITF_BOOL x = nitf_ImageSubheader_setDimensions(getNativeOrThrow(),
        numRows, numCols, &error);
    if (!x)
        throw nitf::NITFException(&error);
}

nitf::Uint32 ImageSubheader::getBandCount() throw(nitf::NITFException)
{
    nitf::Uint32 x = nitf_ImageSubheader_getBandCount(getNativeOrThrow(), &error);
    if (x == NITF_INVALID_BAND_COUNT)
        throw nitf::NITFException(&error);
    return x;
}

void ImageSubheader::createBands(nitf::Uint32 numBands) throw(nitf::NITFException)
{
    if (!nitf_ImageSubheader_createBands(getNativeOrThrow(), numBands, &error))
        throw nitf::NITFException(&error);
}


void ImageSubheader::setCornersFromLatLons(nitf::CornersType type,
                                           double corners[4][2])
    throw(nitf::NITFException)
{
    NITF_BOOL x = nitf_ImageSubheader_setCornersFromLatLons(getNativeOrThrow(),
                                                            type,
                                                            corners,
                                                            &error);
    if (!x)
        throw nitf::NITFException(&error);

}

void ImageSubheader::getCornersAsLatLons(double corners[4][2])
    throw(nitf::NITFException)
{
    NITF_BOOL x = nitf_ImageSubheader_getCornersAsLatLons(getNativeOrThrow(),
                                                          corners,
                                                          &error);
    if (!x)
        throw nitf::NITFException(&error);

}

nitf::CornersType ImageSubheader::getCornersType()
    throw(nitf::NITFException)
{
    return nitf_ImageSubheader_getCornersType(getNativeOrThrow());
}

int ImageSubheader::insertImageComment(std::string comment, int index)
{
    int actualIndex = nitf_ImageSubheader_insertImageComment(getNativeOrThrow(),
                  (char*)comment.c_str(), index, &error);
    if (actualIndex < 0)
        throw nitf::NITFException(&error);
    return actualIndex;
}


void ImageSubheader::removeImageComment(int index)
{
    NITF_BOOL x = nitf_ImageSubheader_removeImageComment(getNativeOrThrow(),
                  index, &error);
    if (!x)
        throw nitf::NITFException(&error);
}


nitf::Field ImageSubheader::getFilePartType()
{
    return nitf::Field(getNativeOrThrow()->filePartType);
}

nitf::Field ImageSubheader::getImageId()
{
    return nitf::Field(getNativeOrThrow()->imageId);
}

nitf::Field ImageSubheader::getImageDateAndTime()
{
    return nitf::Field(getNativeOrThrow()->imageDateAndTime);
}

nitf::Field ImageSubheader::getTargetId()
{
    return nitf::Field(getNativeOrThrow()->targetId);
}

nitf::Field ImageSubheader::getImageTitle()
{
    return nitf::Field(getNativeOrThrow()->imageTitle);
}

nitf::Field ImageSubheader::getImageSecurityClass()
{
    return nitf::Field(getNativeOrThrow()->imageSecurityClass);
}

nitf::FileSecurity ImageSubheader::getSecurityGroup()
{
    return nitf::FileSecurity(getNativeOrThrow()->securityGroup);
}

void ImageSubheader::setSecurityGroup(nitf::FileSecurity value)
{
    //release the owned security group
    nitf::FileSecurity fs = nitf::FileSecurity(getNativeOrThrow()->securityGroup);
    fs.setManaged(false);

    //have the library manage the new securitygroup
    getNativeOrThrow()->securityGroup = value.getNative();
    value.setManaged(true);
}

nitf::Field ImageSubheader::getEncrypted()
{
    return nitf::Field(getNativeOrThrow()->encrypted);
}

nitf::Field ImageSubheader::getImageSource()
{
    return nitf::Field(getNativeOrThrow()->imageSource);
}

nitf::Field ImageSubheader::getNumRows()
{
    return nitf::Field(getNativeOrThrow()->numRows);
}

nitf::Field ImageSubheader::getNumCols()
{
    return nitf::Field(getNativeOrThrow()->numCols);
}

nitf::Field ImageSubheader::getPixelValueType()
{
    return nitf::Field(getNativeOrThrow()->pixelValueType);
}

nitf::Field ImageSubheader::getImageRepresentation()
{
    return nitf::Field(getNativeOrThrow()->imageRepresentation);
}

nitf::Field ImageSubheader::getImageCategory()
{
    return nitf::Field(getNativeOrThrow()->imageCategory);
}

nitf::Field ImageSubheader::getActualBitsPerPixel()
{
    return nitf::Field(getNativeOrThrow()->actualBitsPerPixel);
}

nitf::Field ImageSubheader::getPixelJustification()
{
    return nitf::Field(getNativeOrThrow()->pixelJustification);
}

nitf::Field ImageSubheader::getImageCoordinateSystem()
{
    return nitf::Field(getNativeOrThrow()->imageCoordinateSystem);
}

nitf::Field ImageSubheader::getCornerCoordinates()
{
    return nitf::Field(getNativeOrThrow()->cornerCoordinates);
}

nitf::Field ImageSubheader::getNumImageComments()
{
    return nitf::Field(getNativeOrThrow()->numImageComments);
}

nitf::List ImageSubheader::getImageComments()
{
    return nitf::List(getNativeOrThrow()->imageComments);
}

nitf::Field ImageSubheader::getImageCompression()
{
    return nitf::Field(getNativeOrThrow()->imageCompression);
}

nitf::Field ImageSubheader::getCompressionRate()
{
    return nitf::Field(getNativeOrThrow()->compressionRate);
}

nitf::Field ImageSubheader::getNumImageBands()
{
    return nitf::Field(getNativeOrThrow()->numImageBands);
}

nitf::Field ImageSubheader::getNumMultispectralImageBands()
{
    return nitf::Field(getNativeOrThrow()->numMultispectralImageBands);
}

nitf::BandInfo ImageSubheader::getBandInfo(nitf::Uint32 band) throw(nitf::NITFException)
{
    return nitf::BandInfo(nitf_ImageSubheader_getBandInfo(
        getNativeOrThrow(), band, &error));
}

nitf::Field ImageSubheader::getImageSyncCode()
{
    return nitf::Field(getNativeOrThrow()->imageSyncCode);
}

nitf::Field ImageSubheader::getImageMode()
{
    return nitf::Field(getNativeOrThrow()->imageMode);
}

nitf::Field ImageSubheader::getNumBlocksPerRow()
{
    return nitf::Field(getNativeOrThrow()->numBlocksPerRow);
}

nitf::Field ImageSubheader::getNumBlocksPerCol()
{
    return nitf::Field(getNativeOrThrow()->numBlocksPerCol);
}

nitf::Field ImageSubheader::getNumPixelsPerHorizBlock()
{
    return nitf::Field(getNativeOrThrow()->numPixelsPerHorizBlock);
}

nitf::Field ImageSubheader::getNumPixelsPerVertBlock()
{
    return nitf::Field(getNativeOrThrow()->numPixelsPerVertBlock);
}

nitf::Field ImageSubheader::getNumBitsPerPixel()
{
    return nitf::Field(getNativeOrThrow()->numBitsPerPixel);
}

nitf::Field ImageSubheader::getImageDisplayLevel()
{
    return nitf::Field(getNativeOrThrow()->imageDisplayLevel);
}

nitf::Field ImageSubheader::getImageAttachmentLevel()
{
    return nitf::Field(getNativeOrThrow()->imageAttachmentLevel);
}

nitf::Field ImageSubheader::getImageLocation()
{
    return nitf::Field(getNativeOrThrow()->imageLocation);
}

nitf::Field ImageSubheader::getImageMagnification()
{
    return nitf::Field(getNativeOrThrow()->imageMagnification);
}

nitf::Field ImageSubheader::getUserDefinedImageDataLength()
{
    return nitf::Field(getNativeOrThrow()->userDefinedImageDataLength);
}

nitf::Field ImageSubheader::getUserDefinedOverflow()
{
    return nitf::Field(getNativeOrThrow()->userDefinedOverflow);
}

nitf::Field ImageSubheader::getExtendedHeaderLength()
{
    return nitf::Field(getNativeOrThrow()->extendedHeaderLength);
}

nitf::Field ImageSubheader::getExtendedHeaderOverflow()
{
    return nitf::Field(getNativeOrThrow()->extendedHeaderOverflow);
}

nitf::Extensions ImageSubheader::getUserDefinedSection()
{
    return nitf::Extensions(getNativeOrThrow()->userDefinedSection);
}

void ImageSubheader::setUserDefinedSection(nitf::Extensions value)
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

nitf::Extensions ImageSubheader::getExtendedSection()
{
    return nitf::Extensions(getNativeOrThrow()->extendedSection);
}

void ImageSubheader::setExtendedSection(nitf::Extensions value)
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

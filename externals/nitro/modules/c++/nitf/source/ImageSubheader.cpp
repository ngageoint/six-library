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
#include "nitf/ImageSubheader.hpp"

#include <nitf/ImageIO.h>
#include <nitf/Object.hpp>
#include <nitf/NITFException.hpp>

#include "gsl/gsl.h"

namespace nitf
{
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

ImageSubheader::ImageSubheader() : ImageSubheader(nitf_ImageSubheader_construct(&error))
{
    setManaged(false);
}


nitf::ImageSubheader ImageSubheader::clone() const
{
    nitf::ImageSubheader dolly(nitf_ImageSubheader_clone(getNativeOrThrow(), &error));
    dolly.setManaged(false);
    return dolly;
}

void ImageSubheader::setPixelInformation(std::string pvtype,
                         uint32_t nbpp,
                         uint32_t abpp,
                         std::string justification,
                         std::string irep, std::string icat,
                         std::vector<nitf::BandInfo>& bands)
{
    const size_t bandCount = bands.size();
    #ifdef _MSC_VER
    #pragma warning(push)
    #pragma warning(disable: 26408) // Avoid malloc() and free(), prefer the nothrow version of new with delete (r.10).
    #endif
    auto bandInfo = static_cast<nitf_BandInfo**>(NITF_MALLOC(
            sizeof(nitf_BandInfo*) * bandCount));
    #ifdef _MSC_VER
    #pragma warning(pop)
    #endif

    if (!bandInfo)
    {
        throw nitf::NITFException(Ctxt(FmtX("Out of Memory")));
    }

    for (size_t i = 0; i < bandCount; i++)
    {
        auto clone = nitf_BandInfo_clone(bands[i].getNative(), &error);
        if (!clone)
            throw nitf::NITFException(&error);

        // not sure what's causing this warning
        #ifdef _MSC_VER
        #pragma warning(push)
        #pragma warning(disable: 6386) // Buffer overrun while writing to '...':  the writable size is '...' bytes, but '...' bytes might be written.
        #endif

        bandInfo[i] = clone;

        #ifdef _MSC_VER
        #pragma warning(pop)
        #endif
    }

    const NITF_BOOL x = nitf_ImageSubheader_setPixelInformation(getNativeOrThrow(),
        pvtype.c_str(), nbpp, abpp, justification.c_str(), irep.c_str(),
        icat.c_str(), static_cast<uint32_t>(bandCount), bandInfo, &error);
    if (!x)
        throw nitf::NITFException(&error);
}

void ImageSubheader::setBlocking(uint32_t numRows,
                     uint32_t numCols,
                     uint32_t numRowsPerBlock,
                     uint32_t numColsPerBlock,
                     const std::string& imode)
{
    const NITF_BOOL x = nitf_ImageSubheader_setBlocking(getNativeOrThrow(),
        numRows, numCols, numRowsPerBlock, numColsPerBlock, imode.c_str(),
        &error);
    if (!x)
        throw nitf::NITFException(&error);
}

void ImageSubheader::computeBlocking(uint32_t numRows,
                                     uint32_t numCols,
                                     uint32_t& numRowsPerBlock,
                                     uint32_t& numColsPerBlock,
                                     uint32_t& numBlocksPerCol,
                                     uint32_t& numBlocksPerRow) noexcept
{
    nitf_ImageSubheader_computeBlocking(numRows,
                                        numCols,
                                        &numRowsPerBlock,
                                        &numColsPerBlock,
                                        &numBlocksPerCol,
                                        &numBlocksPerRow);
}

void ImageSubheader::setDimensions(uint32_t numRows, uint32_t numCols)
{
    const NITF_BOOL x = nitf_ImageSubheader_setDimensions(getNativeOrThrow(),
        numRows, numCols, &error);
    if (!x)
        throw nitf::NITFException(&error);
}

uint32_t ImageSubheader::getBandCount() const
{
    const uint32_t x = nitf_ImageSubheader_getBandCount(getNativeOrThrow(), &error);
    if (x == NITF_INVALID_BAND_COUNT)
        throw nitf::NITFException(&error);
    return x;
}

void ImageSubheader::createBands(uint32_t numBands)
{
    if (!nitf_ImageSubheader_createBands(getNativeOrThrow(), numBands, &error))
        throw nitf::NITFException(&error);
}


void ImageSubheader::setCornersFromLatLons(nitf::CornersType type,
                                           double corners[4][2])
{
    const NITF_BOOL x = nitf_ImageSubheader_setCornersFromLatLons(getNativeOrThrow(),
                                                            type,
                                                            corners,
                                                            &error);
    if (!x)
        throw nitf::NITFException(&error);

}

void ImageSubheader::getCornersAsLatLons(double corners[4][2]) const
{
    const NITF_BOOL x = nitf_ImageSubheader_getCornersAsLatLons(getNativeOrThrow(),
                                                          corners,
                                                          &error);
    if (!x)
        throw nitf::NITFException(&error);

}

nitf::CornersType ImageSubheader::getCornersType() const
{
    return nitf_ImageSubheader_getCornersType(getNativeOrThrow());
}

int ImageSubheader::insertImageComment(std::string comment, int index)
{
    const int actualIndex = nitf_ImageSubheader_insertImageComment(getNativeOrThrow(),
                  comment.c_str(), index, &error);
    if (actualIndex < 0)
        throw nitf::NITFException(&error);
    return actualIndex;
}


void ImageSubheader::removeImageComment(int index)
{
    const NITF_BOOL x = nitf_ImageSubheader_removeImageComment(getNativeOrThrow(),
                  index, &error);
    if (!x)
        throw nitf::NITFException(&error);
}


nitf::Field ImageSubheader::getFilePartType() const
{
    return nitf::Field(getNativeOrThrow()->filePartType);
}

nitf::Field ImageSubheader::getImageId() const
{
    return nitf::Field(getNativeOrThrow()->imageId);
}

nitf::Field ImageSubheader::getImageDateAndTime() const
{
    return nitf::Field(getNativeOrThrow()->imageDateAndTime);
}

nitf::Field ImageSubheader::getTargetId() const
{
    return nitf::Field(getNativeOrThrow()->targetId);
}

nitf::Field ImageSubheader::getImageTitle() const
{
    return nitf::Field(getNativeOrThrow()->imageTitle);
}

nitf::Field ImageSubheader::getImageSecurityClass() const
{
    return nitf::Field(getNativeOrThrow()->imageSecurityClass);
}

nitf::FileSecurity ImageSubheader::getSecurityGroup() const
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

nitf::Field ImageSubheader::getEncrypted() const
{
    return nitf::Field(getNativeOrThrow()->encrypted);
}

nitf::Field ImageSubheader::getImageSource() const
{
    return nitf::Field(getNativeOrThrow()->imageSource);
}

nitf::Field ImageSubheader::getNumRows() const
{
    return nitf::Field(getNativeOrThrow()->numRows);
}

nitf::Field ImageSubheader::getNumCols() const
{
    return nitf::Field(getNativeOrThrow()->numCols);
}

nitf::Field ImageSubheader::getPixelValueType() const
{ 
    return nitf::Field(getNativeOrThrow()->pixelValueType);
}

nitf::Field ImageSubheader::getImageRepresentation() const
{
    return nitf::Field(getNativeOrThrow()->imageRepresentation);
}

nitf::Field ImageSubheader::getImageCategory() const
{
    return nitf::Field(getNativeOrThrow()->imageCategory);
}

nitf::Field ImageSubheader::getActualBitsPerPixel() const
{
    return nitf::Field(getNativeOrThrow()->actualBitsPerPixel);
}

nitf::Field ImageSubheader::getPixelJustification() const
{
    return nitf::Field(getNativeOrThrow()->pixelJustification);
}

nitf::Field ImageSubheader::getImageCoordinateSystem() const
{
    return nitf::Field(getNativeOrThrow()->imageCoordinateSystem);
}

nitf::Field ImageSubheader::getCornerCoordinates() const
{
    return nitf::Field(getNativeOrThrow()->cornerCoordinates);
}

nitf::Field ImageSubheader::getNumImageComments() const
{
    return nitf::Field(getNativeOrThrow()->numImageComments);
}

nitf::List ImageSubheader::getImageComments() const
{ 
    return nitf::List(getNativeOrThrow()->imageComments);
}

nitf::Field ImageSubheader::getImageCompression() const
{
    return nitf::Field(getNativeOrThrow()->imageCompression);
}

nitf::Field ImageSubheader::getCompressionRate() const
{
    return nitf::Field(getNativeOrThrow()->compressionRate);
}

nitf::Field ImageSubheader::getNumImageBands() const 
{
    return nitf::Field(getNativeOrThrow()->numImageBands);
}

nitf::Field ImageSubheader::getNumMultispectralImageBands() const
{
    return nitf::Field(getNativeOrThrow()->numMultispectralImageBands);
}

nitf::BandInfo ImageSubheader::getBandInfo(uint32_t band) const
{
    return nitf::BandInfo(nitf_ImageSubheader_getBandInfo(
        getNativeOrThrow(), band, &error));
}

nitf::Field ImageSubheader::getImageSyncCode() const
{
    return nitf::Field(getNativeOrThrow()->imageSyncCode);
}

nitf::Field ImageSubheader::getImageMode() const
{
    return nitf::Field(getNativeOrThrow()->imageMode);
}

nitf::Field ImageSubheader::getNumBlocksPerRow() const
{ 
    return nitf::Field(getNativeOrThrow()->numBlocksPerRow);
}

nitf::Field ImageSubheader::getNumBlocksPerCol() const
{
    return nitf::Field(getNativeOrThrow()->numBlocksPerCol);
}

nitf::Field ImageSubheader::getNumPixelsPerHorizBlock() const
{
    return nitf::Field(getNativeOrThrow()->numPixelsPerHorizBlock);
}

nitf::Field ImageSubheader::getNumPixelsPerVertBlock() const
{
    return nitf::Field(getNativeOrThrow()->numPixelsPerVertBlock);
}

nitf::Field ImageSubheader::getNumBitsPerPixel() const
{ 
    return nitf::Field(getNativeOrThrow()->numBitsPerPixel);
}

nitf::Field ImageSubheader::getImageDisplayLevel() const
{
    return nitf::Field(getNativeOrThrow()->imageDisplayLevel);
}

nitf::Field ImageSubheader::getImageAttachmentLevel() const
{
    return nitf::Field(getNativeOrThrow()->imageAttachmentLevel);
}

nitf::Field ImageSubheader::getImageLocation() const
{
    return nitf::Field(getNativeOrThrow()->imageLocation);
}

nitf::Field ImageSubheader::getImageMagnification() const
{
    return nitf::Field(getNativeOrThrow()->imageMagnification);
}

nitf::Field ImageSubheader::getUserDefinedImageDataLength() const
{
    return nitf::Field(getNativeOrThrow()->userDefinedImageDataLength);
}

nitf::Field ImageSubheader::getUserDefinedOverflow() const
{
    return nitf::Field(getNativeOrThrow()->userDefinedOverflow);
}

nitf::Field ImageSubheader::getExtendedHeaderLength() const
{
    return nitf::Field(getNativeOrThrow()->extendedHeaderLength);
}

nitf::Field ImageSubheader::getExtendedHeaderOverflow() const
{
    return nitf::Field(getNativeOrThrow()->extendedHeaderOverflow);
}

nitf::Extensions ImageSubheader::getUserDefinedSection() const
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

nitf::Extensions ImageSubheader::getExtendedSection() const
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

size_t ImageSubheader::getActualImageDim(size_t dim, size_t numDimsPerBlock) noexcept
{
    if (numDimsPerBlock == 0)
    {
        return dim;
    }
    else
    {
        // If we're blocking then we'll always write full blocks due to how
        // the NITF file layout works
        const size_t numBlocks =
                (dim / numDimsPerBlock) + (dim % numDimsPerBlock != 0);
        return numBlocks * numDimsPerBlock;
    }
}

size_t ImageSubheader::numBytesPerPixel() const
{
    const size_t numBitsPerPixel =
            nitf::Field(getNativeOrThrow()->numBitsPerPixel);
    return gsl::narrow<size_t>(NITF_NBPP_TO_BYTES(numBitsPerPixel));
}

size_t ImageSubheader::getNumBytesOfImageData() const
{
    const size_t numBytes = actualNumRows() * actualNumCols() *
            numImageBands() * numBytesPerPixel();
    return numBytes;
}
}

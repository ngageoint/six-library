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

#ifndef __NITF_IMAGESUBHEADER_HPP__
#define __NITF_IMAGESUBHEADER_HPP__
#pragma once

#include <string>

#include <import/sys.h>

#include "nitf/ImageSubheader.h"
#include "nitf/ImageIO.h"

#include "BandInfo.hpp"
#include "List.hpp"
#include "FileSecurity.hpp"
#include "Extensions.hpp"
#include "System.hpp"
#include "Enum.hpp"

/*!
 *  \file ImageSubheader.hpp
 *  \brief  Contains wrapper implementation for ImageSubheader
 */

namespace nitf
{
    enum class PixelType
    {
        Integer = NITF_IMAGE_IO_PIXEL_TYPE_INT,
        BiValued = NITF_IMAGE_IO_PIXEL_TYPE_B,
        Signed = NITF_IMAGE_IO_PIXEL_TYPE_SI,
        Floating = NITF_IMAGE_IO_PIXEL_TYPE_R,
        Complex = NITF_IMAGE_IO_PIXEL_TYPE_C,
        Pseudo12 = NITF_IMAGE_IO_PIXEL_TYPE_12
    };
    NITF_ENUM_define_string_to_enum_begin(PixelType)
    { "INT", PixelType::Integer }, { "B", PixelType::BiValued }, { "SI", PixelType::Signed },
    { "R", PixelType::Floating }, { "C", PixelType::Complex },      
    { "12", PixelType::Pseudo12 }  // ImageIO.c doesn't look at pixelType in this case, rather (nBits == 12) && (nBitsActual == 12)
    NITF_ENUM_define_string_to_end

    // see ComplexityLevel.c
    //NITF_ENUM(4, ImageRepresentation, MONO, RGB, RGB_LUT, MULTI);
    enum class ImageRepresentation { MONO, RGB, RGB_LUT, MULTI };
    NITF_ENUM_define_string_to_enum_begin(ImageRepresentation) // need to do this manually because of "RGB/LUT"
    { "MONO", ImageRepresentation::MONO }, { "RGB", ImageRepresentation::RGB }, { "RGB/LUT", ImageRepresentation::RGB_LUT }, { "MULTI", ImageRepresentation::MULTI }
    NITF_ENUM_define_string_to_end

    // see nitf_ImageIO_setup_SBR() in ImageIO.c
    //NITF_ENUM(4, BlockingMode, B /*band interleaved by block*/, P /*band interleaved by pixel*/, R /*band interleaved by row*/, S /*band sequential*/);
    enum class BlockingMode { Block, Pixel, Row, Sequential };
    NITF_ENUM_define_string_to_enum_begin(BlockingMode)
    { "B", BlockingMode::Block }, { "P", BlockingMode::Pixel }, { "R", BlockingMode::Row }, { "S", BlockingMode::Sequential }
    NITF_ENUM_define_string_to_end

/*!
 *  \class ImageSubheader
 *  \brief  The C++ wrapper for the nitf_ImageSubheader
 */
DECLARE_CLASS(ImageSubheader)
{
public:

    //! Copy constructor
    ImageSubheader(const ImageSubheader & x);

    //! Assignment Operator
    ImageSubheader & operator=(const ImageSubheader & x);

    //! Set native object
    ImageSubheader(nitf_ImageSubheader * x);

    //! Constructor
    ImageSubheader() noexcept(false);


    //! Clone
    nitf::ImageSubheader clone() const;

    ~ImageSubheader() = default;

    /*!
     *  Set the pixel type and band related information
     *  \param pvtype Pixel value type
     *  \param nbpp Number of bits/pixel
     *  \param abpp Actual number of bits/pixel
     *  \param justification  Pixel justification
     *  \param irep  Image representation
     *  \param icat  Image category
     *  \param bandCount  Number of bands
     *  \param bands  Band information object list
     */
    void setPixelInformation(std::string pvtype,
                             uint32_t nbpp,
                             uint32_t abpp,
                             std::string justification,
                             std::string irep, std::string icat,
                             std::vector<nitf::BandInfo>& bands);
    void setPixelInformation(PixelType pvtype,
                             uint32_t nbpp,
                             uint32_t abpp,
                             std::string justification,
                             ImageRepresentation irep, std::string icat,
                             std::vector<nitf::BandInfo>& bands);

    /*!
     *  This function allows the user to set the corner coordinates from a
     *  set of decimal values.  This function only supports CornersTypes of
     *  NITF_GEO or NITF_DECIMAL.  Others will trigger an error with code
     *  NITF_ERR_INVALID_PARAMETER
     *
     *  In order to set up the type, you should declare a double corners[4][2];
     *  The first dimension is used for the corner itself, and the second
     *  dimension is for lat (0) or lon (1).
     *
     *  The corners MUST be oriented to correspond to
     *
     *  corners[0] = (0, 0),
     *  corners[1] = (0, MaxCol),
     *  corners[2] = (MaxRow, MaxCol)
     *  corners[3] = (MaxRow, 0)
     *
     *  following in line with 2500C.
     */
private:
    void setCornersFromLatLons_(nitf::CornersType type,
        const double (*corners)[2]);
public:
    template<typename T>
    void setCornersFromLatLons(nitf::CornersType type,
                               const T& corners)
    {
        setCornersFromLatLons_(type, corners);
    }

    /*!
     *  This function allows the user to extract corner coordinates as a
     *  set of decimal values.  This function only supports CornersTypes of
     *  NITF_GEO or NITF_DECIMAL.  Others will trigger an error with code
     *  NITF_ERR_INVALID_PARAMETER
     *
     *  The output corners will be oriented to correspond to
     *
     *  corners[0] = (0, 0),
     *  corners[1] = (0, MaxCol),
     *  corners[2] = (MaxRow, MaxCol)
     *  corners[3] = (MaxRow, 0)
     *
     *  following in line with 2500C.
     */
private:
    void getCornersAsLatLons_(double (*corners)[2]) const;
public:
    template<typename T>
    void getCornersAsLatLons(T& corners) const
    {
        getCornersAsLatLons_(corners);
    }

    /*!
     *  Get the type of corners.  This will return NITF_CORNERS_UNKNOWN
     *  in the event that it is not 'U', 'N', 'S', 'D', or 'G'.
     *
     */
    nitf::CornersType getCornersType() const;

    /*!
     * Set the image dimensions and blocking info.
     * The user specifies the number of rows and columns in the image, number
     * of rows and columns per block, and blocking mode. The number of blocks
     * per row and column is calculated. The NITF 2500C large block option can
     * be selected for either dimension by setting the corresponding block
     * dimension to 0.
     *
     * \param numRows           The number of rows
     * \param numCols           The number of columns
     * \param numRowsPerBlock   The number of rows/block
     * \param numColsPerBlock   The number of columns/block
     * \param imode             Image mode
     */
    void setBlocking(uint32_t numRows,
                     uint32_t numCols,
                     uint32_t numRowsPerBlock,
                     uint32_t numColsPerBlock,
                     const std::string& imode);
    void setBlocking(uint32_t numRows,
                     uint32_t numCols,
                     uint32_t numRowsPerBlock,
                     uint32_t numColsPerBlock,
                     BlockingMode imode);

    /*!
     * Compute blocking parameters
     * The user specifies the number of rows and columns in the image and the
     * number of rows and columns per block. The number of blocks per row and
     * column is calculated. The NITF 2500C large block option can be selected
     * for either dimension by setting the corresponding block dimension to 0.
     * If numRowsPerBlock or numColsPerBlock is over the NITF maximum, it is
     * reduced to 0 per 2500C.
     *
     * The number of blocks per column is a backwards way of saying the number
     * of rows of blocks. So the numBlocksPerCol calculation involves row counts
     * and numBlocksPerRow calculation involves column counts.
     *
     * \param numRows           The number of rows
     * \param numCols           The number of columns
     * \param numRowsPerBlock   The number of rows/block
     * \param numColsPerBlock   The number of columns/block
     * \param numBlocksPerCol   The number of rows of blocks
     * \param numBlocksPerRow   The number of columns of blocks
     */
    static
    void computeBlocking(uint32_t numRows,
                         uint32_t numCols,
                         uint32_t& numRowsPerBlock,
                         uint32_t& numColsPerBlock,
                         uint32_t& numBlocksPerCol,
                         uint32_t& numBlocksPerRow) noexcept;

    /*!
     * Set the image dimensions and blocking.
     *
     * The blocking is set to the simplest possible blocking given the
     * dimension, no blocking if possible. Blocking mode is set to B. Whether
     * or not blocking masks are generated is dependent on the compression
     * code at the time the image is written. NITF 2500C large block
     * dimensions are not set by this function. The more general set blocking
     * function must be used to specify this blocking option.
     *
     * \param numRows           The number of rows
     * \param numCols           The number of columns
     */
    void setDimensions(uint32_t numRows, uint32_t numCols);

    //! Get the number of bands
    uint32_t getBandCount() const;

    //! Create new bands
    void createBands(uint32_t numBands);

    //! Insert the given comment at the given index (zero-indexed);
    int insertImageComment(std::string comment, int index);

    //! Remove the given comment at the given index (zero-indexed);
    void removeImageComment(int index);

    //! Get the filePartType
    nitf::Field getFilePartType() const;

    //! Get the imageId
    nitf::Field getImageId() const;
    std::string imageId() const
    {
        return getImageId().toTrimString();
    }

    //! Get the imageDateAndTime
    nitf::Field getImageDateAndTime() const;

    //! Get the targetId
    nitf::Field getTargetId() const;

    //! Get the imageTitle
    nitf::Field getImageTitle() const;

    //! Get the imageSecurityClass
    nitf::Field getImageSecurityClass();
    const nitf::Field getImageSecurityClass() const;
    std::string imageSecurityClass() const
    {
        return getImageSecurityClass(); // nitf::Field implicitly converts to std::string
    }

    //! Get the securityGroup
    nitf::FileSecurity getSecurityGroup() const;

    //! Set the securityGroup
    void setSecurityGroup(nitf::FileSecurity value);

    //! Get the encrypted
    nitf::Field getEncrypted() const;

    //! Get the imageSource
    nitf::Field getImageSource() const;

    //! Get the numRows
    nitf::Field getNumRows() const;
    size_t numRows() const
    {
        return getNumRows();
    }

    //! Get the numCols
    nitf::Field getNumCols() const;
    size_t numCols() const
    {
        return getNumCols();
    }

    types::RowCol<size_t> dims() const
    {
        return types::RowCol<size_t>(numRows(), numCols());
    }

    //! Get the pixelValueType
    nitf::Field getPixelValueType() const;

    //! Get the imageRepresentation
    nitf::Field getImageRepresentation() const;
    std::string imageRepresentation() const
    {
        return getImageRepresentation().toTrimString();
    }

    //! Get the imageCategory
    nitf::Field getImageCategory() const;
    std::string imageCategory() const
    {
        return getImageCategory().toTrimString();
    }

    //! Get the actualBitsPerPixel
    nitf::Field getActualBitsPerPixel() const;
    size_t actualBitsPerPixel() const
    {
        return getActualBitsPerPixel();
    }

    //! Get the pixelJustification
    nitf::Field getPixelJustification() const;
    std::string pixelJustification() const
    {
        return getPixelJustification(); // nitf::Field implicitly converts to std::string
    }

    //! Get the imageCoordinateSystem
    nitf::Field getImageCoordinateSystem() const;

    //! Get the cornerCoordinates
    nitf::Field getCornerCoordinates() const;

    //! Get the numImageComments
    nitf::Field getNumImageComments() const;

    //! Get the imageComments
    nitf::List getImageComments() const;

    //! Get the imageCompression
    nitf::Field getImageCompression() const;
    std::string imageCompression() const
    {
        return getImageCompression().toTrimString();
    }

    //! Get the compressionRate
    nitf::Field getCompressionRate() const;

    //! Get the numImageBands
    nitf::Field getNumImageBands() const;
    size_t numImageBands() const
    {
        return getNumImageBands();
    }

    //! Get the numMultispectralImageBands
    nitf::Field getNumMultispectralImageBands() const;

    //! Get the bandInfo
    nitf::BandInfo getBandInfo(uint32_t band) const;

    //! Get the imageSyncCode
    nitf::Field getImageSyncCode() const;

    //! Get the imageMode
    nitf::Field getImageMode() const;
    std::string imageMode() const
    {
        return getImageMode(); // nitf::Field implicitly converts to std::string
    }

    //! Get the numBlocksPerRow
    nitf::Field getNumBlocksPerRow() const;
    size_t numBlocksPerRow() const
    {
        return getNumBlocksPerRow();
    }

    //! Get the numBlocksPerCol
    nitf::Field getNumBlocksPerCol() const;
    size_t numBlocksPerCol() const
    {
        return getNumBlocksPerCol();
    }

    //! Get the numPixelsPerHorizBlock
    nitf::Field getNumPixelsPerHorizBlock() const;
    size_t numPixelsPerHorizBlock() const
    {
        return getNumPixelsPerHorizBlock();
    }

    //! Get the numPixelsPerVertBlock
    nitf::Field getNumPixelsPerVertBlock() const;
    size_t numPixelsPerVertBlock() const
    {
        return getNumPixelsPerVertBlock();
    }

    //! Get the numBitsPerPixel
    nitf::Field getNumBitsPerPixel() const;
    size_t numBitsPerPixel() const
    {
        return getNumBitsPerPixel();
    }

    //! Get the imageDisplayLevel
    nitf::Field getImageDisplayLevel() const;

    //! Get the imageAttachmentLevel
    nitf::Field getImageAttachmentLevel() const;

    //! Get the imageLocation
    nitf::Field getImageLocation() const;
    std::string imageLocation() const
    {
        return getImageLocation(); // nitf::Field implicitly converts to std::string
    }

    //! Get the imageMagnification
    nitf::Field getImageMagnification() const;

    //! Get the userDefinedImageDataLength
    nitf::Field getUserDefinedImageDataLength() const;

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

    /*!
     * \param dim Number of elements (i.e. rows or columns)
     * \param numDimsPerBlock Number of elements per block.  0 indicates no
     * blocking.
     *
     * \return The actual number of elements, including padding (i.e. will be
     * an even multiple of numDimsPerBlock)
     */
    static
    size_t getActualImageDim(size_t dim, size_t numDimsPerBlock) noexcept;

    /*!
     * \return The number of bytes the image data associated with the image
     * subheader takes up (takes dimensions, bytes/pixel, and blocking into
     * account)
     */
    size_t getNumBytesOfImageData() const;

private:
    size_t actualNumRows() const
    {
        return getActualImageDim(getNumRows(), getNumPixelsPerVertBlock());
    }

    size_t actualNumCols() const
    {
        return getActualImageDim(getNumCols(), getNumPixelsPerHorizBlock());
    }

    size_t numBytesPerPixel() const;

    mutable nitf_Error error{};
};

}
#endif

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

#ifndef __NITF_IMAGESUBHEADER_HPP__
#define __NITF_IMAGESUBHEADER_HPP__

#include "nitf/ImageSubheader.h"
#include "nitf/Object.hpp"
#include "nitf/NITFException.hpp"
#include "nitf/BandInfo.hpp"
#include "nitf/List.hpp"
#include "nitf/FileSecurity.hpp"
#include "nitf/Extensions.hpp"
#include "nitf/System.hpp"
#include <string>

/*!
 *  \file ImageSubheader.hpp
 *  \brief  Contains wrapper implementation for ImageSubheader
 */

namespace nitf
{

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
    ImageSubheader() throw(nitf::NITFException);


    //! Clone
    nitf::ImageSubheader clone() throw(nitf::NITFException);

    /*!
     *  Destructor
     */
    ~ImageSubheader();

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
                             nitf::Uint32 nbpp,
                             nitf::Uint32 abpp,
                             std::string justification,
                             std::string irep, std::string icat,
                             std::vector<nitf::BandInfo>& bands)
        throw(nitf::NITFException);


    /*!
     *  @deprecated - here for backwards compatibility
     *  bandCount WILL get ignored
     */
    void setPixelInformation(std::string pvtype,
                             nitf::Uint32 nbpp,
                             nitf::Uint32 abpp,
                             std::string justification,
                             std::string irep, std::string icat,
                             nitf::Uint32 bandCount,
                             std::vector<nitf::BandInfo>& bands)
        throw(nitf::NITFException);


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
    void setCornersFromLatLons(nitf::CornersType type,
                               double corners[4][2])
        throw(nitf::NITFException);


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
    void getCornersAsLatLons(double corners[4][2]) throw(nitf::NITFException);

    /*!
     *  Get the type of corners.  This will return NITF_CORNERS_UNKNOWN
     *  in the event that it is not 'U', 'N', 'S', 'D', or 'G'.
     *
     */
    nitf::CornersType getCornersType() throw(nitf::NITFException);

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
    void setBlocking(nitf::Uint32 numRows,
                     nitf::Uint32 numCols,
                     nitf::Uint32 numRowsPerBlock,
                     nitf::Uint32 numColsPerBlock,
                     const std::string& imode) throw(nitf::NITFException);

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
    void setDimensions(nitf::Uint32 numRows, nitf::Uint32 numCols)
        throw(nitf::NITFException);


    //! Get the number of bands
    nitf::Uint32 getBandCount() throw(nitf::NITFException);

    //! Create new bands
    void createBands(nitf::Uint32 numBands) throw(nitf::NITFException);

    //! Insert the given comment at the given index (zero-indexed);
    int insertImageComment(std::string comment, int index);

    //! Remove the given comment at the given index (zero-indexed);
    void removeImageComment(int index);

    //! Get the filePartType
    nitf::Field getFilePartType();

    //! Get the imageId
    nitf::Field getImageId();

    //! Get the imageDateAndTime
    nitf::Field getImageDateAndTime();

    //! Get the targetId
    nitf::Field getTargetId();

    //! Get the imageTitle
    nitf::Field getImageTitle();

    //! Get the imageSecurityClass
    nitf::Field getImageSecurityClass();

    //! Get the securityGroup
    nitf::FileSecurity getSecurityGroup();

    //! Set the securityGroup
    void setSecurityGroup(nitf::FileSecurity value);

    //! Get the encrypted
    nitf::Field getEncrypted();

    //! Get the imageSource
    nitf::Field getImageSource();

    //! Get the numRows
    nitf::Field getNumRows();

    //! Get the numCols
    nitf::Field getNumCols();

    //! Get the pixelValueType
    nitf::Field getPixelValueType();

    //! Get the imageRepresentation
    nitf::Field getImageRepresentation();

    //! Get the imageCategory
    nitf::Field getImageCategory();

    //! Get the actualBitsPerPixel
    nitf::Field getActualBitsPerPixel();

    //! Get the pixelJustification
    nitf::Field getPixelJustification();

    //! Get the imageCoordinateSystem
    nitf::Field getImageCoordinateSystem();

    //! Get the cornerCoordinates
    nitf::Field getCornerCoordinates();

    //! Get the numImageComments
    nitf::Field getNumImageComments();

    //! Get the imageComments
    nitf::List getImageComments();

    //! Get the imageCompression
    nitf::Field getImageCompression();

    //! Get the compressionRate
    nitf::Field getCompressionRate();

    //! Get the numImageBands
    nitf::Field getNumImageBands();

    //! Get the numMultispectralImageBands
    nitf::Field getNumMultispectralImageBands();

    //! Get the bandInfo
    nitf::BandInfo getBandInfo(nitf::Uint32 band) throw(nitf::NITFException);

    //! Get the imageSyncCode
    nitf::Field getImageSyncCode();

    //! Get the imageMode
    nitf::Field getImageMode();

    //! Get the numBlocksPerRow
    nitf::Field getNumBlocksPerRow();

    //! Get the numBlocksPerCol
    nitf::Field getNumBlocksPerCol();

    //! Get the numPixelsPerHorizBlock
    nitf::Field getNumPixelsPerHorizBlock();

    //! Get the numPixelsPerVertBlock
    nitf::Field getNumPixelsPerVertBlock();

    //! Get the numBitsPerPixel
    nitf::Field getNumBitsPerPixel();

    //! Get the imageDisplayLevel
    nitf::Field getImageDisplayLevel();

    //! Get the imageAttachmentLevel
    nitf::Field getImageAttachmentLevel();

    //! Get the imageLocation
    nitf::Field getImageLocation();

    //! Get the imageMagnification
    nitf::Field getImageMagnification();

    //! Get the userDefinedImageDataLength
    nitf::Field getUserDefinedImageDataLength();

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

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

#ifndef __NITF_IMAGE_SUBHEADER_H__
#define __NITF_IMAGE_SUBHEADER_H__

/*  We get file security and some constants already  */
/*  defined by including this first                  */
#include "nitf/FileHeader.h"
#include "nitf/BandInfo.h"
#include "nitf/PluginRegistry.h"

#define NITF_IM_SZ     2
#define NITF_IID1_SZ   10
#define NITF_IDATIM_SZ 14
#define NITF_TGTID_SZ  17
#define NITF_IID2_SZ   80
#define NITF_ISCLAS_SZ 1
#define NITF_ISORCE_SZ 42
#define NITF_NROWS_SZ  8
#define NITF_NCOLS_SZ  8
#define NITF_PVTYPE_SZ 3
#define NITF_IREP_SZ   8
#define NITF_ICAT_SZ   8
#define NITF_ABPP_SZ   2
#define NITF_PJUST_SZ  1
#define NITF_ICORDS_SZ 1
#define NITF_IGEOLO_SZ 60
#define NITF_NICOM_SZ  1
#define NITF_ICOM_SZ   80
#define NITF_IC_SZ     2
#define NITF_COMRAT_SZ 4
#define NITF_NBANDS_SZ 1
#define NITF_XBANDS_SZ 5

/*  After band info */
#define NITF_ISYNC_SZ 1
#define NITF_IMODE_SZ 1
#define NITF_NBPR_SZ  4
#define NITF_NBPC_SZ  4
#define NITF_NPPBH_SZ 4
#define NITF_NPPBV_SZ 4
#define NITF_NBPP_SZ  2
#define NITF_IDLVL_SZ 3
#define NITF_IALVL_SZ 3
#define NITF_ILOC_SZ  10
#define NITF_IMAG_SZ  4

#define NITF_UDIDL_SZ   5
#define NITF_UDOFL_SZ   3
#define NITF_IXSHDL_SZ  5
#define NITF_IXSOFL_SZ  3


#define NITF_IM        filePartType
#define NITF_IID1      imageId
#define NITF_IDATIM    imageDateAndTime
#define NITF_TGTID     targetId
#define NITF_IID2      imageTitle
#define NITF_ISCLAS    imageSecurityClass
#define NITF_ISORCE    imageSource
#define NITF_NROWS     numRows
#define NITF_NCOLS     numCols
#define NITF_PVTYPE    pixelValueType
#define NITF_IREP      imageRepresentation
#define NITF_ICAT      imageCategory
#define NITF_ABPP      actualBitsPerPixel
#define NITF_PJUST     pixelJustification
#define NITF_ICORDS    imageCoordinateSystem
#define NITF_IGEOLO    cornerCoordinates
#define NITF_NICOM     numImageComments

/*  This is the really dreadful stuff  */
#define NITF_ICOM      imageComments
#define NITF_IC        imageCompression
#define NITF_COMRAT    compressionRate
#define NITF_NBANDS    numImageBands
#define NITF_XBANDS    numMultispectralImageBands

/*  This stuff is okay again  */
#define NITF_ISYNC     imageSyncCode
#define NITF_IMODE     imageMode
#define NITF_NBPR      numBlocksPerRow
#define NITF_NBPC      numBlocksPerCol
#define NITF_NPPBH     numPixelsPerHorizBlock
#define NITF_NPPBV     numPixelsPerVertBlock
#define NITF_NBPP      numBitsPerPixel
#define NITF_IDLVL     imageDisplayLevel
#define NITF_IALVL     imageAttachmentLevel
#define NITF_ILOC      imageLocation
#define NITF_IMAG      imageMagnification

#define NITF_UDIDL     userDefinedImageDataLength
#define NITF_UDOFL     userDefinedOverflow
#define NITF_IXSHDL    extendedHeaderLength
#define NITF_IXSOFL    extendedHeaderOverflow

/*!< \def NITF_MAX_BAND_COUNT - Maximum band count */
#define NITF_MAX_BAND_COUNT 99999
#define NITF_INVALID_BAND_COUNT ((nitf_Uint32) -1)


NITF_CXX_GUARD

/*!
 *  \brief The ImageSubheader represents all of the fields in the image header.
 */
typedef struct _nitf_ImageSubheader
{
    nitf_Field *filePartType;
    nitf_Field *imageId;
    nitf_Field *imageDateAndTime;
    nitf_Field *targetId;
    nitf_Field *imageTitle;
    nitf_Field *imageSecurityClass;

    nitf_FileSecurity *securityGroup;
    /*  This is already identified from the file header  */
    nitf_Field *encrypted;
    nitf_Field *imageSource;

    nitf_Field *numRows;
    nitf_Field *numCols;

    nitf_Field *pixelValueType;
    nitf_Field *imageRepresentation;
    nitf_Field *imageCategory;

    nitf_Field *actualBitsPerPixel;

    nitf_Field *pixelJustification;
    nitf_Field *imageCoordinateSystem;
    nitf_Field *cornerCoordinates;

    nitf_Field *numImageComments;
    /*  There are numImageComments of these  */
    nitf_List* imageComments; /* list of nitf_Fields */

    nitf_Field *imageCompression;
    nitf_Field *compressionRate;

    nitf_Field *numImageBands;
    nitf_Field *numMultispectralImageBands;

    /*  This contains image band info stuff, including LUT  */
    nitf_BandInfo **bandInfo;

    nitf_Field *imageSyncCode;
    nitf_Field *imageMode;
    nitf_Field *numBlocksPerRow;
    nitf_Field *numBlocksPerCol;
    nitf_Field *numPixelsPerHorizBlock;
    nitf_Field *numPixelsPerVertBlock;
    nitf_Field *numBitsPerPixel;

    nitf_Field *imageDisplayLevel;
    nitf_Field *imageAttachmentLevel;
    nitf_Field *imageLocation;
    nitf_Field *imageMagnification;


    nitf_Field *userDefinedImageDataLength;
    nitf_Field *userDefinedOverflow;

    nitf_Field *extendedHeaderLength;
    nitf_Field *extendedHeaderOverflow;


    /*  This section (unfortunately), also has a userDefinedSection  */
    nitf_Extensions *userDefinedSection;

    /*  This section (also unfortunately), has an extededSection as well  */
    nitf_Extensions *extendedSection;

}
nitf_ImageSubheader;


/*!
 *  Construct a new image subheader object.  This object will
 *  be null set for every field.  The object returned will be
 *  a full-fledged subheader, unless there is an error, in which
 *  case NULL will be returned, and the error object that was
 *  passed in will be populated.
 *
 *  \param error In case of error, when this function returns, this
 *  parameter will be populated
 *
 *  \return The image subheader, or NULL on failure.
 */
NITFAPI(nitf_ImageSubheader *)nitf_ImageSubheader_construct(nitf_Error * error);



NITFAPI(nitf_ImageSubheader *)
nitf_ImageSubheader_clone(nitf_ImageSubheader * source,
                          nitf_Error * error);

/*!
 *  Destroy and image subheader object, and NULL-set the subheader.
 *  A double pointer is make the NULL-set persistant on function
 *  exit.
 *
 *  \param The address of the the image subheader pointer
 */
NITFAPI(void) nitf_ImageSubheader_destruct(nitf_ImageSubheader ** subhdr);

/*!
  \brief  nitf_ImageSubheader_setPixelInformation - Set the pixel type and band
  related information

  nitf_ImageSubheader_setPixelInformation sets the pixel type and band related
  information.

  The following fields are set:

      PVTYPE
      NBPP
      ABPP
      PJUST
      NBANDS
      XBANDS
      IREP
      ICAT
      bandInfo   (array of band info objects)

  The subheader bandInfo field is set using the caller supplied array. This
  reference is retained and destroyed by the subheader object' destructor.
  The user should not destroy the objects in the list and they should be in
  persistent storage.

  Nothing is set until everything is validated. so the returned subheader is
  returned unchange on error, except if the error is associated with setting
  the associated Value objects.

  \return True is returned on success. On error, the error object is set.
*/

NITFAPI(NITF_BOOL) nitf_ImageSubheader_setPixelInformation
(
    nitf_ImageSubheader *subhdr,      /*!< Subheader object to set */
    const char *pvtype,               /*!< Pixel value type */
    nitf_Uint32 nbpp,                 /*!< Number of bits/pixel */
    nitf_Uint32 abpp,                 /*!< Actual number of bits/pixel */
    const char *justification,        /*!< Pixel justification */
    const char *irep,                 /*!< Image representation */
    const char *icat,                 /*!< Image category */
    nitf_Uint32 bandCount,            /*!< Number of bands */
    nitf_BandInfo **bands,            /*!< Band information object list */
    nitf_Error *error                 /*!< For error returns */
);

/*!
  \brief  nitf_ImageSubheader_getBandCount - Get number of bands

  nitf_ImageSubheader_getBandCount returns the number of bands in
  The image associated with the subheader. The band count can come from
  one of two fields, NBANDS or XBANDS. There can be errors decoding the
  value fields and inconsistent values in NBANDS, XBANDS and IREP.

  If there is a problem, NITF_INVALID_BAND_COUNT is returned and the error
  object is set

  \return The number of bands or an error indicator

*/
NITFAPI(nitf_Uint32) nitf_ImageSubheader_getBandCount
(
    nitf_ImageSubheader* subhdr,     /*!< The associated subheader */
    nitf_Error *error                /*!< For error returns */
);

/*!
  \brief  nitf_ImageSubheader_getBandInfo - Return the band information for
  an band

  nitf_ImageSubheader_getBandInfoCount returns band information for a band
  specified by band number. This function range checks the band index and
  returns an error if it is out of range. An error is returned if there are
  no bands set.

  The function returns a pointer to the objects bandInfo field. It is
  not a copy.

  \return The band information array of NULL on error

*/
NITFAPI(nitf_BandInfo *) nitf_ImageSubheader_getBandInfo
(
    nitf_ImageSubheader* subhdr,    /*!< The associated subheader */
    nitf_Uint32 band,               /*!< The band index */
    nitf_Error *error               /*!< For error returns */
);


/*!
  \brief  nitf_ImageSubheader_createBands - updates NBANDS in the ImageSubheader
  and allocates and constructs the Image Subheader's bandInfo array

  nitf_ImageSubheader_createBands creates the specified number of bandInfo
  objects, and updates the bandInfo array. This also updates the NBANDS/XBANDS
  fields in the ImageSubheader. This function assumes that the current size of
  the bandInfo array is EQUAL to the current number of toatl bands (NBANDS +
  XBANDS). An error is returned if memory problems occur, or if the total bands
  will exceed 99999.
*/
NITFAPI(NITF_BOOL) nitf_ImageSubheader_createBands(
    nitf_ImageSubheader * subhdr, /*!< The associated subheader */
    nitf_Uint32 numBands,         /*!< The number of bands to create/add*/
    nitf_Error * error            /*!< For error returns */
);


/*!
  \brief  nitf_ImageSubheader_removeBand - removes a band from the image.

  This removes the underlying BandInfo object and decrements the band count.

  \return NITF_SUCCESS is return on success, on error the error object is set.
*/
NITFAPI(NITF_BOOL) nitf_ImageSubheader_removeBand(
    nitf_ImageSubheader * subhdr, /*!< The associated subheader */
    nitf_Uint32 index,            /*!< The index of the band to remove */
    nitf_Error * error            /*!< For error returns */
);


/*!
  \brief  nitf_ImageSubheader_getDimensions - Get image dimensions

  nitf_ImageSubheader_getDimensions returns the number of rows and columns
  of the image.

  The only errors are associated with fetching field objects

  \return NITF_SUCCESS is return on success, on error the error object is set.
*/

NITFAPI(NITF_BOOL) nitf_ImageSubheader_getDimensions
(
    nitf_ImageSubheader* subhdr,  /*!< Associated image subheader object */
    nitf_Uint32 *numRows,         /*!< Returns the number of rows */
    nitf_Uint32 *numCols,         /*!< Returns the number of columns */
    nitf_Error *error             /*!< Object for error messages */
);

/*!
  \brief  nitf_ImageSubheader_getBlocking - Get image blocking information

  nitf_ImageSubheader_getDimensions returns the blocking information
  including number of rows and columns, number of rows and columns per block,
  number of rows and columns of blocks and image mode

  The imode argument must is a char array with at least NITF_IMODE_SZ+1 bytes

  The only errors are associated with fetching field objects

  \return NITF_SUCCESS is return on success, on error the error object is set.
*/

NITFAPI(NITF_BOOL) nitf_ImageSubheader_getBlocking
(
    nitf_ImageSubheader* subhdr,  /*!< Associated image subheader object */
    nitf_Uint32 *numRows,         /*!< Returns the number of rows */
    nitf_Uint32 *numCols,         /*!< Returns the number of columns */
    nitf_Uint32 *numRowsPerBlock, /*!< Returns the number of rows/block */
    nitf_Uint32 *numColsPerBlock, /*!< Returns the number of columns/block */
    nitf_Uint32 *numBlocksPerRow, /*!< Returns the number of blocks/row */
    nitf_Uint32 *numBlocksPerCol, /*!< Returns the number of blocks/column */
    char *imode,                  /*!< Returns the image mode string */
    nitf_Error *error             /*!< Object for error messages */
);

/*!
  \brief  nitf_ImageSubheader_getCompression - Get image compression

  nitf_ImageSubheader_getCompression returns the comnpression type and
  compression rate strings.

  The imageCompression argument must is a char array with at least
  NITF_IC_SZ+1 bytes

  The compressionRate argument must is a char array with at least
  NITF_CCOMRAT_SZ+1 bytes

  The only errors are associated with fetching field objects.

  \return NITF_SUCCESS is return on success, on error the error object is set.
*/

NITFAPI(NITF_BOOL) nitf_ImageSubheader_getCompression
(
    nitf_ImageSubheader* subhdr,  /*!< Associated image subheader object */
    char *imageCompression,       /*!< Returns the compression type code */
    char *compressionRate,        /*!< Returns the compression rate code */
    nitf_Error *error             /*!< Object for error messages */
);

/*!
  \brief  nitf_ImageSubheader_setDimensions - Set image dimensions

  nitf_ImageSubheader_setDimensions sets the number of rows and columns
  of the image. The blocking is set to the simplest possible blocking given
  the dimension, no blocking if possible. Blocking mode is set to B. Whether
  or not blocking masks are generated is dependent on the compression code at
  the time the image is written. NITF 2500C large block dimensions are not
  set by this function. The more general set blocking function must be used
  to specify this blocking option.

  The only errors are associated with fetching field objects

  \return TRUE is return on success, on error the error object is set.
*/

NITFAPI(NITF_BOOL) nitf_ImageSubheader_setDimensions
(
    nitf_ImageSubheader* subhdr,  /*!< Associated image subheader object */
    nitf_Uint32 numRows,          /*!< The number of rows */
    nitf_Uint32 numCols,          /*!< The number of columns */
    nitf_Error *error             /*!< Object for error messages */
);

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
NITFAPI(NITF_BOOL)
nitf_ImageSubheader_setCornersFromLatLons(nitf_ImageSubheader* subheader,
                                          nitf_CornersType type,
                                          double corners[4][2],
                                          nitf_Error* error);

/*!
 *  Get the type of corners.  This will return NITF_CORNERS_UNKNOWN
 *  in the event that it is not 'U', 'N', 'S', 'D', or 'G'.
 *
 */
NITFAPI(nitf_CornersType)
nitf_ImageSubheader_getCornersType(nitf_ImageSubheader* subheader);

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
NITFAPI(NITF_BOOL)
nitf_ImageSubheader_getCornersAsLatLons(nitf_ImageSubheader* subheader,
                                        double corners[4][2],
                                        nitf_Error *error);

/*!
  \brief  nitf_ImageSubheader_setBlocking - Set image dimensions and blocking

  nitf_ImageSubheader_setDimensions sets the blocking information. The user
  specifies the number of rows and columns in the image, number of rows and
  columns per block, and blocking mode. The number of blocks per row and
  column is calculated. The NITF 2500C large block option can be selected for
  either dimension by setting the corresponding block dimension to 0.

  The only errors are associated with fetching field objects

  \return TRUE is return on success, on error the error object is set.
*/
NITFAPI(NITF_BOOL) nitf_ImageSubheader_setBlocking
(
    nitf_ImageSubheader* subhdr,  /*!< Associated image subheader object */
    nitf_Uint32 numRows,          /*!< The number of rows */
    nitf_Uint32 numCols,          /*!< The number of columns */
    nitf_Uint32 numRowsPerBlock,  /*!< The number of rows/block */
    nitf_Uint32 numColsPerBlock,  /*!< The number of columns/block */
    const char *imode,            /*!< Image mode */
    nitf_Error *error             /*!< Object for error messages */
);

/*!
  \brief  nitf_ImageSubheader_setCompression - Set image compression

  nitf_ImageSubheader_getCompression sets the compression type and
  compression rate

  The imageCompression argument must is a char array with at least
  NITF_IC_SZ+1 bytes. The compressionRate argument must is a char array
  with at least NITF_CCOMRAT_SZ+1 bytes. If either string is too long
  for the field, it is truncated.

  Writing of a masked image with blocking mode S is currently not supported.
  So setting a masked type such as "NM" may result in a error when the
  file is written that is not detected here.

  The only errors are associated with fetching field objects.

  \return TRUE is return on success, on error the error object is set.
*/
NITFAPI(NITF_BOOL) nitf_ImageSubheader_setCompression
(
    nitf_ImageSubheader* subhdr,  /*!< Associated image subheader object */
    const char *imageCompression, /*!< The compression type code */
    const char *compressionRate,  /*!< The compression rate code */
    nitf_Error *error             /*!< Object for error messages */
);


/*!
  \brief  nitf_ImageSubheader_insertImageComment - insert a comment to the
  array of image comments.

  This function adds the given comment string to the list of comments
  associated with this image subheader. The numImageComments field
  value gets incremented by 1. If the number of comments is already equal
  to 9, then -1 is returned, and the comment is not added.

  The function assumes that the numImageComments field value and the actual
  size of the imageComments buffer are consistent with each other.

  If the position is out of the array bounds, or less than zero, the
  comment will be appended to the end.

  If the comment is NULL, a blank comment is inserted. Otherwise, the passed
  comment must be a null-terminated char*.

  \return the index where the comment was inserted, or -1 if an error occurred.
*/
NITFAPI(int) nitf_ImageSubheader_insertImageComment
(
    nitf_ImageSubheader* subhdr,  /*!< Associated image subheader object */
    const char* comment,          /*!< Comment to add - must be null-terminated */
    int position,                 /*!< position to put the comment (zero-index)*/
    nitf_Error *error             /*!< Object for error messages */
);



/*!
  \brief  nitf_ImageSubheader_removeImageComment - add a comment to the end
  of the array of image comments.

  This function removes the comment at the given position from the list of
  comments associated with this image subheader. The numImageComments field
  value gets decremented by 1. If the number of comments is already equal
  to 0, nothing is done.

  The function assumes that the numImageComments field value and the actual
  size of the imageComments buffer are consistent with each other.

  If the position is out of the array bounds, or less than zero, then
  nothing is done.

  \return TRUE is return on success, on error the error object is set.
*/
NITFAPI(NITF_BOOL) nitf_ImageSubheader_removeImageComment
(
    nitf_ImageSubheader* subhdr,  /*!< Associated image subheader object */
    int position,                 /*!< position of comment to remove (zero-index)*/
    nitf_Error *error             /*!< Object for error messages */
);


NITF_CXX_ENDGUARD

#endif

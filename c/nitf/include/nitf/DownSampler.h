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

#ifndef __NITF_DOWNSAMPLER_H__
#define __NITF_DOWNSAMPLER_H__


#include "nitf/System.h"

/*! \def NITF_PIXEL_TYPE_INT - Integer */
#   define NITF_PIXEL_TYPE_INT ((nitf_Uint32) 0x00080000)

/*! \def NITF_PIXEL_TYPE_B - Bi-valued */
#   define NITF_PIXEL_TYPE_B ((nitf_Uint32) 0x00100000)

/*! \def NITF_PIXEL_TYPE_SI - Two's complement signed integer */
#   define NITF_PIXEL_TYPE_SI ((nitf_Uint32) 0x00200000)

/*! \def NITF_PIXEL_TYPE_R - Floating point */
#   define NITF_PIXEL_TYPE_R ((nitf_Uint32) 0x00400000)

/*! \def NITF_PIXEL_TYPE_C - Complex floating point */
#   define NITF_PIXEL_TYPE_C ((nitf_Uint32) 0x00800000)


NITF_CXX_GUARD

struct _nitf_DownSampler;

/*
 * This callback provides the interface for applying a sampling method while
 * reading. The object argument gives access to the userData field which
 * allows for customization of the available fields for this callback.
 * A transformation  between the input window and the output window occurs in
 * this  method.
 *  \param object The associated down-sample object
 *  \param inputWindows Array of input image fragments (one/band)
 *  \param outputWindow Array of sub-sampled image fragments (one/band)
 *  \param numBands Number of bands to down-sample
 *  \param numWindowRows How many rows for the input window
 *  \param numWindowCols How many cols for the input window
 *  \param numInputCols  Number of columns in input buffer, full res
 *  \param numSubWindowCols The number of columns in the sub-window
 *  \param pixelType The pixel type (valid values found in System.h)
 *  \param pixelSize The size of one pixel
 *  \param rowsInLastWindow The number of rows in the final window
 *  \param colsInLastWindow The number of cols in the final window
 *  \param error An error to populate if something bad happened
 *  \return NITF_SUCCESS on success, NITF_FAILURE on failure
 *
 * Note:
 *
 *  The numWindowRows, numWindowCols, and numSubWindowCols values are
 *  in output image units (units of sample windows). For example, with
 *  a pixel skip of 3 in columns, if the sub-window request spans columns
 *  0-299, then numSubWindowCols is 100. If the block is such that a
 *  particular request spans columns 90-149 (60 full resolution columns),
 *  then numWindowCols is 20. The numInputCols value is in full resolution
 *  units. This value gives the length, in pixels of one row in the input
 *  buffer. This buffer is used for all down-sample calls. Since the number
 *  of windows can vary from call to call, this buffer has a worst case
 *  length. Therefore, it is not possible to move from one row to the next
 *  with just the number of sample windows per row (numWindowCols) for the
 *  current request
 */
typedef NITF_BOOL(*NITF_IDOWNSAMPLER_APPLY) (struct _nitf_DownSampler *
        object,
        NITF_DATA ** inputWindows,
        NITF_DATA ** outputWindows,
        nitf_Uint32 numBands,
        nitf_Uint32 numWindowRows,
        nitf_Uint32 numWindowCols,
        nitf_Uint32 numInputCols,
        nitf_Uint32 numSubWindowCols,
        nitf_Uint32 pixelType,
        nitf_Uint32 pixelSize,
        nitf_Uint32 rowsInLastWindow,
        nitf_Uint32 colsInLastWindow,
        nitf_Error * error);


/*!
 *  Callback providing a destruct hook for derived objects.
 *  This method is called by nitf_DownSampler_destruct().
 *  \param data The user data to delete.
 */
typedef void (*NITF_IDOWNSAMPLER_DESTRUCT) (NITF_DATA * userData);

/*!
 *  \struct nitf_IDownSampler
 *  \brief The interface (no user data) for a downsampler
 *
 *  This struct is usually created statically by a derived class
 *  constructor.  It is bound to each instance of that class as
 *  the interface ('iface' field).
 */
typedef struct _nitf_IDownSampler
{
    NITF_IDOWNSAMPLER_APPLY apply;      /*  The apply method  */
    NITF_IDOWNSAMPLER_DESTRUCT destruct;        /*  A destructor hook  */
}
nitf_IDownSampler;


/*!
 *  \struct nitf_DownSampler
 *  \brief The downsampler base class
 *
 *  The downsampler base class (like the nitf_BandSource base class)
 *  has two components:  fields or members (stored in the user data
 *  parameter), and a known interface of functions.  This is, more or
 *  less, the most simplified representation of a 'class.'
 *
 *  \param face        The functional interface
 *  \param rowSkip     The number of rows in the down-sample window
 *  \param colSkip     The number of columns in the down-sample window
 *  \param multiBand   The down-sampler method is muli-band
 *  \param minBands    Minimum number of bands in multi-band method
 *  \param maxBands    Maxmum number of bands in multi-band method
 *  \param types       Mask of type/pixel size flags
 *  \param data        The derived class instance data
 *
 * The multiBand, minBands, and maxBands fields support multi-band methods. The
 * min and max fields specify required band counts. minBands is always at least
 * one. For multi-band method that do not have an upper limit on badn count,
 * maxBands is set to zero.
 *
 * The types field is a mask that specifies the supported pixel types and
 * sizes. Each bit represents one type/size pair (i.e. one byte INT). For the
 * binary pixel type, the byte count is one and for the 12-bit type it is two
 */

typedef struct _nitf_DownSampler
{
    nitf_IDownSampler *iface;   /* The functional interface  */
    nitf_Uint32 rowSkip;        /* The number of rows in the down-sample window */
    nitf_Uint32 colSkip;        /* The number of cols in the down-sample window */
    NITF_BOOL multiBand;        /* The down-sampler method is muli-band */
    nitf_Uint32 minBands;       /* Minimum number of bands in multi-band method */
    nitf_Uint32 maxBands;       /* Maxmum number of bands in multi-band method */
    nitf_Uint32 types;          /* Mask of type/pixel size flags */
    NITF_DATA *data;            /* To be overloaded by derived class  */
}
nitf_DownSampler;


/*! \def NITF_DOWNSAMPLER_TYPE_INT_1BYTE - Integer, one byte pixel */
#   define NITF_DOWNSAMPLER_TYPE_INT_1BYTE ((nitf_Uint32) 0x00000001)
/*! \def NITF_DOWNSAMPLER_TYPE_INT_2BYTE - Integer, two byte pixel */
#   define NITF_DOWNSAMPLER_TYPE_INT_2BYTE ((nitf_Uint32) 0x00000002)
/*! \def NITF_DOWNSAMPLER_TYPE_INT_4BYTE - Integer, eight byte pixel */
#   define NITF_DOWNSAMPLER_TYPE_INT_4BYTE ((nitf_Uint32) 0x00000004)
/*! \def NITF_DOWNSAMPLER_TYPE_INT_8BYTE - Integer, eight byte pixel */
#   define NITF_DOWNSAMPLER_TYPE_INT_8BYTE ((nitf_Uint32) 0x00000008)

/*! \def NITF_DOWNSAMPLER_TYPE_B_1BYTE - Binary, one byte pixel
         Note: The one bit type is presented to the user as one byte pixels
*/
#   define NITF_DOWNSAMPLER_TYPE_B_1BYTE ((nitf_Uint32) 0x00000001)

/*! \def NITF_DOWNSAMPLER_TYPE_SI_1BYTE - Signed integer, one byte pixel */
#   define NITF_DOWNSAMPLER_TYPE_SI_1BYTE ((nitf_Uint32) 0x00000010)
/*! \def NITF_DOWNSAMPLER_TYPE_SI_2BYTE - Signed integer, two byte pixel */
#   define NITF_DOWNSAMPLER_TYPE_SI_2BYTE ((nitf_Uint32) 0x00000020)
/*! \def NITF_DOWNSAMPLER_TYPE_SI_4BYTE - Signed integer, four byte pixel */
#   define NITF_DOWNSAMPLER_TYPE_SI_4BYTE ((nitf_Uint32) 0x00000040)
/*! \def NITF_DOWNSAMPLER_TYPE_SI_8BYTE - Signed integer, eight byte pixel */
#   define NITF_DOWNSAMPLER_TYPE_SI_8BYTE ((nitf_Uint32) 0x00000080)

/*! \def NITF_DOWNSAMPLER_TYPE_R_4BYTE - Float, four byte pixel */
#   define NITF_DOWNSAMPLER_TYPE_R_4BYTE ((nitf_Uint32) 0x00000100)
/*! \def NITF_DOWNSAMPLER_TYPE_R_8BYTE - Float, eight byte pixel */
#   define NITF_DOWNSAMPLER_TYPE_R_8BYTE ((nitf_Uint32) 0x00000200)

/*! \def NITF_DOWNSAMPLER_TYPE_C_8BYTE - Complex, eight byte pixel */
#   define NITF_DOWNSAMPLER_TYPE_C_8BYTE ((nitf_Uint32) 0x00001000)
/*! \def NITF_DOWNSAMPLER_TYPE_C_16BYTE - Complex, sixteen byte pixel */
#   define NITF_DOWNSAMPLER_TYPE_C_16BYTE ((nitf_Uint32) 0x00002000)

/*! \def NITF_DOWNSAMPLER_TYPE_ALL - All types */
#   define NITF_DOWNSAMPLER_TYPE_ALL \
    ( \
      NITF_DOWNSAMPLER_TYPE_INT_1BYTE | \
      NITF_DOWNSAMPLER_TYPE_INT_2BYTE | \
      NITF_DOWNSAMPLER_TYPE_INT_4BYTE | \
      NITF_DOWNSAMPLER_TYPE_INT_8BYTE | \
      NITF_DOWNSAMPLER_TYPE_B_1BYTE   | \
      NITF_DOWNSAMPLER_TYPE_SI_1BYTE  | \
      NITF_DOWNSAMPLER_TYPE_SI_2BYTE  | \
      NITF_DOWNSAMPLER_TYPE_SI_4BYTE  | \
      NITF_DOWNSAMPLER_TYPE_SI_8BYTE  | \
      NITF_DOWNSAMPLER_TYPE_R_4BYTE   | \
      NITF_DOWNSAMPLER_TYPE_R_8BYTE   | \
      NITF_DOWNSAMPLER_TYPE_C_8BYTE   | \
      NITF_DOWNSAMPLER_TYPE_C_16BYTE    \
    )

/*! \def NITF_DOWNSAMPLER_TYPE_ALL_BUT_COMPLEX - All types except complex */
#   define NITF_DOWNSAMPLER_TYPE_ALL_BUT_COMPLEX \
    ( \
      NITF_DOWNSAMPLER_TYPE_INT_1BYTE | \
      NITF_DOWNSAMPLER_TYPE_INT_2BYTE | \
      NITF_DOWNSAMPLER_TYPE_INT_4BYTE | \
      NITF_DOWNSAMPLER_TYPE_INT_8BYTE | \
      NITF_DOWNSAMPLER_TYPE_B_1BYTE   | \
      NITF_DOWNSAMPLER_TYPE_SI_1BYTE  | \
      NITF_DOWNSAMPLER_TYPE_SI_2BYTE  | \
      NITF_DOWNSAMPLER_TYPE_SI_4BYTE  | \
      NITF_DOWNSAMPLER_TYPE_SI_8BYTE  | \
      NITF_DOWNSAMPLER_TYPE_R_4BYTE   | \
      NITF_DOWNSAMPLER_TYPE_R_8BYTE     \
    )

/*!
 *  The pixelSkip down-sample method. This method has a designated skip
 *  value in the row and column, and it skips data in between.
 *
 *  The  row and column skip factors divide the sub-window into non-overlaping
 *  sample windows. The upper left corner pixel of each sample window is
 *  the down-sampled value for that window
 *
 *  For a more comprehensive discussion of the merits and drawbacks of this
 *  type of down-sampling, please refer to the NITF manual 1.1.
 *
 *  \param rowSkip  The number of rows to skip
 *  \param colSkip  The number of columns to skip
 *  \param error  An error to populate if something bad happened
 *  \return This method returns an object on success, and NULL on
 *          failure.
 */
NITFAPI(nitf_DownSampler *) nitf_PixelSkip_construct(nitf_Uint32 rowSkip,
        nitf_Uint32 colSkip,
        nitf_Error * error);


/*!
 *  The maxDownSample selects the maximum pixel
 *
 *  The  row and column skip factors divide the sub-window into non-overlaping
 *  sample windows. The pixel with the maximum value in each sample window is
 *  the down-sampled value for that window. For complex images, the maximum
 *  the maximum absolute value (the corresponding complex value, not the
 *  real absolute value is the down-sample value.
 *
 *  For a more comprehensive discussion of the merits and drawbacks of this
 *  type of down-sampling, please refer to the NITF manual 1.1.
 *
 *  \param rowSkip  The number of rows to skip
 *  \param colSkip  The number of columns to skip
 *  \param error  An error to populate if something bad happened
 *  \return This method returns an object on success, and NULL on
 &          failure.
 */
NITFAPI(nitf_DownSampler *) nitf_MaxDownSample_construct(nitf_Uint32
        rowSkip,
        nitf_Uint32
        colSkip,
        nitf_Error *
        error);

/*!
* \brief Sum of square, two band, down-sample method
*
* The maximum is calculated as the sum of the sum of squares of two bands.
* The caller must supply exactly two bands. The complex pixel type as the
* individual band pixel type is not supported
*
*  For a more comprehensive discussion of the merits and drawbacks of this
*  type of down-sampling, please refer to the NITF manual 1.1.
*
*  \param rowSkip  The number of rows to skip
*  \param colSkip  The number of columns to skip
*  \param error  An error to populate if something bad happened
*
*  \return This method returns an object on success, and NULL on failure.
*/

NITFAPI(nitf_DownSampler *) nitf_SumSq2DownSample_construct(nitf_Uint32
        rowSkip,
        nitf_Uint32
        colSkip,
        nitf_Error *
        error);

/*!
* \brief First band select, two band, down-sample method
*
* The maximum is calculated as the value of the first band. The caller must
* supply exactly two bands. The complex pixel type as the
* individual band pixel type is not supported
*
*  For a more comprehensive discussion of the merits and drawbacks of this
*  type of down-sampling, please refer to the NITF manual 1.1.
*
*  \param rowSkip  The number of rows to skip
*  \param colSkip  The number of columns to skip
*  \param error  An error to populate if something bad happened
*
*  \return This method returns an object on success, and NULL on failure.
*/

NITFAPI(nitf_DownSampler *) nitf_Select2DownSample_construct(nitf_Uint32
        rowSkip,
        nitf_Uint32
        colSkip,
        nitf_Error *
        error);

/*!
 *  The downsampler destructor is a management function.  While it does
 *  free the downsampler, it first destroys any user data using the
 *  interface-defined hook for destruction
 *
 *  \param downsampler The downsampler to destroy
 */
NITFAPI(void) nitf_DownSampler_destruct(nitf_DownSampler ** downsampler);

/*!
    nitf_DownSampler_apply - Apply down-sample method

  nitf_DownSampler_apply apples the down-sample method associated with
  the object

  This function is implemented as a macro, see the definition for the
  type NITF_IDOWNSAMPLER_APPLY
*/

#define nitf_DownSampler_apply(object,inputWindows,outputWindows,numBands, \
                               numWindowRows,numWindowCols,numInputCols,numSubWindowCols, \
                               pixelType,pixelSize,rowsInLastWindow,colsInLastWindow,error) \
((*(object->iface->apply))(object,inputWindows,outputWindows,numBands, \
                           numWindowRows,numWindowCols,numInputCols,numSubWindowCols,  \
                           pixelType, pixelSize,rowsInLastWindow,colsInLastWindow,error)) \

NITF_CXX_ENDGUARD

#endif

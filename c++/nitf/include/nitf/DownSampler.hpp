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

#ifndef __NITF_DOWN_SAMPLER_HPP__
#define __NITF_DOWN_SAMPLER_HPP__

#include "nitf/DownSampler.h"
#include "nitf/IOInterface.hpp"
#include "nitf/System.hpp"
#include "nitf/Object.hpp"
#include <string>

/*!
 *  \file DownSampler.hpp
 *  \brief  Contains wrapper implementations for DownSamplers
 */

namespace nitf
{

/*!
 *  \class DownSampler
 *  \brief  The C++ wrapper for the nitf_DownSampler
 */
DECLARE_CLASS(DownSampler)
{
public:
    //! Copy constructor
    DownSampler(const DownSampler & x)
    {
        setNative(x.getNative());
    }

    //! Assignment Operator
    DownSampler & operator=(const DownSampler & x)
    {
        if (&x != this)
            setNative(x.getNative());
        return *this;
    }

    //! Set native object
    DownSampler(nitf_DownSampler * x)
    {
        setNative(x);
        getNativeOrThrow();
    }

    virtual ~DownSampler(){}

    /*!
     *  Applies a sampling method while reading.
     *  \param inputWindow  The input image fragment
     *  \param outputWindow  A sub-sampled image fragment
     *  \param numWindowRows  How many rows for the input window
     *  \param numWindowCols  How many cols for the input window
     *  \param numInputCols  Number of columns in input buffer, full res
     *  \param numSubWindowCols  The number of columns in the sub-window
     *  \param pixelType  The pixel type (valid values found in System layer)
     *  \param pixelSize  The size of one pixel
     *  \param rowsInLastWindow  The number of rows in the final window
     *  \param colsInLastWindow  The number of cols in the final window
     *
     *  Note:
     *  The numWindowRows, numWindowCols, and numSubWindowCols values
     *  are in output image units (units of sample windows).  For example,
     *  with a pixel skip of 3 in columns, if the sub-window request spans
     *  columns 0-299, then numSubWindowCols is 100.  If the block is
     *  such that a particular request spans columns 90-149 (60 full
     *  resolution columns), then numWindowCols is 20.  The numInputCols
     *  value is in full resolution units.  This value gives the length,
     *  in pixels, of one row in the input buffer.  This buffer is used
     *  for all down sample calls.  Since the number of windows can vary
     *  from call to call, this buffer has a worst case length.  Therefore,
     *  it is not possible to move from one row to the next with just the
     *  number of sample windows per row (numWindowCols) for the current
     *  request.
     */
    virtual void apply(NITF_DATA ** inputWindow,
                       NITF_DATA ** outputWindow,
                       nitf::Uint32 numBands,
                       nitf::Uint32 numWindowRows,
                       nitf::Uint32 numWindowCols,
                       nitf::Uint32 numInputCols,
                       nitf::Uint32 numSubWindowCols,
                       nitf::Uint32 pixelType,
                       nitf::Uint32 pixelSize,
                       nitf::Uint32 rowsInLastWindow,
                       nitf::Uint32 colsInLastWindow) throw (nitf::NITFException);

    nitf::Uint32 getRowSkip();

    nitf::Uint32 getColSkip();

protected:

    DownSampler(){}
    nitf_Error error;
};


/*!
 *  \class PixelSkip
 *  \brief This class represents a down sample method that
 *  has a designated skip value in the row and column,
 *  and it skips the data in between.
 *
 *  The row and column skip factors divide the sub-window
 *  into non-overlapping sample windows.  The upper left
 *  corner pixel of each sample window is the down sampled
 *  value for that window.
 *
 *  For a more comprehensive discussion of the merits and
 *  drawbacks of this type of down sampling, please refer to
 *  the NITF manual.
 */
class PixelSkip : public DownSampler
{
public:
    /*!
     *  Constructor
     *  \param rowSkip  The number of rows to skip
     *  \param colSkip  The number of columns to skip
     */
    PixelSkip(nitf::Uint32 rowSkip,
              nitf::Uint32 colSkip) throw (nitf::NITFException);

    //! Destructor
    ~PixelSkip();
};

/*!
 *  \class MaxDownSample
 *  \brief  Selects the maximum pixel.
 *
 *  The row and column skip factors divide the sub-window
 *  into non-overlapping sample windows.  The pixel with the
 *  maximum value in each sample window is the down sampled
 *  value for that window.  For complex images, the maximum
 *  absolute value (the corresponding complex value, not the
 *  real absolute value) is the down sample value.
 *
 *  For a more comprehensive discussion of the merits and drawbacks
 *  of this type of down sampling, please refer to the NITF manual.
 *
 */
class MaxDownSample : public DownSampler
{
public:
    /*!
     *  Constructor
     *  \param rowSkip  The number of rows to skip
     *  \param colSkip  The number of columns to skip
     */
    MaxDownSample(nitf::Uint32 rowSkip,
                  nitf::Uint32 colSkip) throw (nitf::NITFException);
    //! Destructor
    ~MaxDownSample();
};

/*!
 *  \class SumSq2DownSample
 *  \brief Sum of square, two band, down-sample method
 *
 *  The maximum is calculated as the sum of the sum of squares of
 *  two bands.  The caller must supply exactly two bands.  The
 *  complex pixel type as the individual band pixel type is
 *  not supported.
 *
 *  For a more comprehensive discussion of the merits and drawbacks
 *  of this type of down-sampling, please refer to the NITF manual.
 */
class SumSq2DownSample : public DownSampler
{
public:
    /*!
     *  Constructor
     *  \param rowSkip  The number of rows to skip
     *  \param colSkip  The number of cols to skip
     */
    SumSq2DownSample(nitf::Uint32 rowSkip,
                     nitf::Uint32 colSkip) throw (nitf::NITFException);
    //! Destructor
    ~SumSq2DownSample();
};

/*!
 *  \class Select2DownSample
 *  \brief First band select, two band, down-sample method
 *
 *  The maximum is calculated as the value of the first band.
 *  The caller must supply exactly two bands.  The complex
 *  pixel type as the individual band pixel type is not supported.
 *
 *  For a more comprehensive discussion of the merits and drawbacks
 *  of this type of down-sampling, please refer to the NITF manual.
 */
class Select2DownSample : public DownSampler
{
public:
    /*!
     *  Constructor
     *  \param rowSkip  The number of rows to skip
     *  \param colSkip  The number of cols to skip
     */
    Select2DownSample(nitf::Uint32 rowSkip,
                      nitf::Uint32 colSkip) throw (nitf::NITFException);
    //! Destructor
    ~Select2DownSample();
};
}
#endif

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

#ifndef __NITF_IMAGE_READER_HPP__
#define __NITF_IMAGE_READER_HPP__

#include "nitf/ImageReader.h"
#include "nitf/Object.hpp"
#include "nitf/BlockingInfo.hpp"
#include <string>

/*!
 *  \file ImageReader.hpp
 *  \brief  Contains wrapper implementation for ImageReader
 */

namespace nitf
{

/*!
 *  \class ImageReader
 *  \brief  The C++ wrapper for the nitf_ImageReader
 */
DECLARE_CLASS(ImageReader)
{
public:
    //! Copy constructor
    ImageReader(const ImageReader & x);

    //! Assignment Operator
    ImageReader & operator=(const ImageReader & x);

    //! Set native object
    ImageReader(nitf_ImageReader * x);

    ~ImageReader();

    //! Get the blocking info
    nitf::BlockingInfo getBlockingInfo() throw (nitf::NITFException);

    /*!
     *  Read a sub-window.  See ImageIO::read for more details.
     *  \param  subWindow  The sub-window to read
     *  \param  user  User-defined data buffers for read
     *  \param  padded  Returns TRUE if pad pixels may have been read
     */
    void read(nitf::SubWindow & subWindow, nitf::Uint8 ** user, int * padded)
        throw (nitf::NITFException);

    //!  Set read caching
    void setReadCaching();

private:
    nitf_Error error;
    ImageReader() throw(nitf::NITFException){}
};

}
#endif

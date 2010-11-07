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

#ifndef __NITF_IMAGE_WRITER_HPP__
#define __NITF_IMAGE_WRITER_HPP__

#include "nitf/ImageWriter.h"
#include "nitf/Object.hpp"
#include "nitf/WriteHandler.hpp"
#include "nitf/ImageSource.hpp"
#include "nitf/ImageSubheader.hpp"
#include <string>

/*!
 *  \file ImageWriter.hpp
 *  \brief  Contains wrapper implementation for ImageWriter
 */

namespace nitf
{

/*!
 *  \class ImageWriter
 *  \brief  The C++ wrapper for the nitf_ImageWriter
 */
class ImageWriter : public WriteHandler
{
public:
    /*!
     *  Constructor
     *  \param subheader    The subheader of the Image to write
     */
    ImageWriter(nitf::ImageSubheader& subheader) throw(nitf::NITFException);

    // Set native object
    ImageWriter(nitf_ImageWriter *x) : WriteHandler(x)
    {
    }

    ~ImageWriter();

    /*!
     *  Attach an image source from which to write.
     *  \param imageSource  The image source from which to write
     */
    void attachSource(nitf::ImageSource imageSource)
            throw (nitf::NITFException);

    //! Enable/disable cached writes
    void setWriteCaching(int enable);

    /*!
     *  Function allows the user access to the product's pad pixels.
     *  For example, if you wanted transparent pixels for fill, you would
     *  set this function using arguments (0, 1)
     */
    void setPadPixel(nitf::Uint8* value, nitf::Uint32 length);

private:
    nitf_Error error;
//    bool mAdopt;
//    nitf::ImageSource* mImageSource;
};

}
#endif

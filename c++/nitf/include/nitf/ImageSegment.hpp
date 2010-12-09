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

#ifndef __NITF_IMAGESEGMENT_HPP__
#define __NITF_IMAGESEGMENT_HPP__

#include "nitf/ImageSegment.h"
#include "nitf/ImageSubheader.hpp"
#include "nitf/System.hpp"
#include "nitf/NITFException.hpp"
#include "nitf/Object.hpp"
#include <string>

/*!
 *  \file ImageSegment.hpp
 *  \brief  Contains wrapper implementation for ImageSegment
 */

namespace nitf
{

/*!
 *  \class ImageSegment
 *  \brief  The C++ wrapper for the nitf_ImageSegment
 */
DECLARE_CLASS(ImageSegment)
{
public:
    //! Copy constructor
    ImageSegment(const ImageSegment & x);

    //! Assignment Operator
    ImageSegment & operator=(const ImageSegment & x);

    //! Set native object
    ImageSegment(nitf_ImageSegment * x);

    //! Constructor
    ImageSegment() throw(nitf::NITFException);

    ImageSegment(NITF_DATA * x);

    ImageSegment & operator=(NITF_DATA * x);

     //! Clone
    nitf::ImageSegment clone() throw(nitf::NITFException);

    ~ImageSegment();

    //! Get the subheader
    nitf::ImageSubheader getSubheader();

    //! Set the subheader
    void setSubheader(nitf::ImageSubheader & value);

    //! Get the imageOffset
    nitf::Uint64 getImageOffset() const;

    //! Set the imageOffset
    void setImageOffset(nitf::Uint64 value);

    //! Get the imageEnd
    nitf::Uint64 getImageEnd() const;

    //! Set the imageEnd
    void setImageEnd(nitf::Uint64 value);

private:
    nitf_Error error;
};

}
#endif

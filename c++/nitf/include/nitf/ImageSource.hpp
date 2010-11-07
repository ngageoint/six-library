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

#ifndef __NITF_IMAGESOURCE_HPP__
#define __NITF_IMAGESOURCE_HPP__

#include "nitf/ImageSource.h"
#include "nitf/BandSource.hpp"
#include "nitf/System.hpp"
#include "nitf/NITFException.hpp"
#include "nitf/Object.hpp"
#include <string>

/*!
 *  \file ImageSource.hpp
 *  \brief  Contains wrapper implementation for ImageSource
 */

namespace nitf
{

/*!
 *  \class ImageSource
 *  \brief  The C++ wrapper for the nitf_ImageSource
 */
DECLARE_CLASS(ImageSource)
{
public:
    //! Copy constructor
    ImageSource(const ImageSource & x);

    //! Assignment Operator
    ImageSource & operator=(const ImageSource & x);

    //! Set native object
    ImageSource(nitf_ImageSource * x);

    /*!
     *  Constructor
     */
    ImageSource() throw(nitf::NITFException);

    ~ImageSource();

    //! Add a band
    void addBand(nitf::BandSource bandSource) throw(nitf::NITFException);

    //! Get a band
    nitf::BandSource getBand(int n) throw (nitf::NITFException);

private:
    nitf_Error error;
    // keep a list of the bands
    std::vector<nitf::BandSource*> mBands;
}
;
}
#endif

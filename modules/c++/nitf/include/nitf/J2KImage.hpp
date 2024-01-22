/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2017, MDA Information Systems LLC
 * (C) Copyright 2022, Maxar Technologies, Inc.
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

#ifndef NITF_J2KImage_hpp_INCLUDED_
#define NITF_J2KImage_hpp_INCLUDED_
#pragma once

#include <types/RowCol.h>

#include <j2k/TileWriter.h>

#include "nitf/exports.hpp"

namespace j2k
{
    /*!
     * \class OPJImage
     * \desc RAII wrapper around j2k_image_t.
     */
    class NITRO_NITFCPP_API Image final
    {
        //! The openjpeg image.
        j2k_image_t* mImage = nullptr;

        //! The openjpeg image component parameters.
        j2k_image_comptparm mImageComponentParams;

        void initComponents(const types::RowCol<size_t>& rawImageDims);
        void initImage();

    public:
        /*!
         * Constructor
         *
         * \param rawImageDims Dimensions of the raw image
         */
        Image(const types::RowCol<size_t>& rawImageDims);
        Image(const Image&) = delete;
        Image& operator=(const Image&) = delete;
        Image(Image&&) = default;
        Image& operator=(Image&&) = default;

        /*!
         * Destructor - destroys the openjpeg image.
         */
        ~Image();

        /*!
         * \return The openjpeg image.
         */
        j2k_image_t* getNative() const noexcept
        {
            return mImage;
        }
    };
}

#endif // NITF_J2KImage_hpp_INCLUDED_

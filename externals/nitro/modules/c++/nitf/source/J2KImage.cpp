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

#include "nitf/J2KImage.hpp"

#include <sstream>

#include <except/Exception.h>
#include <sys/Conf.h>
#include <gsl/gsl.h>

j2k::Image::Image(const types::RowCol<size_t>& rawImageDims)
{
    initComponents(rawImageDims);

    // Create our image
    mImage = j2k_image_tile_create(1, &mImageComponentParams, J2K_CLRSPC_GRAY);
    if (mImage == nullptr)
    {
        std::ostringstream os;
        os << "Failed to create an openjpeg image handle using the provided raw image dimensions";
        os << " (rows: "<< rawImageDims.row << ", cols : " << rawImageDims.col << ")!";
        throw except::Exception(Ctxt(os));
    }

    // set up the image
    initImage();
}

j2k::Image::~Image()
{
    j2k_image_destroy(mImage);
}

void j2k::Image::initComponents(const types::RowCol<size_t>& rawImageDims)
{
    // The width and height of our single component will span the
    // full image extent
    mImageComponentParams.w = gsl::narrow<uint32_t>(rawImageDims.col);
    mImageComponentParams.h = gsl::narrow<uint32_t>(rawImageDims.row);

    // Precision and bit depth
    mImageComponentParams.prec = 8;
    mImageComponentParams.bpp = 8;

    // Starting corner of our component in the reference grid
    mImageComponentParams.x0 = 0;
    mImageComponentParams.y0 = 0;

    // Horizontal separation of each sample with respect to the reference grid
    mImageComponentParams.dx = 1;

    // Vertical separation of each sample with respect to the reference grid
    mImageComponentParams.dy = 1;

    // Signed or unsigned data - fixed to unsigned for grayscale image data
    mImageComponentParams.sgnd = 0;
}

void j2k::Image::initImage()
{
    j2k_image_init(mImage, 0, 0,
        gsl::narrow<int>(mImageComponentParams.w), gsl::narrow<int>(mImageComponentParams.h),
        1, // One image component corresponding to the full grayscale image
        J2K_CLRSPC_GRAY);
}

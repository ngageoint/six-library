/* =========================================================================
 * This file is part of six-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * six-c++ is free software; you can redistribute it and/or modify
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
 * License along with this program; If not,
 * see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef __SIX_LEGEND_H__
#define __SIX_LEGEND_H__

#include <vector>

#include <scene/sys_Conf.h>
#include <types/RowCol.h>
#include <mem/ScopedCopyablePtr.h>
#include <six/Types.h>

namespace six
{
/*!
 * \struct Legend
 * \brief A legend associated with a SIDD product
 *
 * See section 2.4.3 of SIDD Volume 2
 *
 */
struct Legend
{
    Legend() = default;

    //! Convenience method to resize the image to match the dimensions
    void setDims(const types::RowCol<size_t>& dims)
    {
        mDims = dims;
        mImage.resize(dims.row * dims.col);
    }

    //! Pixel type of legend image data.
    //  TODO: For now, only MONO8I and RGB8LU are supported
    PixelType mType = PixelType::NOT_SET;

    //! Location of the legend with respect to the upper-left corner of the
    //  image segment it's attached to
    types::RowCol<size_t> mLocation{ 0, 0 };

    //! Dimensions of the legend image pixels
    types::RowCol<size_t> mDims{ 0, 0 };

    //! Image legend pixels.  These are either pixel values or LUT indices
    //  depending on of the pixel type is MONO8I or RGB8LU, respectively.
    std::vector<sys::ubyte> mImage;

    //! LUT associated with image pixels.  Must be present if pixel type is
    //  RGB8LU.
    mem::ScopedCopyablePtr<LUT> mLUT;
};
}

#endif

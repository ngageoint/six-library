/* =========================================================================
 * This file is part of six-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2013, General Dynamics - Advanced Information Systems
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
#ifndef __SIX_NITF_SEGMENT_INFO_H__
#define __SIX_NITF_SEGMENT_INFO_H__

#include "six/Types.h"

namespace six
{
/*!
 *  \class NITFSegmentInfo
 *  \brief Book-keeping for NITF segments
 *
 *  This class contains the per-segment information for an image.
 */
struct NITFSegmentInfo
{
    //! First row in the image segment in real space
    unsigned long firstRow;

    //! Row offset in the CCS (ILOC R)
    //  When you are attached to another segment, ILOC is with respect to that
    //  segment.  Per the spec, we will always attach to the previous segment,
    //  so this will simply be the number of rows in that previous segment.
    unsigned long rowOffset;

    //! Number of rows in this segment
    unsigned long numRows;

    //! The image segment corner points
    LatLonCorners corners;
};

}

#endif

/* =========================================================================
 * This file is part of six.sidd30-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * six.sidd-c++ is free software; you can redistribute it and/or modify
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
#ifndef __SIX_DOWNSTREAM_REPROCESSING_H__
#define __SIX_DOWNSTREAM_REPROCESSING_H__

#include <mem/ScopedCopyablePtr.h>
#include "six/Types.h"
#include "six/Init.h"
#include "six/ParameterCollection.h"

namespace six
{
namespace sidd
{
/*!
 *  \struct GeometricChip
 *  \brief SIDD GeometricChip
 *
 *  Contains information related to downstream chipping of the product.
 */
struct GeometricChip
{
    //!  Size of the chipped product in pixels
    RowColInt chipSize;
    void setChipSize(const  types::RowCol<size_t>& aoiDims)
    {
        chipSize.row = static_cast<ptrdiff_t>(aoiDims.row);
        chipSize.col = static_cast<ptrdiff_t>(aoiDims.col);
    }

    //!  Upper-left corner with respect to the original product
    RowColDouble originalUpperLeftCoordinate;

    //!  Upper-right corner with respect to the original product
    RowColDouble originalUpperRightCoordinate;

    //!  Lower-left corner with respect to the original product
    RowColDouble originalLowerLeftCoordinate;

    //!  Lower-right corner with respect to the original product
    RowColDouble originalLowerRightCoordinate;

    // Given chip pixel coordinates, provides full image pixel coordinates
    RowColDouble
    getFullImageCoordinateFromChip(const RowColDouble& chip) const;

    // Given full image pixel coordinates, provides chip pixel coordinates
    RowColDouble
    getChipCoordinateFromFullImage(const RowColDouble& full) const;

    //! Equality operators
    bool operator==(const GeometricChip& rhs) const;
    bool operator!=(const GeometricChip& rhs) const
    {
        return !(*this == rhs);
    }
};

/*!
 *  \struct ProcessingEvent
 *  \param SIDD ProcessingEvent
 *
 *  Contains information related to downstream processing of
 *  the product
 */
struct ProcessingEvent
{
    //!  Application which applied a modification
    std::string applicationName;

    //!  Date and time defined in UTC.
    DateTime appliedDateTime;

    //!  Interpolation method (optional, empty strings not written)
    std::string interpolationMethod;

    //!  (Optional, Unbounded) List of descriptors for the processing event
    ParameterCollection descriptor;

    bool operator==(const ProcessingEvent& rhs) const;
    bool operator!=(const ProcessingEvent& rhs) const
    {
        return !(*this == rhs);
    }
};

/*!
 *  \struct DownstreamReprocessing
 *  \brief SIDD DownstreamReprocessing block
 *
 *  Contains metadata related to downstream processing of the product.
 */
struct DownstreamReprocessing
{
    /*!
     *  (Optional) chipping information
     */
    mem::ScopedCopyablePtr<GeometricChip> geometricChip;

    //!  (Optional, Unbounded) downstream processing events
    std::vector<mem::ScopedCopyablePtr<ProcessingEvent> > processingEvents;

    //! Equality operator
    bool operator==(const DownstreamReprocessing& rhs) const
    {
        return geometricChip == rhs.geometricChip && processingEvents == rhs.processingEvents;
    }

    bool operator!=(const DownstreamReprocessing& rhs) const
    {
        return !(*this == rhs);
    }
};
}
}
#endif


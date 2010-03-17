/* =========================================================================
 * This file is part of six-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
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
#ifndef __SIX_DOWNSTREAM_REPROCESSING_H__
#define __SIX_DOWNSTREAM_REPROCESSING_H__

#include "six/Types.h"
#include "six/Init.h"
#include "six/Parameter.h"

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

    //!  Upper-left corner with respect to the original product
    RowColDouble originalUpperLeftCoordinate;

    //!  Upper-right corner with respect to the original product
    RowColDouble originalUpperRightCoordinate;

    //!  Lower-left corner with respect to the original product
    RowColDouble originalLowerLeftCoordinate;

    //!  Lower-right corner with respect to the original product
    RowColDouble originalLowerRightCoordinate;

    //!  Copy of this
    GeometricChip* clone() const;
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
    std::vector<Parameter> descriptor;

    //!  Copy of the object
    ProcessingEvent* clone() const;
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
    GeometricChip* geometricChip;
    
    //!  (Optional, Unbounded) downstream processing events
    std::vector<ProcessingEvent*> processingEvents;

    //!  Constructor, initializes optional element to NULL
    DownstreamReprocessing() :
        geometricChip(NULL)
    {
    }

    //!  Destructor, deletes optional chip, events if not NULL
    ~DownstreamReprocessing();
    
    //!  Clone, including a chip if non-NULL or any events
    DownstreamReprocessing* clone() const;
};
}
}
#endif

/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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

#ifndef __NITF_SEGMENT_WRITER_HPP__
#define __NITF_SEGMENT_WRITER_HPP__

#include "nitf/SegmentWriter.h"
#include "nitf/Object.hpp"
#include "nitf/WriteHandler.hpp"
#include "nitf/SegmentSource.hpp"
#include <string>

/*!
 *  \file SegmentWriter.hpp
 *  \brief  Contains wrapper implementation for SegmentWriter
 */

namespace nitf
{
/*!
 *  \class SegmentWriter
 *  \brief  The C++ wrapper for the nitf_SegmentWriter
 */
class SegmentWriter : public WriteHandler
{
public:
    SegmentWriter();

    SegmentWriter(nitf::SegmentSource segmentSource);

    // Set native object
    SegmentWriter(nitf_SegmentWriter *x) : WriteHandler(x)
    {
    }

    ~SegmentWriter() = default;

    /*!
     *  Attach a segment source from which to write.
     *  \param segmentSource  The segment source from which to write
     */
    void attachSource(nitf::SegmentSource segmentSource);

private:
    nitf_Error error{};
//    bool mAdopt;
//    nitf::SegmentSource* mSegmentSource;
};

}
#endif

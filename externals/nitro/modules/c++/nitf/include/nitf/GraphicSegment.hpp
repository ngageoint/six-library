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

#ifndef __NITF_GRAPHICSEGMENT_HPP__
#define __NITF_GRAPHICSEGMENT_HPP__

#include "nitf/GraphicSegment.h"
#include "nitf/System.hpp"
#include "nitf/NITFException.hpp"
#include "nitf/GraphicSubheader.hpp"
#include "nitf/Object.hpp"
#include <string>

/*!
 *  \file GraphicSegment.hpp
 *  \brief  Contains wrapper implementation for GraphicSegment
 */

namespace nitf
{

/*!
 *  \class GraphicSegment
 *  \brief  The C++ wrapper for the nitf_GraphicSegment
 */
DECLARE_CLASS(GraphicSegment)
{
public:
    //! Copy constructor
    GraphicSegment(const GraphicSegment & x);

    //! Assignment Operator
    GraphicSegment & operator=(const GraphicSegment & x);

    //! Set native object
    GraphicSegment(nitf_GraphicSegment * x);

    //! Constructor
    GraphicSegment();

    GraphicSegment(NITF_DATA * x);

    GraphicSegment & operator=(NITF_DATA * x);

    //! Clone
    nitf::GraphicSegment clone() const;


    ~GraphicSegment() = default;
    GraphicSegment(GraphicSegment&&) = default;
    GraphicSegment& operator=(GraphicSegment&&) = default;

    //! Get the subheader
    nitf::GraphicSubheader getSubheader() const;

    //! Set the subheader
    void setSubheader(nitf::GraphicSubheader & value);

    //! Get the offset
    uint64_t getOffset() const;

    //! Set the offset
    void setOffset(uint64_t value);

    //! Get the end
    uint64_t getEnd() const;

    //! Set the end
    void setEnd(uint64_t value);

private:
    mutable nitf_Error error{};
};

}
#endif

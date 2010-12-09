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
    GraphicSegment() throw(nitf::NITFException);

    GraphicSegment(NITF_DATA * x);

    GraphicSegment & operator=(NITF_DATA * x);

    //! Clone
    nitf::GraphicSegment clone() throw(nitf::NITFException);

    ~GraphicSegment();

    //! Get the subheader
    nitf::GraphicSubheader getSubheader();

    //! Set the subheader
    void setSubheader(nitf::GraphicSubheader & value);

    //! Get the offset
    nitf::Uint64 getOffset() const;

    //! Set the offset
    void setOffset(nitf::Uint64 value);

    //! Get the end
    nitf::Uint64 getEnd() const;

    //! Set the end
    void setEnd(nitf::Uint64 value);

private:
    nitf_Error error;
};

}
#endif

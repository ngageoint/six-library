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

#ifndef __NITF_RESEGMENT_HPP__
#define __NITF_RESEGMENT_HPP__

#include "nitf/RESegment.h"
#include "nitf/RESubheader.hpp"
#include "nitf/Object.hpp"
#include <string>

/*!
 *  \file RESegment.hpp
 *  \brief  Contains wrapper implementation for RESegment
 */

namespace nitf
{

/*!
 *  \class RESegment
 *  \brief  The C++ wrapper for the nitf_RESegment
 */
DECLARE_CLASS(RESegment)
{
public:
    //! Copy constructor
    RESegment(const RESegment & x);

    //! Assignment Operator
    RESegment & operator=(const RESegment & x);

    //! Set native object
    RESegment(nitf_RESegment * x);

    //! Default Constructor
    RESegment();

    RESegment(NITF_DATA * x);

    RESegment & operator=(NITF_DATA * x);

    //! Clone
    nitf::RESegment clone() const;

    ~RESegment() = default;

    //! Get the subheader
    nitf::RESubheader getSubheader() const;

    //! Set the subheader
    void setSubheader(nitf::RESubheader & value);

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

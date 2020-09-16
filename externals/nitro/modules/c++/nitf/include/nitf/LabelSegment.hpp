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

#ifndef __NITF_LABELSEGMENT_HPP__
#define __NITF_LABELSEGMENT_HPP__

#include "nitf/LabelSegment.h"
#include "nitf/LabelSubheader.hpp"
#include "nitf/Object.hpp"
#include <string>

/*!
 *  \file LabelSegment.hpp
 *  \brief  Contains wrapper implementation for LabelSegment
 */

namespace nitf
{

/*!
 *  \class LabelSegment
 *  \brief  The C++ wrapper for the nitf_LabelSegment
 */
DECLARE_CLASS(LabelSegment)
{
public:
    //! Copy constructor
    LabelSegment(const LabelSegment & x);

    //! Assignment Operator
    LabelSegment & operator=(const LabelSegment & x);

    //! Set native object
    LabelSegment(nitf_LabelSegment * x);

    //! Constructor
    LabelSegment();

    LabelSegment(NITF_DATA * x);

    LabelSegment & operator=(NITF_DATA * x);

    //! Clone
    nitf::LabelSegment clone();

    ~LabelSegment();

    //! Get the subheader
    nitf::LabelSubheader getSubheader();

    //! Set the subheader
    void setSubheader(nitf::LabelSubheader & value);

    //! Get the offset
    uint64_t getOffset() const;

    //! Set the offset
    void setOffset(uint64_t value);

    //! Get the end
    uint64_t getEnd() const;

    //! Set the end
    void setEnd(uint64_t value);

private:
    nitf_Error error;
};

}
#endif

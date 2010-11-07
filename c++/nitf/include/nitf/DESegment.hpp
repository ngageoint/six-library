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

#ifndef __NITF_DESEGMENT_HPP__
#define __NITF_DESEGMENT_HPP__

#include "nitf/DESegment.h"
#include "nitf/System.hpp"
#include "nitf/NITFException.hpp"
#include "nitf/DESubheader.hpp"
#include "nitf/Object.hpp"
#include <string>

/*!
 *  \file DESegment.hpp
 *  \brief  Contains wrapper implementation for DESegment
 */

namespace nitf
{

/*!
 *  \class DESegment
 *  \brief  The C++ wrapper for the nitf_DESegment
 *
 *  Contains sub-objects that make up a DES.
 */
DECLARE_CLASS(DESegment)
{
public:
    //! Copy constructor
    DESegment(const DESegment & x);

    //! Assignment Operator
    DESegment & operator=(const DESegment & x);

    //! Set native object
    DESegment(nitf_DESegment * x);

    //! Constructor
    DESegment() throw(nitf::NITFException);

    DESegment(NITF_DATA * x);

    DESegment & operator=(NITF_DATA * x);

    //! Clone
    nitf::DESegment clone() throw(nitf::NITFException);

    ~DESegment();

    //! Get the subheader
    nitf::DESubheader getSubheader();

    //! Set the subheader
    void setSubheader(nitf::DESubheader & value);

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

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

#ifndef __NITF_COMPONENTINFO_HPP__
#define __NITF_COMPONENTINFO_HPP__
#pragma once

#include "nitf/ComponentInfo.h"
#include "nitf/System.hpp"
#include "nitf/Field.hpp"
#include "nitf/Object.hpp"
#include <string>

/*!
 *  \file ComponentInfo.hpp
 *  \brief  Contains wrapper implementation for ComponentInfo
 */

namespace nitf
{

/*!
 *  \class ComponentInfo
 *  \brief  The C++ wrapper for the nitf_ComponentInfo
 *
 *  Carries information about the images, graphics, text, or extension
 *  components contained within the file.
 *
 *  The fields from this class are described in the file header.
 *  They contain information about some component section
 *  contained in a NITF file.
 */
DECLARE_CLASS(ComponentInfo)
{
public:

    //! Copy constructor
    ComponentInfo(const ComponentInfo & x);

    //! Assignment Operator
    ComponentInfo & operator=(const ComponentInfo & x);

    //! Set native object
    ComponentInfo(nitf_ComponentInfo * x);

    //! Clone
    nitf::ComponentInfo clone() const;

    ~ComponentInfo() = default;
    ComponentInfo(ComponentInfo&&) = default;
    ComponentInfo& operator=(ComponentInfo&&) = default;

    //! Get the lengthSubheader
    nitf::Field getLengthSubheader() const;

    //! Get the lengthData
    nitf::Field getLengthData() const;

protected:
    /*!
     *  Constructor
     *  \param subHeaderSize  The size of the subheader
     *  \param dataSize  The size of the data
     */
    ComponentInfo(uint32_t subHeaderSize = 0, uint64_t dataSize = 0);

private:
    mutable nitf_Error error{};
};

}
#endif

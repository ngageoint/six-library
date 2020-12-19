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

#pragma once

#include <string>

#include "nitf/DESubheader.h"
#include "nitf/Object.hpp"
#include "nitf/Field.hpp"
#include "nitf/FileSecurity.hpp"
#include "nitf/Extensions.hpp"
#include "nitf/TRE.hpp"

/*!
 *  \file DESubheader.hpp
 *  \brief  Contains wrapper implementation for DESubheader
 */

namespace nitf
{

/*!
 *  \class DESubheader
 *  \brief  The C++ wrapper for the nitf_DESubheader
 */
DECLARE_CLASS(DESubheader)
{
public:

    //! Copy constructor
    DESubheader(const DESubheader & x);

    //! Assignment Operator
    DESubheader & operator=(const DESubheader & x);

    //! Set native object
    DESubheader(nitf_DESubheader * x);

    //! Constructor
    DESubheader() noexcept(false);

    //! Clone
    nitf::DESubheader clone() const;

    ~DESubheader() = default;

    //! Get the filePartType
    nitf::Field getFilePartType() const;
    std::string filePartType() const
    {
        return getFilePartType().toString();
    }

    //! Get the typeID
    nitf::Field getTypeID() const;
    std::string typeID() const
    {
        return getTypeID().toTrimString();
    }

    //! Get the version
    nitf::Field getVersion() const;
    std::string version() const
    {
        return getVersion().toString();
    }

    //! Get the securityClass
    nitf::Field getSecurityClass() const;
    std::string securityClass() const
    {
        return getSecurityClass().toString();
    }

    //! Get the securityGroup
    nitf::FileSecurity getSecurityGroup() const;

    //! Set the securityGroup
    void setSecurityGroup(nitf::FileSecurity value);

    //! Get the overflowedHeaderType
    nitf::Field getOverflowedHeaderType() const;

    //! Get the dataItemOverflowed
    nitf::Field getDataItemOverflowed() const;

    //! Get the subheaderFieldsLength
    nitf::Field getSubheaderFieldsLength() const;

    //! Get the subheaderFields
    nitf::TRE getSubheaderFields() const;

    //! Set the subheaderFields
    void setSubheaderFields(nitf::TRE fields);

    //! Get the dataLength
    uint32_t getDataLength() const;

    //! Set the dataLength
    void setDataLength(uint32_t value);

    //! Get the userDefinedSection
    nitf::Extensions getUserDefinedSection() const;

    //! Set the userDefinedSection
    void setUserDefinedSection(nitf::Extensions value);

private:
    mutable nitf_Error error{};
};

}


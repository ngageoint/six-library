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

#ifndef __NITF_DESUBHEADER_HPP__
#define __NITF_DESUBHEADER_HPP__

#include "nitf/DESubheader.h"
#include "nitf/Object.hpp"
#include "nitf/Field.hpp"
#include "nitf/FileSecurity.hpp"
#include "nitf/Extensions.hpp"
#include "nitf/TRE.hpp"
#include <string>

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
    DESubheader() throw(nitf::NITFException);

    //! Clone
    nitf::DESubheader clone() throw(nitf::NITFException);

    ~DESubheader();

    //! Get the filePartType
    nitf::Field getFilePartType();

    //! Get the typeID
    nitf::Field getTypeID();

    //! Get the version
    nitf::Field getVersion();

    //! Get the securityClass
    nitf::Field getSecurityClass();

    //! Get the securityGroup
    nitf::FileSecurity getSecurityGroup();

    //! Set the securityGroup
    void setSecurityGroup(nitf::FileSecurity value);

    //! Get the overflowedHeaderType
    nitf::Field getOverflowedHeaderType();

    //! Get the dataItemOverflowed
    nitf::Field getDataItemOverflowed();

    //! Get the subheaderFieldsLength
    nitf::Field getSubheaderFieldsLength();

    //! Get the subheaderFields
    nitf::TRE getSubheaderFields();

    //! Set the subheaderFields
    void setSubheaderFields(nitf::TRE fields);

    //! Get the dataLength
    nitf::Uint32 getDataLength() const;

    //! Set the dataLength
    void setDataLength(nitf::Uint32 value);

    //! Get the userDefinedSection
    nitf::Extensions getUserDefinedSection();

    //! Set the userDefinedSection
    void setUserDefinedSection(nitf::Extensions value);

private:
    nitf_Error error;
};

}
#endif

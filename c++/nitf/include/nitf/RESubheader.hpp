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

#ifndef __NITF_RESUBHEADER_HPP__
#define __NITF_RESUBHEADER_HPP__

#include "nitf/RESubheader.h"
#include "nitf/Object.hpp"
#include "nitf/Field.hpp"
#include "nitf/FileSecurity.hpp"
#include "nitf/Extensions.hpp"
#include <string>

/*!
 *  \file RESubheader.hpp
 *  \brief  Contains wrapper implementation for RESubheader
 */

namespace nitf
{

/*!
 *  \class RESubheader
 *  \brief  The C++ wrapper for the nitf_RESubheader
 */
DECLARE_CLASS(RESubheader)
{
public:

    //! Copy constructor
    RESubheader(const RESubheader & x);

    //! Assignment Operator
    RESubheader & operator=(const RESubheader & x);

    //! Set native object
    RESubheader(nitf_RESubheader * x);

     //! Constructor
    RESubheader() throw(nitf::NITFException);

    //! Clone
    nitf::RESubheader clone() throw(nitf::NITFException);

    ~RESubheader();

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

    //! Get the subheaderFieldsLength
    nitf::Field getSubheaderFieldsLength();

    //! Get the subheaderFields
    char * getSubheaderFields() const;

    //! Get the dataLength
    nitf::Uint64 getDataLength() const;

    //! Set the dataLength
    void setDataLength(nitf::Uint32 value);

private:
    nitf_Error error;
};

}
#endif

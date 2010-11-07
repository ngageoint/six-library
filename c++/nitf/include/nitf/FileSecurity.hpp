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

#ifndef __NITF_FILESECURITY_HPP__
#define __NITF_FILESECURITY_HPP__

#include "nitf/FileSecurity.h"
#include "nitf/System.hpp"
#include "nitf/Field.hpp"
#include "nitf/Object.hpp"
#include <string>

/*!
 *  \file FileSecurity.hpp
 *  \brief  Contains wrapper implementation for FileSecurity
 */

namespace nitf
{

/*!
 *  \class FileSecurity
 *  \brief  The C++ wrapper for the nitf_FileSecurity
 *
 *  Contains fields pertaining to security information from
 *  the NITF file.
 */
DECLARE_CLASS(FileSecurity)
{
public:
    //! Copy constructor
    FileSecurity(const FileSecurity & x);

    //! Assignment Operator
    FileSecurity & operator=(const FileSecurity & x);

    //! Set native object
    FileSecurity(nitf_FileSecurity * x);

    //! Constructor
    FileSecurity() throw(nitf::NITFException);

    //! Clone
    nitf::FileSecurity clone() throw(nitf::NITFException);

    ~FileSecurity();

    //! Get the classificationSystem
    nitf::Field getClassificationSystem();

    //! Get the codewords
    nitf::Field getCodewords();

    //! Get the controlAndHandling
    nitf::Field getControlAndHandling();

    //! Get the releasingInstructions
    nitf::Field getReleasingInstructions();

    //! Get the declassificationType
    nitf::Field getDeclassificationType();

    //! Get the declassificationDate
    nitf::Field getDeclassificationDate();

    //! Get the declassificationExemption
    nitf::Field getDeclassificationExemption();

    //! Get the downgrade
    nitf::Field getDowngrade();

    //! Get the downgradeDateTime
    nitf::Field getDowngradeDateTime();

    //! Get the classificationText
    nitf::Field getClassificationText();

    //! Get the classificationAuthorityType
    nitf::Field getClassificationAuthorityType();

    //! Get the classificationAuthority
    nitf::Field getClassificationAuthority();

    //! Get the classificationReason
    nitf::Field getClassificationReason();

    //! Get the securitySourceDate
    nitf::Field getSecuritySourceDate();

    //! Get the securityControlNumber
    nitf::Field getSecurityControlNumber();

private:
    nitf_Error error;
};

}
#endif

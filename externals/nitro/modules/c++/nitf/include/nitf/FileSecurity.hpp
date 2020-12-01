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
    FileSecurity() noexcept(false);

    //! Clone
    nitf::FileSecurity clone() const;

    ~FileSecurity() = default;

    //! Get the classificationSystem
    nitf::Field getClassificationSystem() const;

    //! Get the codewords
    nitf::Field getCodewords() const;

    //! Get the controlAndHandling
    nitf::Field getControlAndHandling() const;

    //! Get the releasingInstructions
    nitf::Field getReleasingInstructions() const;

    //! Get the declassificationType
    nitf::Field getDeclassificationType() const;

    //! Get the declassificationDate
    nitf::Field getDeclassificationDate() const;

    //! Get the declassificationExemption
    nitf::Field getDeclassificationExemption() const;

    //! Get the downgrade
    nitf::Field getDowngrade() const;

    //! Get the downgradeDateTime
    nitf::Field getDowngradeDateTime() const;

    //! Get the classificationText
    nitf::Field getClassificationText() const;

    //! Get the classificationAuthorityType
    nitf::Field getClassificationAuthorityType() const;

    //! Get the classificationAuthority
    nitf::Field getClassificationAuthority() const;

    //! Get the classificationReason
    nitf::Field getClassificationReason() const;

    //! Get the securitySourceDate
    nitf::Field getSecuritySourceDate() const;

    //! Get the securityControlNumber
    nitf::Field getSecurityControlNumber() const;

private:
    mutable nitf_Error error{};
};

}
#endif

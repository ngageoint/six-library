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

#ifndef __NITF_FIELDWARNING_HPP__
#define __NITF_FIELDWARNING_HPP__

#include "nitf/FieldWarning.h"
#include "nitf/System.hpp"
#include "nitf/NITFException.hpp"
#include "nitf/Field.hpp"
#include "nitf/Object.hpp"
#include <string>

/*!
 *  \file FieldWarning.hpp
 *  \brief  Contains wrapper implementation for FieldWarning
 */

namespace nitf
{

/*!
 *  \class FieldWarning
 *  \brief  The C++ wrapper for the nitf_FieldWarning
 *
 *  Describes a warning associated with a field in the NITF
 */
DECLARE_CLASS(FieldWarning)
{
public:
    //! Copy constructor
    FieldWarning(const FieldWarning & x)
    {
        setNative(x.getNative());
    }

    //! Assignment Operator
    FieldWarning & operator=(const FieldWarning & x)
    {
        if (&x != this)
            setNative(x.getNative());
        return *this;
    }

    //! Set native object
    FieldWarning(nitf_FieldWarning * x)
    {
        setNative(x);
        getNativeOrThrow();
    }

    /*!
     *  Constructor
     *  \param fileOffset  The offset in the file to the field
     *  \param field  A string representing the NITF field
     *  \param value  The NITF field value
     *  \param expectation  A string describing the expected field value
     */
    FieldWarning(nitf::Off fileOffset, const std::string& fieldName,
        nitf::Field & field, const std::string& expectation)
            throw(nitf::NITFException)
    {
        setNative(nitf_FieldWarning_construct(fileOffset, fieldName.c_str(),
            field.getNative(), expectation.c_str(), &error));
        getNativeOrThrow();
        setManaged(false);
    }

    ~FieldWarning(){}

    //! Get the fileOffset
    nitf::Off getFileOffset() const
    {
        return getNativeOrThrow()->fileOffset;
    }
    //! Get the field
    std::string getFieldName() const
    {
        return std::string(getNativeOrThrow()->fieldName);
    }
    //! Get the value
    nitf::Field getField()
    {
        return nitf::Field(getNativeOrThrow()->field);
    }
    //! Get the expectation
    std::string getExpectation() const
    {
        return std::string(getNativeOrThrow()->expectation);
    }

private:
    nitf_Error error;
};

}
#endif

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

#include "nitf/FieldWarning.hpp"

using namespace nitf;

FieldWarning::FieldWarning(nitf_FieldWarning* x)
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
FieldWarning::FieldWarning(nitf::Off fileOffset, const std::string& fieldName,
    nitf::Field& field, const std::string& expectation)
{
    setNative(nitf_FieldWarning_construct(fileOffset, fieldName.c_str(),
        field.getNative(), expectation.c_str(), &error));
    getNativeOrThrow();
    setManaged(false);
}
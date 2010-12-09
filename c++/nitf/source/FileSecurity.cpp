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

#include "nitf/FileSecurity.hpp"

using namespace nitf;

FileSecurity::FileSecurity(const FileSecurity & x)
{
    setNative(x.getNative());
}

FileSecurity & FileSecurity::operator=(const FileSecurity & x)
{
    if (&x != this)
        setNative(x.getNative());
    return *this;
}

FileSecurity::FileSecurity(nitf_FileSecurity * x)
{
    setNative(x);
    getNativeOrThrow();
}

FileSecurity::FileSecurity() throw(nitf::NITFException)
{
    setNative(nitf_FileSecurity_construct(&error));
    getNativeOrThrow();
    setManaged(false);
}

nitf::FileSecurity FileSecurity::clone() throw(nitf::NITFException)
{
    nitf::FileSecurity dolly(nitf_FileSecurity_clone(getNativeOrThrow(), &error));
    dolly.setManaged(false);
    return dolly;
}

FileSecurity::~FileSecurity(){}


nitf::Field FileSecurity::getClassificationSystem()
{
    return nitf::Field(getNativeOrThrow()->classificationSystem);
}

nitf::Field FileSecurity::getCodewords()
{
    return nitf::Field(getNativeOrThrow()->codewords);
}

nitf::Field FileSecurity::getControlAndHandling()
{
    return nitf::Field(getNativeOrThrow()->controlAndHandling);
}

nitf::Field FileSecurity::getReleasingInstructions()
{
    return nitf::Field(getNativeOrThrow()->releasingInstructions);
}

nitf::Field FileSecurity::getDeclassificationType()
{
    return nitf::Field(getNativeOrThrow()->declassificationType);
}

nitf::Field FileSecurity::getDeclassificationDate()
{
    return nitf::Field(getNativeOrThrow()->declassificationDate);
}

nitf::Field FileSecurity::getDeclassificationExemption()
{
    return nitf::Field(getNativeOrThrow()->declassificationExemption);
}

nitf::Field FileSecurity::getDowngrade()
{
    return nitf::Field(getNativeOrThrow()->downgrade);
}

nitf::Field FileSecurity::getDowngradeDateTime()
{
    return nitf::Field(getNativeOrThrow()->downgradeDateTime);
}

nitf::Field FileSecurity::getClassificationText()
{
    return nitf::Field(getNativeOrThrow()->classificationText);
}

nitf::Field FileSecurity::getClassificationAuthorityType()
{
    return nitf::Field(getNativeOrThrow()->classificationAuthorityType);
}

nitf::Field FileSecurity::getClassificationAuthority()
{
    return nitf::Field(getNativeOrThrow()->classificationAuthority);
}

nitf::Field FileSecurity::getClassificationReason()
{
    return nitf::Field(getNativeOrThrow()->classificationReason);
}

nitf::Field FileSecurity::getSecuritySourceDate()
{
    return nitf::Field(getNativeOrThrow()->securitySourceDate);
}

nitf::Field FileSecurity::getSecurityControlNumber()
{
    return nitf::Field(getNativeOrThrow()->securityControlNumber);
}

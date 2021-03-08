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

#include <nitf/IOHandle.hpp>

#include "gsl/gsl.h"

namespace nitf
{
IOHandle::IOHandle(const std::string& fname,
                   nitf::AccessFlags access,
                   nitf::CreationFlags creation) :
    IOInterface(open(fname.c_str(), access, creation))
{
    setManaged(false);
}

IOHandle::IOHandle(const char* fname,
                   nitf::AccessFlags access,
                   nitf::CreationFlags creation) :
    IOInterface(open(fname, access, creation))
{
    setManaged(false);
}

nitf_IOInterface*
IOHandle::open(const char* fname,
               nitf::AccessFlags access,
               nitf::CreationFlags creation)
{
    nitf_Error error{};
    const auto access_ = gsl::narrow_cast<int>(access);
    const auto creation_ = gsl::narrow_cast<int>(creation);
    nitf_IOInterface* const ioInterface =
            nitf_IOHandleAdapter_open(fname, access_, creation_, &error);

    if (!ioInterface)
    {
        throw nitf::NITFException(&error);
    }

    return ioInterface;
}
}

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

#ifndef __NITF_IOHANDLE_HPP__
#define __NITF_IOHANDLE_HPP__

#include "nitf/NITFException.hpp"
#include "nitf/System.hpp"
#include "nitf/IOInterface.hpp"
#include <string>

/*!
 * \file IOHandle.hpp
 * \brief Contains wrapper implementation for IOHandle
 */

namespace nitf
{

/*!
 *  \class IOHandle
 *  \brief The C++ wrapper of the nitf_IOHandle
 */
class IOHandle : public IOInterface
{

public:

    /*!
     * Default constructor. Only here for compatibility purposes.
     * Use the other constructor
     */
    explicit IOHandle()
    {
    }

    explicit IOHandle(const std::string& fname, nitf::AccessFlags access =
            NITF_ACCESS_READONLY, nitf::CreationFlags creation =
            NITF_OPEN_EXISTING) throw (nitf::NITFException)
    {
        create(fname, access, creation);
    }

    explicit IOHandle(const char* fname, nitf::AccessFlags access =
            NITF_ACCESS_READONLY, nitf::CreationFlags creation =
            NITF_OPEN_EXISTING) throw (nitf::NITFException)
    {
        std::string fName(fname);
        create(fName, access, creation);
    }

    ~IOHandle()
    {
    }

    void create(const std::string& fname, nitf::AccessFlags access =
            NITF_ACCESS_READONLY, nitf::CreationFlags creation =
            NITF_OPEN_EXISTING) throw (nitf::NITFException)
    {
        nitf_IOInterface *interface = nitf_IOHandleAdapter_open(fname.c_str(),
                                                                access,
                                                                creation,
                                                                &error);
        if (!interface)
            throw nitf::NITFException(&error);
        setNative(interface);
        setManaged(false);
    }

};

}
#endif

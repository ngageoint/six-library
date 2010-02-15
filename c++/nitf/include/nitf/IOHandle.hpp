/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
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
#include "nitf/IOHandle.h"
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
class IOHandle: public NativeIOInterface
{

private:
    nitf_IOHandle handle;
    nitf_Error error;
    bool mAutoClose; //set if you don't want it to auto-close on desruct

public:

    /*!
     * Default constructor. Only here for compatibility purposes.
     * Use the other constructor
     */
    explicit IOHandle() :
        handle(NITF_INVALID_HANDLE_VALUE), mAutoClose(false)
    {
    }

    explicit IOHandle(const std::string& fname, nitf::AccessFlags access =
            NITF_ACCESS_READONLY, nitf::CreationFlags creation =
            NITF_OPEN_EXISTING) throw (nitf::NITFException) :
        mAutoClose(false)
    {
        create(fname, access, creation);
    }

    explicit IOHandle(const char* fname, nitf::AccessFlags access =
            NITF_ACCESS_READONLY, nitf::CreationFlags creation =
            NITF_OPEN_EXISTING) throw (nitf::NITFException) :
        mAutoClose(false)
    {
        std::string fName(fname);
        create(fName, access, creation);
    }

    ~IOHandle()
    {
        if (mAutoClose)
            close();
        if (getNative())
            delete getNative();
    }

    //!  Get native object
    nitf_IOHandle getHandle()
    {
        return handle;
    }

    void create(const std::string& fname, nitf::AccessFlags access =
            NITF_ACCESS_READONLY, nitf::CreationFlags creation =
            NITF_OPEN_EXISTING) throw (nitf::NITFException)
    {
        handle = nitf_IOHandle_create(fname.c_str(), access, creation, &error);
        if (NITF_INVALID_HANDLE(handle))
            throw nitf::NITFException(&error);

        /* now, we must adapt this IOHandle to fit into the IOInterface */
        /* get a nitf_IOInterface* object... */
        nitf_IOInterface *interface = nitf_IOHandleAdaptor_construct(handle,
                &error);
        if (!interface)
            throw nitf::NITFException(&error);
        setNative(interface);
    }

    void setManualClose(bool manualClose)
    {
        mAutoClose = !manualClose;
    }
    void setAutoClose(bool autoClose)
    {
        mAutoClose = autoClose;
    }

};

}
#endif

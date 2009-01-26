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
class IOHandle
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
    explicit IOHandle() : handle(NITF_INVALID_HANDLE_VALUE), mAutoClose(false) {}

    explicit IOHandle(const std::string& fname,
             nitf::AccessFlags access = NITF_ACCESS_READONLY,
             nitf::CreationFlags creation = NITF_OPEN_EXISTING) throw(nitf::NITFException)
            : mAutoClose(false)
    {
        create(fname, access, creation);
    }

    explicit IOHandle(const char* fname,
             nitf::AccessFlags access = NITF_ACCESS_READONLY,
             nitf::CreationFlags creation = NITF_OPEN_EXISTING) throw(nitf::NITFException)
            : mAutoClose(false)
    {
        std::string fName(fname);
        create(fName, access, creation);
    }

    //!  Construct from native object
    explicit IOHandle(nitf_IOHandle n) : mAutoClose(false)
    {
        handle = n;
    }

    ~IOHandle()
    {
        if (mAutoClose) close();
    }

    //!  Get native object
    nitf_IOHandle getHandle() { return handle; }

    void create(const std::string& fname,
                nitf::AccessFlags access = NITF_ACCESS_READONLY,
                nitf::CreationFlags creation = NITF_OPEN_EXISTING)
    throw(nitf::NITFException)
    {
        handle = nitf_IOHandle_create(fname.c_str(), access, creation, &error);
        if (NITF_INVALID_HANDLE(handle))
            throw nitf::NITFException(&error);
    }

    void read(char * buf, size_t size)
    {
        int x = nitf_IOHandle_read(handle, buf, size, &error);
        if (!x)
            throw nitf::NITFException(&error);
    }

    void write(const char * buf, size_t size) throw(nitf::NITFException)
    {
        int x = nitf_IOHandle_write(handle, buf, size, &error);
        if (!x)
            throw nitf::NITFException(&error);
    }

    off_t seek(off_t offset, int whence) throw(nitf::NITFException)
    {
        off_t x = nitf_IOHandle_seek(handle, offset, whence, &error);
        if (!NITF_IO_SUCCESS(x))
            throw nitf::NITFException(&error);
        else
            return x;
    }

    off_t tell() throw(nitf::NITFException)
    {
        off_t x = nitf_IOHandle_tell(handle, &error);
        if (!NITF_IO_SUCCESS(x))
            throw nitf::NITFException(&error);
        else
            return x;
    }

    off_t getSize() throw(nitf::NITFException)
    {
        off_t x = nitf_IOHandle_getSize(handle, &error);
        if (!NITF_IO_SUCCESS(x))
            throw nitf::NITFException(&error);
        else
            return x;
    }

    void setManualClose(bool manualClose) { mAutoClose = !manualClose; }
    void setAutoClose(bool autoClose) { mAutoClose = autoClose; }
    bool isValid() { return !NITF_INVALID_HANDLE(handle) ? true: false; }

    //! Close the handle
    void close()
    {
        if (isValid())
            nitf_IOHandle_close(handle);
        handle = NITF_INVALID_HANDLE_VALUE;
    }

};

}
#endif

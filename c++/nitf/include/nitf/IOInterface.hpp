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

#ifndef __NITF_IO_INTERFACE_HPP__
#define __NITF_IO_INTERFACE_HPP__

#include "nitf/System.hpp"
#include "nitf/Object.hpp"
#include "nitf/IOInterface.h"
#include <string>

extern "C"
{
    NITF_BOOL __nitf_IOInterfaceImpl_read(NITF_DATA* data,
            char* buf, size_t size, nitf_Error* error);

    NITF_BOOL __nitf_IOInterfaceImpl_write(NITF_DATA* data,
            const char* buf, size_t size, nitf_Error* error);

    nitf::Off __nitf_IOInterfaceImpl_seek(NITF_DATA* data,
            nitf::Off offset, int whence, nitf_Error* error);

    nitf::Off __nitf_IOInterfaceImpl_tell(NITF_DATA* data,
                                          nitf_Error* error);

    nitf::Off __nitf_IOInterfaceImpl_getSize(NITF_DATA* data,
                                             nitf_Error* error);

    NITF_BOOL __nitf_IOInterfaceImpl_close(NITF_DATA* data,
                                           nitf_Error* error);

    void __nitf_IOInterfaceImpl_destruct(NITF_DATA* data);
}
/*!
 *  \file IOInterface.hpp
 */
namespace nitf
{

/*!
 *  \class IOInterface
 *  \brief  The C++ wrapper for the nitf_IOInterface
 */
DECLARE_CLASS(IOInterface)
{
public:

    //! Copy constructor
    IOInterface(const IOInterface & x)
    {
        setNative(x.getNative());
    }

    //! Assignment Operator
    IOInterface & operator=(const IOInterface & x)
    {
        if (&x != this)
            setNative(x.getNative());
        return *this;
    }

    // Set native object
    IOInterface(nitf_IOInterface * x)
    {
        setNative(x);
        getNativeOrThrow();
    }

    //! Destructor
    virtual ~IOInterface(){}

    virtual void read(char * buf, size_t size) {}

    virtual void write(const char * buf, size_t size) throw(nitf::NITFException) {}

    virtual nitf::Off seek(nitf::Off offset, int whence) throw(nitf::NITFException) { return -1; }

    virtual nitf::Off tell() throw(nitf::NITFException) { return -1; }

    virtual nitf::Off getSize() throw(nitf::NITFException) { return -1; }

    virtual void close() {}


protected:

    //! Get the native data source interface
    nitf_IIOInterface getIOInterfaceImpl()
    {
        nitf_IIOInterface iIOHandle =
        {
            &__nitf_IOInterfaceImpl_read,
            &__nitf_IOInterfaceImpl_write,
            &__nitf_IOInterfaceImpl_seek,
            &__nitf_IOInterfaceImpl_tell,
            &__nitf_IOInterfaceImpl_getSize,
            &__nitf_IOInterfaceImpl_close,
            &__nitf_IOInterfaceImpl_destruct,
        };
        return iIOHandle;
    }

    //! Constructor
    IOInterface() throw (nitf::NITFException);

    nitf_Error error;
};



/*!
 *  \class NativeIOInterface
 *  The Native wrapper for the IOInterface.
 */
class NativeIOInterface : public IOInterface
{
public:
    // Set native object
    NativeIOInterface(nitf_IOInterface * x) : IOInterface(x) {}

    NativeIOInterface() : IOInterface() {}

    virtual ~NativeIOInterface() {}

    virtual void read(char * buf, size_t size);

    virtual void write(const char * buf, size_t size) throw(nitf::NITFException);

    virtual nitf::Off seek(nitf::Off offset, int whence) throw(nitf::NITFException);

    virtual nitf::Off tell() throw(nitf::NITFException);

    virtual nitf::Off getSize() throw(nitf::NITFException);

    virtual void close();
};

}
#endif

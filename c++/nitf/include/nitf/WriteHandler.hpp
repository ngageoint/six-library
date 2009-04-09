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

#ifndef __NITF_WRITE_HANDLER_HPP__
#define __NITF_WRITE_HANDLER_HPP__

#include "nitf/WriteHandler.h"
#include "nitf/StreamIOWriteHandler.h"
#include "nitf/System.hpp"
#include "nitf/Object.hpp"
#include "nitf/IOInterface.hpp"
#include <string>

/*!
 *  \file WriteHandler.hpp
 *  \brief  Contains wrapper implementations for WriteHandlers
 */
namespace nitf
{

/*!
 *  \class WriteHandler
 *  \brief  The C++ wrapper for the nitf_WriteHandler
 */
DECLARE_CLASS(WriteHandler)
{
public:

    //! Copy constructor
    WriteHandler(const WriteHandler & x)
    {
        setNative(x.getNative());
    }

    //! Assignment Operator
    WriteHandler & operator=(const WriteHandler & x)
    {
        if (&x != this)
            setNative(x.getNative());
        return *this;
    }

    // Set native object
    WriteHandler(nitf_WriteHandler * x)
    {
        setNative(x);
        getNativeOrThrow();
    }

    //! Destructor
    virtual ~WriteHandler(){}

    /*!
     *  Write to the given output IOInterface
     *  \param handle   the output IOInterface
     */
    virtual void write(IOInterface& handle) throw (nitf::NITFException){}

private:

    //! Allows the engine to call the read function for this object
    static NITF_BOOL WriteHandler_write(NITF_DATA * data,
                                        nitf_IOInterface* io,
                                        nitf_Error *error);

    //! Needed for the engine interface
    static void WriteHandler_destruct(NITF_DATA* data);

protected:

    //! Get the native data source interface
    nitf_IWriteHandler getIWriteHandler()
    {
        // Create a data source interface that
        // the native layer can use
        nitf_IWriteHandler WriteHandler =
        {
            &WriteHandler_write,
            &WriteHandler_destruct,
        };
        return WriteHandler;
    }

    //! Constructor
    WriteHandler() throw (nitf::NITFException);

    nitf_Error error;
};



/*!
 *  \class KnownWriteHandler
 *  \brief  The base class for known data sources in the engine.
 */
class KnownWriteHandler : public WriteHandler
{
public:
    //! Constructor
    KnownWriteHandler(): WriteHandler(), mData(NULL), mIface(NULL){}
    //! Destructor
    virtual ~KnownWriteHandler() {}

    /*!
     *  Write data to the given IOInterface
     *  \param handle   The output IOInterface
     */
    virtual void write(IOInterface& handle) throw (nitf::NITFException);

protected:
    NITF_DATA * mData;
    nitf_IWriteHandler * mIface;
};


/*!
 *  \class StreamIOWriteHandler
 *  \brief  Write handler that streams from an input source.
 */
class StreamIOWriteHandler : public KnownWriteHandler
{
public:
    //! Constructor
    StreamIOWriteHandler(IOInterface& sourceHandle, nitf::Uint64 offset,
            nitf::Uint64 bytes);
    ~StreamIOWriteHandler() {}
};


}
#endif

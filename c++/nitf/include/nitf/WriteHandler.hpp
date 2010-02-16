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
#include "nitf/SegmentReader.hpp"
#include <string>

extern "C"
{
    //! Allows the engine to call the read function for this object
    NITF_BOOL __nitf_WriteHandler_write(NITF_DATA * data, 
                                        nitf_IOInterface* io,
                                        nitf_Error *error);
  
    //! Needed for the engine interface
    void __nitf_WriteHandler_destruct(NITF_DATA* data);
}

/*!
 *  \file WriteHandler.hpp
 *  \brief  Contains wrapper implementations for WriteHandlers
 */
namespace nitf
{

/*!
 *  \class WriteHandler
 *  \brief  The C++ wrapper for the nitf_WriteHandler
 *
 *  All WriteHandlers are treated as pointers in the nitf system.
 */
class WriteHandler
{
public:

    //! Destructor
    virtual ~WriteHandler()
    {
    }

    /*!
     *  Write to the given output IOInterface
     *  \param handle   the output IOInterface
     */
    virtual void write(IOInterface& handle) throw (nitf::NITFException) = 0;

    virtual nitf_WriteHandler* getNative() const
    {
        return cppWriteHandler;
    }

private:


protected:

    //! Get the native data source interface
    nitf_IWriteHandler getIWriteHandler()
    {
        // Create a data source interface that
        // the native layer can use
        nitf_IWriteHandler WriteHandler = 
            { 
                &__nitf_WriteHandler_write,
                &__nitf_WriteHandler_destruct
            };
        return WriteHandler;
    }

    //! Constructor
    WriteHandler() throw (nitf::NITFException);

    nitf_Error error;
    nitf_WriteHandler *cppWriteHandler;
};

/*!
 *  \class KnownWriteHandler
 *  \brief  The base class for known data sources in the engine.
 */
class KnownWriteHandler: public WriteHandler
{
public:
    //! Constructor
    KnownWriteHandler() :
        WriteHandler(), knownHandler(NULL)
    {
    }

    //! Destructor
    virtual ~KnownWriteHandler()
    {
        //we don't destroy the native known WriteHandler - the Writer
        //destroys attached WriteHandlers - if you by chance don't attach
        //the handler to a Writer, you can call:
        //nitf_WriteHandler_destruct(handler->getNative());
        //but, we do need to delete the cppWriteHandler since it is not the
        //native one that DOES get deleted by the Writer
        if (cppWriteHandler)
            nitf_WriteHandler_destruct(&cppWriteHandler);
    }

    /*!
     *  Write data to the given IOInterface
     *  \param handle   The output IOInterface
     */
    virtual void write(IOInterface& handle) throw (nitf::NITFException);

    nitf_WriteHandler* getNative() const
    {
        return knownHandler;
    }

protected:
    nitf_WriteHandler *knownHandler;

    void setKnownHandler(nitf_WriteHandler *handler)
            throw (nitf::NITFException);
};

/*!
 *  \class StreamIOWriteHandler
 *  \brief  Write handler that streams from an input source.
 */
class StreamIOWriteHandler: public KnownWriteHandler
{
public:
    //! Constructor
    StreamIOWriteHandler(IOInterface& sourceHandle, nitf::Uint64 offset,
            nitf::Uint64 bytes);
    ~StreamIOWriteHandler()
    {
    }
};

/**
 *  \class SegmentWriteHandler
 *  \brief  Write handler that streams from a SegmentReader.
 */
class SegmentWriteHandler: public KnownWriteHandler
{
public:
    SegmentWriteHandler(SegmentReader reader) :
        mReader(reader)
    {
    }

    virtual ~SegmentWriteHandler()
    {
    }

    void write(IOInterface& handle) throw (nitf::NITFException);

protected:
    SegmentReader mReader;
    static const size_t BLOCK_SIZE = 8192;
};

}
#endif

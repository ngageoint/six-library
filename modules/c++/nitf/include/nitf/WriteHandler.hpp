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

#ifndef __NITF_WRITE_HANDLER_HPP__
#define __NITF_WRITE_HANDLER_HPP__

#include "nitf/System.hpp"
#include "nitf/StreamIOWriteHandler.h"
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
 *
 *  All WriteHandlers are treated as pointers in the nitf system.
 */
DECLARE_CLASS(WriteHandler)
{
public:
    WriteHandler(const WriteHandler & x);
    WriteHandler & operator=(const WriteHandler & x);

    // Set native object
    WriteHandler(nitf_WriteHandler* x);

    ~WriteHandler() = default;

    /*!
     *  Write to the given output IOInterface
     *  \param handle   the output IOInterface
     */
    virtual void write(IOInterface& handle);

protected:
    //! Constructor
    WriteHandler() = default;

    nitf_Error error{};
};


/*!
 *  \class StreamIOWriteHandler
 *  \brief  Write handler that streams from an input source.
 */
class StreamIOWriteHandler : public WriteHandler
{
public:
    //! Constructor
    StreamIOWriteHandler(IOInterface& sourceHandle, uint64_t offset,
            uint64_t bytes);
    ~StreamIOWriteHandler()
    {
    }
};

}
#endif

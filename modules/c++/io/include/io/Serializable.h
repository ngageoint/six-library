/* =========================================================================
 * This file is part of io-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * io-c++ is free software; you can redistribute it and/or modify
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
 * License along with this program; If not, 
 * see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef __IO_SERIALIZABLE_H__
#define __IO_SERIALIZABLE_H__

#include "io/InputStream.h"
#include "io/OutputStream.h"

/*!
 *  \file Serializable.h
 *  \brief File containing the interface for serializable objects
 *
 *  A serializable is an object that can be written to and from a stream.
 *  This stream could be a socket, a file, a memory image -- even a url.
 *  This object has a known derived type SOAPMessage, which is concretely
 *  defined to handle SOAP objects
 */
namespace io
{
/*!
 * \class Serializable
 * \brief Provide the interface for serializable objects
 *
 *  A serializable is an object that can be written to and from a stream.
 *  This stream could be a socket, a file, a memory image -- even a url.
 *  This object has a known derived type SOAPMessage, which is concretely
 *  defined to handle SOAP objects
 */
class Serializable
{
public:
    //! Default constructor
    Serializable()
    {
    }
    //! Deconstructor
    virtual ~Serializable()
    {
    }

    /*!
     * Transfer this object into a byte stream
     */
    virtual void serialize(io::OutputStream& os) = 0;

    /*!
     * Unpack this input stream to the object
     * \param is  Stream to read object from
     */
    virtual void deserialize(io::InputStream& is) = 0;

};
}

#endif

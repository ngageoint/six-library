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

#ifndef __DBG_STREAM_H__
#define __DBG_STREAM_H__

#include <memory>

#include "io/OutputStream.h"
#include "mem/SharedPtr.h"


/*!
 *  \file DbgStream.h
 *  \brief This is the class for debugging with streams
 *   
 *  This class should be used by high-level entities for defining
 *  degug streams.  If the stream is "on", printing will occur to a bound
 *  OutputStream (Impl style).  Otherwise, nothing will occur
 *
 */

namespace io
{
/*!
 *  \class DbgStream
 *  \brief A class for printing to a debug stream
 *
 *  This class should be used by high-level entities for defining
 *  degug streams.  If the stream is "on", printing will occur to a bound
 *  OutputStream (Impl style).  Otherwise, nothing will occur
 *
 */

struct DbgStream : public OutputStream
{
    DbgStream() = default;
    virtual ~DbgStream() = default;

    /*!
     *  Alternate constructor
     *  \param s An output stream to adopt
      *  \param on The state of the stream (on or off)
     */
    DbgStream(OutputStream* s, bool on = false)
    {
        set(s, on);
    }

    using OutputStream::write;

    /*!
     *  Set the output and debug stream
     *  \param s An output stream to adopt
      *  \param on The state of the stream (on or off)
     */
    virtual void set(OutputStream* s, bool on = false)
    {
        mOn = on;
        mStream.reset(s);
    }

    /*!
     * This method uses the bound OutputStream to print,
     * if the switch is on.
     * \param buffer The byte array to write to the stream
     * \param len The length
     * \throw IOException
     */
    virtual void write(const void* buffer, sys::Size_T len)
    {
        if (mOn)
        {
            mStream->write(buffer, len);
        }
    }

    /*!
     *  Is the stream on or off??
     *  \return The state of the stream
     */
    bool isOn()
    {
        return mOn;
    }

    /*!
     *  Set the state of the stream
     *  \param on  The new state of the stream
     */
    void setDebug(bool on)
    {
        mOn = on;
    }
protected:
    //!  The bound stream
    mem::auto_ptr<OutputStream> mStream;
    //!  On or off??
    bool mOn = false;
};
}
#endif

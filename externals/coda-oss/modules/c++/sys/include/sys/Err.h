/* =========================================================================
 * This file is part of sys-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * sys-c++ is free software; you can redistribute it and/or modify
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


#ifndef __SYS_ERR_H__
#define __SYS_ERR_H__

/*!
 *  \file Err.h
 *  \brief Errno like object
 *
 *  This object is very much like errno or the GetLastError() function
 *  in Win32.  It doesnt do anything dazzling.  It just reports the
 *  last error.  This class is sometimes useful, because it understands
 *  that its error id maps to a system string error, and it knows how
 *  to obtain the error
 */

#include "sys/Conf.h"
#include <string.h>

#include "config/Exports.h"

namespace sys
{
//!  The default value for the Err object.  Maps to errno or GetLastError()
constexpr static int __last_err__ = 0;
/*!
 *  \class Err
 *  \brief Errno like object
 *
 *  This object is very much like errno or the GetLastError() function
 *  in Win32.  It doesnt do anything dazzling.  It just reports the
 *  last error.  This class is sometimes useful, because it understands
 *  that its error id maps to a system string error, and it knows how
 *  to obtain the error.
 *
 */

struct CODA_OSS_API Err
{
    /*!
     * Copy constructor
     * \param err The err to take
     */
    Err(const Err& err)
    {
        mErrId = err.getErrID();
    }

    /*!
     * Constructor from int error id
     * \param errNum  The error to initialize with. Defaults to last
     */
    Err() = default;
    Err(int errNum)
    {
        setThis(errNum);
    }
    virtual ~Err() = default;

    /*!
     *  Assignment operator
     *  \param err The err to take
     */
    Err& operator=(const Err& err)
    {
        if (&err != this)
        {
            mErrId = err.getErrID();
        }
        return *this;
    }

    /*!
     *  This is the equivalent of strerror, done in a cross-platform
     *  manner, wrapped in this class.  Prints this object to
     *  its error string
     *  \return a string representation of this error
     *
     */
    virtual std::string toString() const;

    /*!
     *  Set method
     *  \param i An int to initialize from
     */
    void setThis(int i = __last_err__)
    {
        if (i == __last_err__)
        {
            mErrId = getLast();
        }
    }

    //!  Return the last error
    virtual int getLast() const;

    int getErrID() const { return mErrId; }

protected:

    int mErrId = __last_err__;
};

/*!
 *  \class SocketErr
 *  \brief Specialization to handle those weird winsock/bsd errors
 *
 *  The same operations as in Err for sockets
 *
 */
struct SocketErr : public Err
{
    /*!
     *  Copy constructor.  Takes a right-hand-side
     *  \param err An error to initialize from
     *
     */
    SocketErr(const SocketErr& err) :
        Err(err.getErrID())
    {
    }

    /*!
     *  Constructor
     *  \param errNum  An int to initialize from
     *
     */
    SocketErr() = default;
    SocketErr(int errNum)
    {
        setThis(errNum);
    }
    virtual ~SocketErr() = default;

    /*!
     *  Assignment operator
     *  \param err The err to take
     *
     */
    SocketErr& operator=(const SocketErr& err)
    {
        if (&err != this)
        {
            mErrId = err.getErrID();
        }
        return *this;
    }

    //!  Redefined for socket errors
    virtual int getLast() const;

};

}

#endif

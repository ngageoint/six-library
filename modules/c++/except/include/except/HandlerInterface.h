/* =========================================================================
 * This file is part of except-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
 *
 * except-c++ is free software; you can redistribute it and/or modify
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

#ifndef __HANDLER_INTERFACE_H__
#define __HANDLER_INTERFACE_H__

#include "except/Context.h"
#include "except/Error.h"
#include "except/Exception.h"

/*!
 *  \file HandlerInterface.h
 *  \brief This is a class to handle a message in some time and place
 *  It takes a Context for every function, which is usually provided via
 *  macros, and an Exception, Error, status message or warning message.
 *  Here, we simply provide the abstract interface
 *
 *
 */
namespace except
{
/*!
 *  \class HandlerInterface
 *  \brief A class for handling notification events
 *  While this class used to service only Throwable events, it was 
 *  extended to provide a status message and a warning message.
 *  These events propogate to the HandlerInterface, usual via macro,
 *  and any derivation of this interface will define how to handle
 *  this notification event.  The default implementation, used by
 *  ExceptionFactory if nothing is provided simply throws the error
 *  or exception.
 */
class HandlerInterface
{
public:
    //!  Constructor
    HandlerInterface()
    {}
    //!  Destructor
    virtual ~HandlerInterface()
    {}

    /*!
     *  Given an error, and a context in which it was produced, handle
     *  the event
     *  \param c The context in which the error was produced
     *  \param e The error which was generated
     */
    virtual void onRaise(Context c, const Error& e) = 0;

    /*!
     *  Given an exception, and a context in which it was produced, handle
     *  the event
     *  \param c The context in which the exception was produced
     *  \param e The exception which was generated
     */
    virtual void onRaise(Context c, const Exception& e) = 0;

    /*!
     *  Given an status, and a context in which it was produced, handle
     *  the event.
     *  \param c The context in which the status was produced
     *  \param status The status which was generated
     */
    virtual void onStatus(Context c, const std::string& status) = 0;

    /*!
     *  Given an warning, and a context in which it was produced, handle
     *  the event.
     *  \param c The context in which the warning was produced
     *  \param warning The warning which was generated
     */
    virtual void onWarning(Context c, const std::string& warning) = 0;

    /*!
     *  Given a debug statement, and a context in which it was produced,
     *  handle the event.
     *  \param c  The context in which the debug statement was produced
     *  \param dbg  The debug statement that was generated
     */
    virtual void onDebug(Context c, const std::string& dbg) = 0;

};
}


#endif

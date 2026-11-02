/* =========================================================================
 * This file is part of net-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * net-c++ is free software; you can redistribute it and/or modify
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

#ifndef __NET_REQUEST_HANDLER_H__
#define __NET_REQUEST_HANDLER_H__

#include "net/NetConnection.h"

namespace net
{
/*!
 *  \class RequestHandler
 *  \brief Abstract class (function object) to handle a NetConnection
 *
 *  This class should be overloaded by the server developer in order
 *  to provide an activity to be performed over the connection
 *
 *
 */
class RequestHandler
{
public:
    //!  Constructor
    RequestHandler()
    {
    }

    //!  Destructor
    virtual ~RequestHandler()
    {
    }

    /*!
     *  Operation for handling a connection.  Overload this with
     *  a handler.
     *
     *  \param conn The connection
     */
    virtual void operator()(net::NetConnection* conn) = 0;
};

class RequestHandlerFactory
{
public:
    RequestHandlerFactory()
    {
    }
    virtual ~RequestHandlerFactory()
    {
    }
    virtual net::RequestHandler* create() = 0;
};

template<typename ReqHandler_T> class DefaultRequestHandlerFactory: public RequestHandlerFactory
{
public:
    DefaultRequestHandlerFactory()
    {
    }
    virtual ~DefaultRequestHandlerFactory()
    {
    }
    net::RequestHandler* create() override
    {
        return new ReqHandler_T();
    }
};

}

#endif

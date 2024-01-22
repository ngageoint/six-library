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

#ifndef __NET_ALLOC_STRATEGY_H__
#define __NET_ALLOC_STRATEGY_H__

#include "net/RequestHandler.h"

namespace net
{

/*!
 *  \class AllocStrategy
 *  \brief Creates request handlers, abstracting any parallelization
 *  mechansims
 *
 *  This class is used by a NetConnection server to broker off a request
 *  either to other threads, or possibly through other mechanisms.
 *  Single-threaded requests are also possible, in which case the
 *  server would only handle a request at a time and accept() would
 *  not be called on and new connections until the last request.
 *
 *  The utility here is that it separates the server details (handling
 *  sockets) from how requests are processed, and also separates this
 *  allocation strategy from what processing is actually done (which is
 *  up to the writer of the request handler.
 *
 *  This class prefers a very specific interface to arguments passed into
 *  constructors, so that a "builder" pattern can easily construct the
 *  appropriate strategy at runtime, perhaps from a configuration file.
 *  (e.g. <alloc-strategy type="ThreadPool">10</alloc-strategy> vs,
 *        <alloc-strategy type="SingleThreaded"/>
 *  )
 *
 *  In order to fulfill its duties, the alloc strategy is given
 *  1) A factory to come up with the request handler needed when a request
 *     comes in
 *
 *  2) Absolute discretion as to how it uses its factory to produce
 *     handlers
 */
class AllocStrategy
{
public:

    /*!
     *  Constructor.  Set our internal factory to NULL.
     *
     */
    AllocStrategy() :
        mRequestHandlerFactory(nullptr)
    {
    }

    //! Destructor
    virtual ~AllocStrategy()
    {
        if (mRequestHandlerFactory)
            delete mRequestHandlerFactory;
    }

    /*!
     *  Allow the caller (builder?) to give us the blueprint for
     *  handling a request.  When and how we produce this request
     *  is our derived class' responsibility
     * 
     *  \param factory A request handler producer
     */
    virtual void setRequestHandlerFactory(RequestHandlerFactory* factory)
    {
        mRequestHandlerFactory = factory;
    }

    /*!
     *  This is a hook that allows our derived classes one place
     *  for initialization (which prevents them from needing 
     *  to use static variables in handleRequest to persist info from
     *  the first call.  This SHOULD NOT be called until the 
     *  request handler
     *  factory is set.
     */
    virtual void initialize() = 0;

    /*!
     *  Handle an incoming connection.  This is done different ways
     *  depending on the type of derived class.  For example, in a thread
     *  pool, we just add this connection to a synchronized queue.  If
     *  its a single thread strategy, though, this function will block
     *  the caller until the request is satisfied.
     * 
     *  \param conn The connection
     */
    virtual void handleConnection(net::NetConnection* conn) = 0;
protected:
    RequestHandlerFactory* mRequestHandlerFactory;
};

}

#endif

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

#ifndef __NET_PER_REQUEST_THREAD_ALLOC_STRATEGY_H__
#define __NET_PER_REQUEST_THREAD_ALLOC_STRATEGY_H__

#include "sys/Thread.h"
#include "net/RequestHandler.h"
#include "net/AllocStrategy.h"


namespace net
{


/*!
 *  \class RequestHandlerThread
 *  \brief The thread for handling request in our per-request strategy
 *
 *  This class simply overrides the Thread class, providing a 
 *  one-to-one function object for handling a thread.  The thread
 *  then exits after handling the connection.
 *
 *  Note that in most cases, a thread pool implementation will be
 *  more efficient.
 *
 */

class RequestHandlerThread : public sys::Thread
{
    RequestHandler* mRequestHandler;
    NetConnection* mConnection;
public:
    /*!
     *  Constructor
     *  \param handler A (freshly created) request handler
     *  \param connection The network socket connection
     */
    RequestHandlerThread(RequestHandler* handler,
                         NetConnection* connection) :
            mRequestHandler(handler), mConnection(connection)
    {}

    //!  Destructor
    ~RequestHandlerThread()
    {}

    /*!
     *  Overloaded run method for handling a connection.
     * 
     */
    void run() override
    {
        (*mRequestHandler)(mConnection);
        delete mRequestHandler;
    }

};

/*!
 *  \class PerRequestThreadAllocStrategy
 *  \brief Provides a per-thread allocation strategy (one-thread-per-req)
 *
 *  This class offers a multi-threaded allocation strategy where each
 *  request is offered its own thread.
 *
 */
class PerRequestThreadAllocStrategy : public AllocStrategy
{
public:
    //!  Constructor
    PerRequestThreadAllocStrategy()
    {}
    //!  Destructor
    ~PerRequestThreadAllocStrategy()
    {}

    void initialize() override
    {
    }

    /*!
     *  Handle a request from a connection.
     * 
     *  \param conn The network connection
     */
    void handleConnection(NetConnection* conn) override;
};

/*     class ThreadPoolAllocStrategy : public AllocStrategy */
/*     { */
/*     public: */

/*  ThreadPoolAllocStrategy() */
/*  { */


/*  } */

/*  ~ThreadPoolAllocStrategy() {} */

/*  void initialize() */
/*  { */
/*      mThreadPool = new RequestHandlerThreadPool(mRequestHandlerFactory); */
/*  } */

/*  void handleRequest(net::NetConnection* conn) */
/*  { */
/*      mThreadPool.addRequest(conn); */
/*  } */
/*     protected: */
/*  RequestHandlerThreadPool* mThreadPool; */
/*     }; */


}

#endif

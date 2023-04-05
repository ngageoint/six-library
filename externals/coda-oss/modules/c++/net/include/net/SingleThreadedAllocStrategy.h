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

#ifndef __NET_SINGLE_THREADED_ALLOC_STRATEGY_H__
#define __NET_SINGLE_THREADED_ALLOC_STRATEGY_H__

#include "net/AllocStrategy.h"
#include <memory>

namespace net
{

/*!
 *  \class SingleThreadedAllocStrategy
 *  \brief single-threaded allocation strategy
 *
 *  A single-threaded allocation strategy for processing
 *  requests.  Re-uses the same handler for each request.
 *
 */
class SingleThreadedAllocStrategy: public AllocStrategy
{
public:

    //!  Constructor
    SingleThreadedAllocStrategy() :
        mHandler(nullptr)
    {
    }

    //!  Destructor
    ~SingleThreadedAllocStrategy()
    {
        if (mHandler)
            delete mHandler;
    }

    /*!
     *  Set the auto ptr.
     *
     */
    void initialize() override
    {
        mHandler = mRequestHandlerFactory->create();
    }

    /*!
     *  Handle the connection.
     *
     *  \param conn The network connection
     */
    void handleConnection(net::NetConnection* conn) override
    {
        (*mHandler)(conn);
    }
private:
    net::RequestHandler* mHandler;
};

}

#endif

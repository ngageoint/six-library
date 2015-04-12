/* =========================================================================
 * This file is part of mt-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
 *
 * mt-c++ is free software; you can redistribute it and/or modify
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

#include "mt/GenericRequestHandler.h"

void mt::GenericRequestHandler::run()
{
    while (true)
    {
        // Pull a runnable off the queue
        sys::Runnable *handler = NULL;
        mRequest->dequeue(handler);
        if (!handler)
            return;

        // Run the runnable that we pulled off the queue
        handler->run();

        // Delete the runnable we pulled off the queue
        delete handler;
    }
}


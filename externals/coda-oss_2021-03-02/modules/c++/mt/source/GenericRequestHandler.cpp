/* =========================================================================
 * This file is part of mt-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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

#include <memory>

#include "mt/GenericRequestHandler.h"

void mt::GenericRequestHandler::run()
{
    while (true)
    {
        // Pull a runnable off the queue
        sys::Runnable* handler = NULL;
        mRequest->dequeue(handler);
        if (!handler)
        {
            return;
        }

        // Run the runnable that we pulled off the queue
        // It will get deleted when it goes out of scope below
        std::unique_ptr<sys::Runnable> scopedHandler(handler);
        scopedHandler->run();
    }
}


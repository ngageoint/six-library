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


#ifndef __MT_THREAD_POOL_EXCEPTION_H__
#define __MT_THREAD_POOL_EXCEPTION_H__

#include "except/Exception.h"

namespace mt
{
struct ThreadPoolException : public except::Exception
{
    ThreadPoolException() = default;

    /*!
     *  User constructor.  Sets the exception message.
     *  \param message the exception message
     */
    ThreadPoolException(const char *message) :
            except::Exception(message)
    {}

    /*!
     *  User constructor.  Sets the exception message.
     *  \param message the exception message
     */
    ThreadPoolException(const std::string& message) :
            except::Exception(message)
    {}

    /*!
     *  User constructor.  Sets the exception context.
     *  \param c the exception context
     */
    ThreadPoolException(const except::Context& c) :
            except::Exception(c)
    {}

    virtual ~ThreadPoolException() = default;
}
;
}

#endif

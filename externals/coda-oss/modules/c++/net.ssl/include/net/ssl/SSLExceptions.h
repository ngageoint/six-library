/* =========================================================================
 * This file is part of net.ssl-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * net.ssl-c++ is free software; you can redistribute it and/or modify
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

#ifndef __NET_SSL_EXCEPTIONS_H__
#define __NET_SSL_EXCEPTIONS_H__

#include "sys/SystemException.h"

/*!
 *  \file SSLExceptions.h
 *  \brief File containing exceptions specialized for net exceptions
 *
 *  This file contains one exception so far:  SSLException
 */
namespace net
{
namespace ssl
{
    /*!
     *  \class SSLException
     *  \brief Specialized exception for SSL
     */
    class SSLException : public sys::SocketException
    {
    public:
	/*!
	 *  Create a new exception
	 *  \param message  The message
	 */
	SSLException(const char *message) : sys::SocketException(message)
	{
	    if (message != nullptr)
	    {
		mMessage = "SSL: ";
		mMessage += message;
	    }
	}

	/*!
	 *  Create a new exception
	 *  \param message  The message
	 */
	SSLException(const std::string& message) : sys::SocketException(message)
	{
	    mMessage = "SSL: ";
	    mMessage += message;
	
	}
	
	/*!
	 *  Create a new exception
	 *  \param c  The context
	 */
	SSLException(const except::Context& c) : sys::SocketException(c)
	{
	    mMessage = "SSL: ";
	    mMessage += c.getMessage();
	}
	
    };
}
}

#endif

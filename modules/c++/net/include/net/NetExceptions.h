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

#ifndef __NET_EXCEPTIONS_H__
#define __NET_EXCEPTIONS_H__

#include "except/Exception.h"

/*!
 *  \file NetExceptions.h
 *  \brief File containing exceptions specialized for net exceptions
 *
 *  This file contains one exception so far:  MalformedURLException
 */
namespace net
{
/*!
 *  \class MalformedURLException
 *  \brief Specialized exception for bad URLs
 *
 *  This class is an extension of the exception library for
 *  poorly formed URLs.  The URL is usually the input, and the
 *  message currently is auto-constructed
 */
class MalformedURLException : public except::Exception
{
public:
    /*!
     *  Create a new exception
     *  \param url An incorrectly formed URL
     */
    MalformedURLException(const char *url): except::Exception(url)
    {
        if (url != nullptr)
        {
            mMessage = "In: ";
            mMessage += url;
        }
    }

    /*!
     *  Create a new exception
     *  \param url An incorrectly formed URL
     */
    MalformedURLException(const std::string& url) : except::Exception(url)
    {
        mMessage = "In: ";
        mMessage += url;

    }

    /*!
     *  Create a new exception
     *  \param c The exception context
     */
    MalformedURLException(const except::Context& c) : except::Exception(c)
    {
        mMessage = "In: ";
        mMessage += c.getMessage();
    }

};
}

#endif

/* =========================================================================
 * This file is part of sio.lite-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * sio.lite-c++ is free software; you can redistribute it and/or modify
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
#ifndef __SIO_LITE_UNSUPPORTED_DATA_TYPE_EXCEPTION_H__
#define __SIO_LITE_UNSUPPORTED_DATA_TYPE_EXCEPTION_H__

#include <import/except.h>


namespace sio
{
namespace lite
{
class UnsupportedDataTypeException : public except::Exception
{
public:
    //!  Default constructor
    UnsupportedDataTypeException() {}

    /*!
     *  Constructor with a rhs message
     *  \param message The message to report
     *
     */
    UnsupportedDataTypeException(const char *message) :
            except::Exception(message) {}

    /*!
     *  Constructor with a rhs message
     *  \param message The message to report
     *
     */
    UnsupportedDataTypeException(const std::string& message)  :
            except::Exception(message) {}

    /*!
     *  Constructor with a context
     *  \param c The context with the message to report
     *
     */
    UnsupportedDataTypeException(const except::Context& c)  :
            except::Exception(c) {}
};
}
}
#endif


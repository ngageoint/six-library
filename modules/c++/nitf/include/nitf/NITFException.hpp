/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2010, General Dynamics - Advanced Information Systems
 *
 * NITRO is free software; you can redistribute it and/or modify
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
 * License along with this program; if not, If not,
 * see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef __NITF_EXCEPTION_HPP__
#define __NITF_EXCEPTION_HPP__

#include <import/except.h>
#include "nitf/System.hpp"

/*!
 *  \file NITFException.hpp
 *  \brief NITFException class for nitf objects
 */
namespace nitf
{
/*!
 *  \class NITFException
 *  \brief  The C++ wrapper for the nitf_Error
 */
class NITFException : public except::Exception
{
public:
    /*!
     *  Construct from native object
     *  \param error  The native nitf_Error object
     */
    NITFException(nitf_Error* error)
    {
        except::Context context(std::string(error->file),
                                error->line,
                                std::string(error->func),
                                std::string(""),
                                std::string(error->message));
        mMessage = context.getMessage();
        mTrace.pushContext( context );
    }
    /*!
     *  Construct from Context
     *  \param c  The Context
     */
    NITFException(const except::Context& c) : except::Exception(c){}

    /*!
     *  Construct from an error message
     *  \param message  The error message
     */
    NITFException(const std::string& message) : except::Exception(message){}

    /*!
     *  Construct from Throwable and Context
     *  \param t  The Throwable
     *  \param c  The Context
     */
    NITFException(const except::Throwable& t, const except::Context& c) :
            except::Exception(t, c){}

    //! Destructor
    virtual ~NITFException(){}

}
;
}
#endif

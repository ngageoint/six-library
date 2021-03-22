/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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
#pragma once

#include <cstddef> // std::nullptr_t

#include "nitf/coda-oss.hpp"
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
class NITFException  /*final*/ : public except::Exception // no "final", SWIG doesn't like it
{
    static except::Context make_Context_(const nitf_Error* error, const std::string& message)
    {
        return except::Context(error->file, error->line, error->func, "", message);
    }
    static except::Context make_Context(const nitf_Error* error)
    {
        return make_Context_(error, error->message);
    }
    static except::Context make_Context(const nitf_Error* error, const std::string& message)
    {
        return make_Context_(error, message + " (" + std::string(error->message) + ")");
    }

    NITFException(const except::Context& context, std::nullptr_t)
    {
        mMessage = context.getMessage();
        mTrace.pushContext(context);
    }

public:
    /*!
     *  Construct from native object
     *  \param error  The native nitf_Error object
     */
    NITFException(const nitf_Error* error) : NITFException(make_Context(error), nullptr)
    {
    }
    /*!
     *  Construct from native object with message
     *  \param error  The native nitf_Error object
     *  \param message  Additional error message
     */
    NITFException(const nitf_Error* error, const std::string& message) : NITFException(make_Context(error, message), nullptr)
    {
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
}
;
}
#endif

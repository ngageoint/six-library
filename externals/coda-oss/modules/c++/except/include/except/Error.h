/* =========================================================================
 * This file is part of except-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * except-c++ is free software; you can redistribute it and/or modify
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

#ifndef __EXCEPT_ERROR_H__
#define __EXCEPT_ERROR_H__

/*!
 *  \file Error.h
 *  \brief Contains the classes to do with error handling
 *
 *  An error is a throwable pertaining to rather serious errors
 *  i.e., ones that could cripple the system if handled improperly.
 *  For that reason, it is worth considering, upon designing a handler
 *  whether or not to simply abort the program upon receiving an error.
 */
#include "Throwable.h"

/*!
 * Useful macro for defining Exception classes
 */
#define DECLARE_EXTENDED_ERROR_(_Name, Error_, _Base) \
  struct _Name##Error_ : public _Base \
  { \
      _Name##Error_() = default; virtual ~_Name##Error_() = default; \
      _Name##Error_(const except::Context& c) : _Base(c){} \
      _Name##Error_(const std::string& msg) : _Base(msg){} \
      _Name##Error_(const except::Throwable& t, const except::Context& c) : _Base(t, c){} \
      std::string getType() const noexcept override { return #_Name; } \
  }
#define DECLARE_EXTENDED_ERROR(_Name, _Base) DECLARE_EXTENDED_ERROR_(_Name, Error, _Base)

// Need to keep this around for existing code
#define DECLARE_ERROR(_Name) DECLARE_EXTENDED_ERROR(_Name, except::Error)

namespace except
{
/*!
 * \class Error
 * \brief Represents a serious unexpected occurrence during the program
 *
 * The error class is a representation of a throwable object, occurring
 * under serious conditions.  It may be undesirable to handle an error in
 * the manner that you handle an exception.  For this reason, the distinction is
 * made
 */
struct Error : public Throwable
{
    Error() = default;
    virtual ~Error() = default;

    /*!
     * Constructor. Takes a Context
     * \param c The Context
     */
    Error(const Context& c) :
        Throwable(c)
    {
    }

    /*!
     * Constructor.  Takes a message
     * \param message The message
     */
    Error(const std::string& message) :
        Throwable(message)
    {
    }

    /*!
     * Constructor. Takes an Throwable and a Context
     * \param t The Throwable
     * \param c The Context
     */
    Error(const Throwable& t, const Context& c) :
        Throwable(t, c)
    {
    }

    std::string getType() const override 
    {
        return "Error";
    }
};

// Use this in new code: name is FooErrror (not FooErrorEx), base is except::ErrorEx (not except::Error).
#define CODA_OSS_DECLARE_EXTENDED_ERROR(name_, base_) DECLARE_EXTENDED_ERROR_(name_, Error, base_)
#define CODA_OSS_DECLARE_ERROR(name_) CODA_OSS_DECLARE_EXTENDED_ERROR(name_, except::Error)

using ErrorEx = Error;
using Error11 = ErrorEx; // keep old name around for other projects

/*!
 * \class InvalidDerivedTypeError
 * \brief Represents an invalid derived type error.
 */
DECLARE_ERROR(InvalidDerivedType);

}

#endif

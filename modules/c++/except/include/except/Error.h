/* =========================================================================
 * This file is part of except-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
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
#define DECLARE_EXTENDED_ERROR(_Name, _Base) \
  class _Name##Error : public _Base \
  { \
  public: \
      _Name##Error() : _Base(){} \
      _Name##Error(const except::Context& c) : _Base(c){} \
      _Name##Error(const std::string& msg) : _Base(msg){} \
      _Name##Error(const except::Throwable& t, const except::Context& c) : _Base(t, c){} \
      virtual ~_Name##Error(){} \
      virtual std::string getType() const{ return #_Name; } \
  };

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
class Error : public Throwable
{
public:
    /*!
     * Default constructor
     */
    Error() :
        Throwable()
    {
    }

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

    //!  Destructor
    virtual ~Error()
    {
    }

    virtual std::string getType() const
    {
        return "Error";
    }
};

/*!
 * \class InvalidDerivedTypeError
 * \brief Represents an invalid derived type error.
 */
DECLARE_ERROR(InvalidDerivedType)

}

#endif

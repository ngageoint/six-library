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

#ifndef __EXCEPT_EXCEPTION_H__
#define __EXCEPT_EXCEPTION_H__

/*!
 * \file Exception.h
 * \brief Contains the classes to do with exception handling.
 *
 * A Throwable has two possible classifications (according to java, and for our
 * purposes, that is good enough): Errors and Exceptions.
 * This class deals with the latter.
 *
 */
#include <string>
#include <sstream>
#include "except/Throwable.h"

/*!
 * Useful macro for defining Exception classes
 */
#define DECLARE_EXTENDED_EXCEPTION(_Name, _Base) \
  class _Name##Exception : public _Base \
  { \
  public: \
      _Name##Exception() : _Base(){} \
      _Name##Exception(const except::Context& c) : _Base(c){} \
      _Name##Exception(const std::string& msg) : _Base(msg){} \
      _Name##Exception(const except::Throwable& t, const except::Context& c) : _Base(t, c){} \
      virtual ~_Name##Exception(){} \
      virtual std::string getType() const{ return #_Name"Exception"; } \
  };

#define DECLARE_EXCEPTION(_Name) DECLARE_EXTENDED_EXCEPTION(_Name, except::Exception)

namespace except
{

/*!
 * \class Exception
 * \brief (typically non-fatal) throwable.
 *
 * This class is the base for all exceptions.
 */
class Exception : public Throwable
{
public:

    /*!
     * Constructor.
     */
    Exception() :
        Throwable()
    {
    }

    /*!
     * Constructor. Takes a Context
     * \param c The Context
     */
    Exception(const Context& c) :
        Throwable(c)
    {
    }

    /*!
     * Constructor. Takes an Throwable and a Context
     * \param t The Throwable
     * \param c The Context
     */
    Exception(const Throwable& t, const Context& c) :
        Throwable(t, c)
    {
    }

    /*!
     * Constructor.  Takes a message
     * \param message The message
     */
    Exception(const std::string& message) :
        Throwable(message)
    {
    }

    //! Destructor
    virtual ~Exception()
    {
    }

    virtual std::string getType() const
    {
        return "Exception";
    }
};

/*!
 * \class IOException
 * \brief Throwable related to IO problems.
 */
DECLARE_EXCEPTION(IO)

/*!
 * \class FileNotFoundException
 * \brief Throwable related to a file not found.
 */
DECLARE_EXTENDED_EXCEPTION(FileNotFound, except::IOException)

/*!
 * \class BadCastException
 * \brief Exception for bad casting operations
 */
DECLARE_EXCEPTION(BadCast)

/*!
 * \class InvalidFormatException
 * \brief Throwable related to an invalid file format.
 */
DECLARE_EXCEPTION(InvalidFormat)

/*!
 * \class IndexOutOfRangeException
 * \brief Throwable related to an index being out of range.
 */
DECLARE_EXCEPTION(IndexOutOfRange)

/*!
 * \class OutOfMemoryException
 * \brief Throwable related to memory allocation problems.
 */
DECLARE_EXCEPTION(OutOfMemory)

/*!
 * \class NullPointerReference
 * \brief This is responsible for handling a null pointer ref/deref
 * 
 * This class is currently treated as an exception, meaning that its
 * behavior is not necessarily fatal. 
 */
DECLARE_EXCEPTION(NullPointerReference)
//! For backwards-compatibility
typedef NullPointerReferenceException NullPointerReference;

/*!
 * \class NoSuchKeyException
 * \brief Throwable related to unknown keys.
 */
DECLARE_EXCEPTION(NoSuchKey)

/*!
 * \class NoSuchReferenceException
 * \brief Throwable related to unknown references.
 */
DECLARE_EXCEPTION(NoSuchReference)

/*!
 * \class KeyAlreadyExistsException
 * \brief Throwable related to duplicate keys.
 */
DECLARE_EXCEPTION(KeyAlreadyExists)

/*!
 * \class NotImplementedException
 * \brief Throwable related to code not being implemented yet.
 */
DECLARE_EXCEPTION(NotImplemented)

/*!
 * \class InvalidArgumentException
 * \brief Throwable related to an invalid argument being passed.
 */
DECLARE_EXCEPTION(InvalidArgument)

/*!
 * \class SerializationException
 * \brief Throwable related to failing to serialize/deserialize data.
 */
DECLARE_EXTENDED_EXCEPTION(Serialization, except::IOException)

/*!
 * \class ParseException
 * \brief Throwable related to failing to parse data.
 */
DECLARE_EXTENDED_EXCEPTION(Parse, except::IOException)

}

#endif


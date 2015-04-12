/* =========================================================================
 * This file is part of sys-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
 *
 * sys-c++ is free software; you can redistribute it and/or modify
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


#ifndef __SYSTEM_EXCEPTION_H__
#define __SYSTEM_EXCEPTION_H__

#include "sys/Err.h"
#include "except/Error.h"
#include "except/Exception.h"

/*!
 *  \file  SystemException.h
 *  \brief Provide system exceptions that are specialized for each OS
 *
 *  This class makes use of the system-independent Err class to create
 *  system exceptions that leverage the OS-specific API for errors
 */
namespace sys
{
/*!
 *  \class SystemException
 *  \brief An OS-independent exception class
 */
class SystemException : public except::Exception
{
public:

    /*!
     *  Constructs the user message, and retrieves the last error.
     *  \param userMessage The user's message
     */
    SystemException(const std::string& userMessage) :
            except::Exception(userMessage)
    {
        Err e;
        mMessage += std::string(": ") + e.toString();
    }

    /*!
     *  Constructs the user message, and retrieves the last error.
     *  \param c The user's context
     */
    SystemException(const except::Context& c) :
            except::Exception(c)
    {
        Err e;
        mMessage += std::string(": ") + e.toString();
    }

    /*!
     *  Constructs the user message, and retrieves the last error.
     *  \param userMessage The user's message
     */
    SystemException(const char *userMessage) :
            except::Exception(userMessage)
    {
        Err e;
        mMessage += std::string(": ") + e.toString();
    }

    /*!
     *  Constructs from the user message, AND the error
     *  \param userMessage A user's message
     *  \param errorId the system error id
     */
    SystemException(const char *userMessage, int errorId):
            except::Exception(userMessage)
    {
        Err e(errorId);
        mMessage += std::string(": ") + e.toString();
    }

    /*!
     *  Constructs from the user message, AND the error
     *  \param userMessage A user's message
     *  \param errorId the system error id
     */
    SystemException(const std::string& userMessage, int errorId) :
            except::Exception(userMessage)
    {
        Err e(errorId);
        mMessage += std::string(": ") + e.toString();
    }
};

/*!
 *  \class SystemError
 *  \brief Same as SystemException, but for an error
 *
 *  This class is identical to SystemException, except that it is
 *  an error, not an exception
 */
class SystemError : public except::Error
{
public:

    /*!
     *  Constructs the user message, and retrieves the last error.
     *  \param userMessage The user's message
     */
    SystemError(const std::string& userMessage) :
            except::Error(userMessage)
    {
        Err e;
        mMessage += std::string(": ") + e.toString();
    }

    /*!
     *  Constructs the user message, and retrieves the last error.
     *  \param userMessage The user's message
     */
    SystemError(const char *userMessage) :
            except::Error(userMessage)
    {
        Err e;
        mMessage += std::string(": ") + e.toString();
    }

    /*!
     *  Constructs from the user message, AND the error
     *  \param userMessage A user's message
     *  \param errorId the system error id
     */
    SystemError(const char *userMessage, int errorId):
            except::Error(userMessage)
    {
        Err e(errorId);
        mMessage += std::string(": ") + e.toString();
    }

    /*!
     *  Constructs from the user message, AND the error
     *  \param userMessage A user's message
     *  \param errorId the system error id
     */
    SystemError(const std::string& userMessage, int errorId) :
            except::Error(userMessage)
    {
        Err e(errorId);
        mMessage += std::string(": ") + e.toString();
    }

    /*!
     *  Constructs the user message, and retrieves the last error.
     *  \param c The user's context
     */
    SystemError(const except::Context& c) :
            except::Error(c)
    {
        Err e;
        mMessage += std::string(": ") + e.toString();
    }
};


/*     class SocketException : public except::Exception */
/*     { */
/*     public: */


/*  SocketException(const std::string& userMessage) :  */
/*      except::Exception(userMessage)  */
/*  { */
/*      mErr = Err(); */
/*  } */


/*  SocketException(const char *userMessage) : */
/*      except::Exception(userMessage) {} */

/*  SocketException(int errorId, */
/*    const char *userMessage) : */
/*      except::Exception(userMessage)  */
/*  { */
/*      mErr = Err(errorId); */
/*  } */

/*  SocketException(int errorId, */
/*    const char *userMessage) : */
/*      except::Exception(userMessage)  */
/*  { */
/*      mErr = Err(errorId); */
/*  } */

/*  Err& getErr() { return mErr; } */
/*     protected: */
/*  sys::Err mErr; */
/*     }; */

/*     class SocketError : public except::Error */
/*     { */
/*     public: */

/*  SocketError(const std::string& userMessage) :  */
/*      except::Error(userMessage)  */
/*  { */
/*      mErr = Err(); */
/*  } */


/*  SocketError(const char *userMessage) : */
/*      except::Error(userMessage) {} */

/*  SocketError(int errorId, */
/*    const char *userMessage) : */
/*      except::Error(userMessage)  */
/*  { */
/*      mErr = Err(errorId); */
/*  } */

/*  SocketError(int errorId, */
/*    const char *userMessage) : */
/*      except::Error(userMessage)  */
/*  { */
/*      mErr = Err(errorId); */
/*  } */

/*  sys::Err& getErr() { return mErr; } */

/*     protected: */
/*  sys::Err mErr; */

/*     }; */


// Temporary -- eventually want a socket error specific class
// so we can support winsock
typedef SystemException SocketException;
typedef SystemError SocketError;
}
#endif

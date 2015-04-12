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


#ifndef __EXCEPT_CONTEXT_H__
#define __EXCEPT_CONTEXT_H__

#include <string>
#include <iostream>

/*!
 * \file
 * \brief defines a class that contains the information surrounding an
 * exception or error.
 */
namespace except
{

/*!
 * \class Context
 * \brief The information surrounding an exception or error
 *
 * This class contains information such as the file, line, 
 * function and time
 */
class Context
{
public:

    /*!
     * Constructor
     * \param message The message describing the exception
     * \param time The system time when the error occurred.
     * \param func The function where the error occured (not always available)
     * \param file The file where the exception occurred
     * \param line The line number where the exception occurred
     */
    Context(const std::string& file,
            int line,
            const std::string& func,
            const std::string& time,
            const std::string& message) : mMessage(message), mTime(time), mFunc(func),
            mFile(file), mLine(line)
    {}

    /*!
     * Copy constructor
     * \param c The context to copy
     */
    Context(const Context& c);

    /*!
     * Assignment operator
     * \param c The context to copy
     */
    Context& operator=(const Context& c);

    /*!
     * Get the message describing the exception that occurred
     * \return The message
     */
    const std::string& getMessage() const
    {
        return mMessage;
    }

    /*!
    * Get the system time
    * \return The system time
    */
    const std::string& getTime() const
    {
        return mTime;
    }

    /*!
     * Get the function where the exception occurred (may not be available
     * \return The function signature 
     */
    const std::string& getFunction() const
    {
        return mFunc;
    }

    /*!
     * Get the file where the exception occurred
     * \return The file
     */
    const std::string& getFile() const
    {
        return mFile;
    }

    /*!
     * Get the line number
     * \return The line number
     */
    int getLine() const
    {
        return mLine;
    }

    //! The name of the message the exception was thrown
    std::string mMessage;
    //! The date/time the exception was thrown
    std::string mTime;
    //! The name of the function the exception was thrown
    std::string mFunc;
    //! The name of the file the exception was thrown
    std::string mFile;
    //! The line number where the exception was thrown
    int mLine;
};

}

std::ostream& operator<< (std::ostream& os, const except::Context& c);

#endif

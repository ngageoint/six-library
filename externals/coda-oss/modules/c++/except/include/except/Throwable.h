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

#ifndef CODA_OSS_except_Throwable_h_INCLUDED_
#define CODA_OSS_except_Throwable_h_INCLUDED_
#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <exception>
#include <numeric> // std::accumulate

#include "except/Trace.h"

/*!
 * \file Throwable.h
 * \brief Contains the classes to do with error handling
 *
 * A Throwable has two possible classifications (according to java, and for our
 * purposes, that is good enough): Errors and Exceptions.
 */
namespace except
{

/*!
 * \class Throwable
 * \brief The interface for exceptions and errors
 *
 * This class provides the base interface for exceptions and errors.
 */

/*
 * It can be quite convenient to derive from std::exception as often one less
 * "catch" will be needed and we'll have standard what().  But doing so could
 * break existing code as "catch (const std::exception&)" will catch
 * except::Throwable when it didn't before.
 */
#ifndef CODA_OSS_Throwable_isa_std_exception
#define CODA_OSS_Throwable_isa_std_exception 0  // preserve existing behavior
//#define CODA_OSS_Throwable_isa_std_exception 1
#endif
class Throwable
#if CODA_OSS_Throwable_isa_std_exception
    : public std::exception
#endif
{
    void doGetBacktrace();
    Throwable(const Context*, const Throwable* pT = nullptr, const std::string* pMessage = nullptr, bool callGetBacktrace = false);

public:
    Throwable();

    /*!
     * Constructor.  Takes a message
     * \param message The message
     */
    Throwable(const std::string& message);

    /*!
     * Constructor.  Takes a Context.
     * \param c The Context
     */
    Throwable(Context);

    /*!
     * Constructor. Takes a Throwable and a Context
     * \param t The throwable
     * \param c The Context
     */
    Throwable(const Throwable&, Context);

    /*!
     * Destructor
     */
    virtual ~Throwable()
    {
    }

    /*!
     * Get the message
     * \return The message
     */
    std::string getMessage() const
    {
        return mMessage;
    }

    /*!
     * Get the trace
     * \return The trace (const)
     */
    const Trace& getTrace() const
    {
        return mTrace;
    }

    /*!
     * Get the trace
     * \return The trace (non-const)
     */
    Trace& getTrace()
    {
        return mTrace;
    }

    /*!
     * Get the type id
     * \return The type
     */
    virtual std::string getType() const
    {
        return "Throwable";
    }

    virtual std::string toString() const
    {
        std::ostringstream s;
        s << getType() << ": " << getMessage();

        const Trace& t = getTrace();
        if (t.getSize() > 0)
            s << ": " << t;
        return s.str();
    }

    const std::vector<std::string>& getBacktrace() const
    {
        return mBacktrace;
    }

    // It seems that overloading constructors creates ambiguities ... so allow for a "fluent" way
    // of doing this.: throw Exception(...).backtrace()
    Throwable& backtrace()
    {
        doGetBacktrace();
        return *this;
    }

    virtual std::string toString(bool includeBacktrace) const
    {
        // Adding the backtrace to existing toString() output could substantally alter existing strings.
        std::string backtrace;
        if (includeBacktrace)
        {
            backtrace = "***** getBacktrace() *****\n";
            backtrace +=  std::accumulate(mBacktrace.begin(), mBacktrace.end(), std::string());
        }
        return toString() + backtrace;
    }

    const char* what() const noexcept
#if CODA_OSS_Throwable_isa_std_exception
        override final // derived classes override toString()
#endif
    {
        // adding this to toString() output could (significantly) alter existing display
        mWhat = toString(true /*includeBacktrace*/); // call any derived toString()
        return mWhat.c_str();
    }

protected:
    //! The name of exception trace
    Trace mTrace;
    //! The name of the message the exception was thrown
    std::string mMessage;

private:
    mutable std::string mWhat;
    std::vector<std::string> mBacktrace;
};
}

#endif // CODA_OSS_except_Throwable_h_INCLUDED_

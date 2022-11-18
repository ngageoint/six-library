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

#include <assert.h>

#include "except/Throwable.h"

#include "except/Backtrace.h"

void except::Throwable::doGetBacktrace()
{
    // This could be time-consuming or generate a lot of (noisy) output; only do
    // it if requested
    bool supported;
    (void)except::getBacktrace(supported, mBacktrace);
}

template <typename TThrowable>
except::Throwable::Throwable(const Context* pContext, const TThrowable* pThrowable, const std::string* pMessage, bool callGetBacktrace, std::nullptr_t)
{
    if (pThrowable != nullptr)
    {
        // Copy t's exception stack and push c onto local one
        mTrace = pThrowable->getTrace();
    }

    if (pContext != nullptr)
    {
        assert(pMessage == nullptr);

        // Push context onto exception stack
        mTrace.pushContext(*pContext);

        // Assign c's message as our internal one
        mMessage = pContext->getMessage();
    }
    
    if (pMessage != nullptr)
    {
        assert(pContext == nullptr);
        mMessage = *pMessage;
    }

    // This will record a back-trace from where the Throwable object was instantiated.
    // That's not necessarily where the "throw" will occur, but it's often the case; Throwable
    // instances ususally aren't passed around.  That is, hardly anybody does:
    //    Exception e; // Throwable instance
    //    might_throw(e);
    // rather, the idiom is usually
    //    throw Exception(...); // instantiate and throw
    if (callGetBacktrace)
    {
        doGetBacktrace();
    }
}
except::Throwable::Throwable(const Context* pContext, const Throwable* pThrowable, const std::string* pMessage, bool callGetBacktrace)
: Throwable(pContext, pThrowable, pMessage, callGetBacktrace, nullptr)
{
}
except::Throwable::Throwable(const Context* pContext, const Throwable11* pThrowable, const std::string* pMessage, bool callGetBacktrace)
: Throwable(pContext, pThrowable, pMessage, callGetBacktrace, nullptr)
{
}

except::Throwable::Throwable(const std::string& message) : Throwable(nullptr, static_cast<const Throwable*>(nullptr), &message)
{
}

except::Throwable::Throwable(except::Context c) : Throwable(&c)
{
}

except::Throwable::Throwable(const except::Throwable& t, except::Context c) : Throwable(&c, &t)
{
}
except::Throwable::Throwable(const except::Throwable11& t, except::Context c) : Throwable(&c, &t)
{
}
except::Throwable::Throwable(const except::Throwable11& t) : Throwable(nullptr, &t)
{
}

//******************************************************************************

void except::Throwable11::doGetBacktrace()
{
    // This could be time-consuming or generate a lot of (noisy) output; only do
    // it if requested
    bool supported;
    (void)except::getBacktrace(supported, mBacktrace);
}

template <typename TThrowable>
except::Throwable11::Throwable11(const Context* pContext,
                                 const TThrowable* pThrowable,
                                 const std::string* pMessage,
                                 bool callGetBacktrace,
                                 std::nullptr_t)
{
    if (pThrowable != nullptr)
    {
        // Copy t's exception stack and push c onto local one
        mTrace = pThrowable->getTrace();
    }

    if (pContext != nullptr)
    {
        assert(pMessage == nullptr);

        // Push context onto exception stack
        mTrace.pushContext(*pContext);

        // Assign c's message as our internal one
        mMessage = pContext->getMessage();
    }

    if (pMessage != nullptr)
    {
        assert(pContext == nullptr);
        mMessage = *pMessage;
    }

    // This will record a back-trace from where the Throwable object was
    // instantiated. That's not necessarily where the "throw" will occur, but
    // it's often the case; Throwable instances ususally aren't passed around.
    // That is, hardly anybody does:
    //    Exception e; // Throwable instance
    //    might_throw(e);
    // rather, the idiom is usually
    //    throw Exception(...); // instantiate and throw
    if (callGetBacktrace)
    {
        doGetBacktrace();
    }
}
except::Throwable11::Throwable11(const Context* pContext,
                             const Throwable11* pThrowable,
                             const std::string* pMessage,
                             bool callGetBacktrace) :
    Throwable11(pContext, pThrowable, pMessage, callGetBacktrace, nullptr)
{
}
except::Throwable11::Throwable11(const Context* pContext,
                                 const Throwable* pThrowable,
                                 const std::string* pMessage,
                                 bool callGetBacktrace) :
    Throwable11(pContext, pThrowable, pMessage, callGetBacktrace, nullptr)
{
}

except::Throwable11::Throwable11(const std::string& message) :
    Throwable11(nullptr, static_cast<const Throwable11*>(nullptr), &message)
{
}

except::Throwable11::Throwable11(const except::Context& c) : Throwable11(&c)
{
}

except::Throwable11::Throwable11(const except::Throwable11& t,
                                 const except::Context& c) :
    Throwable11(&c, &t)
{
}
except::Throwable11::Throwable11(const except::Throwable& t,
                                 const except::Context& c) :
    Throwable11(&c, &t)
{
}
except::Throwable11::Throwable11(const except::Throwable& t) :
    Throwable11(nullptr, &t)
{
}

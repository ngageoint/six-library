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

#include <assert.h>

#include <cstddef> // std::nullptr_t
#include <utility> // std::forward
#include <tuple> // std::ignore

#include "nitf/coda-oss.hpp"
#include "nitf/System.hpp"

/*!
 *  \file NITFException.hpp
 *  \brief NITFException class for nitf objects
 */
namespace nitf
{
    class Error final
    {
        const nitf_Error* pError = nullptr;
        template<size_t sz>
        static std::string to_string(const char(&s)[sz])
        {
            // avoid array -> pointer decay; code-analysis diagnostic
            return std::string(s, sz);
        }
    public:
        Error(const nitf_Error* error) noexcept : pError(error) { assert(pError != nullptr);  }
        Error(const Error&) = delete;
        Error& operator=(const Error&) = delete;

        std::string message() const
        {
            return to_string(pError->message);
        }
        std::string file() const
        {
            return to_string(pError->file);
        }
        int line() const noexcept
        {
            return pError->line;
        }
        std::string func() const
        {
            return to_string(pError->func);
        }
        int level() const noexcept
        {
            return pError->level;
        }
    };

/*!
 *  \class NITFException
 *  \brief  The C++ wrapper for the nitf_Error
 */
class NITFException  /*final*/ : public except::Exception // no "final", SWIG doesn't like it
{
    static except::Context make_Context_(const Error& error, const std::string& message)
    {
        return except::Context(error.file(), error.line(), error.func(), "", message);
    }
    static except::Context make_Context(const nitf_Error* pError)
    {
        const Error error(pError);
        return make_Context_(error, error.message());
    }
    static except::Context make_Context(const nitf_Error* pError, const std::string& message)
    {
        const Error error(pError);
        return make_Context_(error, message + " (" + error.message() + ")");
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
};

// These are here because if the native call fails, we'll throw a NITFException
//
template<typename TReturn, typename Func, typename Native, typename... Args>
inline TReturn callNative(Func f, Native* pNative, Args&&... args) noexcept // no indication from call regarding failure
{
    // nitf_Error is just part of the "protocol," the call doesn't actually use it.
    // Often used for simple "get" routines, e.g., see Component_getWidth()
    nitf_Error error{};
    return f(pNative, std::forward<Args>(args)..., &error);
}

template<typename TReturn, typename Func, typename Native, typename... Args>
inline TReturn callNativeOrThrow(Func f, Native* pNative, Args&&... args) // c.f. getNativeOrThrow()
{
    nitf_Error error{};
    const auto retval = f(pNative, std::forward<Args>(args)..., &error);
    if (!retval)
    {
        throw nitf::NITFException(&error);
    }
    return retval;
}
template<typename Func, typename Native, typename... Args>
inline void callNativeOrThrowV(Func f, Native* pNative, Args&&... args) // c.f. getNativeOrThrow()
{
    // save caller the trouble of figuring out a return type that won't be used
    constexpr nitf_Error* pError = nullptr; // only needed for decltype()
    using retval_t = decltype(f(pNative, std::forward<Args>(args)..., pError));
    std::ignore = callNativeOrThrow<retval_t>(f, pNative, std::forward<Args>(args)...);
}
}
#endif

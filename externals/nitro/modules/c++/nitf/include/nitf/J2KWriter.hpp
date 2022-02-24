/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 * (C) Copyright 2022, Maxar Technologies, Inc.
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

#ifndef NITF_J2KWriter_hpp_INCLUDED_
#define NITF_J2KWriter_hpp_INCLUDED_
#pragma once

#include <std/filesystem>
#include <std/span>

#include "config/compiler_extensions.h"

#include "j2k/j2k_Writer.h"

#include "nitf/coda-oss.hpp"
#include "nitf/Object.hpp"
#include "nitf/System.hpp"
#include "nitf/NITFException.hpp"
#include "nitf/IOHandle.hpp"

namespace j2k
{
    // Our own wrapper class so we can initialize j2k_WriterOptions
    class WriterOptions final
    {
        j2k_WriterOptions value_{};
    public:
        // is there any reason to complicate this with getters and setters?
        double& compressionRatio;
        uint32_t& numResolutions;
        WriterOptions() noexcept
            : compressionRatio(value_.compressionRatio),
            numResolutions(value_.numResolutions) { }
        WriterOptions(const WriterOptions&) = default;
        WriterOptions& operator=(const WriterOptions&) = delete;
        WriterOptions(WriterOptions&&) = default;
        WriterOptions& operator=(WriterOptions&&) = delete;
        ~WriterOptions() = default;

        // for use when calling C code
        j2k_WriterOptions* getNative() const noexcept
        {
            CODA_OSS_disable_warning_push
            #if _MSC_VER
            #pragma warning(disable: 26492) // Don't use const_cast to cast away const or volatile (type.3).
            #endif  
            // C API isn't "const correct" everywhere :-(
            return const_cast<j2k_WriterOptions*>(&value_);
            CODA_OSS_disable_warning_pop
        }
    };

/*!
    *  \class j2k::Writer
    *  \brief  The C++ wrapper for the j2k_Writer
    */
namespace details
{
NITRO_DECLARE_CLASS_J2K(Writer)
{
    Writer() = default;

public:
    // TODO: put in Object.hpp?
    template<typename Func, typename... Args>
    void callNativeOrThrowV(Func f, Args&&... args) // c.f. getNativeOrThrow()
    {
        nitf::callNativeOrThrowV(f, getNativeOrThrow(), std::forward<Args>(args)...);
    }

    Writer(const Writer&) = delete;
    Writer& operator=(const Writer&) = delete;
    Writer(Writer&&) = default;
    Writer& operator=(Writer&&) = default;
    ~Writer() = default;

    //! Set native object
    Writer(j2k_Writer*);
};
}
struct Container; // forward
struct Writer final
{
    Writer() = delete;
    Writer(const Writer&) = delete;
    Writer& operator=(const Writer&) = delete;
    Writer(Writer&&) = default;
    Writer& operator=(Writer&&) = default;
    ~Writer() = default;

    //! Set native object
    Writer(j2k_Writer*&&);

    Writer(const Container&, const WriterOptions&);

    void setTile(uint32_t tileX, uint32_t tileY, std::span<const uint8_t> buf);
    void write(nitf::IOHandle&);
private:
    details::Writer impl_;
};
}
#endif // NITF_J2KWriter_hpp_INCLUDED_

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
#include "nitf/exports.hpp"
#include "nitf/Object.hpp"
#include "nitf/System.hpp"
#include "nitf/NITFException.hpp"
#include "nitf/IOHandle.hpp"

namespace j2k
{
    // Our own wrapper class so we can initialize j2k_WriterOptions
    class NITRO_NITFCPP_API WriterOptions final
    {
        j2k_WriterOptions value_{};
    public:
        // make this look like CompressionParameters since that's existing code
        WriterOptions() = default;
        WriterOptions(const j2k_WriterOptions& v) noexcept : value_(v) {}
        WriterOptions(float compressionRatio, uint32_t numResolutions) noexcept :
            WriterOptions(j2k_WriterOptions{ compressionRatio , numResolutions }) {}
        WriterOptions(const WriterOptions&) = default;
        WriterOptions& operator=(const WriterOptions&) = default;
        WriterOptions(WriterOptions&&) = default;
        WriterOptions& operator=(WriterOptions&&) = default;
        ~WriterOptions() = default;

        float getCompressionRatio() const noexcept { return value_.compressionRatio; }
        uint32_t getNumResolutions() const noexcept { return value_.numResolutions; }

        void setCompressionRatio(float compRatio) noexcept { value_.compressionRatio = compRatio; }
        void setNumResolutions(uint32_t numResoltuions) noexcept { value_.numResolutions = numResoltuions;  }

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
    template<typename TReturn, typename Func, typename... Args>
    TReturn callNativeOrThrow(Func f, Args&&... args) const // c.f. getNativeOrThrow()
    {
        return nitf::callNativeOrThrow<TReturn>(f, getNativeOrThrow(), std::forward<Args>(args)...);
    }
    template<typename Func, typename... Args>
    void callNativeOrThrowV(Func f, Args&&... args) const // c.f. getNativeOrThrow()
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
struct NITRO_NITFCPP_API Writer final
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

    const Container& getContainer() const;
    void setTile(uint32_t tileX, uint32_t tileY, std::span<const uint8_t> buf);
    void write(nitf::IOHandle&);
private:
    details::Writer impl_;
    const Container* pContainer_ = nullptr;
};

// Encapsulate some messy code that's needed to call Writer::setTile()
struct NITRO_NITFCPP_API WriteTiler final
{
    WriteTiler(Writer&, std::span<const uint8_t>);
    WriteTiler(const WriteTiler&) = delete;
    WriteTiler& operator=(const WriteTiler&) = delete;
    WriteTiler(WriteTiler&&) = delete;
    WriteTiler& operator=(WriteTiler&&) = delete;
    ~WriteTiler() = default;

    void setTile(uint32_t tileX, uint32_t tileY, uint32_t i);

private:
    Writer& writer_;
    std::span<const uint8_t> buf_;
    size_t tileSize_ = 0;
    uint32_t num_x_tiles_ = 0;
    uint32_t numComponents_ = 0;
};
}
#endif // NITF_J2KWriter_hpp_INCLUDED_

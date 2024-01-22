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

#ifndef NITF_J2KReader_hpp_INCLUDED_
#define NITF_J2KReader_hpp_INCLUDED_
#pragma once

#include <memory>
#include <std/filesystem>
#include <std/span>

#include "j2k/j2k_Reader.h"

#include "nitf/coda-oss.hpp"
#include "nitf/exports.hpp"
#include "nitf/Object.hpp"
#include "nitf/System.hpp"
#include "nitf/NITFException.hpp"
#include "nitf/IOHandle.hpp"
#include "nitf/J2KContainer.hpp"

namespace j2k
{
// Use when calling Reader::readRegion(); memory is allocated
// for us, it needs to be NRT_FREE()'d.
using Buffer = std::unique_ptr<uint8_t[], decltype(&NRT_FREE)>;
inline Buffer make_Buffer(uint8_t* p = nullptr) noexcept
{
    // so that clients don't have to deal with decltype(&NRT_FREE)
    return Buffer(p, &NRT_FREE);
}

/*!
*  \class j2k::Reader
*  \brief  The C++ wrapper for the j2k_Reader
*/
namespace details
{
NITRO_DECLARE_CLASS_J2K(Reader)
{
    Reader() = default;

public:
    // TODO: put in Object.hpp?
    template<typename TReturn, typename Func, typename... Args>
    TReturn callNativeOrThrow(Func f, Args&&... args) const // c.f. getNativeOrThrow()
    {
        return nitf::callNativeOrThrow<TReturn>(f, getNativeOrThrow(), std::forward<Args>(args)...);
    }

    Reader(const Reader&) = delete;
    Reader& operator=(const Reader&) = delete;
    Reader(Reader&&) = default;
    Reader& operator=(Reader&&) = default;
    ~Reader() = default;

    //! Set native object
    Reader(j2k_Reader*);

    Reader(nitf::IOHandle&);
    Reader(const std::filesystem::path&);
};
}
struct NITRO_NITFCPP_API Reader final
{
    Reader() = delete;
    Reader(const Reader&) = delete;
    Reader& operator=(const Reader&) = delete;
    Reader(Reader&&) = default;
    Reader& operator=(Reader&&) = default;
    ~Reader() = default;

    //! Set native object
    Reader(j2k_Reader*&&);

    Reader(nitf::IOHandle&);
    Reader(const std::filesystem::path&);

    Container getContainer() const;

    std::span<uint8_t> readRegion(uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1, Buffer&);
    std::span<uint8_t> readRegion(uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1, std::span<uint8_t>);

private:
    details::Reader impl_;
};
}
#endif // NITF_J2KReader_hpp_INCLUDED_


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

#ifndef NITF_J2KContainer_hpp_INCLUDED_
#define NITF_J2KContainer_hpp_INCLUDED_
#pragma once

#include "nitf/coda-oss.hpp"

#include "j2k/Container.h"
#include "j2k/Writer.h"

#include "nitf/Object.hpp"
#include "nitf/System.hpp"
#include "nitf/NITFException.hpp"
#include "nitf/J2KComponent.hpp"
#include "nitf/J2KWriter.hpp"

namespace j2k
{
/*!
    *  \class j2k::Container
    *  \brief  The C++ wrapper for the j2k_Container
    */
namespace details
{
NITRO_DECLARE_CLASS_J2K(Container)
{
    Container() = default;

public:
    // TODO: put in Object.hpp?
    template<typename TReturn, typename Func, typename... Args>
    TReturn callNativeOrThrow(Func f, Args&&... args) const // c.f. getNativeOrThrow()
    {
        return nitf::callNativeOrThrow<TReturn>(f, getNativeOrThrow(), std::forward<Args>(args)...);
    }
    template<typename TReturn, typename Func, typename... Args>
    TReturn callNative(Func f, Args&&... args) const // c.f. getNativeOrThrow()
    {
        return nitf::callNative<TReturn>(f, getNativeOrThrow(), std::forward<Args>(args)...);
    }

    Container(const Container&) = delete;
    Container& operator=(const Container&) = delete;
    Container(Container&&) = default;
    Container& operator=(Container&&) = default;
    ~Container() = default;

    //! Set native object
    Container(j2k_Container*);
};
}
struct Container final
{
    Container(const Container&) = delete;
    Container& operator=(const Container&) = delete;
    Container(Container&&) = default;
    Container& operator=(Container&&) = default;
    ~Container() = default;

    //! Set native object
    Container(j2k_Container*&&);

    uint32_t getNumComponents() const;
    Component getComponent(uint32_t) const;

    uint32_t getWidth() const;
    uint32_t getHeight() const;

    uint32_t getGridWidth() const;
    uint32_t getGridHeight() const;

    uint32_t getTileWidth() const;
    uint32_t getTileHeight() const;

    uint32_t getTilesX() const;
    uint32_t getTilesY() const;

    int getImageType() const;

    Writer createWriter(const WriterOptions&) const;

    // Not part of the C API
    size_t tileSize() const; // getTileWidth() * getTileHeight()
    size_t numBytes(uint32_t) const;
    ptrdiff_t bufferOffset(uint32_t tileX, uint32_t tileY, uint32_t i) const;

private:
    details::Container impl_;
};
}
#endif // NITF_J2KContainer_hpp_INCLUDED_


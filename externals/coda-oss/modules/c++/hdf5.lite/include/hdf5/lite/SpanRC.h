/* =========================================================================
 * This file is part of hdf5.lite-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2022, Maxar Technologies, Inc.
 *
 * hdf5.lite-c++ is free software; you can redistribute it and/or modify
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

#ifndef CODA_OSS_hdf5_lite_SpanRC_h_INCLUDED_
#define CODA_OSS_hdf5_lite_SpanRC_h_INCLUDED_
#pragma once

/*!
 * \file  SpanRC.h
 *
 * This is a super-simple version of C++23's mdspan.  It's here because 1) don't want widespread use, and
 * 2) CODA already has a View2D.
 */

#include <assert.h>

#include "config/Exports.h"
#include "coda_oss/span.h"
#include "types/RowCol.h"

namespace hdf5
{
namespace lite
{

template<typename T>
struct SpanRC final
{
    using size_type =  types::RowCol<size_t>;
    using element_type = T;
    using pointer = T*;
    using reference = T&;

    SpanRC() = default;
    SpanRC(pointer p, size_type rc) noexcept : s_(p, rc.area()), rc_(rc)
    {
    }
    SpanRC(pointer p, size_t r, size_t c) noexcept : SpanRC(p, size_type(r, c))
    {
    }
    SpanRC(const SpanRC&) noexcept = default;

    constexpr pointer data() const noexcept
    {
        return s_.data();
    }
    
    /*constexpr*/ reference operator[](size_t idx) const noexcept
    {
        assert(idx < size());  // prevents "constexpr" in C++11
        return data()[idx];
    }
    /*constexpr*/ reference operator()(size_t r, size_t c) const noexcept
    {
        const auto offset = (r * dims().col) + c;
        return (*this)[offset];
    }
    /*constexpr*/ reference operator[](size_type idx) const noexcept
    {
        return (*this)(idx.row, idx.col);
    }

    constexpr size_t size() const noexcept
    {
        assert(s_.size() == rc_.area());
        return s_.size();
    }
    constexpr size_t area() const noexcept
    {
        return size();
    }

    constexpr size_type size_bytes() const noexcept
    {
        return s_.size_bytes();
    }

    constexpr bool empty() const noexcept
    {
        return s_.empty();
    }

    const auto& dims() const
    {
        return rc_;
    }

    private:
    types::RowCol<size_t> rc_;
    coda_oss::span<T> s_;
};

}
}

#endif // CODA_OSS_hdf5_lite_SpanRC_h_INCLUDED_

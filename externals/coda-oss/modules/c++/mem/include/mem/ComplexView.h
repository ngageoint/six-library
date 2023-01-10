/* =========================================================================
 * This file is part of mem-c++
 * =========================================================================
 *
 * (C) Copyright 2013 - 2014, MDA Information Systems LLC
 * (C) Copyright 2023, Maxar Technologies, Inc.
 *
 * mem-c++ is free software; you can redistribute it and/or modify
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

#ifndef CODA_OSS_mem_ComplexView_h_INCLUDED_
#define CODA_OSS_mem_ComplexView_h_INCLUDED_
#pragma once

#include <assert.h>

#include <vector>
#include <complex>
#include <stdexcept>

#include "coda_oss/span.h"

namespace mem
{

 /*!
 *  \class ComplexView
 *  \brief These classes class provide read-only views onto a collection of complex
 * numbers. For the simple case, it's roughtly std::span<const std::complex<T>>. However,
 * sometimes the data is in two parallel arrays:
 *    const float* reals;
 *    const float* imags;
 * This classes have (almost) the same access routines regardless of how the underlying data is actually stored.
 * 
 * Thing are intentinally kept very simple (for now), e.g., no iterators, because it's not yet clear
 * what functionality will be needed; YAGNI.
 */

// Copy the data into a "normal" std::vector<std::complex<T>> instead of parallel arrays.
namespace details
{
template <typename TView>
inline auto copy_view(TView view)
{
    using cxvalue_type = typename TView::cxvalue_type;  // i.e., std::complex<float>
    std::vector<cxvalue_type> retval(view.size());
    for (size_t i = 0; i < view.size(); i++)
    {
        retval[i] = view[i];
    }
    return retval;
}
}  // namespace details

template <typename T>
struct ComplexSpanView final
{
    using size_type = size_t;
    using value_type = std::complex<T>;
    using cxvalue_type = value_type;

    ComplexSpanView() = delete;
    ~ComplexSpanView() = default;
    using span_t = coda_oss::span<const cxvalue_type>;
    explicit ComplexSpanView(span_t data) : data_(data)
    {
    }
    ComplexSpanView(const ComplexSpanView&) = default;
    ComplexSpanView& operator=(const ComplexSpanView&) = default;
    ComplexSpanView(ComplexSpanView&&) = default;
    ComplexSpanView& operator=(ComplexSpanView&&) = default;

    // This class is almost the same as std::span<const std::complex<T>> ... but don't
    // provide data() as ComplexSpansView stores complex numbers in two parallel arrays. 
    //const value_type* data() const noexcept = delete;

    constexpr auto real(size_type idx) const noexcept
    {
        return data_[idx].real();
    }
    constexpr auto imag(size_type idx) const noexcept
    {
        return data_[idx].imag();
    }

    constexpr const cxvalue_type& operator[](size_type idx) const noexcept
    {
        assert(idx < size());
        return data_[idx];
    }

    constexpr size_type size() const noexcept
    {
        return data_.size();
    }

private:
    span_t data_;
};
template <typename T>
inline auto make_ComplexSpanView(coda_oss::span<const std::complex<T>> s)
{
    return ComplexSpanView<T>(s);
}

template <typename T>
inline auto copy(ComplexSpanView<T> view) // for completeness and generic code
{
    return details::copy_view(view);
}

template <typename TVectorLike>
struct ComplexArrayView final
{
    using size_type = size_t;
    using value_type = typename TVectorLike::value_type;
    using cxvalue_type = value_type;

    ComplexArrayView() = delete;
    ~ComplexArrayView() = default;
    using span_t = coda_oss::span<const cxvalue_type>;
    explicit ComplexArrayView(const TVectorLike& data) :
        view(span_t(data.data(), data.size()))
    {
    }
    ComplexArrayView(const ComplexArrayView&) = default;
    ComplexArrayView& operator=(const ComplexArrayView&) = default;
    ComplexArrayView(ComplexArrayView&&) = default;
    ComplexArrayView& operator=(ComplexArrayView&&) = default;

    constexpr auto real(size_type idx) const noexcept
    {
        return view.real(idx);
    }
    constexpr auto imag(size_type idx) const noexcept
    {
        return view.imag(idx);
    }

    constexpr const cxvalue_type& operator[](size_type idx) const noexcept
    {
        return view[idx];
    }

    constexpr size_type size() const noexcept
    {
        return view.size();
    }

    private:
    ComplexSpanView<typename value_type::value_type> view;
};
template <typename TVectorLike>
inline auto make_ComplexArrayView(const TVectorLike& v)
{
    return ComplexArrayView<TVectorLike>(v);
}

template <typename TVectorLike>
inline auto copy(ComplexArrayView<TVectorLike> view)  // for completeness and generic code
{
    return details::copy_view(view);
}

template <typename T>
struct ComplexSpansView final // "Span_s_,", i.e., two spans. Avoiding "parallel" because that can conjure up multi-threading thoughts.
{
    using size_type = size_t;
    using value_type = T;
    using cxvalue_type = std::complex<value_type>;

    ComplexSpansView() = delete;
    ~ComplexSpansView() = default;
    using span_t = coda_oss::span<const value_type>;
    ComplexSpansView(span_t reals, span_t imags) :
        reals_(reals), imags_(imags)
    {
        if (reals_.size() != imags_.size())
        {
            throw std::invalid_argument("real and imag must be the same size.");
        }
    }
    ComplexSpansView(const T* reals, const T* imags, size_t sz) :
        ComplexSpansView(span_t(reals, sz), span_t(imags, sz))
    {
    }
    ComplexSpansView(const ComplexSpansView&) = default;
    ComplexSpansView& operator=(const ComplexSpansView&) = default;
    ComplexSpansView(ComplexSpansView&&) = default;
    ComplexSpansView& operator=(ComplexSpansView&&) = default;

    constexpr auto real(size_type idx) const noexcept
    {
        assert(idx < reals_.size());
        return reals_[idx];
    }
    constexpr auto imag(size_type idx) const noexcept
    {
        assert(idx < imags_.size());
        return imags_[idx];
    }

    constexpr cxvalue_type operator[](size_type idx) const noexcept
    {
        assert(idx < size());

        // Note that this is a COPY because the "real" and "imag" parts MUST be
        // next to each other. https://en.cppreference.com/w/cpp/numeric/complex
        return cxvalue_type(real(idx), imag(idx));
    }

    constexpr size_type size() const noexcept
    {
        assert(reals_.size() == imags_.size());
        return reals_.size();
    }

    private:
    span_t reals_;
    span_t imags_;
};
template <typename T>
inline auto make_ComplexSpansView(coda_oss::span<const T> reals, coda_oss::span<const T> imags)
{
    return ComplexSpansView<T>(reals, imags);
}

template <typename T>
inline auto copy(ComplexSpansView<T> view)
{
    return details::copy_view(view);
}

template <typename TVectorLike>  // e.g., std::vector<float>
struct ComplexArraysView final // "Array_s_,", i.e., two arrays. Avoiding "parallel" because that can conjure up multi-threading thoughts.
{
    using size_type = size_t;
    using value_type = typename TVectorLike::value_type; // i.e., float
    using cxvalue_type = std::complex<value_type>;

    ComplexArraysView() = delete;
    ~ComplexArraysView() = default;
    using span_t = coda_oss::span<const value_type>;
    ComplexArraysView(const TVectorLike& reals, const TVectorLike& imags) :
        view(span_t(reals.data(), reals.size()), span_t(imags.data(), imags.size()))
    {
    }
    ComplexArraysView(const ComplexArraysView&) = default;
    ComplexArraysView& operator=(const ComplexArraysView&) = default;
    ComplexArraysView(ComplexArraysView&&) = default;
    ComplexArraysView& operator=(ComplexArraysView&&) = default;

    constexpr auto real(size_type idx) const noexcept
    {
        return view.real(idx);
    }
    constexpr auto imag(size_type idx) const noexcept
    {
        return view.imag(idx);
    }

    constexpr cxvalue_type operator[](size_type idx) const noexcept
    {
        return view[idx];
    }

    constexpr size_type size() const noexcept
    {
        return view.size();
    }

    private:
    ComplexSpansView<value_type> view;
};
template <typename TVectorLike>
inline auto make_ComplexArraysView(const TVectorLike& reals, const TVectorLike& imags)
{
    return ComplexArraysView<TVectorLike>(reals, imags);
}

template <typename TVectorLike>
inline auto copy(ComplexArraysView<TVectorLike> view)
{
    return details::copy_view(view);
}

} 

#endif  // CODA_OSS_mem_ComplexView_h_INCLUDED_

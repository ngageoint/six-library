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

#pragma once
#ifndef CODA_OSS_mem_ComplexView_h_INCLUDED_
#define CODA_OSS_mem_ComplexView_h_INCLUDED_

#include <assert.h>
#include <stdint.h>

#include <vector>
#include <complex>
#include <stdexcept>
#include <functional>
#include <iterator>

#include "coda_oss/span.h"
#include "config/disable_compiler_warnings.h"

namespace mem
{

 /*!
 *  \class ComplexViewConstIterator
 *  \brief Iterate over a ComplexParallelView class only needing to know the
 * "axis" of the std::complex<>, i.e., "float" or "double".
 * 
 * This allows view-independent code to be written without having to use
 * a TView-template; instead ComplexViewConstIterator<T> can be used.
 * And often such code is dependent on T (i.e., "float", but not "double")
 * which means implementation can be in a CPP file.
 */
template <typename TAxis>
struct ComplexViewConstIterator final
{
    // https://stackoverflow.com/questions/8054273/how-to-implement-an-stl-style-iterator-and-avoid-common-pitfalls
    using iterator_category = std::random_access_iterator_tag;
    using value_type = std::complex<TAxis>;
    using difference_type = ptrdiff_t;
    using size_type = size_t;
    using pointer = const value_type*;
    using reference = const value_type&;

    template <typename TView>
    explicit ComplexViewConstIterator(TView view) :
        size_(view.size()) // Saving the size avoids the need for another constructor.
    {
        index_f_ = [view](size_type i) { return view[i]; };        
    }
    // Help ensure the iterators use the same view
    auto end_() const
    {
        auto retval = *this;
        retval.index_ = size_;
        return retval;
    }

    ComplexViewConstIterator() = default;
    ~ComplexViewConstIterator() = default;
    ComplexViewConstIterator(const ComplexViewConstIterator&) = default;
    ComplexViewConstIterator& operator=(const ComplexViewConstIterator&) = default;
    ComplexViewConstIterator(ComplexViewConstIterator&&) = default;
    ComplexViewConstIterator& operator=(ComplexViewConstIterator&&) = default;

    bool operator==(const ComplexViewConstIterator& rhs) const
    {
        // Checking the target() helps ensure the same type of view is used.
        return (index_ == rhs.index_) && (size_ == rhs.size_) &&
                (index_f_.target_type() == rhs.index_f_.target_type());
    }
    bool operator!=(const ComplexViewConstIterator& rhs) const
    {
        return !(*this == rhs);
    }

    ComplexViewConstIterator& operator++()
    {
        ++index_;
        return *this;
    }
    ComplexViewConstIterator operator++(int)
    {
        auto retval = *this;
        ++(*this);
        return retval;
    }
    ComplexViewConstIterator& operator+=(size_type i)
    {
        index_ += i;
        return *this;
    }

    difference_type operator-(const ComplexViewConstIterator& other) const
    {
        return index_ - other.index_;
    }

    reference operator*() const
    {
        return *(current());
    }

    pointer operator->() const
    {
        return current();
    }

private:
    // Need to store the current value because ->() returns a pointer
    mutable value_type current_value_;
    pointer current() const
    {
        current_value_ = index_f_(index_);
        return &current_value_;
    }

    size_type index_ = 0;
    std::function<value_type(size_t)> index_f_;
    size_type size_;  // used to create the end_() iterator
};

 /*!
 *  \class ComplexView
 *  \brief These classes class provide read-only views onto a collection of complex
 * numbers. For the simple case, it's roughtly std::span<const std::complex<T>>. However,
 * sometimes the data is in two parallel arrays:
 *    const float* reals;
 *    const float* imags;
 * This classes have (almost) the same access routines regardless of how the underlying data is actually stored.
 * 
 * Thing are intentinally kept very simple (for now), because it's not yet clear
 * what functionality will be needed.
 */

// Storing an array of std::complex is called "interleaved," e.g., https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2023/p2663r2.html

template <typename T>
struct ComplexInterleavedView final
{
    using size_type = size_t;
    using value_type = std::complex<T>;
    using cxvalue_t_ = value_type;
    using span_t_ = coda_oss::span<const cxvalue_t_>;
    using axis_t_ = typename cxvalue_t_::value_type; // i.e., float
    using const_iterator = ComplexViewConstIterator<axis_t_>;
    using iterator = const_iterator;

    ComplexInterleavedView() = delete;
    ~ComplexInterleavedView() = default;
    explicit ComplexInterleavedView(span_t_ data) : data_(data)
    {
    }
    ComplexInterleavedView(const ComplexInterleavedView&) = default;
    ComplexInterleavedView& operator=(const ComplexInterleavedView&) = default;
    ComplexInterleavedView(ComplexInterleavedView&&) = default;
    ComplexInterleavedView& operator=(ComplexInterleavedView&&) = default;

    // This class is almost the same as std::span<const std::complex<T>> ... but
    // don't provide data() as ComplexParallelView stores complex numbers in two
    // parallel arrays. Also see reals(), imags() and values(); below.
    // const value_type* data() const noexcept = delete;

    constexpr auto real(size_type idx) const noexcept
    {
        return data_[idx].real();
    }
    constexpr auto imag(size_type idx) const noexcept
    {
        return data_[idx].imag();
    }

    // const& vs. value makes little difference here as cxvalue_type is
    // std::complex<> (copying one pointer vs. two doubles).  But this
    // provides a (subtle) way for clients to know what "view style" they're
    // using, should that be needed; it also more closely matches std::span<>.
    constexpr const cxvalue_t_& index(size_type idx) const noexcept // i.e., std::complex<float>
    {
        return data_[idx];
    }
    constexpr const auto& operator[](size_type idx) const noexcept
    {
        return index(idx);
    }

    constexpr size_type size() const noexcept
    {
        return data_.size();
    }
    
    const_iterator begin() const
    {
        // not data_.begin(), we want our "generic" ComplexViewConstIterator iterator
        return ComplexViewConstIterator<axis_t_>(data_);
    }
    const_iterator end() const
    {
        return begin().end_();
    }

private:
    template <typename TAxisFunc>
    auto copy_axis(TAxisFunc axis) const
    {
        std::vector<axis_t_> retval(size());
        for (size_t i = 0; i < size(); i++)
        {
            retval[i] = (this->*axis)(i);  // call via pointer to member function
        }
        return retval;
    }

public:
    auto reals() const
    {
        return copy_axis(&ComplexInterleavedView::real);
    }
    auto imags() const
    {
        return copy_axis(&ComplexInterleavedView::imag);
    }
    auto values() const
    {
        return std::vector<cxvalue_t_>(data_.begin(), data_.end());
    }

private:
    span_t_ data_; // i.e., std::span<const std::complex<float>>
};
template <typename T>
inline auto make_ComplexInterleavedView(coda_oss::span<const std::complex<T>> s)
{
    return ComplexInterleavedView<T>(s);
}
template <typename TVectorLike>
inline auto make_ComplexInterleavedView(const TVectorLike& v)
{
    using cxvalue_t = typename TVectorLike::value_type; // i.e., std::complex<float>
    using span_t = coda_oss::span<const cxvalue_t>;

    return make_ComplexInterleavedView(span_t(v.data(), v.size()));
}

// This is not the "normal" storage in C++ (that would be "interleaved" via std::complex, above).  However,
// it's one more likely to need additional "support."
template <typename T>
struct ComplexParallelView final // Two parallel arrays, absolutely nothing to to with multi-treading.
{
    using size_type = size_t;
    using value_type = T;
    using cxvalue_t_ = std::complex<value_type>;
    using axis_t_ = typename cxvalue_t_::value_type;  // i.e., float
    using span_t_ = coda_oss::span<const value_type>;
    using const_iterator = ComplexViewConstIterator<axis_t_>;
    using iterator = const_iterator;

    ComplexParallelView() = delete;
    ~ComplexParallelView() = default;
    ComplexParallelView(span_t_ reals, span_t_ imags) : reals_(reals), imags_(imags)
    {
        if (reals_.size() != imags_.size())
        {
            throw std::invalid_argument("real and imag must be the same size.");
        }
    }
    ComplexParallelView(const T* reals, const T* imags, size_t sz) :
        ComplexParallelView(span_t_(reals, sz), span_t_(imags, sz))
    {
    }
    ComplexParallelView(const ComplexParallelView&) = default;
    ComplexParallelView& operator=(const ComplexParallelView&) = default;
    ComplexParallelView(ComplexParallelView&&) = default;
    ComplexParallelView& operator=(ComplexParallelView&&) = default;

    constexpr auto real(size_type idx) const noexcept
    {
        return reals_[idx];
    }
    constexpr auto imag(size_type idx) const noexcept
    {
        return imags_[idx];
    }

    constexpr cxvalue_t_ index(size_type idx) const noexcept // i.e., std::complex<float>
    {
        // Note that this is a COPY because the "real" and "imag" parts MUST be
        // next to each other. https://en.cppreference.com/w/cpp/numeric/complex
        return cxvalue_t_(real(idx), imag(idx));
    }
    constexpr auto operator[](size_type idx) const noexcept
    {
        return index(idx);
    }

    constexpr size_type size() const noexcept
    {
        return reals_.size();
    }

    iterator begin() const
    {
        return iterator(*this);
    }
    iterator end() const
    {
        return begin().end_();
    }

    auto reals() const
    {
        return std::vector<value_type>(reals_.begin(), reals_.end());
    }
    auto imags() const
    {
        return std::vector<value_type>(imags_.begin(), imags_.end());
    }
    auto values() const
    {
        std::vector<cxvalue_t_> retval(size());
        for (size_t i = 0; i < size(); i++)
        {
            retval[i] = (*this)[i];
        }
        return retval;
    }

private:
    span_t_ reals_; // i.e., std::span<const float>
    span_t_ imags_;
};
template <typename T>
inline auto make_ComplexParallelView(coda_oss::span<const T> reals, coda_oss::span<const T> imags)
{
    return ComplexParallelView<T>(reals, imags);
}
template <typename TVectorLike>
inline auto make_ComplexParallelView(const TVectorLike& reals_, const TVectorLike& imags_)
{
    using value_type = typename TVectorLike::value_type;  // i.e., float
    using cxvalue_t = std::complex<value_type>; // i.e., std::complex<float>
    using axis_t = typename cxvalue_t::value_type;  // i.e., float
    using span_t = coda_oss::span<const axis_t>;

    const span_t reals(reals_.data(), reals_.size());
    const span_t imags(imags_.data(), imags_.size());
    return make_ComplexParallelView(reals, imags);
}

} 

#endif  // CODA_OSS_mem_ComplexView_h_INCLUDED_

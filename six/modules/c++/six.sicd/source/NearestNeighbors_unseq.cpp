/* =========================================================================
* This file is part of six.sicd-c++
* =========================================================================
*
* (C) Copyright 2021, Maxar Technologies, Inc.
*
* six.sicd-c++ is free software; you can redistribute it and/or modify
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
#include "six/sicd/NearestNeighbors.h"

#include <math.h>
#include <assert.h>

#include <cassert>
#include <memory>
#include <algorithm>
#include <functional>
#include <std/numbers>
#include <std/mdspan>

#include <gsl/gsl.h>
#include <sys/Span.h>
#include <mt/Algorithm.h>

#include "six/sicd/Exports.h"

#undef min
#undef max

using zfloat = six::zfloat;
using AMP8I_PHS8I = six::AMP8I_PHS8I_t;

#if SIX_sicd_has_sisd

using sisd_intv = int;
using sisd_intv_mask = bool;

using sisd_floatv = float;
using sisd_floatv_mask = bool;

constexpr auto sisd_elements_per_iteration = 1;
inline size_t size(sisd_intv) noexcept { return sisd_elements_per_iteration; }
inline ptrdiff_t ssize(sisd_intv v) noexcept { return gsl::narrow<ptrdiff_t>(size(v)); }
inline size_t size(sisd_floatv) noexcept { return sisd_elements_per_iteration; }
inline ptrdiff_t ssize(sisd_floatv v) noexcept { return gsl::narrow<ptrdiff_t>(size(v)); }

using sisd_zfloatv = std::complex<float>;
inline size_t size(sisd_zfloatv) noexcept { return sisd_elements_per_iteration; }
inline ptrdiff_t ssize(sisd_zfloatv v) noexcept { return gsl::narrow<ptrdiff_t>(size(v)); }

inline void copy_from(std::span<const float> p, sisd_floatv& result)
{
    assert(p.size() == size(result));
    result = p[0];
}
inline void copy_from(std::span<const zfloat> p, sisd_zfloatv& result)
{
    assert(p.size() == size(result));
    result = p[0];
}

static inline sisd_intv roundi(sisd_floatv v)  // match vcl::roundi()
{
    return gsl::narrow_cast<sisd_intv>(std::round(v));
}

template<size_t N>
inline auto lookup(sisd_intv indexv, const std::array<zfloat, N>& phase_directions)
{
    return phase_directions[indexv];
}
inline auto lookup(sisd_intv indexv, std::span<const float> magnitudes)
{
    assert(magnitudes.size() == six::AmplitudeTableSize);

    // The index may be out of range. This is expected because `i` might be "don't care."
    if ((indexv >= 0) && (indexv < std::ssize(magnitudes)))
    {
        return magnitudes[indexv];
    }
    return NAN; // propogate "don't care"
}

static inline auto simd_select(bool test, sisd_intv t, sisd_intv f)
{
    return test ? t : f;
}
static inline auto simd_select(bool test, sisd_floatv t, sisd_floatv f)
{
    return test ? t : f;
}

inline bool any_of(sisd_intv m)
{
    return m != 0;
}
inline bool all_of(sisd_intv m)
{
    return m != 0;
}

#endif // SIX_sicd_has_sisd

#if SIX_sicd_has_VCL

#define VCL_NAMESPACE vcl
#if _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4100) // '...': unreferenced formal parameter
#pragma warning(disable: 4127) // conditional expression is constant
#pragma warning(disable: 4244) // '...': conversion from '...' to '...', possible loss of data
#pragma warning(disable: 4723) // potential divide by 0
#endif
#include "six/sicd/vectorclass/version2/vectorclass.h"
#include "six/sicd/vectorclass/version2/vector.h"
#include "six/sicd/vectorclass/version2/vectormath_trig.h"
#include "six/sicd/vectorclass/complex/complexvec1.h"
#if _MSC_VER
#pragma warning(pop)
#endif

constexpr auto vcl_elements_per_iteration = 8;
using vcl_intv = vcl::Vec<vcl_elements_per_iteration, int32_t>;
using vcl_floatv = vcl::Vec<vcl_elements_per_iteration, float>;

template<size_t N, typename T>
inline ptrdiff_t ssize(const vcl::Vec<N, T>& v) noexcept
{
    return v.size();
}

using vcl_zfloatv = vcl::Complex8f;
auto real(const vcl_zfloatv& z)
{
    return z.real();
}
auto imag(const vcl_zfloatv& z)
{
    return z.imag();
}
inline size_t size(const vcl_zfloatv& z) noexcept
{
    return z.size();
}
inline int ssize(const vcl_zfloatv& z) noexcept
{
    return z.size();
}

inline bool any_of(const vcl_intv& m)
{
    return horizontal_or(m);
}

inline void copy_from(std::span<const float> p, vcl_floatv& result)
{
    assert(p.size() == result.size());
    result.load(p.data());
}
inline void copy_from(std::span<const zfloat> p, vcl_zfloatv& result)
{
    // https://en.cppreference.com/w/cpp/numeric/complex
    // > For any pointer to an element of an array of `std::complex<T>` named `p` and any valid array index `i`, ...
    // > is the real part of the complex number `p[i]`, ...
    result.load(reinterpret_cast<const float*>(p.data()));
}

template<size_t N>
inline auto lookup(const vcl_intv& indexv, const std::array<zfloat, N>& phase_directions)
{
    return vcl::lookup<N>(indexv, phase_directions.data());
}
inline auto lookup(const vcl_intv& indexv, std::span<const float> magnitudes)
{
    assert(magnitudes.size() == six::AmplitudeTableSize);
    return vcl::lookup<six::AmplitudeTableSize>(indexv, magnitudes.data());
}

#endif // SIX_sicd_has_VCL

#if SIX_sicd_has_ximd
#ifndef SIX_Ximd_ENABLED
#define SIX_Ximd_ENABLED 1
#endif
#include "unseq_Ximd.h"

template<typename T>
using ximd = six::ximd::Ximd<T>;

using ximd_intv = ximd<int>;
using ximd_intv_mask = six::ximd::ximd_mask;

using ximd_floatv = ximd<float>;
using ximd_floatv_mask = six::ximd::ximd_mask;

constexpr auto ximd_elements_per_iteration = ximd_floatv::size();

template<typename T>
static inline ptrdiff_t ssize(const ximd<T>& v) noexcept
{
    return gsl::narrow<ptrdiff_t>(v.size());
}

template<typename TGenerator, typename T>
static inline auto generate(TGenerator&& generator, ximd<T>)
{
    return ximd<T>(generator, nullptr);
}

static inline void copy_from(std::span<const float> p, ximd_floatv& result)
{
    assert(p.size() == result.size());
    result.copy_from(p.data());
}

template<typename TTest, typename TResult>
static auto ximd_simd_select_(const TTest& test, const TResult& t, const TResult& f)
{
    TResult retval;
    for (size_t i = 0; i < test.size(); i++)
    {
        retval[i] = test[i] ? t[i] : f[i];
    }
    return retval;
}
static inline auto simd_select(const ximd_intv_mask& test, const  ximd_intv& t, const  ximd_intv& f)
{
    return ximd_simd_select_(test, t, f);
}
static inline auto simd_select(const ximd_floatv_mask& test, float t, float f)
{
    return ximd_simd_select_(test, ximd_floatv{ t }, ximd_floatv{ f });
}

#endif // SIX_sicd_has_ximd

#if SIX_sicd_has_simd

#include <experimental/simd>

// https://en.cppreference.com/w/cpp/experimental/simd/simd_cast
// > The TS specification is missing `simd_cast` and `static_simd_cast` overloads for `simd_mask`. ...
namespace stdx
{
    using namespace std::experimental;
    using namespace std::experimental::__proposed;
}

using simd_intv = stdx::simd<int>;
using simd_intv_mask = typename simd_intv::mask_type;

using simd_floatv = stdx::rebind_simd<float, simd_intv>;
using simd_floatv_mask = typename simd_floatv::mask_type;

constexpr auto simd_elements_per_iteration = simd_floatv::size();

template<typename T>
static inline auto ssize(const simd<T>& v) noexcept
{
    return gsl::narrow<int>(v.size());
}

template<typename TGenerator, typename T>
static inline auto generate(TGenerator&& generator, simd<T>)
{
    return simd<T>(generator);
}

static inline void copy_from(std::span<const float> p, simd_floatv& result)
{
    assert(p.size() == result.size());
    result.copy_from(p.data(), stdx::element_aligned);
}

template<typename TTest, typename TResult>
static auto simd_simd_select_(const TTest& test, const TResult& t, const TResult& f)
{
    // https://en.cppreference.com/w/cpp/experimental/simd/where_expression
    // > ... All other elements are left unchanged.
    TResult retval;
    where(test, retval) = t;
    where(!test, retval) = f;
    return retval;
}
template<typename TMask>
static inline auto simd_select(const TMask& test_, const  simd_intv& t, const  simd_intv& f)
{
    //const auto test = test_.__cvt(); // https://github.com/VcDevel/std-simd/issues/41
    const auto test = stdx::static_simd_cast<simd_intv_mask>(test_); // https://github.com/VcDevel/std-simd/issues/41
    return simd_simd_select_(test, t, f);
}
template<typename TMask>
static inline auto simd_select(const TMask& test_, const  simd_floatv& t, const  simd_floatv& f)
{
    //const auto test = test_.__cvt(); // https://github.com/VcDevel/std-simd/issues/41
    const auto test = stdx::static_simd_cast<simd_floatv_mask>(test_); // https://github.com/VcDevel/std-simd/issues/41
    return simd_simd_select_(test, t, f);
}

#endif // SIX_sicd_has_simd

#if SIX_sicd_has_ximd || SIX_sicd_has_simd
// Manage a SIMD complex as an array of two SIMDs
template<typename FloatV>
using zfloatv = std::array<FloatV, 2>;

template<typename FloatV>
inline auto& real(zfloatv<FloatV>& z) noexcept
{
    return z[0];
}
template<typename FloatV>
inline const auto& real(const zfloatv<FloatV>& z) noexcept
{
    return z[0];
}
template<typename FloatV>
inline auto& imag(zfloatv<FloatV>& z) noexcept
{
    return z[1];
}
template<typename FloatV>
inline const auto& imag(const zfloatv<FloatV>& z) noexcept
{
    return z[1];
}
template<typename FloatV>
inline size_t size(const zfloatv<FloatV>& z) noexcept
{
    auto retval = real(z).size();
    assert(retval == imag(z).size());
    return retval;
}
template<typename FloatV>
inline ptrdiff_t ssize(const zfloatv<FloatV>& z) noexcept
{
    return gsl::narrow<ptrdiff_t>(size(z));
}
#if SIX_sicd_has_ximd
using ximd_zfloatv = zfloatv<ximd_floatv>;
#endif
#if SIX_sicd_has_simd
using simd_zfloatv = zfloatv<simd_floatv>;
#endif

template <typename FloatV,
    typename TGeneratorReal, typename TGeneratorImag>
static inline void zgenerate(TGeneratorReal&& generate_real, TGeneratorImag&& generate_imag, zfloatv<FloatV>& result)
{
    real(result) = generate(generate_real, FloatV{});
    imag(result) = generate(generate_imag, FloatV{});
}

template<typename FloatV>
static inline void copy_from(std::span<const zfloat> mem, zfloatv<FloatV>& result)
{
    const auto generate_real = [&](size_t i) { return mem[i].real(); };
    const auto generate_imag = [&](size_t i) { return mem[i].imag(); };
    zgenerate(generate_real, generate_imag, result);
}

// There are slicker ways of doing this, but `std::enable_if` is complex.  This
// is simple and easy to understand.  Simplify with concepts in C++20?

template<typename IntV, typename FloatV>
static auto lround_(const FloatV& v)
{
    return lround(v);
}
#if SIX_sicd_has_ximd
static inline auto roundi(const ximd_floatv& v)  // match vcl::roundi()
{
    return lround_<ximd_intv>(v);
}
#endif
#if SIX_sicd_has_simd
static inline auto roundi(const simd_floatv& v)  // match vcl::roundi()
{
    return lround_<simd_intv>(v);
}
#endif

template<typename ZFloatV, typename IntV, size_t N>
static auto lookup_(const IntV& indexv, const std::array<zfloat, N>& phase_directions)
{
    // It seems that the "generator" constuctor is called with SIMD instructions.
    // https://en.cppreference.com/w/cpp/experimental/simd/simd/simd
    // > The calls to `generator` are unsequenced with respect to each other.

    const auto generate_real = [&](size_t i) {
        const auto i_ = indexv[i];
        return phase_directions[i_].real();
    };
    const auto generate_imag = [&](size_t i) {
        const auto i_ = indexv[i];
        return phase_directions[i_].imag();
    };
    ZFloatV retval;
    zgenerate(generate_real, generate_imag, retval);
    return retval;
}
#if SIX_sicd_has_ximd
template<size_t N>
static inline auto lookup(const ximd_intv& indexv, const std::array<zfloat, N>& phase_directions)
{
    return lookup_<ximd_zfloatv>(indexv, phase_directions);
}
#endif
#if SIX_sicd_has_simd
template<size_t N>
static inline auto lookup(const simd_intv& indexv, const std::array<zfloat, N>& phase_directions)
{
    return lookup_<simd_zfloatv>(indexv, phase_directions);
}
#endif

template<typename FloatV, typename IntV>
static auto lookup_(const IntV& indexv, std::span<const float> floats)
{
    const auto lookup_f = [&](size_t i) {
        auto i_ = indexv[i];

        // The index may be out of range. This is expected because `i` might be "don't care."
        if (i_ != -1)
        {
            i_ = static_cast<uint8_t>(i_);
            return floats[i_];
        }
        return NAN; // propogate "don't care"
    };
    return generate(lookup_f, FloatV{});
}
#if SIX_sicd_has_ximd
static inline auto lookup(const ximd_intv& indexv, std::span<const float> magnitudes)
{
    return lookup_<ximd_floatv>(indexv, magnitudes);
}
#endif
#if SIX_sicd_has_simd
static inline auto lookup(const simd_intv& indexv, std::span<const float> magnitudes)
{
    return lookup_<simd_floatv>(indexv, magnitudes);
}
#endif
#endif // SIX_sicd_has_ximd || SIX_sicd_has_simd

/******************************************************************************************************/

template<typename ZFloatV>
inline auto arg(const ZFloatV& z)
{
    // https://en.cppreference.com/w/cpp/numeric/complex/arg
    // > `std::atan2(std::imag(z), std::real(z))`
    return atan2(imag(z), real(z));
}

template<typename ZFloatV>
static auto getPhase(const ZFloatV& v, float phase_delta)
{
    // Phase is determined via arithmetic because it's equally spaced.
    // There's an intentional conversion to zero when we cast 256 -> uint8. That wrap around
    // handles cases that are close to 2PI.
    constexpr auto two_pi = std::numbers::pi_v<float> *2.0f;

    auto phase = arg(v);
    phase += simd_select(phase < 0.0f, two_pi, 0.0f); // Wrap from [0, 2PI]
    return roundi(phase / phase_delta);
}

// https://en.cppreference.com/w/cpp/algorithm/lower_bound
template<typename IntV, typename FloatV>
inline auto lower_bound(std::span<const float> magnitudes, const FloatV& v)
{
    assert(magnitudes.size() == six::AmplitudeTableSize);

    IntV first = gsl::narrow<int>(std::distance(magnitudes.begin(), magnitudes.begin()));
    const IntV last = gsl::narrow<int>(std::distance(magnitudes.begin(), magnitudes.end()));

    auto count = last - first;
    while (any_of(count > 0))
    {
        auto it = first;
        const auto step = count / 2;
        it += step; // std::advance(it, step);

        //if (magnitudes[it] < value)
        //{
        //    first = ++it;
        //    count -= step + 1;
        //}
        //else
        //    count = step;
        const auto c = lookup(it, magnitudes); // magnituides[it]

        auto next = it; ++next; // ... ++it;
        auto advance = count; advance -= step + 1;  // ...  -= step + 1;

        const auto test = (count > 0) && (c < v);
        first = simd_select(test, ++it, first); // first = ++it;
        auto count_ = count; count_ -= step + 1;  // count -= step + 1;
        count = simd_select(test, count_, step); // `count -= step + 1` —<OR>— `count = step`
    }
    return first;
}

/*!
 * Find the nearest element given an iterator range.
 * @param value query value
 * @return index of nearest value within the iterator range.
 */
template<typename IntV, typename FloatV>
static auto nearest(std::span<const float> magnitudes, const FloatV& value)
{
    // see `::nearest()` in **NearestNeighbors.cpp**
    assert(magnitudes.size() == six::AmplitudeTableSize);

    const auto it = ::lower_bound<IntV>(magnitudes, value);

    const IntV zero = 0;
    if (all_of(it == zero))
    {
        return zero;
    }

    const auto prev_it = it - 1; // const auto prev_it = std::prev(it);
    const IntV end = gsl::narrow<int>(magnitudes.size());
    if (all_of(it == end))
    {
        return prev_it;
    }

    const auto v0 = value - lookup(prev_it, magnitudes); // value - *prev_it
    const auto v1 = lookup(it, magnitudes) - value; // *it - value
    auto retval = simd_select(it == 0, zero, // if (it == begin) return 0;
        simd_select(it == end, prev_it,  // it == end ? prev_it  : ...
            simd_select(v0 <= v1, prev_it, it) //  (value - *prev_it <= *it - value ? prev_it : it);
        ));
    return retval;
}

template<typename IntV, typename FloatV, typename ZFloatV>
static auto find_nearest(std::span<const float> magnitudes,
    const FloatV& phase_direction_real, const FloatV& phase_direction_imag,
    const ZFloatV& v)
{
    // We have to do a 1D nearest neighbor search for magnitude.
    // But it's not the magnitude of the input complex value - it's the projection of
    // the complex value onto the ray of candidate magnitudes at the simd_selected phase.
    // i.e. dot product.
    const auto projection = (phase_direction_real * real(v)) + (phase_direction_imag * imag(v));
    //assert(std::abs(projection - std::abs(v)) < 1e-5); // TODO ???
    return nearest<IntV>(magnitudes, projection);
}

#if SIX_sicd_has_VCL || SIX_sicd_has_sisd
template<typename IntV, typename FloatV>
static auto lookup_and_find_nearest(const six::sicd::details::ComplexToAMP8IPHS8I& converter,
    const IntV& phase, const  FloatV& v)
{
    const auto phase_direction_real = lookup(phase, converter.get_phase_directions().real);
    const auto phase_direction_imag = lookup(phase, converter.get_phase_directions().imag);
    return ::find_nearest<IntV>(converter.magnitudes(), phase_direction_real, phase_direction_imag, v);
}
#endif
#if SIX_sicd_has_ximd || SIX_sicd_has_simd
template<typename IntV, typename FloatV>
static auto lookup_and_find_nearest(const six::sicd::details::ComplexToAMP8IPHS8I& converter,
    const IntV& phase, const zfloatv<FloatV>& v)
{
    const auto& phase_directions = converter.get_phase_directions().value;
    const auto phase_direction = lookup(phase, phase_directions);
    return ::find_nearest<IntV>(converter.magnitudes(), real(phase_direction), imag(phase_direction), v);
}
#endif

#if SIX_sicd_ComplexToAMP8IPHS8I_unseq

/******************************************************************************************************/

template<typename IntV>
struct AMP8I_PHS8I_unseq final
{
    IntV amplitude;
    IntV phase;
};

// This isn't someplace more widely available because:
// 1. there is no standard iteration for `mdspan` (not even in C++23), and
// 2. the particular needs are quite varied and require a lot of parameterization.
//
// Keep things simple for now and do this "in place."
template<typename T, typename TExtents, typename TValueType = std::span<T>>
struct mdspan_iterator final
{
    using difference_type = std::ptrdiff_t;
    using size_type = size_t;
    using value_type = TValueType;
    using pointer = TValueType*;
    using const_pointer = const TValueType*;
    using reference = TValueType;
    using const_reference = const TValueType;
    using iterator_category = std::random_access_iterator_tag;

    mdspan_iterator() = default;
    mdspan_iterator(coda_oss::mdspan<T, TExtents> md, difference_type row = 0) : md_(md), row_(row) {}

    value_type operator*() const
    {
        return make_span();
    }

    mdspan_iterator& operator++() noexcept
    {
        ++row_;
        return *this;
    }

    mdspan_iterator& operator+=(const difference_type n) noexcept
    {
        row_ += n;
        return *this;
    }
    mdspan_iterator operator+(const difference_type n) const noexcept
    {
        auto ret = *this;
        ret += n;
        return ret;
    }

    difference_type operator-(const mdspan_iterator& rhs) const noexcept
    {
        return row_ - rhs.row_;
    }

    bool operator!=(const mdspan_iterator& rhs) const noexcept
    {
        return row_ != rhs.row_;
    }

private:
    coda_oss::mdspan<T, TExtents> md_;
    difference_type row_ = 0;

    auto make_span() const
    {
        // We know our mdspan is contiguous, so this is OK
        auto&& v = md_(row_, 0); // beginning of the the current row
        return std::span<T>(&v, md_.extent(1)); // span for the whole row
    }
};
template<typename TValueType, typename T, typename TExtents>
auto begin(coda_oss::mdspan<T, TExtents> md)
{
    return mdspan_iterator<T, TExtents, TValueType>(md, 0);
}
template<typename T, typename TExtents>
auto end(coda_oss::mdspan<T, TExtents> md)
{
    return mdspan_iterator<T, TExtents>(md, md.extent(0));
}

template<typename T, typename TExtents>
using const_mdspan_iterator = mdspan_iterator<std::add_const_t<T>, TExtents>;
template<typename T, typename TExtents>
auto cbegin(coda_oss::mdspan<T, TExtents> md)
{
    return const_mdspan_iterator<T, TExtents>(md, 0);
}
template<typename T, typename TExtents>
auto cend(coda_oss::mdspan<T, TExtents> md)
{
    return const_mdspan_iterator<T, TExtents>(md, md.extent(0));
}

// Inside of `std::transform()`, there is an assignment; something like
// ```
//    *dest = func(*first);
// ```
// By returning our own class from `func()`, we can take control of the assignment operator.
// (Unlike most other operators, `operator=()` *must* be a member-function.)
template<typename IntV>
inline void assign(size_t i, AMP8I_PHS8I& lhs, const AMP8I_PHS8I_unseq<IntV>& rhs)
{
    const auto i_ = gsl::narrow<int>(i);
    lhs.amplitude = gsl::narrow<uint8_t>(rhs.amplitude[i_]);
    lhs.phase = gsl::narrow<uint8_t>(rhs.phase[i_]);
}
#if SIX_sicd_has_sisd
inline void assign(size_t, AMP8I_PHS8I& lhs, const AMP8I_PHS8I_unseq<sisd_intv>& rhs)
{
    lhs.amplitude = gsl::narrow<uint8_t>(rhs.amplitude);
    lhs.phase = gsl::narrow<uint8_t>(rhs.phase);
}
#endif

struct mdspan_iterator_value final
{
    std::span<AMP8I_PHS8I> p_;

    mdspan_iterator_value(std::span<AMP8I_PHS8I> s) : p_(s) {}
    template<typename IntV>
    mdspan_iterator_value& operator=(const AMP8I_PHS8I_unseq<IntV>& other) {
        //assert(p_.size() <= size(other.amplitude));
        for (size_t i = 0; i < p_.size(); i++)
        {
            assign(i, p_[i], other);
        }
        return *this;
    }
};

template<typename ZFloatV>
using IntV = decltype(::getPhase(ZFloatV{}, 0.0f));

template<typename ZFloatV>
auto six::sicd::NearestNeighbors::unseq_nearest_neighbors(std::span<const zfloat> p) const // TODO: std::span<T, N> ... ?  The compiler can sometimes do better optimization with fixed-size structures.
{
    ZFloatV v;
    assert(p.size() == size(v));
    copy_from(p, v);

    using intv_t = IntV<ZFloatV>;
    AMP8I_PHS8I_unseq<intv_t> retval;
    retval.phase = ::getPhase(v, converter_.phase_delta());
    retval.amplitude = lookup_and_find_nearest(converter_, retval.phase, v);

    return retval;
}

template<size_t elements_per_iteration>
static void finish_nearest_neighbors_unseq(const six::sicd::NearestNeighbors& impl,
    std::span<const zfloat> inputs, std::span<AMP8I_PHS8I> results)
{
    // Then finish off anything left
    const auto remaining_count = inputs.size() % elements_per_iteration;
    if (remaining_count > 0)
    {
        const auto remaining_index = inputs.size() - remaining_count;
        const auto remaining_inputs = sys::make_span(&(inputs[remaining_index]), remaining_count);
        const auto remaining_results = sys::make_span(&(results[remaining_index]), remaining_count);
        impl.nearest_neighbors_seq(remaining_inputs, remaining_results);
    }
}

template<typename ZFloatV, int elements_per_iteration>
void six::sicd::NearestNeighbors::nearest_neighbors_T(execution_policy policy,
    std::span<const zfloat> inputs, std::span<AMP8I_PHS8I> results) const
{
    // View the data as chunks of *elements_per_iteration*.  This allows iterating
    // to go *elements_per_iteration* at a time; and each chunk can be processed
    // using `nearest_neighbors_unseq_T()`, above.
    using extents_t = coda_oss::dextents<size_t, 2>; // two dimensions: M×N
    const extents_t extents{ inputs.size() / elements_per_iteration, elements_per_iteration };
    const coda_oss::mdspan<const zfloat, extents_t> md_inputs(inputs.data(), extents);
    assert(md_inputs.size() <= inputs.size());
    auto const b = cbegin(md_inputs);
    auto const e = cend(md_inputs);

    const coda_oss::mdspan<AMP8I_PHS8I, extents_t> md_results(results.data(), extents);
    assert(md_results.size() <= results.size());
    auto const d = begin<mdspan_iterator_value>(md_results);

    const auto func = [&](const auto& v)
    {
        return unseq_nearest_neighbors<ZFloatV>(v);
    };

    if (policy == execution_policy::unseq)
    {
        std::transform(/*std::execution::unseq,*/ b, e, d, func);
    }
    else if (policy == execution_policy::par_unseq)
    {
        //std::transform(std::execution::par_unseq, b, e, d, func);
        mt::Transform_par(b, e, d, func);
    }
    else
    {
        throw std::logic_error("Unsupported execution_policy");
    }

    // Then finish off anything left
    finish_nearest_neighbors_unseq<elements_per_iteration>(*this, inputs, results);
}

#if SIX_sicd_has_simd
static const std::string unseq_simd = "simd";
#endif
#if SIX_sicd_has_VCL
static const std::string unseq_vcl = "vcl";
#endif
#if SIX_sicd_has_ximd
static const std::string unseq_ximd = "ximd";
#endif
#if SIX_sicd_has_sisd
static const std::string unseq_sisd = "sisd";
#endif
static std::string nearest_neighbors_unseq_ =
#if SIX_sicd_has_simd
unseq_simd;
#elif SIX_sicd_has_VCL
unseq_vcl;
#elif SIX_sicd_has_ximd
unseq_ximd;
#elif SIX_sicd_has_sisd
unseq_sisd;
#else
#error "Don't know how to implement six_sicd_set_nearest_neighbors_unseq()"
#endif
std::string SIX_SICD_API six_sicd_set_nearest_neighbors_unseq(std::string unseq)
{
    // We'll "validate" when the string is actually used; this minimizes
    // the places that need updating when things change.
    auto retval = nearest_neighbors_unseq_;
    nearest_neighbors_unseq_ = std::move(unseq);
    return retval;
}

void six::sicd::NearestNeighbors::nearest_neighbors_(execution_policy policy,
    std::span<const zfloat> inputs, std::span<AMP8I_PHS8I_t> results) const
{
    #if CODA_OSS_DEBUG && SIX_sicd_has_sisd
    // If we're in UNSEQ code with a sequential execution policy, then use
    // the generic UNSEQ code ... but with non-SIMD types.
    //
    // Note that `nearest_neighbors_T()` is expecting an UNSEQ policy.
    if (policy == execution_policy::seq)
    {
        return nearest_neighbors_T<sisd_zfloatv, sisd_elements_per_iteration>(execution_policy::unseq, inputs, results);
    }
    if (policy == execution_policy::par)
    {
        return nearest_neighbors_T<sisd_zfloatv, sisd_elements_per_iteration>(execution_policy::par_unseq, inputs, results);
    }
    #endif // CODA_OSS_DEBUG && SIX_sicd_has_sisd

    // This is very simple as it's only used for unit-testing
    const auto& unseq = ::nearest_neighbors_unseq_;
    #if SIX_sicd_has_simd
    if (unseq == unseq_simd)
    {
        return nearest_neighbors_T<simd_zfloatv, simd_elements_per_iteration>(policy, inputs, results);
    }
    #endif
    #if SIX_sicd_has_VCL
    if (unseq == unseq_vcl)
    {
        return nearest_neighbors_T<vcl_zfloatv, vcl_elements_per_iteration>(policy, inputs, results);
    }
    #endif
    #if SIX_sicd_has_ximd
    // XIMD is a hack just for development, it shouldn't normally be enabled
    // for a release build; only use it for debug.
    if (unseq == unseq_ximd)
    {
        #if CODA_OSS_DEBUG
        // The XIMD implementations are dramatically slower when running unittests;
        // that's mostly because there's lots of IO. This is the  "more interesting" code path.
        return nearest_neighbors_T<ximd_zfloatv, ximd_elements_per_iteration>(policy, inputs, results);

        #else
        // Use the corresponding "regular" (not UNSEQ) policy, this is either "par" or "seq".
        if (policy == execution_policy::par_unseq)
        {
            return nearest_neighbors_par(inputs, results);
        }
        if (policy == execution_policy::unseq)
        {
            return nearest_neighbors_seq(inputs, results);
        }
        // We shouldn't be here otherwise; throw the exception, below.
        #endif // CODA_OSS_DEBUG
    }
    #endif // SIX_sicd_has_ximd

    #if SIX_sicd_has_sisd
    if (unseq == unseq_sisd)
    {
        return nearest_neighbors_T<sisd_zfloatv, sisd_elements_per_iteration>(policy, inputs, results);
    }
    #endif

    throw std::logic_error("Don't know how to implement nearest_neighbors_() for unseq=" + unseq);
}
void six::sicd::NearestNeighbors::nearest_neighbors_unseq(std::span<const zfloat> inputs, std::span<AMP8I_PHS8I> results) const
{
    nearest_neighbors_(execution_policy::unseq, inputs, results);
}
void six::sicd::NearestNeighbors::nearest_neighbors_par_unseq(std::span<const zfloat> inputs, std::span<AMP8I_PHS8I> results) const
{
    nearest_neighbors_(execution_policy::par_unseq, inputs, results);
}

#if SIX_sicd_has_sisd
void six::sicd::NearestNeighbors::unseq_nearest_neighbors(execution_policy policy, std::span<const zfloat> inputs, std::span<AMP8I_PHS8I> results) const
{
    nearest_neighbors_(policy, inputs, results);
}
#endif // SIX_sicd_has_sisd

#endif // SIX_sicd_ComplexToAMP8IPHS8I_unseq

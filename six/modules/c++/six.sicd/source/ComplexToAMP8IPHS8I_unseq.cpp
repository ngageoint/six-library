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
#include "six/AmplitudeTable.h"

#include <math.h>
#include <assert.h>

#include <cassert>
#include <memory>
#include <std/numbers>
#include <algorithm>
#include <functional>
#include <type_traits>

#include <gsl/gsl.h>
#include <math/Utilities.h>
#include <units/Angles.h>
#include <sys/Span.h>
#include <mt/Algorithm.h>

#include "six/sicd/Utilities.h"

#undef min
#undef max

using zfloat = six::zfloat;
using AMP8I_PHS8I = six::AMP8I_PHS8I_t;

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
#include "six/sicd/vectorclass/version2/vectormath_trig.h"
#include "six/sicd/vectorclass/complex/complexvec1.h"
#if _MSC_VER
#pragma warning(pop)
#endif

using vcl_intv = vcl::Vec8i; 
using vcl_floatv = vcl::Vec8f;
constexpr auto vcl_elements_per_iteration = vcl_floatv::size();

inline int ssize(const vcl_intv& z) noexcept
{
    return z.size();
}
inline int ssize(const vcl_floatv& z) noexcept
{
    return z.size();
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

inline auto arg(const vcl_zfloatv& z)
{
    // https://en.cppreference.com/w/cpp/numeric/complex/arg
    // > `std::atan2(std::imag(z), std::real(z))`
    return atan2(z.imag(), z.real()); // arg()
}

template<typename T>
inline auto if_add(const T& f, const vcl_floatv& a, float b)
{
    return vcl::if_add(f, a, b);
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
inline auto copy_from(std::span<const zfloat> p, vcl_zfloatv& result)
{
    // https://en.cppreference.com/w/cpp/numeric/complex
    // > For any pointer to an element of an array of `std::complex<T>` named `p` and any valid array index `i`, ...
    // > is the real part of the complex number `p[i]`, ...
    result.load(reinterpret_cast<const float*>(p.data()));
}

template<size_t N>
inline auto lookup(const vcl_intv& zindex, const std::array<zfloat, N>& phase_directions)
{
    return vcl::lookup<N>(zindex, phase_directions.data());
}
inline auto lookup(const vcl_intv& zindex, std::span<const float> magnitudes)
{
    assert(magnitudes.size() == six::AmplitudeTableSize);
    return vcl::lookup<six::AmplitudeTableSize>(zindex, magnitudes.data());
}

#endif // SIX_sicd_has_VCL

#if SIX_sicd_has_valarray
#include <valarray>

using valarray_intv = std::valarray<int>;
using valarray_intv_mask = std::valarray<bool>;

using valarray_floatv = std::valarray<float>;
using valarray_floatv_mask = std::valarray<bool>;

constexpr auto valarray_elements_per_iteration = 4; // valarray is not fixed size

template<typename T>
static inline auto ssize(const std::valarray<T>& v) noexcept
{
    return gsl::narrow<int>(v.size());
}

template <typename TValArray, typename G>
static auto valarray_generate(G&& generator) noexcept
{
    TValArray retval(valarray_elements_per_iteration);
    for (size_t i = 0; i < retval.size(); i++)
    {
        retval[i] = generator(i);
    }
    return retval;
}
template <typename TGenerator>
static inline auto generate(TGenerator&& generator, valarray_intv)
{
    return valarray_generate<valarray_intv>(generator);
}
template <typename TGenerator>
static inline auto generate(TGenerator&& generator, valarray_floatv)
{
    return valarray_generate<valarray_floatv>(generator);
}

// Manage a SIMD complex as an array of two SIMDs
using valarray_zfloatv = std::array<valarray_floatv, 2>;
inline auto& real(valarray_zfloatv& z) noexcept
{
    return z[0];
}
inline const auto& real(const valarray_zfloatv& z) noexcept
{
    return z[0];
}
inline auto& imag(valarray_zfloatv& z) noexcept
{
    return z[1];
}
inline const auto& imag(const valarray_zfloatv& z) noexcept
{
    return z[1];
}
inline size_t size(const valarray_zfloatv& z) noexcept
{
    auto retval = real(z).size();
    assert(retval == imag(z).size());
    return retval;
}
inline auto ssize(const valarray_zfloatv& z) noexcept
{
    return gsl::narrow<int>(size(z));
}

inline auto arg(const valarray_zfloatv& z)
{
    // https://en.cppreference.com/w/cpp/numeric/complex/arg
    // > `std::atan2(std::imag(z), std::real(z))`
    return std::atan2(imag(z), real(z));  // arg()
}

template <typename TGeneratorReal, typename TGeneratorImag>
static inline auto generate(TGeneratorReal&& generate_real, TGeneratorImag&& generate_imag, valarray_zfloatv)
{
    valarray_zfloatv retval;
    real(retval) = generate(generate_real, valarray_floatv{});
    imag(retval) = generate(generate_imag, valarray_floatv{});
    return retval;
}

static inline auto copy_from(std::span<const float> p, valarray_floatv& result)
{
    assert(p.size() == result.size());
    result = valarray_floatv(p.data(), p.size());
}
static inline auto copy_from(std::span<const zfloat> p, valarray_zfloatv& result)
{
    const auto generate_real = [&](size_t i) { return p[i].real(); };
    const auto generate_imag = [&](size_t i) { return p[i].imag(); };
    result = generate(generate_real, generate_imag, valarray_zfloatv{});
}

template<typename TTest, typename TResult>
static auto valarray_select_(const TTest& test, const TResult& t, const TResult& f)
{
    TResult retval;
    for (size_t i = 0; i < test.size(); i++)
    {
        retval[i] = test[i] ? t[i] : f[i];
    }
    return retval;
}
static inline auto select(const valarray_floatv_mask& test, const  valarray_floatv& t, const  valarray_floatv& f)
{
    return valarray_select_(test, t, f);
}
static inline auto select(const valarray_intv_mask& test, const  valarray_intv& t, const  valarray_intv& f)
{
    return valarray_select_(test, t, f);
}

static auto if_add(const valarray_floatv_mask& m, const valarray_floatv& v, typename valarray_floatv::value_type c)
{
    const auto generate_add = [&](size_t i) {
        return m[i] ? v[i] + c : v[i];
    };
    return generate(generate_add, valarray_floatv{});
}

#endif // SIX_sicd_has_valarray

#if SIX_sicd_has_ximd
#ifndef CODA_OSS_Ximd_ENABLE
#define CODA_OSS_Ximd_ENABLE 1
#endif
#include <sys/Ximd.h>

template<typename T>
using ximd = sys::ximd::Ximd<T>;

using ximd_intv = ximd<int>;
using ximd_intv_mask = sys::ximd::ximd_mask;

using ximd_floatv = ximd<float>;
using ximd_floatv_mask = sys::ximd::ximd_mask;

constexpr auto ximd_elements_per_iteration = ximd_floatv::size();

template<typename T>
static inline auto ssize(const ximd<T>& v) noexcept
{
    return gsl::narrow<int>(v.size());
}

template <typename TGenerator>
static inline auto generate(TGenerator&& generator, ximd_intv)
{
    return ximd_intv::generate(generator);
}
template <typename TGenerator>
static inline auto generate(TGenerator&& generator, ximd_floatv)
{
    return ximd_floatv::generate(generator);
}

// Manage a SIMD complex as an array of two SIMDs
using ximd_zfloatv = std::array<ximd_floatv, 2>;
inline auto& real(ximd_zfloatv& z) noexcept
{
    return z[0];
}
inline const auto& real(const ximd_zfloatv& z) noexcept
{
    return z[0];
}
inline auto& imag(ximd_zfloatv& z) noexcept
{
    return z[1];
}
inline const auto& imag(const ximd_zfloatv& z) noexcept
{
    return z[1];
}
inline size_t size(const ximd_zfloatv& z) noexcept
{
    auto retval = real(z).size();
    assert(retval == imag(z).size());
    return retval;
}
inline auto ssize(const ximd_zfloatv& z) noexcept
{
    return gsl::narrow<int>(size(z));
}

inline auto arg(const ximd_zfloatv& z)
{
    // https://en.cppreference.com/w/cpp/numeric/complex/arg
    // > `std::atan2(std::imag(z), std::real(z))`
    return atan2(imag(z), real(z));  // arg()
}

template <typename TGeneratorReal, typename TGeneratorImag>
static inline auto generate(TGeneratorReal&& generate_real, TGeneratorImag&& generate_imag, ximd_zfloatv)
{
    ximd_zfloatv retval;
    real(retval) = generate(generate_real, ximd_floatv{});
    imag(retval) = generate(generate_imag, ximd_floatv{});
    return retval;
}

static inline void copy_from(std::span<const float> p, ximd_floatv& result)
{
    assert(p.size() == result.size());
    result.copy_from(p.data());
}
static inline void copy_from(std::span<const zfloat> p, ximd_zfloatv& result)
{
    const auto generate_real = [&](size_t i) { return p[i].real(); };
    const auto generate_imag = [&](size_t i) { return p[i].imag(); };
    result = generate(generate_real, generate_imag, ximd_zfloatv{});
}

template<typename TTest, typename TResult>
static auto ximd_select_(const TTest& test, const TResult& t, const TResult& f)
{
    TResult retval;
    for (size_t i = 0; i < test.size(); i++)
    {
        retval[i] = test[i] ? t[i] : f[i];
    }
    return retval;
}
static inline auto select(const ximd_floatv_mask& test, const  ximd_floatv& t, const  ximd_floatv& f)
{
    return ximd_select_(test, t, f);
}
static inline auto select(const ximd_intv_mask& test, const  ximd_intv& t, const  ximd_intv& f)
{
    return ximd_select_(test, t, f);
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

template<typename T>
using simd = stdx::simd<T>;

using simd_intv = simd<int>;
using simd_intv_mask = typename simd_intv::mask_type;

using simd_floatv = simd<float>;
using simd_floatv_mask = typename simd_floatv::mask_type;

constexpr auto simd_elements_per_iteration = simd_floatv::size();

template<typename T>
static inline auto ssize(const simd<T>& v) noexcept
{
    return gsl::narrow<int>(v.size());
}

template <typename TGenerator>
static inline auto generate(TGenerator&& generator, simd_intv)
{
    return simd_intv(generator);
}
template <typename TGenerator>
static inline auto generate(TGenerator&& generator, simd_floatv)
{
    return simd_floatv(generator);
}

// Manage a SIMD complex as an array of two SIMDs
using simd_zfloatv = std::array<simd_floatv, 2>;
inline auto& real(simd_zfloatv& z) noexcept
{
    return z[0];
}
inline const auto& real(const simd_zfloatv& z) noexcept
{
    return z[0];
}
inline auto& imag(simd_zfloatv& z) noexcept
{
    return z[1];
}
inline const auto& imag(const simd_zfloatv& z) noexcept
{
    return z[1];
}
inline size_t size(const simd_zfloatv& z) noexcept
{
    auto retval = real(z).size();
    assert(retval == imag(z).size());
    return retval;
}
inline auto ssize(const simd_zfloatv& z) noexcept
{
    return gsl::narrow<int>(size(z));
}

inline auto arg(const simd_zfloatv& z)
{
    // https://en.cppreference.com/w/cpp/numeric/complex/arg
    // > `std::atan2(std::imag(z), std::real(z))`
    return atan2(imag(z), real(z));  // arg()
}

template <typename TGeneratorReal, typename TGeneratorImag>
static inline auto generate(TGeneratorReal&& generate_real, TGeneratorImag&& generate_imag, simd_zfloatv)
{
    simd_zfloatv retval;
    for (size_t i = 0; i < size(retval); i++)
    {
        real(retval)[i] = generate_real(i);
        imag(retval)[i] = generate_imag(i);
    }
    return retval;
}

static inline auto copy_from(std::span<const float> p, simd_floatv& result)
{
    assert(p.size() == result.size());
    result.copy_from(p.data(), stdx::element_aligned);
}
static inline auto copy_from(std::span<const zfloat> p, simd_zfloatv& result)
{
    const auto generate_real = [&](size_t i) { return p[i].real(); };
    const auto generate_imag = [&](size_t i) { return p[i].imag(); };
    result = generate(generate_real, generate_imag, simd_zfloatv{});
}

template<typename TTest, typename TResult>
static auto simd_select_(const TTest& test, const TResult& t, const TResult& f)
{
    // https://en.cppreference.com/w/cpp/experimental/simd/where_expression
    // > ... All other elements are left unchanged.
    TResult retval;
    where(test, retval) = t;
    where(!test, retval) = f;
    return retval;
}
template<typename TMask>
static inline auto select(const TMask& test_, const  simd_floatv& t, const  simd_floatv& f)
{
    //const auto test = test_.__cvt(); // https://github.com/VcDevel/std-simd/issues/41
    const auto test = stdx::static_simd_cast<simd_floatv_mask>(test_); // https://github.com/VcDevel/std-simd/issues/41
    return simd_select_(test, t, f);
}
template<typename TMask>
static inline auto select(const TMask& test_, const  simd_intv& t, const  simd_intv& f)
{
    //const auto test = test_.__cvt(); // https://github.com/VcDevel/std-simd/issues/41
    const auto test = stdx::static_simd_cast<simd_intv_mask>(test_); // https://github.com/VcDevel/std-simd/issues/41
    return simd_select_(test, t, f);
}

#endif // SIX_sicd_has_simd

#if SIX_sicd_has_ximd || SIX_sicd_has_simd

// There are slicker ways of doing this, but `std::enable_if` is complex.  This
// is simple and easy to understand.  Simplify with concedpts in C++20?

template<typename IntV, typename FloatV>
static auto roundi_(const FloatV& v)  // match vcl::roundi()
{
    const auto rounded = round(v);
    const auto generate_roundi = [&](size_t i)
    { return static_cast<typename IntV::value_type>(rounded[i]); };
    return generate(generate_roundi, IntV{});
}
#if SIX_sicd_has_ximd
static inline auto roundi(const ximd_floatv& v)  // match vcl::roundi()
{
    return roundi_<ximd_intv>(v);
}
#endif
#if SIX_sicd_has_simd
static inline auto roundi(const simd_floatv& v)  // match vcl::roundi()
{
    return roundi_<simd_intv>(v);
}
#endif

template<typename TFloatVMask, typename TFloatV>
static auto if_add_(const TFloatVMask& m, const TFloatV& v, typename TFloatV::value_type c)
{
    const auto generate_add = [&](size_t i) {
        return m[i] ? v[i] + c : v[i];
    };
    return generate(generate_add, TFloatV{});
}
#if SIX_sicd_has_ximd
static inline auto if_add(const ximd_floatv_mask& m, const ximd_floatv& v, typename ximd_floatv::value_type c)
{
    return if_add_(m, v, c);
}
#endif
#if SIX_sicd_has_simd
static inline auto if_add(const simd_floatv_mask& m, const simd_floatv& v, typename simd_floatv::value_type c)
{
    return if_add_(m, v, c);
}
#endif

template<typename ZFloatV, typename IntV, size_t N>
static auto lookup_(const IntV& zindex, const std::array<zfloat, N>& phase_directions)
{
    // It seems that the "generator" constuctor is called with SIMD instructions.
    // https://en.cppreference.com/w/cpp/experimental/simd/simd/simd
    // > The calls to `generator` are unsequenced with respect to each other.

    const auto generate_real = [&](size_t i) {
        const auto i_ = zindex[i];
        return phase_directions[i_].real();
    };
    const auto generate_imag = [&](size_t i) {
        const auto i_ = zindex[i];
        return phase_directions[i_].imag();
    };
    return generate(generate_real, generate_imag, ZFloatV{});
}
#if SIX_sicd_has_valarray
template<size_t N>
static inline auto lookup(const valarray_intv& zindex, const std::array<zfloat, N>& phase_directions)
{
    return lookup_<valarray_zfloatv>(zindex, phase_directions);
}
#endif
#if SIX_sicd_has_ximd
template<size_t N>
static inline auto lookup(const ximd_intv& zindex, const std::array<zfloat, N>& phase_directions)
{
    return lookup_<ximd_zfloatv>(zindex, phase_directions);
}
#endif
#if SIX_sicd_has_simd
template<size_t N>
static inline auto lookup(const simd_intv& zindex, const std::array<zfloat, N>& phase_directions)
{
    return lookup_<simd_zfloatv>(zindex, phase_directions);
}
#endif

template<typename FloatV, typename IntV>
static auto lookup_(const IntV& zindex, std::span<const float> magnitudes)
{
    const auto lookup_f = [&](size_t i) {
        const auto i_ = zindex[i];

        // The index may be out of range. This is expected because `i` might be "don't care."
        if ((i_ >= 0) && (i_ < std::ssize(magnitudes)))
        {
            return magnitudes[i_];
        }
        return NAN; // propogate "don't care"
    };
    return generate(lookup_f, FloatV{});
}
#if SIX_sicd_has_ximd
static inline auto lookup(const ximd_intv& zindex, std::span<const float> magnitudes)
{
    return lookup_<ximd_floatv>(zindex, magnitudes);
}
#endif
#if SIX_sicd_has_simd
static inline auto lookup(const simd_intv& zindex, std::span<const float> magnitudes)
{
    return lookup_<simd_floatv>(zindex, magnitudes);
}
#endif
#endif // SIX_sicd_has_ximd || SIX_sicd_has_simd

/******************************************************************************************************/

// Put as much stuff stand-alone in this file as is reasonably possible so that
// the code can more easily be used in isolated test cases.  Developing
// SIMD code isn't necessarily straight-forward, even with help from libraries.

struct ComplexToAMP8IPHS8I_unseq_Impl final
{
    const six::sicd::details::ComplexToAMP8IPHS8I& converter;
    
#if SIX_sicd_ComplexToAMP8IPHS8I_unseq
    template<typename ZFloatV, size_t N>
    auto nearest_neighbors_unseq_T(const std::array<const zfloat, N>&) const; // TODO: std::span<T, N> ... ?
    template<typename ZFloatV, int elements_per_iteration>
    void nearest_neighbors_unseq_(std::span<const zfloat> inputs, std::span<AMP8I_PHS8I> results) const;
    void nearest_neighbors_unseq(std::span<const zfloat> inputs, std::span<AMP8I_PHS8I> results) const;

    template<typename ZFloatV, int elements_per_iteration>
    void nearest_neighbors_par_unseq_T(std::span<const zfloat> inputs, std::span<AMP8I_PHS8I> results) const;
    void nearest_neighbors_par_unseq(std::span<const zfloat> inputs, std::span<AMP8I_PHS8I> results) const;
#endif 

    //! The sorted set of possible magnitudes order from small to large.
    std::span<const float> magnitudes;

    //! The difference in phase angle between two UINT phase values.
    float phase_delta;

    //! Unit vector rays that represent each direction that phase can point.
    const std::array<zfloat, six::AmplitudeTableSize>& phase_directions; // interleaved, std::complex<float>
#ifdef SIX_sicd_has_VCL
    const std::array<float, six::AmplitudeTableSize>& phase_directions_real;
    const std::array<float, six::AmplitudeTableSize>& phase_directions_imag;
#endif
};



template<typename ZFloatV>
static auto getPhase(const ZFloatV& v, float phase_delta)
{
    // Phase is determined via arithmetic because it's equally spaced.
    // There's an intentional conversion to zero when we cast 256 -> uint8. That wrap around
    // handles cases that are close to 2PI.
    auto phase = arg(v);
    phase = if_add(phase < 0.0f, phase, std::numbers::pi_v<float> * 2.0f); // Wrap from [0, 2PI]
    return roundi(phase / phase_delta);
}

// https://en.cppreference.com/w/cpp/algorithm/lower_bound
/*
template<class ForwardIt, class T>
ForwardIt lower_bound(ForwardIt first, ForwardIt last, const T& value)
{
    ForwardIt it;
    typename std::iterator_traits<ForwardIt>::difference_type count, step;
    count = std::distance(first, last);

    while (count > 0)
    {
        it = first;
        step = count / 2;
        std::advance(it, step);

        if (*it < value)
        {
            first = ++it;
            count -= step + 1;
        }
        else
            count = step;
    }

    return first;
}
*/
template<typename IntV, typename FloatV>
inline auto lower_bound_(std::span<const float> magnitudes, const FloatV& v)
{
    IntV first = 0;
    const IntV last = gsl::narrow<int>(magnitudes.size());

    auto count = last - first;
    while (any_of(count > 0))
    {
        auto it = first;
        const auto step = count / 2;
        it += step;

        auto next = it; ++next; // ... ++it;
        auto advance = count; advance -= step + 1;  // ...  -= step + 1;

        const auto c = lookup(it, magnitudes); // magnituides[it]
        const auto test = c < v;
        it = select(test, next, it); // ... ++it
        first = select(test, it, first); // first = ...
        count = select(test, advance, step); // count -= step + 1 <...OR...> count = step
    }
    return first;
}
template<typename IntV, typename FloatV>
inline auto lower_bound(std::span<const float> magnitudes, const FloatV& value)
{
    auto retval = lower_bound_<IntV>(magnitudes, value);

    #if CODA_OSS_DEBUG
    for (int i = 0; i < ssize(value); i++)
    {
        const auto it = std::lower_bound(magnitudes.begin(), magnitudes.end(), value[i]);
        const auto result = gsl::narrow<int>(std::distance(magnitudes.begin(), it));
        assert(retval[i] == result);
    }
    #endif

    return retval;
}

template<typename IntV, typename FloatV>
static auto nearest(std::span<const float> magnitudes, const FloatV& value)
{
    assert(magnitudes.size() == six::AmplitudeTableSize);

    /*
        const auto it = std::lower_bound(begin, end, value);
        if (it == begin) return 0;

        const auto prev_it = std::prev(it);
        const auto nearest_it = it == end ? prev_it  :
            (value - *prev_it <= *it - value ? prev_it : it);
        const auto distance = std::distance(begin, nearest_it);
        assert(distance <= std::numeric_limits<uint8_t>::max());
        return gsl::narrow<uint8_t>(distance);
    */
    const auto it = ::lower_bound<IntV>(magnitudes, value);
    const auto prev_it = it - 1; // const auto prev_it = std::prev(it);

    const auto v0 = value - lookup(prev_it, magnitudes); // value - *prev_it
    const auto v1 = lookup(it, magnitudes) - value; // *it - value
    //const auto nearest_it = select(v0 <= v1, prev_it, it); //  (value - *prev_it <= *it - value ? prev_it : it);
    
    const IntV end = gsl::narrow<int>(magnitudes.size());
    //const auto end_test = select(it == end, prev_it, nearest_it); // it == end ? prev_it  : ...
    const IntV zero = 0;
    auto retval = select(it == 0, zero, // if (it == begin) return 0;
        select(it == end, prev_it,  // it == end ? prev_it  : ...
            select(v0 <= v1, prev_it, it) //  (value - *prev_it <= *it - value ? prev_it : it);
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
    // the complex value onto the ray of candidate magnitudes at the selected phase.
    // i.e. dot product.
    const auto projection = (phase_direction_real * real(v)) + (phase_direction_imag * imag(v));
    //assert(std::abs(projection - std::abs(v)) < 1e-5); // TODO ???
    return nearest<IntV>(magnitudes, projection);
}

#if SIX_sicd_has_VCL
static auto lookup_and_find_nearest(const ComplexToAMP8IPHS8I_unseq_Impl& impl,
    const vcl_intv& phase, const  vcl_zfloatv& v)
{
    const auto phase_direction_real = lookup(phase, impl.phase_directions_real);
    const auto phase_direction_imag = lookup(phase, impl.phase_directions_imag);
    return ::find_nearest<vcl_intv>(impl.magnitudes, phase_direction_real, phase_direction_imag, v);
}
#endif

#if SIX_sicd_has_valarray
static auto lookup_and_find_nearest(const ComplexToAMP8IPHS8I_unseq_Impl& impl,
    const valarray_intv& phase, const  valarray_zfloatv& v)
{
    const auto phase_direction = lookup(phase, impl.phase_directions);
    return ::find_nearest<valarray_intv>(impl.magnitudes, real(phase_direction), imag(phase_direction), v);
}
#endif

#if SIX_sicd_has_ximd || SIX_sicd_has_simd
template<typename IntV, typename ZFloatV>
static auto lookup_and_find_nearest_(const ComplexToAMP8IPHS8I_unseq_Impl& impl,
    const IntV& phase, const  ZFloatV& v)
{
    const auto phase_direction = lookup(phase, impl.phase_directions);
    return ::find_nearest<IntV>(impl.magnitudes, real(phase_direction), imag(phase_direction), v);
}
#if SIX_sicd_has_ximd
static auto lookup_and_find_nearest(const ComplexToAMP8IPHS8I_unseq_Impl& impl,
    const ximd_intv& phase, const  ximd_zfloatv& v)
{
    return lookup_and_find_nearest_(impl, phase, v);
}
#endif
#if SIX_sicd_has_simd
static auto lookup_and_find_nearest(const sComplexToAMP8IPHS8I_unseq_Impl& impl,
    const simd_intv& phase, const  simd_zfloatv& v)
{
    return lookup_and_find_nearest_(impl, phase, v);
}
#endif
#endif

#if SIX_sicd_ComplexToAMP8IPHS8I_unseq

template<typename IntV>
struct AMP8I_PHS8I_unseq final
{
    IntV amplitude;
    IntV phase;
};

// Cast a blob of T* into "chunks" of T[N]; "undecay" an array.
// This isn't quite kosher, see: https://stackoverflow.com/questions/47924103/pointer-interconvertibility-vs-having-the-same-address
template<size_t N, typename T>
static inline auto array_cast(std::span<const T> data)
{
    using chunk_t = std::array<const T, N>;
    const auto number_of_chunks = data.size() / N;
    const void* const pData = data.data();
    return sys::make_span(static_cast<const chunk_t*>(pData), number_of_chunks);
}

// Inside of `std::transform()`, there is a copy; something like
// ```
//    *dest = func(*first);
// ```
// By returning our own class from `func()`, we can take control of the assignment operator
// and use that to copy from `AMP8I_PHS8I_unseq` to `std::array<AMP8I_PHS8I, N>&`.
// (Unlike most other operators, `operator=()` *must* be a member-function.)
//
// Using inheritance to avoid padding at the end of the `struct`. ... needed?
template<typename IntV, size_t N>
struct AMP8I_PHS8I_array final : public std::array<AMP8I_PHS8I, N>
{
    AMP8I_PHS8I_array& operator=(const AMP8I_PHS8I_unseq<IntV>&) = delete; // should only be using move-assignment
    AMP8I_PHS8I_array& operator=(AMP8I_PHS8I_unseq<IntV>&& other)
    {
        for (int i = 0; i < gsl::narrow<int>(N); i++)
        {
            (*this)[i].phase = gsl::narrow<uint8_t>(other.phase[i]);
            (*this)[i].amplitude = gsl::narrow<uint8_t>(other.amplitude[i]);
        }
        return *this;
    }
};
template<typename IntV, size_t N>
static inline auto AMP8I_PHS8I_array_cast(std::span<AMP8I_PHS8I> data)
{
    // This isn't quite kosher, see `array_cast()` above.
    using chunk_t = AMP8I_PHS8I_array<IntV, N>;
    const auto number_of_chunks = data.size() / N;
    void* const pDest = data.data();
    return std::span<chunk_t>(static_cast<chunk_t*>(pDest), number_of_chunks);
}

// The compiler can sometimes do better optimization with fixed-size structures.
// TODO: std::span<T, N> ... ?
template<typename ZFloatV, size_t N>
auto ComplexToAMP8IPHS8I_unseq_Impl::nearest_neighbors_unseq_T(const std::array<const zfloat, N>& p) const
{
    ZFloatV v;
    assert(p.size() == size(v));

    copy_from(p, v);
    #if CODA_OSS_DEBUG
    for (int i = 0; i < ssize(v); i++)
    {
        //const auto z = p[i];
        //assert(real(v)[i] == z.real());
        //assert(imag(v)[i] == z.imag());
    }
    #endif

    using intv_t = decltype(::getPhase(v, phase_delta));
    AMP8I_PHS8I_unseq<intv_t> retval;

    retval.phase = ::getPhase(v, phase_delta);
    #if CODA_OSS_DEBUG
    for (int i = 0; i < ssize(retval.phase); i++)
    {
        const auto phase_ = converter.getPhase(p[i]);
        assert(static_cast<uint8_t>(retval.phase[i]) == phase_);
    }
    #endif

    retval.amplitude = lookup_and_find_nearest(*this, retval.phase, v);
    #if CODA_OSS_DEBUG
    for (int i = 0; i < ssize(retval.amplitude); i++)
    {
        const auto i_ = retval.phase[i];
        const auto a = converter.find_nearest(phase_directions[i_], p[i]);
        assert(a == retval.amplitude[i]);
    }
    #endif

    return retval;
}

template<size_t elements_per_iteration>
static void finish_nearest_neighbors_unseq(const ComplexToAMP8IPHS8I_unseq_Impl& unseq_impl,
    std::span<const zfloat> inputs, std::span<AMP8I_PHS8I> results)
{
    // Then finish off anything left
    const auto remaining_count = inputs.size() % elements_per_iteration;
    if (remaining_count > 0)
    {
        const auto remaining_index = inputs.size() - remaining_count;
        const auto remaining_inputs = sys::make_span(&(inputs[remaining_index]), remaining_count);
        const auto remaining_results = sys::make_span(&(results[remaining_index]), remaining_count);
        unseq_impl.converter.nearest_neighbors_seq(remaining_inputs, remaining_results);
    }
}

template<typename ZFloatV, int elements_per_iteration>
void ComplexToAMP8IPHS8I_unseq_Impl::nearest_neighbors_unseq_(std::span<const zfloat> inputs, std::span<AMP8I_PHS8I> results) const
{
    using intv_t = decltype(::getPhase(ZFloatV{}, phase_delta));

    // View the data as chunks of *elements_per_iteration*.  This allows iterating
    // to go *elements_per_iteration* at a time; and each chunk can be processed
    // using `nearest_neighbors_unseq_T()`, above.
    //
    // This isn't quite kosher, see: https://stackoverflow.com/questions/47924103/pointer-interconvertibility-vs-having-the-same-address
    auto const first = array_cast<elements_per_iteration>(inputs);
    auto const dest = AMP8I_PHS8I_array_cast<intv_t, elements_per_iteration>(results);

    const auto func = [&](const auto& v)
    {
        return nearest_neighbors_unseq_T<ZFloatV>(v);
    };
    std::transform(first.begin(), first.end(), dest.begin(), func);

    // Then finish off anything left
    finish_nearest_neighbors_unseq<elements_per_iteration>(*this, inputs, results);
}

template<typename ZFloatV, int elements_per_iteration>
void ComplexToAMP8IPHS8I_unseq_Impl::nearest_neighbors_par_unseq_T(std::span<const zfloat> inputs, std::span<AMP8I_PHS8I> results) const
{
    using intv_t = decltype(::getPhase(ZFloatV{}, phase_delta));

    // View the data as chunks of *elements_per_iteration*.  This allows iterating
    // to go *elements_per_iteration* at a time; and each chunk can be processed
    // using `nearest_neighbors_unseq_T()`, above.
    //
    // This isn't quite kosher, see: https://stackoverflow.com/questions/47924103/pointer-interconvertibility-vs-having-the-same-address
    auto const first = array_cast<elements_per_iteration>(inputs);
    auto const dest = AMP8I_PHS8I_array_cast<intv_t, elements_per_iteration>(results);

    const auto func = [&](const auto& v)
    {
        return nearest_neighbors_unseq_T<ZFloatV>(v);
    };
    mt::Transform_par(first.begin(), first.end(), dest.begin(), func);

    // Then finish off anything left
    finish_nearest_neighbors_unseq<elements_per_iteration>(*this, inputs, results);
}

static std::string nearest_neighbors_unseq_ =
#if SIX_sicd_has_simd
"simd";
#elif SIX_sicd_has_VCL
"vcl";
#elif SIX_sicd_has_valarray
"valarray";
#elif SIX_sicd_has_ximd
"ximd";
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

void ComplexToAMP8IPHS8I_unseq_Impl::nearest_neighbors_unseq(std::span<const zfloat> inputs, std::span<AMP8I_PHS8I> results) const
{
    // TODO: there could be more complicated logic here to determine which UNSEQ
    // implementation to use.


    // This is very simple as it's only used for unit-testing
    const auto& unseq = ::nearest_neighbors_unseq_;
    if (unseq == "simd")
    {
        #if SIX_sicd_has_simd
        return nearest_neighbors_unseq_<simd_zfloatv, simd_elements_per_iteration>(inputs, results);
        #endif
    }
    if (unseq == "vcl")
    {
        #if SIX_sicd_has_VCL
        return nearest_neighbors_unseq_<vcl_zfloatv, vcl_elements_per_iteration>(inputs, results);
        #endif
    }
    if (unseq == "valarray")
    {
        #if SIX_sicd_has_valarray
        return nearest_neighbors_unseq_<valarray_zfloatv, valarray_elements_per_iteration>(inputs, results);
        #endif
    }
    if (unseq == "ximd")
    {
        #if SIX_sicd_has_ximd
        return nearest_neighbors_unseq_<ximd_zfloatv, ximd_elements_per_iteration>(inputs, results);
        #endif
    }

    throw std::logic_error("Don't know how to implement nearest_neighbors_unseq() for unseq=" + unseq);
}
std::vector<AMP8I_PHS8I> six::sicd::details::ComplexToAMP8IPHS8I::nearest_neighbors_unseq(
    std::span<const zfloat> inputs, const six::AmplitudeTable* pAmplitudeTable)
{
    // TODO: there could be more complicated logic here to determine which UNSEQ
    // implementation to use.

    // make a structure to quickly find the nearest neighbor
    const auto& converter = make_(pAmplitudeTable);

    const ComplexToAMP8IPHS8I_unseq_Impl unseq_impl{ converter, converter.magnitudes, converter.phase_delta, converter.phase_directions
#ifdef SIX_sicd_has_VCL
        , converter.phase_directions_real, converter.phase_directions_imag
#endif
    };

    std::vector<AMP8I_PHS8I> retval(inputs.size());
    unseq_impl.nearest_neighbors_unseq(inputs, sys::make_span(retval));
    return retval;
}

void ComplexToAMP8IPHS8I_unseq_Impl::nearest_neighbors_par_unseq(std::span<const zfloat> inputs, std::span<AMP8I_PHS8I> results) const
{
    // TODO: there could be more complicated logic here to determine which UNSEQ
    // implementation to use.

    // This is very simple as it's only used for unit-testing
    const auto& unseq = ::nearest_neighbors_unseq_;
    if (unseq == "simd")
    {
        #if SIX_sicd_has_simd
        return nearest_neighbors_par_unseq_T<simd_zfloatv, simd_elements_per_iteration>(inputs, results);
        #endif
    }
    if (unseq == "vcl")
    {
        #if SIX_sicd_has_VCL
        return nearest_neighbors_par_unseq_T<vcl_zfloatv, vcl_elements_per_iteration>(inputs, results);
        #endif
    }
    if (unseq == "valarray")
    {
        #if SIX_sicd_has_valarray
        return nearest_neighbors_par_unseq_T<valarray_zfloatv, valarray_elements_per_iteration>(inputs, results);
        #endif
    }
    if (unseq == "ximd")
    {
        #if SIX_sicd_has_ximd
        return nearest_neighbors_par_unseq_T<ximd_zfloatv, ximd_elements_per_iteration>(inputs, results);
        #endif
    }

    throw std::logic_error("Don't know how to implement nearest_neighbors_par_unseq() for unseq=" + unseq);
}
std::vector<AMP8I_PHS8I> six::sicd::details::ComplexToAMP8IPHS8I::nearest_neighbors_par_unseq(
    std::span<const zfloat> inputs, const six::AmplitudeTable* pAmplitudeTable)
{
    // make a structure to quickly find the nearest neighbor
    const auto& converter = make_(pAmplitudeTable);

    const ComplexToAMP8IPHS8I_unseq_Impl unseq_impl{ converter, converter.magnitudes, converter.phase_delta, converter.phase_directions
#ifdef SIX_sicd_has_VCL
        , converter.phase_directions_real, converter.phase_directions_imag
#endif
    };

    std::vector<AMP8I_PHS8I> retval(inputs.size());
    unseq_impl.nearest_neighbors_par_unseq(inputs, sys::make_span(retval));
    return retval;
}
#endif // SIX_sicd_ComplexToAMP8IPHS8I_unseq

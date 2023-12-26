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

#include <coda_oss/CPlusPlus.h>
#if CODA_OSS_cpp17
    // <execution> is broken with the older version of GCC we're using
    #if (__GNUC__ >= 10) || _MSC_VER
    #include <execution>
    #define SIX_six_sicd_ComplexToAMP8IPHS8I_has_execution 1
    #endif
#endif

#include <gsl/gsl.h>
#include <math/Utilities.h>
#include <units/Angles.h>

#include "six/sicd/Utilities.h"

#undef min
#undef max

// https://github.com/ngageoint/six-library/pull/537#issuecomment-1026453353
/*
I can add more detail, but be warned that my powerpoint skills aren't amazing and this is best drawn on a whiteboard or graph paper.
The 8-bit amplitude and phase values that we're mapping to have a systematic distribution in the complex domain. Here's what I mean
by systematic distribution:
![radial_plot](https://user-images.githubusercontent.com/16496326/151909984-f0b0cd4f-6607-4c05-9d98-038e54118daf.png)

A couple things to point out:
- The graph only shows 16 phase angles and 7 magnitudes per angle. A more realistic plot would have 256 phase angles and 256 magnitudes per angle.
- The `phase_direction[]` array contains a unit vector for each possible phase direction. Just the second vector is labeled in the plot.
- The `phase_delta` is the angular difference between each `phase_direction`.
- The `magnitudes` are the ordered set of possible magnitudes and they're identical for each phase angle.

Given any complex value `V`, we can determine the nearest phase angle by computing `round(phase(V) / phase_delta)`.
Determining the nearest magnitude value for `V` is slighly more complex:
![dot_product](https://user-images.githubusercontent.com/16496326/151910329-bede78ff-e4cb-4d6f-93f8-fb72b66cb361.png)

The complex value `V` is projected onto the nearest `phase_direction` via the dot product.
The resulting green point is then what's used to find the nearest magnitude via binary search (`std::lower_bound`).
*/

/*!
    * Get the phase of a complex value.
    * @param v complex value
    * @return phase between [0, 2PI]
    */

inline auto GetPhase(std::complex<double> v)
{
    // There's an intentional conversion to zero when we cast 256 -> uint8. That wrap around
    // handles cases that are close to 2PI.
    double phase = std::arg(v);
    if (phase < 0.0) phase += std::numbers::pi * 2.0; // Wrap from [0, 2PI]
    return phase;
}
static inline auto get_phase_(float phase_delta, std::complex<double> v)
{
    // Phase is determined via arithmetic because it's equally spaced.
    const auto phase = GetPhase(v);
    return gsl::narrow_cast<uint8_t>(std::round(phase / phase_delta));
}
uint8_t six::sicd::details::ComplexToAMP8IPHS8I::getPhase(six::zfloat v) const
{
    return get_phase_(phase_delta, v);
}

template<typename TToComplexFunc>
static std::vector<float> make_magnitudes_(TToComplexFunc toComplex)
{
    std::vector<float> retval;
    retval.reserve(UINT8_MAX + 1);
    for (const auto amplitude : six::sicd::Utilities::iota_0_256())
    {
        // AmpPhase -> Complex
        const auto phase = amplitude;
        const auto complex = toComplex(amplitude, phase);
        retval.push_back(std::abs(complex));
    }

    // I don't know if we can guarantee that the amplitude table is non-decreasing.
    // Check to verify property at runtime.
    if (!std::is_sorted(retval.begin(), retval.end()))
    {
        throw std::runtime_error("magnitudes must be sorted");
    }
    return retval;
}
static inline auto make_magnitudes(const six::AmplitudeTable& amplitudeTable)
{
    const auto toComplex = [&](auto amplitude, auto phase)
    {
        return six::sicd::Utilities::toComplex(amplitude, phase, amplitudeTable);
    };
    return make_magnitudes_(toComplex);
}
static inline auto make_magnitudes()
{
    static const auto toComplex = [](auto amplitude, auto phase)
    {
        return six::sicd::Utilities::toComplex(amplitude, phase);
    };
    return make_magnitudes_(toComplex);
}

static const std::vector<float>& get_magnitudes(const six::AmplitudeTable* pAmplitudeTable,
    std::vector<float>& uncached_magnitudes)
{
    if (pAmplitudeTable == nullptr)
    {
        static const auto magnitudes = make_magnitudes(); // OK to cache, won't change
        return magnitudes;
    }
    
    uncached_magnitudes = make_magnitudes(*pAmplitudeTable);
    return uncached_magnitudes;
}

six::sicd::details::ComplexToAMP8IPHS8I::ComplexToAMP8IPHS8I(const six::AmplitudeTable *pAmplitudeTable)
    : magnitudes(get_magnitudes(pAmplitudeTable, uncached_magnitudes))
{
    const auto p0 = GetPhase(Utilities::toComplex(1, 0, pAmplitudeTable));
    const auto p1 = GetPhase(Utilities::toComplex(1, 1, pAmplitudeTable));
    assert(p0 == 0.0);
    assert(p1 > p0);
    phase_delta = gsl::narrow_cast<float>(p1 - p0);
    for(size_t i = 0; i < 256; i++)
    {
        const units::Radians<float> angle{ static_cast<float>(p0) + i * phase_delta };
        float y, x;
        SinCos(angle, y, x);
        phase_directions[i] = { x, y };
    }
}

/*!
 * Find the nearest element given an iterator range.
 * @param value query value
 * @return index of nearest value within the iterator range.
 */
static inline uint8_t nearest(const std::vector<float>& magnitudes, float value)
{
    const auto begin = magnitudes.begin();
    const auto end = magnitudes.end();

    const auto it = std::lower_bound(begin, end, value);
    if (it == begin) return 0;

    const auto prev_it = std::prev(it);
    const auto nearest_it = it == end ? prev_it  :
        (value - *prev_it <= *it - value ? prev_it : it);
    const auto distance = std::distance(begin, nearest_it);
    assert(distance <= std::numeric_limits<uint8_t>::max());
    return gsl::narrow<uint8_t>(distance);
}
static inline auto find_nearest(const std::vector<float>& magnitudes, six::zfloat phase_direction,
    six::zfloat v)
{
    // We have to do a 1D nearest neighbor search for magnitude.
    // But it's not the magnitude of the input complex value - it's the projection of
    // the complex value onto the ray of candidate magnitudes at the selected phase.
    // i.e. dot product.
    const auto projection = (phase_direction.real() * v.real()) + (phase_direction.imag() * v.imag());
    //assert(std::abs(projection - std::abs(v)) < 1e-5); // TODO ???
    return nearest(magnitudes, projection);
}
six::AMP8I_PHS8I_t six::sicd::details::ComplexToAMP8IPHS8I::nearest_neighbor_(const six::zfloat &v) const
{
    six::AMP8I_PHS8I_t retval;
    retval.phase = getPhase(v);

    auto&& phase_direction = phase_directions[retval.phase];
    retval.amplitude = find_nearest(magnitudes, phase_direction, v);
    return retval;
}
template <typename TInputIt, typename TOutputIt>
void six::sicd::details::ComplexToAMP8IPHS8I::nearest_neighbors_seq(TInputIt first, TInputIt last, TOutputIt dest) const
{
    std::transform(first, last, dest, [&](const auto& v) { return nearest_neighbor_(v); });
}
std::vector<six::AMP8I_PHS8I_t> six::sicd::details::ComplexToAMP8IPHS8I::nearest_neighbors_seq(
    std::span<const zfloat> inputs, const six::AmplitudeTable* pAmplitudeTable)
{
    // make a structure to quickly find the nearest neighbor
    const auto& converter = make_(pAmplitudeTable);

    std::vector<six::AMP8I_PHS8I_t> retval(inputs.size());
    converter.nearest_neighbors_seq(inputs.begin(), inputs.end(), retval.begin());
    return retval;
}

six::AMP8I_PHS8I_t six::sicd::nearest_neighbor(const details::ComplexToAMP8IPHS8I& i, const six::zfloat& v)
{
    return i.nearest_neighbor_(v);
}

 // Yes, this is duplicated code :-(  1) hopefully it will go away someday "soon,"
 // that is, we'll be at C++17; 2) the cutoff/dimension values may be different.
 //
 // First of all, C++11's std::async() is now (in 2023) thought of as maybe a
 // bit "half baked," and perhaps shouldn't be emulated.  Then, C++17 added
 // parallel algorithms which might be a better way of satisfying our immediate
 // needs (below) ... although we're still at C++14.
template <typename InputIt, typename OutputIt, typename TFunc>
static inline OutputIt transform_async(const InputIt first1, const InputIt last1, OutputIt d_first, TFunc f,
    typename std::iterator_traits<InputIt>::difference_type cutoff)
{
    // https://en.cppreference.com/w/cpp/thread/async
    const auto len = std::distance(first1, last1);
    if (len < cutoff)
    {
        return std::transform(first1, last1, d_first, f);
    }

    constexpr auto policy = std::launch::async;

    const auto mid1 = first1 + len / 2;
    const auto d_mid = d_first + len / 2;
    auto handle = std::async(policy, transform_async<InputIt, OutputIt, TFunc>, mid1, last1, d_mid, f, cutoff);
    transform_async(first1, mid1, d_first, f, cutoff);
    return handle.get();
}
template <typename TInputs, typename TResults, typename TFunc>
static inline void transform(std::span<const TInputs> inputs, std::span<TResults> results, TFunc f)
{
#if SIX_six_sicd_ComplexToAMP8IPHS8I_has_execution
    std::ignore = std::transform(std::execution::par, inputs.begin(), inputs.end(), results.begin(), f);
#else
    constexpr ptrdiff_t cutoff_ = 0; // too slow w/o multi-threading
    // The value of "default_cutoff" was determined by testing; there is nothing special about it, feel free to change it.
    constexpr auto dimension = 128 * 8;
    constexpr auto default_cutoff = dimension * dimension;
    const auto cutoff = cutoff_ == 0 ? default_cutoff : cutoff_;        
    std::ignore = transform_async(inputs.begin(), inputs.end(), results.begin(), f, cutoff);
#endif // CODA_OSS_cpp17
}

template <typename TInputIt, typename TOutputIt>
void six::sicd::details::ComplexToAMP8IPHS8I::nearest_neighbors_par(TInputIt first, TInputIt last, TOutputIt dest) const
{
    const auto nearest_neighbor = [&](const auto& v)
    {
        return this->nearest_neighbor_(v);
    };

#if SIX_six_sicd_ComplexToAMP8IPHS8I_has_execution
    std::ignore = std::transform(std::execution::par, first, last, dest, nearest_neighbor);
#else
    constexpr ptrdiff_t cutoff_ = 0; // too slow w/o multi-threading
    // The value of "default_cutoff" was determined by testing; there is nothing special about it, feel free to change it.
    constexpr auto dimension = 128 * 8;
    constexpr auto default_cutoff = dimension * dimension;
    const auto cutoff = cutoff_ == 0 ? default_cutoff : cutoff_;
    std::ignore = transform_async(first, last, dest, nearest_neighbor, cutoff);
#endif // SIX_six_sicd_ComplexToAMP8IPHS8I_has_execution
}
std::vector<six::AMP8I_PHS8I_t> six::sicd::details::ComplexToAMP8IPHS8I::nearest_neighbors_par(
    std::span<const zfloat> inputs, const six::AmplitudeTable* pAmplitudeTable)
{
    // make a structure to quickly find the nearest neighbor
    const auto& converter = make_(pAmplitudeTable);

    std::vector<six::AMP8I_PHS8I_t> retval(inputs.size());
    converter.nearest_neighbors_par(inputs.begin(), inputs.end(), retval.begin());
    return retval;
}

std::vector<six::AMP8I_PHS8I_t> six::sicd::details::ComplexToAMP8IPHS8I::nearest_neighbors(
    std::span<const zfloat> inputs, const six::AmplitudeTable* pAmplitudeTable)
{
    // TODO: there could be more complicated logic here to decide between
    // _seq, _par, and _unseq.
    return nearest_neighbors_unseq(inputs, pAmplitudeTable);
}

#define VCL_NAMESPACE vcl
#include "six/sicd/vectorclass/version2/vectorclass.h"
#include "six/sicd/vectorclass/version2/vectormath_trig.h"
#include "six/sicd/vectorclass/complex/complexvec1.h"

template<typename TInputIter, typename TOutputIter>
static inline void get_phase_unseq_1(float phase_delta,
    TInputIter first, TInputIter last, TOutputIter dest)
{
    dest->phase = get_phase_(phase_delta, *first);
}
template<typename TInputIter, typename TOutputIter>
static inline void get_phase_unseq_2(float phase_delta,
    TInputIter first, TInputIter last, TOutputIter dest)
{
    const auto next = std::next(first);
    if (next == last)
    {
        get_phase_unseq_1(phase_delta, first, last, dest);
        return;
    }

    const vcl::Complex2f v(first->real(), first->imag(), next->real(), next->imag());
    // Phase is determined via arithmetic because it's equally spaced.
    // There's an intentional conversion to zero when we cast 256 -> uint8. That wrap around
    // handles cases that are close to 2PI.
    auto phase = atan2(v.imag(), v.real()); // arg()
    phase = select(phase < 0.0, phase + std::numbers::pi_v<float> *2.0f, phase); // Wrap from [0, 2PI]
    const auto phase_ = roundi(phase / phase_delta);

    dest->phase = gsl::narrow_cast<uint8_t>(phase_[0]);
    ++dest;
    dest->phase = gsl::narrow_cast<uint8_t>(phase_[1]);
}

//template<typename TInputIter, typename TOutputIter>
//static void nearest_neighbor(const  std::array<six::zfloat, UINT8_MAX + 1>& phase_directions, const std::vector<float>& magnitudes,
//    TInputIter first, TInputIter last, TOutputIter dest)
//{
//    const auto& v = *first;
//    auto& result = *dest;
//
//    // We have to do a 1D nearest neighbor search for magnitude.
//    // But it's not the magnitude of the input complex value - it's the projection of
//    // the complex value onto the ray of candidate magnitudes at the selected phase.
//    // i.e. dot product.
//    auto&& phase_direction = phase_directions[result.phase];
//    const auto projection = (phase_direction.real() * v.real()) + (phase_direction.imag() * v.imag());
//    //assert(std::abs(projection - std::abs(v)) < 1e-5); // TODO ???
//    result.amplitude = nearest(magnitudes, projection);
//}
//
//template<typename TInputIter, typename TOutputIter>
//static inline void nearest_neighbor_vcl(const  std::array<six::zfloat, UINT8_MAX + 1>& phase_directions, const std::vector<float>& magnitudes,
//    TInputIter first, TInputIter last, TOutputIter dest)
//{
//    const auto next = std::next(first);
//    if (next == last)
//    {
//        nearest_neighbor(phase_directions, magnitudes, first, last, dest);
//        return;
//    }
//    const auto dest_next = std::next(dest);
//
//    const vcl::Complex2f v(first->real(), first->imag(), next->real(), next->imag());
//    // We have to do a 1D nearest neighbor search for magnitude.
//    // But it's not the magnitude of the input complex value - it's the projection of
//    // the complex value onto the ray of candidate magnitudes at the selected phase.
//    // i.e. dot product.     
//    const vcl::Complex2f phase_direction(phase_directions[dest->phase].real(), phase_directions[dest->phase].imag(),
//        phase_directions[dest_next->phase].real(), phase_directions[dest_next->phase].imag());
//    const auto projection = (phase_direction.real() * v.real()) + (phase_direction.imag() * v.imag());
//
//    dest->amplitude = nearest(magnitudes, projection[0]);
//    dest_next->amplitude = nearest(magnitudes, projection[1]);
//}

template<typename TInputIter, typename TOutputIter>
static inline void find_nearest_2(const  std::array<six::zfloat, UINT8_MAX + 1>& phase_directions, const std::vector<float>& magnitudes,
    TInputIter first, TInputIter last, TOutputIter dest)
{
    auto phase_direction = &(phase_directions[dest->phase]);
    dest->amplitude = find_nearest(magnitudes, *phase_direction, *first);

    ++first; ++dest;
    phase_direction = &(phase_directions[dest->phase]);
    dest->amplitude = find_nearest(magnitudes, *phase_direction, *first);
}

template <typename TInputIt, typename TOutputIt>
void six::sicd::details::ComplexToAMP8IPHS8I::nearest_neighbors_unseq(TInputIt first, TInputIt last, TOutputIt dest) const
{
    for (; first != last; ++first, ++dest)
    {
        const auto distance = std::distance(first, last);
        assert(distance >= 0); // should be out of the loop!
        if (distance == 0)
        {
            nearest_neighbors_seq(first, last, dest);
            continue;
        }

        assert(std::next(first) != last); // don't support 3 or 4 ... yet
        get_phase_unseq_2(phase_delta, first, last, dest);
        find_nearest_2(phase_directions, magnitudes, first, last, dest);
        ++first; ++dest;
    }
}
std::vector<six::AMP8I_PHS8I_t> six::sicd::details::ComplexToAMP8IPHS8I::nearest_neighbors_unseq(
    std::span<const zfloat> inputs, const six::AmplitudeTable* pAmplitudeTable)
{
    // make a structure to quickly find the nearest neighbor
    const auto& converter = make_(pAmplitudeTable);

    std::vector<six::AMP8I_PHS8I_t> retval(inputs.size());
    converter.nearest_neighbors_unseq(inputs.begin(), inputs.end(), retval.begin());
    return retval;
}

const six::sicd::details::ComplexToAMP8IPHS8I& six::sicd::details::ComplexToAMP8IPHS8I::make_(const six::AmplitudeTable* pAmplitudeTable)
{
    if (pAmplitudeTable == nullptr)
    {
        // this won't change, so OK to cache
        static const six::sicd::details::ComplexToAMP8IPHS8I tree;
        return tree;
    }
    else
    {
        // Might have already "cached" this on the AmplitudeTable instance.
        auto pFromComplex = pAmplitudeTable->getFromComplex();
        if (pFromComplex != nullptr)
        {
            return *pFromComplex;
        }

        // constructor is private, can't use make_unique
        std::unique_ptr<sicd::details::ComplexToAMP8IPHS8I> pTree(new six::sicd::details::ComplexToAMP8IPHS8I(pAmplitudeTable));
        pAmplitudeTable->cacheFromComplex_(std::move(pTree));
        return *(pAmplitudeTable->getFromComplex());
    }
}
const six::sicd::details::ComplexToAMP8IPHS8I& six::sicd::make_ComplexToAMP8IPHS8I(const six::AmplitudeTable* pAmplitudeTable)
{
    return six::sicd::details::ComplexToAMP8IPHS8I::make_(pAmplitudeTable);
}
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

#include <gsl/gsl.h>
#include <math/Utilities.h>
#include <units/Angles.h>

#include "six/sicd/Utilities.h"

#undef min
#undef max

#define VCL_NAMESPACE vcl
#include "six/sicd/vectorclass/version2/vectorclass.h"
#include "six/sicd/vectorclass/version2/vectormath_trig.h"
#include "six/sicd/vectorclass/complex/complexvec1.h"

// https://en.cppreference.com/w/cpp/numeric/complex/arg
// > `std::atan2(std::imag(z), std::real(z))`
inline auto arg(const vcl::Complex2f& z)
{
    return atan2(z.imag(), z.real()); // arg()
}
inline auto arg(const vcl::Complex4f& z)
{
    return atan2(z.imag(), z.real()); // arg()
}
inline auto arg(const vcl::Complex8f& z)
{
    return atan2(z.imag(), z.real()); // arg()
}

inline auto interleave(const vcl::Vec4i& a, const vcl::Vec4i& b)
{
    // The blend() indicies are based on one large array
    const auto index0 = vcl::blend4<0, 4, 1, 5>(a, b); // i.e., a[0], b[0], a[1], b[1]
    const auto index1 = vcl::blend4<2, 6, 3, 7>(a, b);    
    return vcl::Vec8i(index0, index1);
}
inline auto interleave(const vcl::Vec8i& a, const vcl::Vec8i& b)
{
    // The blend() indicies are based on one large array
    const auto index0 = vcl::blend8<0, 8, 1, 9, 2, 10, 3, 11>(a, b); // i.e., a[0], b[0], a[1], b[1], ...
    const auto index1 = vcl::blend8<4, 12, 5, 13, 6, 14, 7, 15>(a, b);
    return vcl::Vec16i(index0, index1);
}

// There's no lookup() for vcl::ComplexN, implement using floats
static auto lookup(const vcl::Vec4i& zindex, const std::array<six::zfloat, UINT8_MAX + 1>& table_)
{
    const auto table = reinterpret_cast<const float*>(table_.data());
    constexpr auto size_as_floats = (UINT8_MAX + 1) * 2; // table_t is six::zfloat

    // A `six::zfloat` at *n* is at *2n* when viewed as `float`.
    const auto real_index = zindex * 2;
    // The imaginary part is after the real part
    const auto imag_index = real_index + 1; // [n] is real, [n+1] is imag
    const auto index = interleave(real_index, imag_index);

    auto lookup = vcl::lookup<size_as_floats>(index, table);
    return vcl::Complex4f(lookup);
}
static auto lookup(const vcl::Vec8i& zindex, const std::array<six::zfloat, UINT8_MAX + 1>& table_)
{
    const auto table = reinterpret_cast<const float*>(table_.data());
    constexpr auto size_as_floats = (UINT8_MAX + 1) * 2; // table_t is six::zfloat

    // A `six::zfloat` at *n* is at *2n* when viewed as `float`.
    const auto real_index = zindex * 2;
    // The imaginary part is after the real part
    const auto imag_index = real_index + 1; // [n] is real, [n+1] is imag
    const auto index = interleave(real_index, imag_index);

    auto lookup = vcl::lookup<size_as_floats>(index, table);
    return vcl::Complex8f(lookup);
}

//template<typename T>
//inline size_t lower_bound_(const std::vector<float>& magnitudes, const T& value)
//{
//    size_t first = 0;
//    const auto last = magnitudes.size();
//
//    ptrdiff_t count = last - first;
//    while (count > 0)
//    {
//        auto it = first;
//        const auto step = count / 2;
//        it += step;
//
//        if (magnitudes[it] < value)
//        {
//            first = ++it;
//            count -= step + 1;
//        }
//        else
//            count = step;
//    }
//    return first;
//}
template<typename TRetval, typename TValue>
inline auto lower_bound(const std::vector<float>& magnitudes, const TValue& value)
{
    const auto begin = magnitudes.begin();
    const auto end = magnitudes.end();

    TRetval retval;
    for (int i = 0; i < value.size(); i++)
    {
        //const auto result = lower_bound_(magnitudes, value[i]);
        const auto it = std::lower_bound(begin, end, value[i]);
        const auto result = std::distance(begin, it);
        retval.insert(i, gsl::narrow<int>(result));
    }
    return retval;
}

template<typename TVclComplex>
static auto getPhase(const TVclComplex& v, float phase_delta)
{
    // Phase is determined via arithmetic because it's equally spaced.
    // There's an intentional conversion to zero when we cast 256 -> uint8. That wrap around
    // handles cases that are close to 2PI.
    auto phase = arg(v);
    phase = if_add(phase < 0.0, phase, std::numbers::pi_v<float> * 2.0f); // Wrap from [0, 2PI]
    return roundi(phase / phase_delta);
}

template<typename TRetval, typename TValue>
static auto nearest_T(const std::vector<float>& magnitudes, const TValue& value)
{
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
    const auto it = ::lower_bound<TRetval>(magnitudes, value);
    const auto prev_it = it - 1; // const auto prev_it = std::prev(it);

    const auto v0 = value - vcl::lookup<256>(prev_it, magnitudes.data()); // value - *prev_it
    const auto v1 = vcl::lookup<256>(it, magnitudes.data()) - value; // *it - value
    //const auto nearest_it = select(v0 <= v1, prev_it, it); //  (value - *prev_it <= *it - value ? prev_it : it);
    
    const TRetval end(gsl::narrow<int>(magnitudes.size()));
    //const auto end_test = select(it == end, prev_it, nearest_it); // it == end ? prev_it  : ...
    const TRetval zero(0);
    auto retval = select(it == 0, zero, // if (it == begin) return 0;
        select(it == end, prev_it,  // it == end ? prev_it  : ...
            select(v0 <=v1, prev_it, it) //  (value - *prev_it <= *it - value ? prev_it : it);
        ));
    return retval;
}
inline auto nearest_(const std::vector<float>& magnitudes, const vcl::Vec4f& value)
{
    return nearest_T<vcl::Vec4i>(magnitudes, value);
}
inline auto nearest_(const std::vector<float>& magnitudes, const vcl::Vec8f& value)
{
    return nearest_T<vcl::Vec8i>(magnitudes, value);
}

template<typename TVclComplex, typename TOutputIter>
void six::sicd::details::ComplexToAMP8IPHS8I::nearest_neighbors_unseq_n(const six::zfloat* p, TOutputIter dest) const
{
    // https://en.cppreference.com/w/cpp/numeric/complex
    // > For any pointer to an element of an array of `std::complex<T>` named `p` and any valid array index `i`, ...
    // > is the real part of the complex number `p[i]`, ...
    TVclComplex v;
    v.load(reinterpret_cast<const float*>(p));

    const auto phase = ::getPhase(v, phase_delta);

    // We have to do a 1D nearest neighbor search for magnitude.
    // But it's not the magnitude of the input complex value - it's the projection of
    // the complex value onto the ray of candidate magnitudes at the selected phase.
    // i.e. dot product.
    const auto phase_directions_ = lookup(phase, phase_directions);
    const auto projection = (phase_directions_.real() * v.real()) + (phase_directions_.imag() * v.imag());
    const auto nearest = nearest_(magnitudes, projection);

    const auto results_ = interleave(nearest, phase); // amplitude, phase
    vcl::Vec16uc results;
    for (int i = 0; i < results_.size(); i++)
    {
        results.insert(i, gsl::narrow<uint8_t>(results_[i]));
    }

    auto pDest = &(*dest);
    results.store_partial(TVclComplex::size()*2, pDest);

    ////constexpr auto size = TVclComplex::size();
    //for (int i = 0; i < size; i++)
    //{
    //    dest->phase = gsl::narrow_cast<uint8_t>(phase[i]);

    //    // We have to do a 1D nearest neighbor search for magnitude.
    //    // But it's not the magnitude of the input complex value - it's the projection of
    //    // the complex value onto the ray of candidate magnitudes at the selected phase.
    //    // i.e. dot product.
    //    //dest->amplitude = find_nearest(magnitudes, phase_directions[dest->phase], p[i]);
    //    //assert(std::abs(projection[i] - std::abs(v)) < 1e-5); // TODO ???
    //    //dest->amplitude = nearest(magnitudes, projection[i]);
    //    dest->amplitude = nearest[i];

    //    ++dest;
    //}
}
template <typename TInputIt, typename TOutputIt>
void six::sicd::details::ComplexToAMP8IPHS8I::nearest_neighbors_unseq(TInputIt first, TInputIt last, TOutputIt dest) const
{
    for (; first != last; ++first, ++dest)
    {
        const auto distance = std::distance(first, last);
        assert(distance > 0); // should be out of the loop!
        switch (distance % 8)
        {
            case 1:
            {
                nearest_neighbors_seq(first, last, dest);
                break;
            }
            case 2: case 3:
            {
                nearest_neighbors_unseq_n<vcl::Complex2f>(&(*first), dest);
                first += 1; dest += 1;
                break;
            }
            case 4: case 5: case 6: case 7:
            {
                nearest_neighbors_unseq_n<vcl::Complex4f>(&(*first), dest);
                first += 3; dest += 3;
                break;
            }
            case 0:
            {
                nearest_neighbors_unseq_n<vcl::Complex8f>(&(*first), dest);
                first += 7; dest += 7;
                break;
            }

            default:
                throw std::logic_error("Can't figure out which nearest_neighbors_unseq() to use");
        }
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

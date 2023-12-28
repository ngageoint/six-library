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

// https://en.cppreference.com/w/cpp/experimental/simd
using zfloatv = vcl::Complex8f;
using floatv = vcl::Vec8f;
using intv = vcl::Vec8i;

// https://en.cppreference.com/w/cpp/numeric/complex/arg
// > `std::atan2(std::imag(z), std::real(z))`
inline auto arg(const floatv& real, const floatv& imag)
{
    return atan2(imag, real); // arg()
}
inline auto arg(const zfloatv& z)
{
    return arg(z.real(), z.imag());
}

static inline auto interleave(const intv& a, const intv& b)
{
    // The blend() indicies are based on one large array
    auto index0 = vcl::blend8<0, 8, 1, 9, 2, 10, 3, 11>(a, b); // i.e., a[0], b[0], a[1], b[1], ...
    auto index1 = vcl::blend8<4, 12, 5, 13, 6, 14, 7, 15>(a, b);
    return vcl::Vec16i(std::move(index0), std::move(index1));
}

// There's no `vcl::lookup()` for `std::complex<T>*`, implement using floats
template<size_t N>
static auto lookup(const intv& zindex, const std::array<six::zfloat, N>& table_)
{
    const auto table = reinterpret_cast<const float*>(table_.data());
    constexpr auto size_as_floats = N * 2; // table_t is six::zfloat

    // A `six::zfloat` at *n* is at *2n* when viewed as `float`.
    const auto real_index = zindex * 2;
    // The imaginary part is after the real part
    const auto imag_index = real_index + 1; // [n] is real, [n+1] is imag
    const auto index = interleave(real_index, imag_index);

    auto lookup = vcl::lookup<size_as_floats>(index, table);
    return zfloatv(std::move(lookup));
}

static auto getPhase(const zfloatv& v, float phase_delta)
{
    // Phase is determined via arithmetic because it's equally spaced.
    // There's an intentional conversion to zero when we cast 256 -> uint8. That wrap around
    // handles cases that are close to 2PI.
    auto phase = arg(v);
    phase = if_add(phase < 0.0, phase, std::numbers::pi_v<float> * 2.0f); // Wrap from [0, 2PI]
    return roundi(phase / phase_delta);
}

inline auto lower_bound(std::span<const float> magnitudes, const floatv& value)
{
    const auto begin = magnitudes.begin();
    const auto end = magnitudes.end();

    intv retval;
    for (int i = 0; i < value.size(); i++)
    {
        const auto it = std::lower_bound(begin, end, value[i]);
        const auto result = std::distance(begin, it);
        retval.insert(i, gsl::narrow<int>(result));
    }
    return retval;
}
static auto nearest(std::span<const float> magnitudes, const floatv& value)
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
    const auto it = ::lower_bound(magnitudes, value);
    const auto prev_it = it - 1; // const auto prev_it = std::prev(it);

    const auto v0 = value - vcl::lookup<six::AmplitudeTableSize>(prev_it, magnitudes.data()); // value - *prev_it
    const auto v1 = vcl::lookup<six::AmplitudeTableSize>(it, magnitudes.data()) - value; // *it - value
    //const auto nearest_it = select(v0 <= v1, prev_it, it); //  (value - *prev_it <= *it - value ? prev_it : it);
    
    const intv end = gsl::narrow<int>(magnitudes.size());
    //const auto end_test = select(it == end, prev_it, nearest_it); // it == end ? prev_it  : ...
    const intv zero = 0;
    auto retval = select(it == 0, zero, // if (it == begin) return 0;
        select(it == end, prev_it,  // it == end ? prev_it  : ...
            select(v0 <=v1, prev_it, it) //  (value - *prev_it <= *it - value ? prev_it : it);
        ));
    return retval;
}

static auto find_nearest(std::span<const float> magnitudes, const zfloatv& phase_direction, const zfloatv& v)
{
    // We have to do a 1D nearest neighbor search for magnitude.
    // But it's not the magnitude of the input complex value - it's the projection of
    // the complex value onto the ray of candidate magnitudes at the selected phase.
    // i.e. dot product.
    const auto projection = (phase_direction.real() * v.real()) + (phase_direction.imag() * v.imag());
    //assert(std::abs(projection - std::abs(v)) < 1e-5); // TODO ???
    return nearest(magnitudes, projection);
}
static auto find_nearest(std::span<const float> magnitudes, const floatv& phase_direction_real, const floatv& phase_direction_imag,
    const zfloatv& v)
{
    // We have to do a 1D nearest neighbor search for magnitude.
    // But it's not the magnitude of the input complex value - it's the projection of
    // the complex value onto the ray of candidate magnitudes at the selected phase.
    // i.e. dot product.
    const auto projection = (phase_direction_real * v.real()) + (phase_direction_imag * v.imag());
    //assert(std::abs(projection - std::abs(v)) < 1e-5); // TODO ???
    return nearest(magnitudes, projection);
}

template< typename TOutputIter>
void six::sicd::details::ComplexToAMP8IPHS8I::nearest_neighbors_unseq_(const six::zfloat* p, TOutputIter dest) const
{
    // https://en.cppreference.com/w/cpp/numeric/complex
    // > For any pointer to an element of an array of `std::complex<T>` named `p` and any valid array index `i`, ...
    // > is the real part of the complex number `p[i]`, ...
    zfloatv v;
    v.load(reinterpret_cast<const float*>(p));

    const auto phase = ::getPhase(v, phase_delta);

    //const auto phase_direction = lookup(phase, phase_directions);
    //const auto amplitude = ::find_nearest(magnitudes, phase_direction, v);
    const auto phase_direction_real = vcl::lookup<six::AmplitudeTableSize>(phase, phase_directions_real.data());
    const auto phase_direction_imag = vcl::lookup<six::AmplitudeTableSize>(phase, phase_directions_imag.data());
    //const auto amplitude = ::find_nearest(magnitudes, phase_direction_real, phase_direction_imag, v);

    // interleave() and store() is slower than an explicit loop.
    for (int i = 0; i < v.size(); i++)
    {
        dest->phase = gsl::narrow_cast<uint8_t>(phase[i]);

        //dest->amplitude = find_nearest(phase_directions[dest->phase], p[i]);
        //const auto phase_direction_ = phase_direction.extract(i);
        //dest->amplitude = find_nearest(six::zfloat(phase_direction_.real(), phase_direction_.imag()), p[i]);
        dest->amplitude = find_nearest(six::zfloat(phase_direction_real[i], phase_direction_imag[i]), p[i]);
        //dest->amplitude = gsl::narrow_cast<uint8_t>(amplitude[i]);

        ++dest;
    }
}

template <typename TInputIt, typename TOutputIt>
void six::sicd::details::ComplexToAMP8IPHS8I::nearest_neighbors_unseq(TInputIt first, TInputIt last, TOutputIt dest) const
{
    // The above code is simpler (no templates) if we use just a single VCL
    // complex type: `zfloatv`.  If there is any performance differ4ence,
    // it will only be for extreme edge cases since the smaller types are only used
    // at the end of the loop.
    //
    // It also makes this loop simpler as we handle all non-multiples-of-8 in
    // the same way.
    for (; first != last; ++first, ++dest)
    {
        const auto distance = std::distance(first, last);
        assert(distance > 0); // should be out of the loop!
        if (distance % 8 == 0)
        {
            nearest_neighbors_unseq_(&(*first), dest);
            first += 7; dest += 7;
        }
        else
        {
            nearest_neighbors_seq(first, last, dest);
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

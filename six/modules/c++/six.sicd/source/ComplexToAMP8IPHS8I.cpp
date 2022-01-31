/* =========================================================================
* This file is part of six.sicd-c++
* =========================================================================
*
* (C) Copyright 2022, Maxar Technologies, Inc.
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
#include "six/sicd/ComplexToAMP8IPHS8I.h"

#include <math.h>
#include <cassert>

#include <gsl/gsl.h>
#include <math/Utilities.h>
#include <units/Angles.h>

#include <six/sicd/Utilities.h>

namespace six {
namespace sicd {

 /*!
 * Get the phase of a complex value.
 * @param v complex value
 * @return phase between [0, 2PI]
 */
 inline long double GetPhase(const std::complex<long double>& v)
{
    auto phase = std::arg(v);
    if (phase < 0.0) phase += M_PI * 2.0; // Wrap from [0, 2PI]
    return phase;
}

ComplexToAMP8IPHS8I::ComplexToAMP8IPHS8I(const six::AmplitudeTable *pAmplitudeTable) {

    // Be careful with indexing so that we don't wrap-around in the loops.
    for (uint16_t i = 0; i <= UINT8_MAX; i++) {
        // AmpPhase -> Complex
        ImageData::AMP8I_PHS8I_t v;
        v.first = gsl::narrow<uint8_t>(i);
        v.second = gsl::narrow<uint8_t>(i);
        const auto complex = Utilities::from_AMP8I_PHS8I(v.first, v.second, pAmplitudeTable);
        magnitudes[i] = { std::abs(complex) };
    }

    // I don't know if we can guarantee that the amplitude table is non-decreasing.
    // Check to verify property at runtime.
    if (!std::is_sorted(magnitudes.begin(), magnitudes.end()))
    {
        throw std::runtime_error("magnitudes must be sorted");
    }

    const auto p0 = GetPhase(Utilities::from_AMP8I_PHS8I(1, 0, pAmplitudeTable));
    const auto p1 = GetPhase(Utilities::from_AMP8I_PHS8I(1, 1, pAmplitudeTable));
    assert(p0 == 0);
    assert(p1 > p0);
    phase_delta = p1 - p0;
    for(size_t i = 0; i <= UINT8_MAX; i++) {
        const units::Radians<long double> angle{ p0 + gsl::narrow_cast<long double>(i) * phase_delta };
        long double y, x;
        math::SinCos(angle.value(), y, x);
        phase_directions[i] = { x, y };
    }
}

/*!
 * Find the nearest element given an iterator range.
 * @tparam TIter type of iterator
 * @param begin beginning of search range
 * @param end end of search range
 * @param value query value
 * @return index of nearest value within the iterator range.
 */
template<typename TIter>
static uint8_t nearest(const TIter& begin, const TIter& end, long double value)
{
    auto it = std::lower_bound(begin, end, value);
    if(it == begin) return 0;
    auto prev_it = std::prev(it);
    auto nearest = (it == end || value - *prev_it <= *it - value) ? prev_it : it;
    assert(std::distance(begin, nearest) <= std::numeric_limits<uint8_t>::max());
    return gsl::narrow<uint8_t>(std::distance(begin, nearest));
}

ImageData::AMP8I_PHS8I_t ComplexToAMP8IPHS8I::nearest_neighbor(const std::complex<float> &v) const
{
    ImageData::AMP8I_PHS8I_t ans;

    // Phase is determined via arithmetic because it's equally spaced.
    // There's an intentional conversion to zero when we cast 256 -> uint8. That wrap around
    // handles cases that are close to 2PI.
    ans.second = gsl::narrow<uint8_t>(std::round(GetPhase(v) / phase_delta));

    // We have to do a 1D nearest neighbor search for magnitude.
    // But it's not the magnitude of the input complex value - it's the projection of
    // the complex value onto the ray of candidate magnitudes at the selected phase.
    // I.e. dot product.
    const auto direction = phase_directions[ans.second];
    const auto projection = direction.real() * v.real() + direction.imag() * v.imag();
    assert(std::abs(projection - std::abs(v)) < 1e-5);
    ans.first = nearest(magnitudes.begin(), magnitudes.end(), projection);
    return ans;
}

}
}

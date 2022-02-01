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
#include "six/sicd/ComplexToAMP8IPHS8I.h"

#include <math.h>

#include <cassert>
#include <std/memory>

#include <gsl/gsl.h>
#include <six/sicd/Utilities.h>
#include <math/Utilities.h>

/*!
    * Get the phase of a complex value.
    * @param v complex value
    * @return phase between [0, 2PI]
    */
inline long double GetPhase(const std::complex<float>& v)
{
    long double phase = std::arg(v);
    if (phase < 0.0) phase += M_PI * 2.0; // Wrap from [0, 2PI]
    return phase;
}

static std::vector<long double> make_magnitudes(const six::AmplitudeTable* pAmplitudeTable)
{
    std::vector<long double> retval(UINT8_MAX + 1);
    for (size_t i = 0; i <= UINT8_MAX; i++) // Be careful with indexing so that we don't wrap-around in the loops.
    {
        static_assert(sizeof(size_t) > sizeof(uint8_t), "size_t can't hold UINT8_MAX!");
        // AmpPhase -> Complex
        six::sicd::AMP8I_PHS8I_t v;
        v.first = v.second = gsl::narrow<uint8_t>(i);
        const auto complex = six::sicd::Utilities::from_AMP8I_PHS8I(v.first, v.second, pAmplitudeTable);
        retval[i] = std::abs(complex);
    }

    // I don't know if we can guarantee that the amplitude table is non-decreasing.
    // Check to verify property at runtime.
    if (!std::is_sorted(retval.begin(), retval.end()))
    {
        throw std::runtime_error("magnitudes must be sorted");
    }
    return retval;
}
static const std::vector<long double>* get_magnitudes(const six::AmplitudeTable* pAmplitudeTable,
    std::unique_ptr<const std::vector<long double>>& pUncachedMagnitudes)
{
    if (pAmplitudeTable == nullptr)
    {
        static const auto magnitudes = make_magnitudes(nullptr); // OK to cache, won't change
        return &magnitudes;
    }
    
    auto result = make_magnitudes(pAmplitudeTable);
    pUncachedMagnitudes = std::make_unique<const std::vector<long double>>(std::move(result));
    return pUncachedMagnitudes.get();
}

six::sicd::details::ComplexToAMP8IPHS8I::ComplexToAMP8IPHS8I(const six::AmplitudeTable *pAmplitudeTable)
    : pMagnitudes(get_magnitudes(pAmplitudeTable, pUncachedMagnitudes))
{
    const auto p0 = GetPhase(Utilities::from_AMP8I_PHS8I(1, 0, pAmplitudeTable));
    const auto p1 = GetPhase(Utilities::from_AMP8I_PHS8I(1, 1, pAmplitudeTable));
    assert(p0 == 0);
    assert(p1 > p0);
    phase_delta = p1 - p0;
    for(size_t i = 0; i <= UINT8_MAX; i++) // Be careful with indexing so that we don't wrap-around in the loops.
    {
        static_assert(sizeof(size_t) > sizeof(uint8_t), "size_t can't hold UINT8_MAX!");
        long double y, x;
        math::SinCos(p0 + gsl::narrow_cast<long double>(i) * phase_delta, y, x);
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
    const auto it = std::lower_bound(begin, end, value);
    if(it == begin) return 0;

    const auto prev_it = std::prev(it);
    const auto nearest = (it == end || value - *prev_it <= *it - value) ? prev_it : it;
    const auto distance = std::distance(begin, nearest);
    assert(distance <= std::numeric_limits<uint8_t>::max());
    return gsl::narrow<uint8_t>(distance);
}

six::sicd::AMP8I_PHS8I_t six::sicd::details::ComplexToAMP8IPHS8I::nearest_neighbor(const std::complex<float> &v) const
{
    six::sicd::AMP8I_PHS8I_t retval;

    // Phase is determined via arithmetic because it's equally spaced.
    // There's an intentional conversion to zero when we cast 256 -> uint8. That wrap around
    // handles cases that are close to 2PI.
    retval.second = gsl::narrow_cast<uint8_t>(std::round(GetPhase(v) / phase_delta));

    // We have to do a 1D nearest neighbor search for magnitude.
    // But it's not the magnitude of the input complex value - it's the projection of
    // the complex value onto the ray of candidate magnitudes at the selected phase.
    // i.e. dot product.
    auto&& phase_direction = phase_directions[retval.second];
    const auto projection = phase_direction.real() * v.real() + phase_direction.imag() * v.imag();
    //assert(std::abs(projection - std::abs(v)) < 1e-5); // TODO ???
    const auto& magnitudes = *pMagnitudes;
    retval.first = nearest(magnitudes.begin(), magnitudes.end(), projection);
    return retval;
}

const six::sicd::details::ComplexToAMP8IPHS8I* six::sicd::details::ComplexToAMP8IPHS8I::make(const six::AmplitudeTable* pAmplitudeTable, std::unique_ptr<ComplexToAMP8IPHS8I>& pTree)
{
    if (pAmplitudeTable == nullptr)
    {
        // this won't change, so OK to cache
        static const six::sicd::details::ComplexToAMP8IPHS8I tree{};
        return &tree;
    }
    else
    {
        pTree.reset(new six::sicd::details::ComplexToAMP8IPHS8I(pAmplitudeTable));
        return pTree.get();
    }
}
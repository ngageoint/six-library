/* =========================================================================
 * This file is part of six.sicd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 * © Copyright 2024, Maxar Technologies, Inc.
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
#include <std/numbers>
#include <algorithm>
#include <functional>
#include <stdexcept>

#include <mt/Algorithm.h>
#include <gsl/gsl.h>

#include "six/sicd/ImageData.h"

#undef min
#undef max

using zfloat = six::zfloat;
using AMP8I_PHS8I = six::AMP8I_PHS8I_t;

six::sicd::NearestNeighbors::NearestNeighbors(const details::ComplexToAMP8IPHS8I& converter) : converter_(converter) {}

uint8_t six::sicd::NearestNeighbors::getPhase(zfloat z) const
{
    return converter_.getPhase(z);
}

/*!
 * Find the nearest element given an iterator range.
 * @param value query value
 * @return index of nearest value within the iterator range.
 */
static uint8_t nearest(std::span<const float> magnitudes, float value)
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
uint8_t six::sicd::NearestNeighbors::find_nearest(zfloat phase_direction, zfloat v) const
{
    // We have to do a 1D nearest neighbor search for magnitude.
    // But it's not the magnitude of the input complex value - it's the projection of
    // the complex value onto the ray of candidate magnitudes at the selected phase.
    // i.e. dot product.
    const auto projection = (phase_direction.real() * v.real()) + (phase_direction.imag() * v.imag());
    //assert(std::abs(projection - std::abs(v)) < 1e-5); // TODO ???
    return nearest(converter_.magnitudes(), projection);
}
AMP8I_PHS8I six::sicd::NearestNeighbors::nearest_neighbor(const zfloat &v) const
{
    AMP8I_PHS8I retval;
    retval.phase = getPhase(v);

    auto&& phase_direction = converter_.get_phase_directions().value[retval.phase];
    retval.amplitude = find_nearest(phase_direction, v);
    return retval;
}
void six::sicd::NearestNeighbors::nearest_neighbors_seq(std::span<const zfloat> inputs, std::span<AMP8I_PHS8I_t> results) const
{
    std::transform(inputs.begin(), inputs.end(), results.begin(),
        [&](const auto& v) { return nearest_neighbor(v); });
}

void six::sicd::NearestNeighbors::nearest_neighbors_par(std::span<const zfloat> inputs, std::span<AMP8I_PHS8I_t> results) const
{
    const auto func = [&](const auto& v)
    {
        return nearest_neighbor(v);
    };
    std::ignore = mt::Transform_par(inputs.begin(), inputs.end(), results.begin(), func);
}

void six::sicd::NearestNeighbors::nearest_neighbors(std::span<const zfloat> inputs, std::span<AMP8I_PHS8I_t> results) const
{
    #if SIX_sicd_ComplexToAMP8IPHS8I_unseq
    return nearest_neighbors_par_unseq(inputs, results); // there might be additional logic here

    #else
    return nearest_neighbors_par(inputs, results);     // No SIMD support, use parallel

    #endif
}
void six::sicd::NearestNeighbors::nearest_neighbors(execution_policy policy, std::span<const zfloat> inputs, std::span<AMP8I_PHS8I_t> results) const
{
    #if SIX_sicd_ComplexToAMP8IPHS8I_unseq    
    if (policy == execution_policy::par_unseq)
    {
        return nearest_neighbors_par_unseq(inputs, results);
    }
    if (policy == execution_policy::unseq)
    {
        return nearest_neighbors_unseq(inputs, results);
    }
    #endif

    if (policy == execution_policy::par)
    {
        return nearest_neighbors_par(inputs, results);
    }
    if (policy == execution_policy::seq)
    {
        return nearest_neighbors_seq(inputs, results);
    }

    // https://en.cppreference.com/w/cpp/algorithm/execution_policy_tag_t
    // > If the implementation cannot parallelize or vectorize (e.g. due to lack of resources),
    // > all standard execution policies can fall back to sequential execution.
    #if CODA_OSS_DEBUG
    throw std::logic_error("Unhandled execution_policy value.");
    #else
    return nearest_neighbors_seq(inputs, results); // "... fall back to sequential execution."
    #endif
}

AMP8I_PHS8I six::sicd::nearest_neighbor(const details::ComplexToAMP8IPHS8I& converter_, const zfloat& v)
{
    const NearestNeighbors converter(converter_);
    return converter.nearest_neighbor(v);
}

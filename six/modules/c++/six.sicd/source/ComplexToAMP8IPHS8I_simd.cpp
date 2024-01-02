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
#include <execution>
#include <array>
#include <span>

#include <gsl/gsl.h>
#include <math/Utilities.h>
#include <units/Angles.h>
#include <sys/Span.h>

#include "six/sicd/Utilities.h"

#if SIX_sicd_has_simd

#include <experimental/simd>

// https://en.cppreference.com/w/cpp/experimental/simd
using floatv = std::experimental::native_simd<float>;
// using doublev = std::experimental::rebind_simd_t<double, floatv>;
using intv = std::experimental::rebind_simd_t<int, floatv>;
using zfloatv = std::array<floatv, 2>;

static inline auto load(std::span<const six::zfloat> p)
{
    const auto generate_real = [&](size_t i) {
        return p[i].real();
    };
    const auto generate_imag = [&](size_t i) {
        return p[i].imag();
    };

    zfloatv retval;
    retval[0] = floatv(generate_real);
    retval[1] = floatv(generate_imag);
    return retval;
}


// https://en.cppreference.com/w/cpp/numeric/complex/arg
// > `std::atan2(std::imag(z), std::real(z))`
inline auto arg(const floatv& real, const floatv& imag)
{
    return atan2(imag, real); // arg()
}

static auto getPhase(const floatv& real, const floatv& imag, float phase_delta)
{
    // Phase is determined via arithmetic because it's equally spaced.
    // There's an intentional conversion to zero when we cast 256 -> uint8. That wrap around
    // handles cases that are close to 2PI.
    auto phase = arg(real, imag);
    where (phase < 0.0f, phase) += std::numbers::pi_v<float> * 2.0f; // Wrap from [0, 2PI]
    return round(phase / phase_delta);
}
static inline auto getPhase(const zfloatv& v, float phase_delta)
{
    return getPhase(v[0], v[1], phase_delta);
}

#if 0

inline auto lower_bound_(std::span<const float> magnitudes, const floatv& v)
{
    intv first = 0;
    const intv last = gsl::narrow<int>(magnitudes.size());

    auto count = last - first;
    while (horizontal_or(count > 0))
    {
        auto it = first;
        const auto step = count / 2;
        it += step;

        auto next = it; ++next; // ... ++it;
        auto advance = count; advance -= step + 1;  // ...  -= step + 1;

        const auto c = vcl::lookup<six::AmplitudeTableSize>(it, magnitudes.data()); // magnituides[it]
        const auto test = c < v;
        it = select(test, next, it); // ... ++it
        first = select(test, it, first); // first = ...
        count = select(test, advance, step); // count -= step + 1 <...OR...> count = step
    }
    return first;
}
inline auto lower_bound(std::span<const float> magnitudes, const floatv& value)
{
    //const auto begin = magnitudes.begin();
    //const auto end = magnitudes.end();

    auto retval = lower_bound_(magnitudes, value);

    //intv retval_;
    //for (int i = 0; i < value.size(); i++)
    //{
    //    const auto it = std::lower_bound(begin, end, value[i]);
    //    const auto result = std::distance(begin, it);
    //    retval_.insert(i, gsl::narrow<int>(result));

    //    assert(retval[i] == retval_[i]);
    //}

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
#endif // #if 0

void six::sicd::details::ComplexToAMP8IPHS8I::Impl::nearest_neighbors_unseq_(std::span<const six::zfloat> p, std::span<AMP8I_PHS8I_t> results) const
{
    const auto v = load(p);

    const auto phase = ::getPhase(v, phase_delta);
    for (size_t i = 0; i < phase.size(); i++)
    {
        const auto ph = getPhase(p[i]);
        assert(ph == gsl::narrow_cast<uint8_t>(phase[i]));
    }

    /*
    //const auto phase_direction = lookup(phase, phase_directions);
    //const auto amplitude = ::find_nearest(magnitudes, phase_direction, v);
    const auto phase_direction_real = vcl::lookup<six::AmplitudeTableSize>(phase, phase_directions_real.data());
    const auto phase_direction_imag = vcl::lookup<six::AmplitudeTableSize>(phase, phase_directions_imag.data());
    const auto amplitude = ::find_nearest(magnitudes, phase_direction_real, phase_direction_imag, v);

    // interleave() and store() is slower than an explicit loop.
    auto dest = results.begin();
    for (int i = 0; i < v.size(); i++)
    {
        dest->phase = gsl::narrow_cast<uint8_t>(phase[i]);

        //dest->amplitude = find_nearest(phase_directions[dest->phase], p[i]);
        //const auto phase_direction_ = phase_direction.extract(i);
        //dest->amplitude = find_nearest(six::zfloat(phase_direction_.real(), phase_direction_.imag()), p[i]);
        //dest->amplitude = find_nearest(six::zfloat(phase_direction_real[i], phase_direction_imag[i]), p[i]);
        dest->amplitude = gsl::narrow_cast<uint8_t>(amplitude[i]);

        ++dest;
    }
    */
}

void six::sicd::details::ComplexToAMP8IPHS8I::Impl::nearest_neighbors_unseq(std::span<const six::zfloat> inputs, std::span<AMP8I_PHS8I_t> results) const
{
    auto first = inputs.begin();
    const auto last = inputs.end();
    auto dest = results.begin();

    // The above code is simpler (no templates) if we use just a single VCL
    // complex type: `zfloatv`.  If there is any performance difference,
    // it will only be for extreme edge cases since the smaller types are only used
    // at the end of the loop.
    //
    // It also makes this loop simpler as we handle all non-multiples-of-8 in
    // the same way.
    constexpr auto elements_per_iteration = 8;

    // Can do these checks one-time outside of the loop
    const auto distance = std::distance(first, last);

    // First, do multiples of 8
    const auto distance_ = distance - (distance % elements_per_iteration);
    const auto last_ = first + distance_;
    for (; first != last_; first += elements_per_iteration, dest += elements_per_iteration)
    {
        const auto f = sys::make_span(&(*first), elements_per_iteration);
        const auto d = sys::make_span(&(*dest), elements_per_iteration);
        nearest_neighbors_unseq_(f, d);
    }

    // Then finish off anything left
    assert(std::distance(first, last) < elements_per_iteration);
    for (; first != last; ++first, ++dest)
    {
        const auto f = sys::make_span(&(*first), 1);
        const auto d = sys::make_span(&(*dest), 1);
        nearest_neighbors_seq(f, d);
    }
}
std::vector<six::AMP8I_PHS8I_t> six::sicd::details::ComplexToAMP8IPHS8I::nearest_neighbors_unseq_vcl(
    std::span<const zfloat> inputs, const six::AmplitudeTable* pAmplitudeTable)
{
    // make a structure to quickly find the nearest neighbor
    const auto& converter = make_(pAmplitudeTable);

    std::vector<six::AMP8I_PHS8I_t> retval(inputs.size());
    converter.impl.nearest_neighbors_unseq(inputs, sys::make_span(retval));
    return retval;
}

/**********************************************************************

// This is here (instead of **ComplexToAMP8IPHS8I.cpp**) because par_unseq() might
// need to know implementation details of _unseq()
using input_it = std::span<const six::zfloat>::iterator;
using output_it = std::span<six::AMP8I_PHS8I_t>::iterator;

struct const_iterator final
{
    using Type = input_it::value_type;

    using iterator_category = std::random_access_iterator_tag;
    using value_type = std::remove_cv_t<Type>;
    using difference_type = std::ptrdiff_t;
    using pointer = Type*;
    using reference = Type&;
    using const_reference = const Type&;

    input_it current_;

    const_iterator() = default;
    const_iterator(input_it it) : current_(it) {}

    const_reference operator*() const noexcept
    {
        return *current_;
    }

    const_iterator& operator++() noexcept
    {
        for (ptrdiff_t i = 0; i < 8; i++)
        {
            ++current_;
        }
        return *this;
    }

    const_iterator& operator--() noexcept
    {
        --current_;
        return *this;
    }

    const_iterator& operator-=(const difference_type n) noexcept
    {
        current_ -= n;
        return *this;
    }
    const_iterator operator-(const difference_type n) const noexcept
    {
        auto ret = *this;
        ret -= n;
        return ret;
    }
    difference_type operator-(const const_iterator& rhs) const noexcept
    {
        return current_ - rhs.current_;
    }

    bool operator!=(const const_iterator& rhs) const noexcept
    {
        return current_ != rhs.current_;
    }
};

struct result_wrapper final
{
    output_it current_;

    result_wrapper& operator=(const std::vector<six::AMP8I_PHS8I_t>& values)
    {
        for (auto& v : values)
        {
            *current_ = v;
            ++current_;
        }
        return *this;
    }
};

struct iterator final
{
    using Type = output_it::value_type;

    using iterator_category = std::random_access_iterator_tag;
    using value_type = std::remove_cv_t<Type>;
    using difference_type = std::ptrdiff_t;
    using pointer = Type*;
    using reference = Type&;

    output_it current_;

    iterator() = default;
    iterator(output_it it) : current_(it) {}

    result_wrapper operator*() noexcept
    {
        return result_wrapper{ current_};
    }

    iterator& operator++() noexcept
    {
        for (ptrdiff_t i = 0; i < 8; i++)
        {
            ++current_;
        }
        return *this;
    }

    iterator& operator--() noexcept
    {
        --current_;
        return *this;
    }

    iterator& operator-=(const difference_type n) noexcept
    {
        current_ -= n;
        return *this;
    }

    bool operator!=(const iterator& rhs) const noexcept
    {
        return current_ != rhs.current_;
    }
};

void six::sicd::details::ComplexToAMP8IPHS8I::Impl::nearest_neighbors_par_unseq(std::span<const six::zfloat> inputs, std::span<AMP8I_PHS8I_t> results) const
{
    const auto first = inputs.begin();
    const auto last = inputs.end();
    auto dest = results.begin();

    const auto func = [&](const auto& v) {
        std::span<const six::zfloat> values(&v, 8);

        std::vector<six::AMP8I_PHS8I_t> retval(values.size());
        nearest_neighbors_unseq(values, sys::make_span(retval));
        return retval;
    };

    std::ignore = std::transform(std::execution::seq,
        const_iterator{ first }, const_iterator{ last }, iterator{ dest }, func);
}
#if SIX_sicd_ComplexToAMP8IPHS8I_unseq
std::vector<six::AMP8I_PHS8I_t> six::sicd::details::ComplexToAMP8IPHS8I::nearest_neighbors_par_unseq(
    std::span<const zfloat> inputs, const six::AmplitudeTable* pAmplitudeTable)
{
    // make a structure to quickly find the nearest neighbor
    const auto& converter = make_(pAmplitudeTable);

    std::vector<six::AMP8I_PHS8I_t> retval(inputs.size());
    converter.impl.nearest_neighbors_par_unseq(inputs, sys::make_span(retval));
    return retval;
}
#endif //  SIX_sicd_ComplexToAMP8IPHS8I_unseq

**********************************************************************/

#endif // SIX_sicd_has_simd

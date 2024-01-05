/* =========================================================================
 * This file is part of sys-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 * © Copyright 2024, Maxar Technologies, Inc.
 *
 * sys-c++ is free software; you can redistribute it and/or modify
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

#include <complex>
#include <vector>
#include <std/span>
#include <std/numbers>

// These are unittests, always enable unless explicitly disabled
#ifndef CODA_OSS_Ximd_ENABLED
#define CODA_OSS_Ximd_ENABLED 1
#endif
#include <sys/Ximd.h>

#include <gsl/gsl.h>
#include <sys/Span.h>

#include "TestCase.h"

using zfloat = std::complex<float>;

template<typename T>
using simd = sys::ximd::Ximd<T>;
using intv = simd<int>;
using floatv = simd<float>;
using intv_mask = sys::ximd::ximd_mask;
using floatv_mask = sys::ximd::ximd_mask;

// Manage a SIMD complex as an array of two SIMDs
using zfloatv = std::array<floatv, 2>;
static inline auto& real(zfloatv& z) noexcept
{
    return z[0];
}
static inline const auto& real(const zfloatv& z) noexcept
{
    return z[0];
}
static inline auto& imag(zfloatv& z) noexcept
{
    return z[1];
}
static inline const auto& imag(const zfloatv& z) noexcept
{
    return z[1];
}
static inline size_t size(const zfloatv& z) noexcept
{
    auto retval = real(z).size();
    assert(retval == imag(z).size());
    return retval;
}

static inline auto arg(const zfloatv& z)
{
    // https://en.cppreference.com/w/cpp/numeric/complex/arg
    // > `std::atan2(std::imag(z), std::real(z))`
    return atan2(imag(z), real(z));  // arg()
}

template <typename TGeneratorReal, typename TGeneratorImag>
static inline auto make_zfloatv(TGeneratorReal&& generate_real, TGeneratorImag&& generate_imag)
{
    zfloatv retval;
    for (size_t i = 0; i < size(retval); i++)
    {
        real(retval)[i] = generate_real(i);
        imag(retval)[i] = generate_imag(i);
    }
    return retval;
}

template <typename T>
static inline auto copy_from(std::span<const T> p)
{
    simd<T> retval;
    assert(p.size() == retval.size());
    retval.copy_from(p.data());
    return retval;
}
static inline auto copy_from(std::span<const zfloat> p)
{
    const auto generate_real = [&](size_t i) { return p[i].real(); };
    const auto generate_imag = [&](size_t i) { return p[i].imag(); };
    return make_zfloatv(generate_real, generate_imag);
}

template<typename T>
static inline auto copy_to(const simd<T>& v)
{
    std::vector<typename simd<T>::value_type> retval(v.size());
    v.copy_to(retval.data());
    return retval;
}
static inline auto copy_to(const zfloatv& v)
{
    std::vector<zfloat> retval;
    for (size_t i = 0; i < size(v); i++)
    {        
        retval.emplace_back(real(v)[i], imag(v)[i]);
    }
    return retval;
}

TEST_CASE(testDefaultConstructor)
{
    // sanity check implementation and utility routines
    TEST_ASSERT_EQ(floatv::size(), size(zfloatv{}));

    // intv v = 1;
    intv v;
    v = 1;
    for (size_t i = 0; i < v.size(); i++)
    {
        TEST_ASSERT_EQ(1, v[i]);
    }
}

// Sample code from SIX; see **ComplexToAMP8IPHS8I.cpp**.
static auto GetPhase(std::complex<double> v)
{
    // There's an intentional conversion to zero when we cast 256 -> uint8. That wrap around
    // handles cases that are close to 2PI.
    double phase = std::arg(v);
    if (phase < 0.0) phase += coda_oss::numbers::pi * 2.0; // Wrap from [0, 2PI]
    return phase;
}
static uint8_t getPhase(zfloat v, float phase_delta)
{
    // Phase is determined via arithmetic because it's equally spaced.
    const auto phase = GetPhase(v);
    return gsl::narrow_cast<uint8_t>(std::round(phase / phase_delta));
}

static inline auto if_add(const floatv_mask& m, const floatv& lhs, typename floatv::value_type rhs)
{
    const auto generate_add = [&](size_t i) {
        return m[i] ? lhs[i] + rhs : lhs[i];
    };
    return floatv(generate_add);
}
static inline auto roundi(const floatv& v)  // match vcl::roundi()
{
    const auto rounded = round(v);
    const auto generate_roundi = [&](size_t i)
    { return static_cast<typename intv::value_type>(rounded[i]); };
    return intv(generate_roundi);
}
static auto getPhase(const zfloatv& v, float phase_delta)
{
    // Phase is determined via arithmetic because it's equally spaced.
    // There's an intentional conversion to zero when we cast 256 -> uint8. That wrap around
    // handles cases that are close to 2PI.
    auto phase = arg(v);
    //where(phase < 0.0f, phase) += std::numbers::pi_v<float> *2.0f; // Wrap from [0, 2PI]
    phase = if_add(phase < 0.0f, phase, std::numbers::pi_v<float> * 2.0f); // Wrap from [0, 2PI]
    return roundi(phase / phase_delta);
}
static inline const auto& cxValues()
{
    //static const std::vector<zfloat> retval{/*{0, 0},*/ {1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}, {-1, -1}, {0, -1}, {1, -1}};
    static const std::vector<zfloat> retval{{0.0, 0.0}, {1.0, 1.0}, {10.0, -10.0}, {-100.0, 100.0}, {-1000.0, -1000.0}, // sample data from SIX
            {-1.0, -1.0}, {1.0, -1.0}, {-1.0, 1.0} // "pad" to multiple of floatv::size()
    };
    return retval;
}

static auto expected_getPhase_values(const std::string& testName, float phase_delta)
{
    auto&& values = cxValues();
    TEST_ASSERT(values.size() % floatv::size() == 0);
    std::vector<uint8_t> expected;
    for (auto&& v : values)
    {
        expected.push_back(getPhase(v, phase_delta));
    }
    return expected;
}

static auto toComplex_(double A, uint8_t phase)
{
    // The phase values should be read in (values 0 to 255) and converted to float by doing:
    // P = (1 / 256) * input_value
    const double P = (1.0 / 256.0) * phase;

    // To convert the amplitude and phase values to complex float (i.e. real and imaginary):
    // S = A * cos(2 * pi * P) + j * A * sin(2 * pi * P)
    const double angle = 2 * std::numbers::pi * P;
    const auto sin_angle = sin(angle);
    const auto cos_angle = cos(angle);
    zfloat S(gsl::narrow_cast<float>(A * cos_angle), gsl::narrow_cast<float>(A * sin_angle));
    return S;
}
inline static auto toComplex(uint8_t amplitude, uint8_t phase)
{
    // A = input_amplitude(i.e. 0 to 255)
    const double A = amplitude;
    return toComplex_(A, phase);
}

static auto phase_delta()
{
    static const auto p0 = GetPhase(toComplex(1, 0));
    static const auto p1 = GetPhase(toComplex(1, 1));
    assert(p0 == 0.0);
    assert(p1 > p0);
    static const auto retval = gsl::narrow_cast<float>(p1 - p0);
    return retval;
}

static auto load(const std::vector<zfloat>& values)
{
    std::vector<zfloatv> retval;
    constexpr auto sz = floatv::size();
    auto const pValues = sys::make_span(values);
    auto p = sys::make_span(pValues.data(), sz);
    for (size_t i = 0; i < values.size() / sz; i++)
    {
        retval.push_back(copy_from(p));
        p = sys::make_span(p.data() + sz, p.size());
    }
    return retval;
}

TEST_CASE(testGetPhase)
{
    static const auto& expected = expected_getPhase_values(testName, phase_delta());

    const auto valuesv = load(cxValues());
    std::vector<uint8_t> actual;
    for (auto&& zvaluev : valuesv)
    {
        const auto phase = getPhase(zvaluev, phase_delta());
        const auto phase_ = copy_to(phase);
        for (auto&& ph : copy_to(phase))
        {
            actual.push_back(gsl::narrow_cast<uint8_t>(ph));
        }
    }

    TEST_ASSERT_EQ(actual.size(), expected.size());
    for (size_t i = 0; i < actual.size(); i++)
    {
        TEST_ASSERT_EQ(actual[i], expected[i]);
    }
}

// Again, more sample code from SIX
static constexpr size_t AmplitudeTableSize = 256;
static auto getPhaseDirections_()
{
    //! Unit vector rays that represent each direction that phase can point.
    std::array<zfloat, AmplitudeTableSize> phase_directions; // interleaved, std::complex<float>

     const auto p0 = GetPhase(toComplex(1, 0));
    for (size_t i = 0; i < AmplitudeTableSize; i++)
    {
        const float angle = static_cast<float>(p0) + i * phase_delta();
        const auto y = sin(angle);
        const auto x = cos(angle);
        phase_directions[i] = {x, y};
    }
    return phase_directions;
}
static inline const auto& getPhaseDirections()
{
    static const auto retval = getPhaseDirections_();
    return retval;
}

template <size_t N>
static inline auto lookup(const intv& zindex, const std::array<zfloat, N>& phase_directions)
{
    const auto generate_real = [&](size_t i)
    {
        const auto i_ = zindex[i];
        return phase_directions[i_].real();
    };
    const auto generate_imag = [&](size_t i)
    {
        const auto i_ = zindex[i];
        return phase_directions[i_].imag();
    };
    return make_zfloatv(generate_real, generate_imag);
}

TEST_CASE(testLookup)
{
    const auto& phase_directions = getPhaseDirections();

    static const auto& expected_getPhase = expected_getPhase_values(testName, phase_delta());
    std::vector<zfloat> expected;
    for (auto&& phase : expected_getPhase)
    {
        expected.push_back(phase_directions[phase]);
    }
    
    const auto valuesv = load(cxValues());
    std::vector<zfloat> actual;
    for (auto&& zvaluev : valuesv)
    {
        const auto phase = getPhase(zvaluev, phase_delta());
        const auto phase_direction = lookup<AmplitudeTableSize>(phase, phase_directions);

        for (auto&& pd : copy_to(phase_direction))
        {
            actual.push_back(pd);
        }
    }

    TEST_ASSERT_EQ(actual.size(), expected.size());
    for (size_t i = 0; i < actual.size(); i++)
    {
        TEST_ASSERT_EQ(actual[i], expected[i]);
    }
}

// And ... more sample code from SIX
static auto iota_0_256_()
{
    static_assert(sizeof(size_t) > sizeof(uint8_t), "size_t can't hold UINT8_MAX!");

    std::vector<uint8_t> retval;
    retval.reserve(UINT8_MAX + 1);
    for (size_t i = 0; i <= UINT8_MAX; i++) // Be careful with indexing so that we don't wrap-around in the loop.
    {
        retval.push_back(gsl::narrow<uint8_t>(i));
    }
    assert(retval.size() == UINT8_MAX + 1);
    return retval;
}
static inline std::vector<uint8_t> iota_0_256()
{
    static const auto retval = iota_0_256_();
    return retval;
}

static auto make_magnitudes_()
{
    std::vector<float> result;
    result.reserve(AmplitudeTableSize);
    for (const auto amplitude : iota_0_256())
    {
        // AmpPhase -> Complex
        const auto phase = amplitude;
        const auto complex = toComplex(amplitude, phase);
        result.push_back(std::abs(complex));
    }

    // I don't know if we can guarantee that the amplitude table is non-decreasing.
    // Check to verify property at runtime.
    if (!std::is_sorted(result.begin(), result.end()))
    {
        throw std::runtime_error("magnitudes must be sorted");
    }

    std::array<float, AmplitudeTableSize> retval;
    std::copy(result.begin(), result.end(), retval.begin());
    return retval;
}
static inline std::span<const float> magnitudes()
{
    //! The sorted set of possible magnitudes order from small to large.
    static const auto cached_magnitudes = make_magnitudes_();
    return sys::make_span(cached_magnitudes);
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
static uint8_t find_nearest(zfloat phase_direction, zfloat v)
{
    // We have to do a 1D nearest neighbor search for magnitude.
    // But it's not the magnitude of the input complex value - it's the projection of
    // the complex value onto the ray of candidate magnitudes at the selected phase.
    // i.e. dot product.
    const auto projection = (phase_direction.real() * v.real()) + (phase_direction.imag() * v.imag());
    //assert(std::abs(projection - std::abs(v)) < 1e-5); // TODO ???
    return nearest(magnitudes(), projection);
}

template<typename TTest, typename TResult>
static inline auto select(const TTest& test, const TResult& t, const TResult& f)
{
    TResult retval;
    for (size_t i = 0; i < test.size(); i++)
    {
        retval[i] = test[i] ? t[i] : f[i];
    }
    return retval;
}

static auto lookup(const intv& zindex, std::span<const float> magnitudes)
{
    const auto generate = [&](size_t i) {
        const auto i_ = zindex[i];
        
        // The index may be out of range. This is expected because `i` might be "don't care."
        if ((i_ >= 0) && (i_ < std::ssize(magnitudes)))
        {
            return magnitudes[i_];
        }
        return NAN; // propogate "don't care"
    };
    return floatv(generate);
}

static inline auto lower_bound_(std::span<const float> magnitudes, const floatv& v)
{
    intv first; first = 0;
    intv last; last = gsl::narrow<int>(magnitudes.size());

    auto count = last - first;
    while (any_of(count > 0))
    {
        auto it = first;
        const auto step = count / 2;
        it += step;

        auto next = it; ++next; // ... ++it;
        auto advance = count; advance -= step + 1;  // ...  -= step + 1;

        const auto c = lookup(it, magnitudes); // magnituides[it]

        const auto test = c < v; // (c < v).__cvt(); // https://github.com/VcDevel/std-simd/issues/41

        //where(test, it) = next; // ... ++it
        //where(test, first) = it; // first = ...
        //// count -= step + 1 <...OR...> count = step
        //where(test, count) = advance;
        //where(!test, count) = step;
        it = select(test, next, it); // ... ++it
        first = select(test, it, first); // first = ...
        count = select(test, advance, step); // count -= step + 1 <...OR...> count = step
    }
    return first;
}
static inline auto lower_bound(std::span<const float> magnitudes, const floatv& value)
{
    return lower_bound_(magnitudes, value);
}

static auto nearest(std::span<const float> magnitudes, const floatv& value)
{
    assert(magnitudes.size() == AmplitudeTableSize);

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
    const auto it = lower_bound(magnitudes, value);
    const auto prev_it  = it - 1;  // const auto prev_it = std::prev(it);

    const auto v0 = value - lookup(prev_it, magnitudes); // value - *prev_it
    const auto v1 = lookup(it, magnitudes) - value; // *it - value
    //const auto nearest_it = select(v0 <= v1, prev_it, it); //  (value - *prev_it <= *it - value ? prev_it : it);
    
    intv end; end = gsl::narrow<int>(magnitudes.size());
    //const auto end_test = select(it == end, prev_it, nearest_it); // it == end ? prev_it  : ...
    intv zero; zero = 0;
    auto retval = select(it == 0, zero, // if (it == begin) return 0;
        select(it == end, prev_it,  // it == end ? prev_it  : ...
            select(v0 <= v1, prev_it, it) //  (value - *prev_it <= *it - value ? prev_it : it);
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
    const auto projection = (phase_direction_real * real(v)) + (phase_direction_imag * imag(v));
    //assert(std::abs(projection - std::abs(v)) < 1e-5); // TODO ???
    return nearest(magnitudes, projection);
}
static inline auto find_nearest(std::span<const float> magnitudes, const zfloatv& phase_direction, const zfloatv& v)
{
    return find_nearest(magnitudes, real(phase_direction), imag(phase_direction), v);
}

TEST_CASE(testFindNearest)
{
    const auto& values = cxValues();
    const auto& phase_directions = getPhaseDirections();

    static const auto& expected_getPhase = expected_getPhase_values(testName, phase_delta());
    std::vector<zfloat> expected_phase_directions;
    for (auto&& phase : expected_getPhase)
    {
        expected_phase_directions.push_back(phase_directions[phase]);
    }
    std::vector<uint8_t> expected;
    for (size_t i = 0; i < values.size(); i++)
    {
        auto a = find_nearest(expected_phase_directions[i], values[i]);
        expected.push_back(a);
    }
    
    const auto valuesv = load(cxValues());
    std::vector<uint8_t> actual;
    for (auto&& v : valuesv)
    {
        const auto phase = getPhase(v, phase_delta());
        const auto phase_direction = lookup<AmplitudeTableSize>(phase, phase_directions);
        const auto amplitude = find_nearest(magnitudes(), phase_direction, v);

        for (auto&& a : copy_to(amplitude))
        {
            actual.push_back(static_cast<uint8_t>(a));
        }
    }

    TEST_ASSERT_EQ(actual.size(), expected.size());
    for (size_t i = 0; i < actual.size(); i++)
    {
        TEST_ASSERT_EQ(actual[i], expected[i]);
    }
}

TEST_MAIN(
    TEST_CHECK(testDefaultConstructor);

    TEST_CHECK(testGetPhase);
    TEST_CHECK(testLookup);
    TEST_CHECK(testFindNearest);
)

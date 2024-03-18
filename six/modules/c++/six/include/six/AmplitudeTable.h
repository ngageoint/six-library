/* =========================================================================
 * This file is part of six-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * six-c++ is free software; you can redistribute it and/or modify
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
#pragma once
#ifndef SIX_six_AmplitudeTable_h_INCLUDED_
#define SIX_six_AmplitudeTable_h_INCLUDED_

#include <stdint.h>
#include <limits.h>

#include <vector>
#include <limits>
#include <string>
#include <memory>
#include <array>
#include <std/mdspan>

#include <import/except.h>
#include <coda_oss/CPlusPlus.h>
#include <sys/Dbg.h>

#include <nitf/LookupTable.hpp>
#include <scene/sys_Conf.h>

#include "six/Complex.h"
#include "six/Exports.h"

namespace six
{
/*!
 *  \struct LUT
 *  \brief Lookup table
 *
 *  This lookup table object has a number of entries (for pixel ops,
 *  this is usually 256), and an output space (size) which can be anything
 *  (e.g., 2 for short) depending on the type that is being housed
 *
 */
struct SIX_SIX_API LUT
{
    std::vector<unsigned char> table;
    size_t numEntries = 0;
    size_t elementSize = 0;

    //!  Initialize with a number of entries and known output space
    LUT(size_t entries, size_t outputSpace) :
        table(entries * outputSpace),
        numEntries(entries),
        elementSize(outputSpace)
    {
    }

    //!  Initialize with an existing LUT, which we copy
    LUT(const unsigned char* interleavedLUT,
        size_t entries,
        size_t outputSpace) :
        table(interleavedLUT, interleavedLUT + entries * outputSpace),
        numEntries(entries),
        elementSize(outputSpace)
    {
    }

    //! Initialize from nitf::LookupTable read from a NITF
    LUT(const nitf::LookupTable& lookupTable) : LUT(lookupTable.getEntries(), lookupTable.getTables())
    {
        // NITF stores the tables consecutively.
        // Need to interleave them for SIX
        if (elementSize == 3)
        {
            // Imagine the vector is a matrix and then transpose it
            for (size_t ii = 0; ii < table.size(); ++ii)
            {
                table[(ii % numEntries) * elementSize +
                    (ii / numEntries)] = lookupTable.getTable()[ii];
            }
        }

        // I'm not sure why this is a special case, but elements get
        // swapped if we try to use the above formula
        else if (elementSize == 2)
        {
            for (size_t ii = 0; ii < numEntries; ++ii)
            {
                table[2 * ii] = lookupTable.getTable()[numEntries + ii];
                table[2 * ii + 1] = lookupTable.getTable()[ii];
            }
        }
    }

    virtual ~LUT() = default;

    bool operator==(const LUT& rhs) const
    {
        return (table == rhs.table &&
                numEntries == rhs.numEntries &&
                elementSize == rhs.elementSize);
    }

    //!  Gives back a pointer at table[i * elementSize]
    unsigned char* operator[](size_t i) noexcept
    {
        return &(table[i * elementSize]);
    }

    //!  Gives back a pointer at table[i * elementSize]
    const unsigned char* operator[](size_t i) const noexcept
    {
        return &(table[i * elementSize]);
    }

    unsigned char* getTable() noexcept
    {
        return table.empty() ? nullptr : table.data();
    }

    const unsigned char* getTable() const noexcept
    {

        return table.empty() ? nullptr : table.data();
    }

    virtual void clone(std::unique_ptr<LUT>& result) const
    {
        result = std::make_unique<LUT>(getTable(), numEntries, elementSize);
    }
    virtual LUT* clone() const
    {
        std::unique_ptr<LUT> result;
        clone(result);
        return result.release();
    }
};

/*!
 *  \struct AmplitudeTable
 *  \brief SICD 'AmpTable' parameter
 *
 *  This is a fixed size (256-element) LUT.  For AMP8I_PHS8I data,
 *  the amplitude and phase parts are stored as unsigned 8-bit integers.
 *  If an amplitude table is given, the amplitude component should be
 *  interpreted as an index into the AmpTable, ultimately yielding the
 *  double precision amplitude value
 */

 // Store the computed `six::zfloat` for every possible 
 // amp/phs pair, a total of 256*256 values.
static constexpr size_t AmplitudeTableSize = 256; // "This is a fixed size (256-element) LUT"
using Amp8iPhs8iLookup_t = std::mdspan<const six::zfloat, std::dextents<size_t, 2>>;
 
 // More descriptive than std::pair<uint8_t, uint8_t>
struct SIX_SIX_API AMP8I_PHS8I_t final
{
    uint8_t amplitude;
    uint8_t phase;
};

// Control a few details of the ComplexToAMP8IPHS8I implementation, especially "unseq" (i.e., SIMD).
#ifndef SIX_sicd_has_VCL
    // Do we have the "vectorclass" library? https://github.com/vectorclass/version2
    #if !CODA_OSS_cpp17 // VCL needs C++17
        #define SIX_sicd_has_VCL 0
    #else
        // __has_include is part of C++17
        #if __has_include("../../../six.sicd/include/six/sicd/vectorclass/version2/vectorclass.h") || \
            __has_include("six/sicd/vectorclass/version2/vectorclass.h")
            #if _MSC_VER
            // Compiler error: bug in MSVC or VCL?
            #define SIX_sicd_has_VCL !CODA_OSS_cpp20 // TODO: enable for C++20
            #else
            #define SIX_sicd_has_VCL 1
            #endif
        #else
            #define SIX_sicd_has_VCL 0
        #endif // __has_include
    #endif // C++17
#endif

#ifndef SIX_sicd_has_simd
    // Do we have `std::experimental::simd? https://en.cppreference.com/w/cpp/experimental/simd
    #if (__GNUC__ >= 11) && CODA_OSS_cpp20
        // https://github.com/VcDevel/std-simd "... shipping with GCC since version 11."
        #define SIX_sicd_has_simd 1
    #else
        #define SIX_sicd_has_simd 0
    #endif // __GNUC__
#endif

#ifndef SIX_sicd_has_ximd
    // This is a "hacked up" version of std::experimental::simd using std::array.
    // It's primarily for development and testing: VCL needs C++17 and
    // std::experimental::simd is G++11/C++20.
    #define SIX_sicd_has_ximd CODA_OSS_DEBUG
    //#define SIX_sicd_has_ximd 0
#endif

#ifndef SIX_sicd_has_sisd
    // This is just normal `int`s and `float`s (not even `std::array`s) made
    // to look like SIMD types.  Why? Generic code: the same templatized
    // code works everywhere.
    #define SIX_sicd_has_sisd 1
#endif

#ifndef SIX_sicd_ComplexToAMP8IPHS8I_unseq
    #if SIX_sicd_has_VCL || SIX_sicd_has_simd || SIX_sicd_has_ximd
    #define SIX_sicd_ComplexToAMP8IPHS8I_unseq 1
    #else
    #define SIX_sicd_ComplexToAMP8IPHS8I_unseq 0
    #endif // SIX_sicd_have_VCL || SIX_sicd_has_simd
#endif // SIX_sicd_ComplexToAMP8IPHS8I_unseq

// Don't know yet whether SISD code actually make sense ... ease
// development/testing and the eventual transition.
#if !SIX_sicd_ComplexToAMP8IPHS8I_unseq
    #if SIX_sicd_has_sisd && CODA_OSS_DEBUG
        #undef SIX_sicd_ComplexToAMP8IPHS8I_unseq
        #define SIX_sicd_ComplexToAMP8IPHS8I_unseq 1
    #endif
#endif

// We're still at C++14, so we don't have the types in <execution>
// https://en.cppreference.com/w/cpp/algorithm/execution_policy_tag
// For now, our use is very limited; so don't try to
// mimic C++17 (these should be types, not `enum` values).
enum class execution_policy
{
    seq, par, par_unseq, unseq
};

struct AmplitudeTable; // forward
namespace sicd
{
namespace details
{

/*!
 * \brief A utility that's used to convert complex values into 8-bit amplitude and phase values.
 * 
 * *** Implemetned in SIX.SICD ***
 */
class ComplexToAMP8IPHS8I final
{
    /*!
     * Create a lookup structure that converts from complex to amplitude and phase.
     * @param pAmplitudeTable optional amplitude table.
     */
    explicit ComplexToAMP8IPHS8I(const six::AmplitudeTable* pAmplitudeTable = nullptr);

public:
    static const ComplexToAMP8IPHS8I& make_(const six::AmplitudeTable* pAmplitudeTable); // AmplitudeTable* = NULL is cached

    ~ComplexToAMP8IPHS8I() = default;
    ComplexToAMP8IPHS8I(const ComplexToAMP8IPHS8I&) = delete;
    ComplexToAMP8IPHS8I& operator=(const ComplexToAMP8IPHS8I&) = delete;
    ComplexToAMP8IPHS8I(ComplexToAMP8IPHS8I&&) = delete; // implicitly deleted because of =delete for copy
    ComplexToAMP8IPHS8I& operator=(ComplexToAMP8IPHS8I&&) = delete; // implicitly deleted because of =delete for copy

    std::span<const float> magnitudes() const;
    float phase_delta() const;

    //! Unit vector rays that represent each direction that phase can point.
    struct phase_directions final
    {
        std::array<zfloat, AmplitudeTableSize> value; // interleaved, std::complex<float>
        #if SIX_sicd_has_VCL || SIX_sicd_has_sisd
        std::array<float, AmplitudeTableSize> real;
        std::array<float, AmplitudeTableSize> imag;
        #endif
    };
    const phase_directions& get_phase_directions() const;

    uint8_t getPhase(six::zfloat v) const;

private:
    //! The sorted set of possible magnitudes order from small to large.
    std::array<float, AmplitudeTableSize> uncached_magnitudes_;
    std::span<const float> magnitudes_;

    //! The difference in phase angle between two UINT phase values.
    float phase_delta_;

    //! Unit vector rays that represent each direction that phase can point.
    phase_directions phase_directions_;
};
}
}

struct SIX_SIX_API AmplitudeTable final : public LUT
{
    //!  Constructor.  Creates a 256-entry table
    AmplitudeTable(size_t elementSize) noexcept(false) :
        LUT(AmplitudeTableSize /*i.e., 256*/, elementSize)
    {
    }
    AmplitudeTable() noexcept(false) :  AmplitudeTable(sizeof(double)) 
    {
    }
    AmplitudeTable(const nitf::LookupTable& lookupTable) noexcept(false) : LUT(lookupTable)
    {
        if (size() != AmplitudeTableSize)
        {
            throw std::invalid_argument("lookupTable should have 256 elements.");
        }
    }

    AmplitudeTable(const AmplitudeTable&) = delete; // use clone()
    AmplitudeTable& operator=(const AmplitudeTable&) = delete; // use clone()
    AmplitudeTable(AmplitudeTable&&) = default;
    AmplitudeTable& operator=(AmplitudeTable&&) = default;

    size_t size() const noexcept
    {
        return numEntries;
    }
    static constexpr auto ssize() noexcept // signed size()
    {
        return gsl::narrow<ptrdiff_t>(AmplitudeTableSize);
    }

    bool operator==(const AmplitudeTable& rhs) const
    {
        const LUT* pThis = this;
        const LUT* pRHS = &rhs;
        return *(pThis) == *(pRHS);
    }
    bool operator!=(const AmplitudeTable& rhs) const
    {
        return !(*this == rhs);
    }

    const double& index(size_t ii) const noexcept
    {
        const void* const this_ii = (*this)[ii];
        return *static_cast<const double*>(this_ii);
    }
    double& index(size_t ii) noexcept
    {
        void* const this_ii = (*this)[ii];
        return *static_cast<double*>(this_ii);
    }

    void clone(std::unique_ptr<AmplitudeTable>& ret) const
    {
        ret = std::make_unique<AmplitudeTable>();
        for (size_t ii = 0; ii < numEntries; ++ii)
        {
            void* const ret_ii = (*ret)[ii];
            *static_cast<double*>(ret_ii) = index(ii);
        }

        // Don't copy the lookup table; it will be re-created if needed.
    }
    void clone(std::unique_ptr<LUT>& ret) const override
    {
        std::unique_ptr<AmplitudeTable> result;
        clone(result);
        ret.reset(result.release());
    }
    AmplitudeTable* clone() const override
    {
        std::unique_ptr<AmplitudeTable> ret;
        clone(ret);
        return ret.release();
    }

    // This is a "cache" mostly because this is a convenient place to store the data; it
    // doesn't take that long to generate the lookup table.  Note that existing code wants
    // to work with a `const AmplitudeTable &`, thus `mutable` ... <shrug>.
    void cacheLookup_(std::vector<six::zfloat>&& lookup_) const
    {
        lookup = std::move(lookup_);
    }
    const std::vector<six::zfloat>* getLookup() const
    {
        return lookup.empty() ? nullptr : &lookup;
    }

    // Again, this is a convenient place to store the data as it depends on an AmplitudeTable instance.
    void cacheFromComplex_(std::unique_ptr<sicd::details::ComplexToAMP8IPHS8I>&& fromComplex) const
    {
        pFromComplex = std::move(fromComplex);
    }
    const sicd::details::ComplexToAMP8IPHS8I* getFromComplex() const
    {
        return pFromComplex.get();
    }

private:
    mutable std::vector<zfloat> lookup;
    mutable std::unique_ptr<sicd::details::ComplexToAMP8IPHS8I> pFromComplex;    
};

}

#endif // SIX_six_AmplitudeTable_h_INCLUDED_

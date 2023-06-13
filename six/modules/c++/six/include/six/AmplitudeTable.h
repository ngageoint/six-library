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
#include <complex>
#include <memory>
#include <array>

#include <types/complex.h>
#include <import/except.h>

#include <nitf/LookupTable.hpp>
#include <scene/sys_Conf.h>

#include "six/Enums.h"
#include "six/Types.h"

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
struct LUT
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
 //! Fixed size 256 element array of complex values.
using phase_values_t = std::array<six::zfloat, UINT8_MAX + 1>;
//! Fixed size 256 x 256 matrix of complex values.
using Amp8iPhs8iLookup_t = std::array<phase_values_t, UINT8_MAX + 1>;

// More descriptive than std::pair<uint8_t, uint8_t>
struct AMP8I_PHS8I_t final
{
    uint8_t amplitude;
    uint8_t phase;
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
    static const ComplexToAMP8IPHS8I& make(const six::AmplitudeTable* pAmplitudeTable); // AmplitudeTable* = NULL is cached

    ~ComplexToAMP8IPHS8I() = default;
    ComplexToAMP8IPHS8I(const ComplexToAMP8IPHS8I&) = delete;
    ComplexToAMP8IPHS8I& operator=(const ComplexToAMP8IPHS8I&) = delete;
    ComplexToAMP8IPHS8I(ComplexToAMP8IPHS8I&&) = default;
    ComplexToAMP8IPHS8I& operator=(ComplexToAMP8IPHS8I&&) = default;

    /*!
     * Get the nearest amplitude and phase value given a complex value
     * @param v complex value to query with
     * @return nearest amplitude and phase value
     */
    AMP8I_PHS8I_t nearest_neighbor(const six::zfloat& v) const;

private:
    //! The sorted set of possible magnitudes order from small to large.
    std::vector<long double> uncached_magnitudes; // Order is important! This must be ...
    const std::vector<long double>& magnitudes; // ... before this.

    //! The difference in phase angle between two UINT phase values.
    long double phase_delta;
    //! Unit vector rays that represent each direction that phase can point.
    std::array<std::complex<long double>, UINT8_MAX + 1> phase_directions;
};
}
}

struct AmplitudeTable final : public LUT
{
    //!  Constructor.  Creates a 256-entry table
    AmplitudeTable(size_t elementSize) noexcept(false) :
        LUT(UINT8_MAX + 1 /*i.e., 256*/, elementSize)
    {
    }
    AmplitudeTable() noexcept(false) :  AmplitudeTable(sizeof(double)) 
    {
    }
    AmplitudeTable(const nitf::LookupTable& lookupTable) noexcept(false) : LUT(lookupTable)
    {
        if (size() != 256)
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
    void cacheLookup_(std::unique_ptr<Amp8iPhs8iLookup_t>&& lookup) const
    {
        pLookup = std::move(lookup);
    }
    const Amp8iPhs8iLookup_t* getLookup() const
    {
        return pLookup.get();
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
    mutable std::unique_ptr<Amp8iPhs8iLookup_t> pLookup; // to big for the stack
    mutable std::unique_ptr<sicd::details::ComplexToAMP8IPHS8I> pFromComplex;    
};

}

#endif // SIX_six_AmplitudeTable_h_INCLUDED_

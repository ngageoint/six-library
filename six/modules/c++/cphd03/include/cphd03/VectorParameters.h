/* =========================================================================
 * This file is part of cphd03-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * cphd03-c++ is free software; you can redistribute it and/or modify
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


#ifndef __CPHD03_VECTOR_PARAMETERS_H__
#define __CPHD03_VECTOR_PARAMETERS_H__

#include <ostream>

#include <nitf/coda-oss.hpp>
#include <mem/ScopedCopyablePtr.h>
#include <six/Init.h>

namespace cphd03
{
struct FxParameters
{
    FxParameters();

    bool operator==(const FxParameters& other) const
    {
        return Fx0 == other.Fx0 &&
               FxSS == other.FxSS &&
               Fx1 == other.Fx1 &&
               Fx2 == other.Fx2;
    }

    bool operator!=(const FxParameters& other) const
    {
        return !((*this) == other);
    }

    size_t Fx0;
    size_t FxSS;
    size_t Fx1;
    size_t Fx2;
};

std::ostream& operator<< (std::ostream& os, const FxParameters& d);

struct TOAParameters
{
    TOAParameters();

    bool operator==(const TOAParameters& other) const
    {
        return deltaTOA0 == other.deltaTOA0 &&
               toaSS == other.toaSS;
    }

    bool operator!=(const TOAParameters& other) const
    {
        return !((*this) == other);
    }

    size_t deltaTOA0;
    size_t toaSS;
};

std::ostream& operator<< (std::ostream& os, const TOAParameters& d);

struct VectorParameters
{
    VectorParameters();

    // These functions return the offset, in bytes, to the parameter requested
    // -1 is returned if the parameter is not in the vector
    sys::Off_T txTimeOffset() const;
    sys::Off_T txPosOffset() const;
    sys::Off_T rcvTimeOffset() const;
    sys::Off_T rcvPosOffset() const;
    sys::Off_T srpTimeOffset() const;
    sys::Off_T srpPosOffset() const;
    sys::Off_T tropoSRPOffset() const;
    sys::Off_T ampSFOffset() const;
    sys::Off_T Fx0Offset() const;
    sys::Off_T FxSSOffset() const;
    sys::Off_T Fx1Offset() const;
    sys::Off_T Fx2Offset() const;
    sys::Off_T deltaTOA0Offset() const;
    sys::Off_T toaSSOffset() const;

    void clearAmpSF()
    {
        ampSF = six::Init::undefined<sys::Off_T>();
    }

    bool operator==(const VectorParameters& other) const;

    bool operator!=(const VectorParameters& other) const
    {
        return !((*this) == other);
    }

    // Size, in bytes, of each of the fields in the binary VBM
    sys::Off_T txTime;
    sys::Off_T txPos;
    sys::Off_T rcvTime;
    sys::Off_T rcvPos;
    sys::Off_T srpTime;        // Optional
    sys::Off_T srpPos;
    sys::Off_T tropoSRP;       // Optional   TropoSRP appears before AmpSF in the binary data
    sys::Off_T ampSF;          // Optional


    mem::ScopedCopyablePtr<FxParameters> fxParameters;
    mem::ScopedCopyablePtr<TOAParameters> toaParameters;
};

std::ostream& operator<< (std::ostream& os, const VectorParameters& d);
}

#endif

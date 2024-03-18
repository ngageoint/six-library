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

#pragma once
#ifndef __CPHD03_VECTOR_PARAMETERS_H__
#define __CPHD03_VECTOR_PARAMETERS_H__

#include <ostream>

#include <scene/sys_Conf.h>
#include <mem/ScopedCopyablePtr.h>
#include <six/Init.h>

#include "cphd03/Exports.h"

namespace cphd03
{
struct SIX_CPHD03_API FxParameters
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

struct SIX_CPHD03_API TOAParameters
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

struct SIX_CPHD03_API VectorParameters final
{
    VectorParameters();

    // These functions return the offset, in bytes, to the parameter requested
    // -1 is returned if the parameter is not in the vector
    int64_t txTimeOffset() const;
    int64_t txPosOffset() const;
    int64_t rcvTimeOffset() const;
    int64_t rcvPosOffset() const;
    int64_t srpTimeOffset() const;
    int64_t srpPosOffset() const;
    int64_t tropoSRPOffset() const;
    int64_t ampSFOffset() const;
    int64_t Fx0Offset() const;
    int64_t FxSSOffset() const;
    int64_t Fx1Offset() const;
    int64_t Fx2Offset() const;
    int64_t deltaTOA0Offset() const;
    int64_t toaSSOffset() const;

    void clearAmpSF()
    {
        ampSF = six::Init::undefined<int64_t>();
    }

    bool operator==(const VectorParameters& other) const;

    bool operator!=(const VectorParameters& other) const
    {
        return !((*this) == other);
    }

    // Size, in bytes, of each of the fields in the binary VBM
    int64_t txTime;
    int64_t txPos;
    int64_t rcvTime;
    int64_t rcvPos;
    int64_t srpTime;        // Optional
    int64_t srpPos;
    int64_t tropoSRP;       // Optional   TropoSRP appears before AmpSF in the binary data
    int64_t ampSF;          // Optional


    mem::ScopedCopyablePtr<FxParameters> fxParameters;
    mem::ScopedCopyablePtr<TOAParameters> toaParameters;
};

SIX_CPHD03_API std::ostream& operator<< (std::ostream& os, const VectorParameters& d);
}

#endif

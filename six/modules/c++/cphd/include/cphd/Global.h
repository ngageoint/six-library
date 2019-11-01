/* =========================================================================
 * This file is part of cphd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2019, MDA Information Systems LLC
 *
 * cphd-c++ is free software; you can redistribute it and/or modify
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

#ifndef __CPHD_GLOBAL_H__
#define __CPHD_GLOBAL_H__

#include <ostream>

#include <mem/ScopedCopyablePtr.h>
#include <six/sicd/RadarCollection.h>

#include <cphd/Enums.h>
#include <cphd/Types.h>

namespace cphd
{
/*
 *  \struct Timeline
 *
 *  \brief Parameters that describe the collection times for
 *   the data contained in the product
 */
struct Timeline
{
    //! Constructor
    Timeline();

    //! Equality operators
    bool operator==(const Timeline& other) const
    {
        return collectionStart == other.collectionStart &&
            rcvCollectionStart == other.rcvCollectionStart &&
            txTime1 == other.txTime1 &&
            txTime2 == other.txTime2;
    }
    bool operator!=(const Timeline& other) const
    {
        return !((*this) == other);
    }

    //! Collection Start date and time (UTC)
    DateTime collectionStart;

    //! (Optional) Receive only platform collection date
    //! and start time.
    DateTime rcvCollectionStart; // Optional

    //! Earliest TxTime value for any signal vector in the
    //! product
    double txTime1;

    //! Latest TxTime value for any signal vector in the
    //! product
    double txTime2;
};

/*
 *  \struct FxBand
 *
 *  \brief Parameters that describe the FX frequency limits
 *   for the signal array(s) contained in the product
 */
struct FxBand
{
    //! Constructor
    FxBand();

    //! Equality operators
    bool operator==(const FxBand& other) const
    {
        return fxMin == other.fxMin &&
            fxMax == other.fxMax;
    }
    bool operator!=(const FxBand& other) const
    {
        return !((*this) == other);
    }

    //! Minimum fx_1 value for any signal vector in the
    //! product.
    double fxMin;

    //! Maximum fx_2 value for any signal vector in the
    //! product
    double fxMax;
};

/*
 *  \struct TOASwath
 *
 *  \brief Parameters that describe the TOA swath limits
 *   for the signal array(s) contained in the product
 */
struct TOASwath
{
    //! Constructor
    TOASwath();

    //! Equality operators
    bool operator==(const TOASwath& other) const
    {
        return toaMin == other.toaMin &&
            toaMax == other.toaMax;
    }
    bool operator!=(const TOASwath& other) const
    {
        return !((*this) == other);
    }

    //! Minimum DTOA_1 value for any signal vector in
    //! the product
    double toaMin;

    //! Maximum DTOA_2 value for any signal vector
    //! in the product
    double toaMax;
};

/*
 *  \struct TropoParameters
 *
 *  \brief (Optional) Parameters that compute the propogation
 *   delay due to the troposphere
 */
struct TropoParameters
{
    //! Constructor
    TropoParameters();

    //! Equality operators
    bool operator==(const TropoParameters& other) const
    {
        return n0 == other.n0 && refHeight == other.refHeight;
    }
    bool operator!=(const TropoParameters& other) const
    {
        return !((*this) == other);
    }

    //! Refractivity value of the troposphere for the
    //! imaged scene used to form the product
    //! (dimensionless).
    double n0;

    //! Reference Height for the N0 value.
    RefHeight refHeight;
};

/*
 *  \struct IonoParameters
 *
 *  \brief (Optional) Parameters that compute the propogation
 *   delay due to the ionosphere
 */
struct IonoParameters
{
    //! Constructor
    IonoParameters();

    //! Equality operators
    bool operator==(const IonoParameters& other) const
    {
        return tecv == other.tecv && f2Height == other.f2Height;
    }
    bool operator!=(const IonoParameters& other) const
    {
        return !((*this) == other);
    }

    //! Total Electron Content (TEC)
    double tecv;

    //! (Optional) The F2 height of the ionosphere.
    double f2Height; // Optional
};

/*
 *  \sturct Global
 *
 *  \brief Global parameters that apply to metadata components
 *   and CPHD signal arrays
 */
struct Global
{
    //! Constructor
    Global();

    //! Equality operators
    bool operator==(const Global& other) const;
    bool operator !=(const Global& other) const
    {
        return !((*this) == other);
    }

    /*
     *  \func getDomainType
     *
     *  \brief Get domain represented by the sample dimension
     */
    DomainType getDomainType() const
    {
        return domainType;
    }

    //! Indicates the domain represented by the sample
    //! dimension of the CPHD signal array(s)
    DomainType domainType;

    //! Phase SGN applied to compute target signal
    //! phase as a function of target delta TOA
    PhaseSGN sgn;

    //! Parameters that describe the collection times for
    //! the data contained in the product
    Timeline timeline;

    //! Parameters that describe the FX frequency limits
    //! for the signal array(s) contained in the product.
    FxBand fxBand;

    //! Parameters that describe the TOA swath limits
    //! for the signal array(s) contained in the product
    TOASwath toaSwath;

    //! (Optional) Parameters used to compute the propagation
    //! delay due to the troposphere.
    mem::ScopedCopyablePtr<TropoParameters> tropoParameters;

    //! (Optional) Parameters used to compute propagation effects
    //! due to the ionosphere
    mem::ScopedCopyablePtr<IonoParameters> ionoParameters;
};

//! Ostream operators
std::ostream& operator<< (std::ostream& os, const Global& d);
std::ostream& operator<< (std::ostream& os, const Timeline& d);
std::ostream& operator<< (std::ostream& os, const FxBand& d);
std::ostream& operator<< (std::ostream& os, const TOASwath& d);
std::ostream& operator<< (std::ostream& os, const TropoParameters& d);
std::ostream& operator<< (std::ostream& os, const IonoParameters& d);
}

#endif

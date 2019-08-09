/* =========================================================================
 * This file is part of six.sicd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2019, MDA Information Systems LLC
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
#ifndef __CPHD_PVP_H__
#define __CPHD_PVP_H__

#include <ostream>
#include <vector>
#include <stddef.h>

#include "six/Types.h"
#include "six/Init.h"
#include "six/Parameter.h"
#include "six/ParameterCollection.h"

namespace cphd
{
/*!
 *  \struct CollectionID
 *  \brief CollectionID parameter
 *
 *  Collection ID used by CPHD, representing the tag
 *  <CollectionID>. This block contains general information about
 *  the collection.
 */
struct PVPType
{
    PVPType();

    bool operator==(const PVPType& other) const
    {
        return size == other.size && 
        offset == other.offset &&
        format == other.format;
    }

    bool operator!=(const PVPType& other) const
    {
        return !((*this) == other);
    }

    size_t size;
    size_t offset;
    std::string format;
};

struct APVPType
{
    APVPType();

    bool operator==(const APVPType& other) const
    {
        return size == other.size && 
        offset == other.offset &&
        name == other.name &&
        format == other.format;
    }

    bool operator!=(const APVPType& other) const
    {
        return !((*this) == other);
    }

    size_t size;
    size_t offset;
    std::string name;
    std::string format;
};


struct Pvp
{
    //!  Constructor
    Pvp()
    {}

    /*!
     * Transmit time for the center of the transmitted pulse relative to the
     * transmit platform Collection Start Time (sec).
     */
    PVPType TxTime;

    /*!
     * Transmit APC position at time txc(v) in Earth Centered Fixed
     * (ECF) coordinates (meters). Components in X, Y, Z order.
     * 3 8-byte floats required.
     */
    PVPType TxPos;

    /*!
     * Transmit APC velocity at time txc(v) in ECF coordinates
     * (meters/sec).
     * 3 8-byte floats required.
     */
    PVPType TxVel;

    /*!
     * Receive time for the center of the echo from the SRP relative to the
     * transmit platform Collection Start Time (sec). Time is the Time of
     * Arrival (TOA) of the received echo from the SRP for the signal
     * transmitted at txc(v).
     */
    PVPType RcvTime;


    /*!
     * Receive APC position at time trc(v)SRP in ECF coordinates
     * (meters).
     * 3 8-byte floats required.
     */
    PVPType RcvPos;

    /*!
     * Receive APC velocity at time trc(v)SRP in ECF coordinates
     * (meters/sec).
     * 3 8-byte floats required
     */
    PVPType RcvVel;

    /*!
     * Stabilization Reference Point (SRP) position in ECF coordinates
     * (meters).
     * 3 8-byte floats required.
     */
    PVPType SRPPos;

    /*!
     * (Optional) Amplitude Scale Factor to be applied to all samples of the signal
     * vector. For signal vector v, each sample value is multiplied by
     * Amp_SF(v) to yield the proper sample values for the vector. 
     */
    PVPType AmpSF;

    /*!
     * The DOPPLER shift micro parameter. Parameter accounts for the
     * time dilation/Doppler shift for the echoes from all targets with the
     * same time dilation/Doppler shift as the SRP echo.
     */
    PVPType aFDOP;

    /*!
     * First order phase micro parameter (i.e. linear phase). Accounts for
     * linear phase vs. FX frequency for targets with time
     * dilation/Doppler shift different than the echo from the SRP.
     * Provides precise linear phase prediction for Linear FM waveforms.
     */
    PVPType aFRR1;

    /*!
     * Second order phase micro parameter (i.e. quadratic phase).
     * Accounts for quadratic phase vs. FX frequency for targets with
     * time dilation/Doppler shift different than the echo from the SRP.
     * Provides precise quadratic phase prediction for Linear FM
     * waveforms. 
     */
    PVPType aFRR2;

    /*!
     * The FX domain frequency limits of the transmitted waveform
     * retained in the signal vector (Hz).
     * Saved transmit band is from fx_1(v) < fx < fx_2(v)
     * For the vector: FX_BW(v) = fx_2(v) – fx_1(v)
     */
    PVPType FX1;

    /*!
     * The FX domain frequency limits of the transmitted waveform
     * retained in the signal vector (Hz).
     * Saved transmit band is from fx_1(v) < fx < fx_2(v)
     * For the vector: FX_BW(v) = fx_2(v) – fx_1(v)
     */
    PVPType FX2;

    /*!
     * The FX domain frequency limits for out-of-band noise signal for
     * frequencies below fx_1(v) and above fx_2(v). May ONLY be
     * included for Domain_Type = FX.
     * For any vector: fx_N1 < fx_1 & fx_2 < fx_N2
     * When included in a product, fx_N1 & fx_N2 are both included.
    */
    PVPType FXN1;

    /*!
     * The FX domain frequency limits for out-of-band noise signal for
     * frequencies below fx_1(v) and above fx_2(v). May ONLY be
     * included for Domain_Type = FX.
     * For any vector: fx_N1 < fx_1 & fx_2 < fx_N2
     * When included in a product, fx_N1 & fx_N2 are both included.
    */
    PVPType FXN2;

    /*!
     * The  change in TOA limits for the full resolution echoes retained in the
     * signal vector (sec). Full resolution echoes are formed with
     * FX_BW(v) saved bandwidth.
     * Full resolution TOA limits: TOA_1 < TOA < TOA_2
     */
    PVPType TOA1;

    /*!
     * The  change in TOA limits for the full resolution echoes retained in the
     * signal vector (sec). Full resolution echoes are formed with
     * FX_BW(v) saved bandwidth.
     * Full resolution TOA limits: TOA_1 < TOA < TOA_2
     */
    PVPType TOA2;


    PVPType TOAE1;
    PVPType TOAE2;
    PVPType TDTropoSRP;
    PVPType TDIonoSRP;
    PVPType SC0;
    PVPType SCSS;
    PVPType SIGNAL;
    std::vector<APVPType> AddedPVP;


    //!  Destructor
    virtual ~Pvp()
    {}

    bool operator==(const Pvp& other) const
    {
        if( !(TxTime == other.TxTime && TxPos == other.TxPos &&
                TxVel == other.TxVel && RcvTime == other.RcvTime &&
                RcvPos == other.RcvPos && RcvVel == other.RcvVel &&
                SRPPos == other.SRPPos && aFDOP == other.aFDOP &&
                aFRR1 == other.aFRR1 && aFRR2 == other.aFRR2 &&
                FX1 == other.FX1 && FX2 == other.FX2 &&
                TOA1 == other.TOA1 && TOA2 == other.TOA2 &&
                TDTropoSRP == other.TDTropoSRP && SC0 == other.SC0 &&
                SCSS == other.SCSS &&
                AmpSF == other.AmpSF && FXN1 == other. FXN1 &&
                FXN2 == other.FXN2 && TOAE1 == other.TOAE1 &&
                TOAE2 == other.TOAE2 && TDIonoSRP == other.TDIonoSRP &&
                SIGNAL == other.SIGNAL ))
        {
            return false;
        }

        // Checking Added per vec parameters

        if( AddedPVP.size() != other.AddedPVP.size())
        {
            return false;
        }

        for (size_t i = 0; i < AddedPVP.size(); ++i)
        {
            if(AddedPVP[i] !=  other.AddedPVP[i] )
            {
                return false;
            }
        }

        return true;
    }

    bool operator!=(const Pvp& other) const
    {
        return !((*this) == other);
    }
};

// std::ostream& operator<< (std::ostream& os, const Pvp& pvp);
}
#endif


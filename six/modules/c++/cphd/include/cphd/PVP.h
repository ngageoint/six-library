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
#include <map>
#include <six/Types.h>
#include <six/Init.h>
#include <six/Parameter.h>
#include <six/ParameterCollection.h>

namespace cphd
{
struct PVPType
{
    PVPType();

    bool operator==(const PVPType& other) const
    {
        return mSize == other.getSize() && 
        mOffset == other.getOffset() &&
        mFormat == other.getFormat();
    }

    bool operator!=(const PVPType& other) const
    {
        return !((*this) == other);
    }

    void setData(size_t size_in, size_t offset_in, std::string format_in)
    {
        mSize = size_in;
        mOffset = offset_in;
        mFormat = format_in;
    }

    size_t getSize() const
    {
        return mSize;
    }

    size_t getOffset() const
    {
        return mOffset;
    }

    std::string getFormat() const
    {
        return mFormat;
    }

protected:
    size_t mSize;
    size_t mOffset;
    std::string mFormat;
};

struct APVPType : PVPType
{
    APVPType();

    bool operator==(const APVPType& other) const
    {
        if ( (PVPType)(*this) != (PVPType)other )
        {
            return false;
        }
        return mName == other.mName;
    }

    bool operator!=(const APVPType& other) const
    {
        return !((*this) == other);
    }

    void setName(std::string name)
    {
        mName = name;
    }

    std::string getName() const
    {
        return mName;
    }

private:
    std::string mName;
};

/*
 * 
 */
struct Pvp
{

    /*!
     * Transmit time for the center of the transmitted pulse relative to the
     * transmit platform Collection Start Time (sec).
     */
    PVPType txTime;

    /*!
     * Transmit APC position at time txc(v) in Earth Centered Fixed
     * (ECF) coordinates (meters). Components in X, Y, Z order.
     * 3 8-byte floats required.
     */
    PVPType txPos;

    /*!
     * Transmit APC velocity at time txc(v) in ECF coordinates
     * (meters/sec).
     * 3 8-byte floats required.
     */
    PVPType txVel;

    /*!
     * Receive time for the center of the echo from the srp relative to the
     * transmit platform Collection Start Time (sec). Time is the Time of
     * Arrival (toa) of the received echo from the srp for the signal
     * transmitted at txc(v).
     */
    PVPType rcvTime;


    /*!
     * Receive APC position at time trc(v)srp in ECF coordinates
     * (meters).
     * 3 8-byte floats required.
     */
    PVPType rcvPos;

    /*!
     * Receive APC velocity at time trc(v)srp in ECF coordinates
     * (meters/sec).
     * 3 8-byte floats required
     */
    PVPType rcvVel;

    /*!
     * Stabilization Reference Point (srp) position in ECF coordinates
     * (meters).
     * 3 8-byte floats required.
     */
    PVPType srpPos;

    /*!
     * (Optional) Amplitude scale Factor to be applied to all samples of the signal
     * vector. For signal vector v, each sample value is multiplied by
     * Amp_SF(v) to yield the proper sample values for the vector. 
     */
    mem::ScopedCopyablePtr<PVPType> ampSF;

    /*!
     * The DOPPLER shift micro parameter. Parameter accounts for the
     * time dilation/Doppler shift for the echoes from all targets with the
     * same time dilation/Doppler shift as the srp echo.
     */
    PVPType aFDOP;

    /*!
     * First order phase micro parameter (i.e. linear phase). Accounts for
     * linear phase vs. FX frequency for targets with time
     * dilation/Doppler shift different than the echo from the srp.
     * Provides precise linear phase prediction for Linear FM waveforms.
     */
    PVPType aFRR1;

    /*!
     * Second order phase micro parameter (i.e. quadratic phase).
     * Accounts for quadratic phase vs. FX frequency for targets with
     * time dilation/Doppler shift different than the echo from the srp.
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
    PVPType fx1;

    /*!
     * The FX domain frequency limits of the transmitted waveform
     * retained in the signal vector (Hz).
     * Saved transmit band is from fx_1(v) < fx < fx_2(v)
     * For the vector: FX_BW(v) = fx_2(v) – fx_1(v)
     */
    PVPType fx2;

    /*!
     * (Optional) The FX domain frequency limits for out-of-band noise signal for
     * frequencies below fx_1(v) and above fx_2(v). May ONLY be
     * included for Domain_Type = FX.
     * For any vector: fx_N1 < fx_1 & fx_2 < fx_N2
     * When included in a product, fx_N1 & fx_N2 are both included.
    */
    mem::ScopedCopyablePtr<PVPType> fxN1;

    /*!
     * (Optional) The FX domain frequency limits for out-of-band noise signal for
     * frequencies below fx_1(v) and above fx_2(v). May ONLY be
     * included for Domain_Type = FX.
     * For any vector: fx_N1 < fx_1 & fx_2 < fx_N2
     * When included in a product, fx_N1 & fx_N2 are both included.
    */
    mem::ScopedCopyablePtr<PVPType> fxN2;

    /*!
     * The  change in toa limits for the full resolution echoes retained in the
     * signal vector (sec). Full resolution echoes are formed with
     * FX_BW(v) saved bandwidth.
     * Full resolution toa limits: toa_1 < toa < toa_2
     */
    PVPType toa1;

    /*!
     * The  change in toa limits for the full resolution echoes retained in the
     * signal vector (sec). Full resolution echoes are formed with
     * FX_BW(v) saved bandwidth.
     * Full resolution toa limits: toa_1 < toa < toa_2
     */
    PVPType toa2;

    /*!
     * (Optional) The TOA limits for all echoes retained in the signal vector (sec).
     */
    mem::ScopedCopyablePtr<PVPType> toaE1;

    /*!
     * (Optional) The TOA limits for all echoes retained in the signal vector (sec).
     */
    mem::ScopedCopyablePtr<PVPType> toaE2;

    /*!
     * Two-way time delay due to the troposphere (sec) that was added
     * when computing the propagation time for the SRP
     */
    PVPType tdTropoSRP;

    /*
     * (Optional) Two-way time delay due to the ionosphere (sec) that was added
     * when computing the propagation time for the SRP 
     */
    mem::ScopedCopyablePtr<PVPType> tdIonoSRP;

    /*
     * FX DOMAIN & TOA DOMAIN: The domain signal vector coordinate value for
     * sample s = 0 (Hz).
     */
    PVPType sc0;

    /*
     * FX DOMAIN & TOA DOMAIN: The domain signal vector coordinate value for
     * signal coordinate sample spacing (Hz).
     */
    PVPType scss;

    /*
     * (Optional) Integer parameter that may be included to indicate the signal
     * content for some vectors is known or is likely to be distorted.
     */
    mem::ScopedCopyablePtr<PVPType> signal;

    /*
     * (Optional) User defined PV parameters
     */
    std::vector<APVPType> addedPVP;

    //! Default Constructor
    Pvp();

    //! Constructor initializes addedPVP size
    Pvp(size_t numAdditionalParams);


    bool operator==(const Pvp& other) const
    {
        return txTime == other.txTime && txPos == other.txPos &&
                txVel == other.txVel && rcvTime == other.rcvTime &&
                rcvPos == other.rcvPos && rcvVel == other.rcvVel &&
                srpPos == other.srpPos && aFDOP == other.aFDOP &&
                aFRR1 == other.aFRR1 && aFRR2 == other.aFRR2 &&
                fx1 == other.fx1 && fx2 == other.fx2 &&
                toa1 == other.toa1 && toa2 == other.toa2 &&
                tdTropoSRP == other.tdTropoSRP && sc0 == other.sc0 &&
                scss == other.scss &&
                ampSF == other.ampSF && fxN1 == other. fxN1 &&
                fxN2 == other.fxN2 && toaE1 == other.toaE1 &&
                toaE2 == other.toaE2 && tdIonoSRP == other.tdIonoSRP &&
                signal == other.signal && addedPVP == other.addedPVP;
    }

    bool operator!=(const Pvp& other) const
    {
        return !((*this) == other);
    }

    size_t getReqSetSize() const;

    void validate(size_t size, size_t offset);

    void setData(PVPType& param, size_t size, size_t offset, std::string format);

    void setData(size_t size, size_t offset, std::string format, std::string name, size_t idx);

    void setNumAddedParameters(size_t size);

private:
    std::vector<six::BooleanType> mParamLocations;

};
std::ostream& operator<< (std::ostream& os, const PVPType& p);
std::ostream& operator<< (std::ostream& os, const APVPType& a);
std::ostream& operator<< (std::ostream& os, const Pvp& p);
}
#endif


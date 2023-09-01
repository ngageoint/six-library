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

#pragma once
#ifndef SIX_cphd_PVP_h_INCLUDED_
#define SIX_cphd_PVP_h_INCLUDED_

#include <stddef.h>

#include <ostream>
#include <vector>
#include <map>

#include <six/Types.h>
#include <six/Init.h>
#include <six/Parameter.h>
#include <six/ParameterCollection.h>
#include <six/XsElement.h>

#include "cphd/Types.h"

namespace cphd
{

/*!
 *  \struct PVPType
 *
 *  \brief Specifies a defined Per Vector Parameter.
 *
 *  Size and Offset specify the size and placement of
 *  the binary parameter in the set of Per Vector
 *  parameters provided for each vector.
 */
struct PVPType
{
    static constexpr size_t WORD_BYTE_SIZE = 8;

    /*!
     *  \func PVPType
     *
     *  \brief Default constructor
     */
    PVPType();

    //! Equality operators
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

    //! Setter Functions
    void setOffset(size_t offset)
    {
        mOffset = offset;
    }
    void setSize(size_t size)
    {
        mSize = size;
    }
    void setFormat(const std::string& format)
    {
        mFormat = format;
    }

    //! Get size
    size_t getSize() const
    {
        return mSize;
    }
    //! Get size in bytes
    size_t getByteSize() const
    {
        return mSize * WORD_BYTE_SIZE;
    }
    //! Get offset
    size_t getOffset() const
    {
        return mOffset;
    }

    //! Get offset in bytes
    size_t getByteOffset() const
    {
        return mOffset * WORD_BYTE_SIZE;
    }

    //! Get format
    std::string getFormat() const
    {
        return mFormat;
    }

protected:
    //! Size of parameter
    XsPositiveInteger_fixed2 mSize; // <xs:element name="Size"
    //! Offset of parameter
    XsNonNegativeInteger mOffset; // xs:element name="Offset"
    //! Format of parameter
    std::string mFormat;
};
void setOffset(PVPType&, size_t offset);


/*!
 *  \struct APVPType
 *
 *  \brief Additional per vector parameter
 *
 *  Specifies additional (or custom) per vector parameters
 */
struct APVPType : PVPType
{
    //! Constructor
    APVPType();

    //! Equality operators
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

    /*!
     *  \func setData
     *
     *  \brief set PVPType data
     *
     *  \param size Size of the expected parameter
     *  \param offset Offset of the expected parameter
     *  \param format Format of the expected parameter
     *  \param name Unique name of the expected parameter
     *   See spec table 10.2 page 120 for allowed binary formats
     */
    void setData(size_t size, size_t offset, const std::string& format, const std::string& name)
    {
        mSize = size;
        mOffset = offset;
        mFormat = format;
        mName = name;
    }

    /*
     *  \func getName
     *
     *  \brief Get the name of the additional parameter
     *
     *  \return Returns name of the parameter
     */
    std::string getName() const
    {
        return mName;
    }

private:
    //! String contains the unique name of the parameter
    std::string mName;
};

/*!
 *  \struct PerVectorParameterXYZ
 */
struct PerVectorParameterXYZ final
{
    PerVectorParameterXYZ();
    ~PerVectorParameterXYZ() = default;
    PerVectorParameterXYZ(const PerVectorParameterXYZ&) = default;
    PerVectorParameterXYZ& operator=(const PerVectorParameterXYZ&) = default;
    PerVectorParameterXYZ(PerVectorParameterXYZ&&) = default;
    PerVectorParameterXYZ& operator=(PerVectorParameterXYZ&&) = default;

    bool operator==(const PerVectorParameterXYZ& other) const
    {
        return (param == other.param);
    }
    bool operator!=(const PerVectorParameterXYZ& other) const
    {
        return !((*this) == other);
    }

    PVPType param;
};

/*!
 *  \struct PerVectorParameterEB
 *
 *  \brief Per vector parameters for Electrical Boresight
 */
struct PerVectorParameterEB final
{
    PerVectorParameterEB();
    ~PerVectorParameterEB() = default;
    PerVectorParameterEB(const PerVectorParameterEB&) = default;
    PerVectorParameterEB& operator=(const PerVectorParameterEB&) = default;
    PerVectorParameterEB(PerVectorParameterEB&&) = default;
    PerVectorParameterEB& operator=(PerVectorParameterEB&&) = default;

    bool operator==(const PerVectorParameterEB& other) const
    {
        return (param == other.param);
    }
    bool operator!=(const PerVectorParameterEB& other) const
    {
        return !((*this) == other);
    }

    PVPType param;
};

/*!
 *  \struct TxAntenna
 *  \brief Specify the Transmit Antenna ACF orientation and the EB Steering vector
 *
 * Parameters included that specify the Transmit Antenna ACF orientation and the EB Steering vector.
 * (New in CPHD 1.1.0)
 */
struct TxAntenna final
{
    bool operator==(const TxAntenna& other) const
    {
        return (txACX == other.txACX)
            && (txACY == other.txACY)
            && (txEB == other.txEB);
    }
    bool operator!=(const TxAntenna& other) const
    {
        return !((*this) == other);
    }

    //! TxACX PVP Structure
    six::XsElement<PerVectorParameterXYZ> txACX{ "TxACX" };

    //! TxACY PVP Structure
    six::XsElement<PerVectorParameterXYZ> txACY{ "TxACY" };

    //! TxEB PVP Structure
    six::XsElement<PerVectorParameterEB> txEB{ "TxEB" };
};

/*!
 *  \struct RcvAntenna
 *  \brief Specify the Receive Antenna orientation and the Steering vector.
 *
 * Parameters included that specify the Receive Antenna ACF orientation and the EB Steering vector.
 * (New in CPHD 1.1.0)
 */
struct RcvAntenna final
{
    bool operator==(const RcvAntenna& other) const
    {
        return (rcvACX == other.rcvACX)
            && (rcvACY == other.rcvACY)
            && (rcvEB == other.rcvEB);
    }
    bool operator!=(const RcvAntenna& other) const
    {
        return !((*this) == other);
    }

    //! RcvACX PVP Structure
    six::XsElement<PerVectorParameterXYZ> rcvACX{ "RcvACX" };

    //! RcvACY PVP Structure
    six::XsElement<PerVectorParameterXYZ> rcvACY{ "RcvACY" };

    //! RcvEB PVP Structure
    six::XsElement<PerVectorParameterEB> rcvEB{ "RcvEB" };
};

/*!
 *  \struct Pvp
 *
 *  \brief Structure used to specify the Per Vector
 *  parameters
 *
 *  Provided for each channel of a given product.
 */
struct Pvp final
{
    /*!
     *  Transmit time for the center of the transmitted pulse relative to the
     *  transmit platform Collection Start Time (sec).
     */
    PVPType txTime;

    /*!
     *  Transmit APC position at time txc(v) in Earth Centered Fixed
     *  (ECF) coordinates (meters). Components in X, Y, Z order.
     *  3 8-byte floats required.
     */
    PVPType txPos;

    /*!
     *  Transmit APC velocity at time txc(v) in ECF coordinates
     *  (meters/sec).
     *  3 8-byte floats required.
     */
    PVPType txVel;

    /*!
     *  Receive time for the center of the echo from the srp relative to the
     *  transmit platform Collection Start Time (sec). Time is the Time of
     *  Arrival (toa) of the received echo from the srp for the signal
     *  transmitted at txc(v).
     */
    PVPType rcvTime;


    /*!
     *  Receive APC position at time trc(v)srp in ECF coordinates
     *  (meters).
     *  3 8-byte floats required.
     */
    PVPType rcvPos;

    /*!
     *  Receive APC velocity at time trc(v)srp in ECF coordinates
     *  (meters/sec).
     *  3 8-byte floats required
     */
    PVPType rcvVel;

    /*!
     *  Stabilization Reference Point (srp) position in ECF coordinates
     *  (meters).
     *  3 8-byte floats required.
     */
    PVPType srpPos;

    /*!
     *  (Optional) Amplitude scale Factor to be applied to all samples of the signal
     *  vector. For signal vector v, each sample value is multiplied by
     *  Amp_SF(v) to yield the proper sample values for the vector.
     */
    PVPType ampSF;

    /*!
     *  The DOPPLER shift micro parameter. Parameter accounts for the
     *  time dilation/Doppler shift for the echoes from all targets with the
     *  same time dilation/Doppler shift as the srp echo.
     */
    PVPType aFDOP;

    /*!
     *  First order phase micro parameter (i.e. linear phase). Accounts for
     *  linear phase vs. FX frequency for targets with time
     *  dilation/Doppler shift different than the echo from the srp.
     *  Provides precise linear phase prediction for Linear FM waveforms.
     */
    PVPType aFRR1;

    /*!
     *  Second order phase micro parameter (i.e. quadratic phase).
     *  Accounts for quadratic phase vs. FX frequency for targets with
     *  time dilation/Doppler shift different than the echo from the srp.
     *  Provides precise quadratic phase prediction for Linear FM
     *  waveforms.
     */
    PVPType aFRR2;

    /*!
     *  The FX domain frequency limits of the transmitted waveform
     *  retained in the signal vector (Hz).
     *  Saved transmit band is from fx_1(v) < fx < fx_2(v)
     *  For the vector: FX_BW(v) = fx_2(v) – fx_1(v)
     */
    PVPType fx1;

    /*!
     *  The FX domain frequency limits of the transmitted waveform
     *  retained in the signal vector (Hz).
     *  Saved transmit band is from fx_1(v) < fx < fx_2(v)
     *  For the vector: FX_BW(v) = fx_2(v) – fx_1(v)
     */
    PVPType fx2;

    /*!
     *  (Optional) The FX domain frequency limits for out-of-band noise signal for
     *  frequencies below fx_1(v) and above fx_2(v). May ONLY be
     *  included for Domain_Type = FX.
     *  For any vector: fx_N1 < fx_1 & fx_2 < fx_N2
     *  When included in a product, fx_N1 & fx_N2 are both included.
    */
    PVPType fxN1;

    /*!
     *  (Optional) The FX domain frequency limits for out-of-band noise signal for
     *  frequencies below fx_1(v) and above fx_2(v). May ONLY be
     *  included for Domain_Type = FX.
     *  For any vector: fx_N1 < fx_1 & fx_2 < fx_N2
     *  When included in a product, fx_N1 & fx_N2 are both included.
    */
    PVPType fxN2;

    /*!
     *  The  change in toa limits for the full resolution echoes retained in the
     *  signal vector (sec). Full resolution echoes are formed with
     *  FX_BW(v) saved bandwidth.
     *  Full resolution toa limits: Dtoa_1 < Dtoa < Dtoa_2
     */
    PVPType toa1;

    /*!
     *  The  change in toa limits for the full resolution echoes retained in the
     *  signal vector (sec). Full resolution echoes are formed with
     *  FX_BW(v) saved bandwidth.
     *  Full resolution toa limits: Dtoa_1 < Dtoa < Dtoa_2
     */
    PVPType toa2;

    /*!
     *  (Optional) The TOA limits for all echoes retained in the signal vector (sec).
     */
    PVPType toaE1;

    /*!
     *  (Optional) The TOA limits for all echoes retained in the signal vector (sec).
     */
    PVPType toaE2;

    /*!
     *  Two-way time delay due to the troposphere (sec) that was added
     *  when computing the propagation time for the SRP
     */
    PVPType tdTropoSRP;

    /*!
     *  (Optional) Two-way time delay due to the ionosphere (sec) that was added
     *  when computing the propagation time for the SRP
     */
    PVPType tdIonoSRP;

    /*!
     *  FX DOMAIN & TOA DOMAIN: The domain signal vector coordinate value for
     *  sample s = 0 (Hz).
     */
    PVPType sc0;

    /*!
     *  FX DOMAIN & TOA DOMAIN: The domain signal vector coordinate value for
     *  signal coordinate sample spacing (Hz).
     */
    PVPType scss;

    /*!
     *  (Optional) Integer parameter that may be included to indicate the signal
     *  content for some vectors is known or is likely to be distorted.
     */
    PVPType signal;

    //! (Optional) Parameters included that specify the Transmit Antenna ACF orientation and the EB Steering vector
    six::XsElement_minOccurs0<TxAntenna> txAntenna{ "TxAntenna" }; // new in CPHD 1.1.0

    //! (Optional) Parameters included that specify the Receive Antenna ACF orientation and the EB Steering vector
    six::XsElement_minOccurs0<RcvAntenna> rcvAntenna{ "RcvAntenna" }; // new in CPHD 1.1.0

    /*
     *  (Optional) User defined PV parameters
     */
    std::map<std::string, APVPType> addedPVP;

    /*
     *  \func Constructor
     *  \brief Initialize default values for each parameter
     *
     */
    Pvp();

    //! Equality operators
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
                signal == other.signal
                && txAntenna == other.txAntenna
                && rcvAntenna == other.rcvAntenna
                && addedPVP == other.addedPVP;
    }
    bool operator!=(const Pvp& other) const
    {
        return !((*this) == other);
    }

    //! Get size of pvp set in blocks
    size_t getReqSetSize() const;

    //! Get total byte size of pvp set
    size_t sizeInBytes() const;

    /*
     *  \func setOffset
     *
     *  \brief Validate and set the offset of parameters
     *
     *  \param offset The offset of the parameter to be expected for the param
     *  \param[out] param The PVPType parameter that should be set
     *
     *  \throws except::Exception If param offset or size overlaps another parameter, or
     *   if format is invalid
     */
    void setOffset(size_t offset, PVPType& param);

    /*
     *  \func append
     *
     *  \brief Validate and append parameter to the next available block
     *
     *  \param[out] param The PVPType parameter that should be set
     *
     *  \throws except::Exception If param offset or size overlaps another parameter, or
     *   if format is invalid
     */
    void append(PVPType& param);

    /*
     *  \func setParameter
     *
     *  \brief Validate and set the metadata of an additional parameter identified by name
     *
     *  \param size The size of the parameter to be expected for the param
     *  \param offset The offset of the parameter to be expected for the param
     *  \param format The string format of the parameter to be expected for the param
     *  \param name The unique identifier of the additional parameter that should be set
     *
     *  \throws except::Exception If param offset or size overlaps another parameter,
     *   if format is invalid or if name is not unique
     */
    void setCustomParameter(size_t size, size_t offset, const std::string& format, const std::string& name);

    /*
     *  \func setCustomParameter
     *
     *  \brief Validate and append additional parameter to the next available block
     *
     *  \param size The size of the parameter to be expected for the param
     *  \param format The string format of the parameter to be expected for the param
     *  \param name The unique identifier of the additional parameter that should be set
     *
     *  \throws except::Exception If param offset or size overlaps another parameter,
     *   if format is invalid or if name is not unique
     */
    void appendCustomParameter(size_t size, const std::string& format, const std::string& name);

private:
    /*
     *  Validate parameter size and offset when setting parameter
     */
    void validate(size_t size, size_t offset);

    /*
     *  Marks filled bytes
     */
    std::vector<bool> mParamLocations;

    /*
     * Set default size and format for each parameter
     */
    void setDefaultValues(size_t size, const std::string& format, PVPType& param);

    /*
     * Initializes default size and format for parameters
     */
    void initialize();
};

//! Ostream operators
std::ostream& operator<< (std::ostream& os, const PVPType& p);
std::ostream& operator<< (std::ostream& os, const APVPType& a);
std::ostream& operator<< (std::ostream& os, const Pvp& p);
}
#endif // SIX_cphd_PVP_h_INCLUDED_

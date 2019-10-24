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

#ifndef __CPHD_PVP_BLOCK_H__
#define __CPHD_PVP_BLOCK_H__

#include <ostream>
#include <vector>
#include <complex>
#include <stddef.h>
#include <typeinfo>
#include <map>
#include <sys/Conf.h>
#include <cphd/Types.h>
#include <cphd/Data.h>
#include <cphd/PVP.h>
#include <cphd/Metadata.h>
#include <cphd/ByteSwap.h>
#include <cphd/ComplexParameter.h>

namespace cphd
{

/*!
 * PVP Block (Actual data)
 */
struct PVPBlock
{
    /*!
     * Stores set of parameters
     */
    struct PVPSet
    {
        //! Default constructor
        PVPSet();

        //! Set parameters from input
        void write(const Pvp& p, const sys::byte* input);

        //! Get parameters into output
        void read(const Pvp& p, sys::ubyte* output) const;

        //! Equality operators
        bool operator==(const PVPSet& other) const
        {
            if(!(txTime == other.txTime && txPos == other.txPos &&
                    txVel == other.txVel && rcvTime == other.rcvTime &&
                    rcvPos == other.rcvPos && rcvVel == other.rcvVel &&
                    srpPos == other.srpPos && aFDOP == other.aFDOP &&
                    aFRR1 == other.aFRR1 && aFRR2 == other.aFRR2 &&
                    fx1 == other.fx1 && fx2 == other.fx2 &&
                    toa1 == other.toa1 && toa2 == other.toa2 &&
                    tdTropoSRP == other.tdTropoSRP && sc0 == other.sc0 &&
                    scss == other.scss &&
                    ampSF == other.ampSF && fxN1 == other.fxN1 &&
                    fxN2 == other.fxN2 && toaE1 == other.toaE1 &&
                    toaE2 == other.toaE2 && tdIonoSRP == other.tdIonoSRP &&
                    signal == other.signal))
            {
                return false;
            }
                if (addedPVP.size() != other.addedPVP.size())
                {
                    return false;
                }
                for (auto it = addedPVP.begin(); it != addedPVP.end(); ++it)
                {
                    if(it->second.str() != other.addedPVP.find(it->first)->second.str())
                    {
                        return false;
                    }
                }
                return true;
        }
        bool operator!=(const PVPSet& other) const
        {
            return !((*this) == other);
        }

        //! Required Parameters
        double txTime;
        Vector3 txPos;
        Vector3 txVel;
        double rcvTime;
        Vector3 rcvPos;
        Vector3 rcvVel;
        Vector3 srpPos;
        double aFDOP;
        double aFRR1;
        double aFRR2;
        double fx1;
        double fx2;
        double toa1;
        double toa2;
        double tdTropoSRP;
        double sc0;
        double scss;

        //! (Optional) Parameters
        mem::ScopedCopyablePtr<double> ampSF;
        mem::ScopedCopyablePtr<double> fxN1;
        mem::ScopedCopyablePtr<double> fxN2;
        mem::ScopedCopyablePtr<double> toaE1;
        mem::ScopedCopyablePtr<double> toaE2;
        mem::ScopedCopyablePtr<double> tdIonoSRP;
        mem::ScopedCopyablePtr<double> signal;

        //! (Optional) Additional parameters
        std::map<std::string,ComplexParameter> addedPVP;

    private:
        friend std::ostream& operator<< (std::ostream& os, const PVPBlock::PVPSet& p);
    };

    /*
     * Constructor
     */
    PVPBlock() :
        mNumBytesPerVector(0)
    {
    }

    /*
     *  \func Constructor
     *  \brief Sets up the internal structure of a VBM based on metadata.
     *
     *  \param data A filled out data structure. This will be used to
     *         extract the number of channels and vectors of the VBM.
     *         This will also be used to store off the number of bytes
     *         per vector regardless of the optional values.
    */
    PVPBlock(const Data& d, const Pvp& p);

    /*!
     * Custom constructor
     * Set mData object size and Pvp
     */
    PVPBlock(size_t numBytesPerVector,
             size_t numChannels,
             const std::vector<size_t>& numVectors,
             const Pvp& p);

    //! Verify channel and vector
    void verifyChannelVector(size_t channel, size_t vector) const;

    /*
     * Getters
     */
    double getTxTime(size_t channel, size_t set);
    Vector3 getTxPos(size_t channel, size_t set);
    Vector3 getTxVel(size_t channel, size_t set);
    double getRcvTime(size_t channel, size_t set);
    Vector3 getRcvPos(size_t channel, size_t set);
    Vector3 getRcvVel(size_t channel, size_t set);
    Vector3 getSRPPos(size_t channel, size_t set);
    double getaFDOP(size_t channel, size_t set);
    double getaFRR1(size_t channel, size_t set);
    double getaFRR2(size_t channel, size_t set);
    double getFx1(size_t channel, size_t set);
    double getFx2(size_t channel, size_t set);
    double getTOA1(size_t channel, size_t set);
    double getTOA2(size_t channel, size_t set);
    double getTdTropoSRP(size_t channel, size_t set);
    double getSC0(size_t channel, size_t set);
    double getSCSS(size_t channel, size_t set);
    double getAmpSF(size_t channel, size_t set);
    double getFxN1(size_t channel, size_t set);
    double getFxN2(size_t channel, size_t set);
    double getTOAE1(size_t channel, size_t set);
    double getTOAE2(size_t channel, size_t set);
    double getTdIonoSRP(size_t channel, size_t set);
    double getSignal(size_t channel, size_t set);

    template<typename T> T getAddedPVP(size_t channel, size_t set, const std::string& name)
    {
        verifyChannelVector(channel, set);
        if(mData[channel][set].addedPVP.count(name) == 1)
        {
            AddedPVP<T> aP;
            return aP.getAddedPVP(mData[channel][set].addedPVP.find(name)->second, channel, set, name);
        }
        throw except::Exception(Ctxt(
                "Parameter was not set"));
    }

    /*
     * Setters
     */
    void setTxTime(double value, size_t channel, size_t set);
    void setTxPos(const Vector3& value, size_t channel, size_t set);
    void setTxVel(const Vector3& value, size_t channel, size_t set);
    void setRcvTime(double value, size_t channel, size_t set);
    void setRcvPos(const Vector3& value, size_t channel, size_t set);
    void setRcvVel(const Vector3& value, size_t channel, size_t set);
    void setSRPPos(const Vector3& value, size_t channel, size_t set);
    void setaFDOP(double value, size_t channel, size_t set);
    void setaFRR1(double value, size_t channel, size_t set);
    void setaFRR2(double value, size_t channel, size_t set);
    void setFx1(double value, size_t channel, size_t set);
    void setFx2(double value, size_t channel, size_t set);
    void setTOA1(double value, size_t channel, size_t set);
    void setTOA2(double value, size_t channel, size_t set);
    void setTdTropoSRP(double value, size_t channel, size_t set);
    void setSC0(double value, size_t channel, size_t set);
    void setSCSS(double value, size_t channel, size_t set);
    void setAmpSF(double value, size_t channel, size_t set);
    void setFxN1(double value, size_t channel, size_t set);
    void setFxN2(double value, size_t channel, size_t set);
    void setTOAE1(double value, size_t channel, size_t set);
    void setTOAE2(double value, size_t channel, size_t set);
    void setTdIonoSRP(double value, size_t channel, size_t set);
    void setSignal(double value, size_t channel, size_t set);

    template<typename T> void setAddedPVP(T value, size_t channel, size_t set, const std::string& name)
    {
        verifyChannelVector(channel, set);
        if(mPvp.addedPVP.count(name) != 0)
        {
            if(mData[channel][set].addedPVP.count(name) == 0)
            {
                mData[channel][set].addedPVP[name] = ComplexParameter();
                mData[channel][set].addedPVP.find(name)->second.setValue(value);
                return;
            }
            throw except::Exception(Ctxt(
                                "Additional parameter requested already exists"));
        }
        throw except::Exception(Ctxt(
                                "Parameter was not specified in XML"));
    }

    /*
     *  \func getPVPdata
     *  \brief This will return a contiguous buffer all the PVP data.
     *
     *  \param channel 0 based index
     *  \param [Output]data Will be filled with PVP data. This will
     *         be resized and zeroed internally.
     */
    void getPVPdata(size_t channel,
                    std::vector<sys::ubyte>& data) const;

    /*
     *  \func getPVPdata
     *  \brief Same as above but uses a void pointer.
     *
     *  \param channel 0 based index
     *  \param [Output]data A preallocated buffer for the data.
     */
    void getPVPdata(size_t channel,
                    void* data) const;

    /*
     *  \func getNumBytesVBP
     *  \brief Returns the number of bytes of each vector in the PVPBlock.
     *         This does not necessarily match the number of bytes used in
     *         the case where a CPHD file had a larger number allocated than
     *         actually used.
     */
    size_t getNumBytesPVPSet() const;

    /*
     *  \func getPVPsize
     *  \brief Returns the number of bytes in a PVP channel.
     *
     *  \param channel 0 based index
     */
    size_t getPVPsize(size_t channel) const;

    // Read in the entire PVP Array, return number of bytes read or -1 if error
    // startPVP = cphd header keyword "PVP_BYTE_OFFSET"
    // sizePVP = cphd header keyword "PVP_DATA_SIZE"
    sys::Off_T load(io::SeekableInputStream& inStream,
                    sys::Off_T startPVP,
                    sys::Off_T sizePVP,
                    size_t numThreads);

    bool operator==(const PVPBlock& other) const
    {
        return mData == other.mData &&
               mNumBytesPerVector == other.mNumBytesPerVector;
    }

    bool operator!=(const PVPBlock& other) const
    {
        return !((*this) == other);
    }

private:

    /*
     * Struct Template Specialization for getAddedPVP function
     * mimics function template specialization
     */
    template<typename T>
    struct AddedPVP
    {
        T getAddedPVP(ComplexParameter& val, size_t channel, size_t set, const std::string& name)
        {
            return static_cast<T>(val);
        }
    };

    template<typename T>
    struct AddedPVP<std::complex<T> >
    {
        std::complex<T> getAddedPVP(ComplexParameter& val, size_t channel, size_t set, const std::string& name)
        {
            return val.getComplex<T>();
        }
    };

    // The PVP Block [Num Channles][Num Parameters]
    std::vector<std::vector<PVPSet> > mData;
    size_t mNumBytesPerVector;
    Pvp mPvp;
    friend std::ostream& operator<< (std::ostream& os, const PVPBlock& p);
};
}
#endif


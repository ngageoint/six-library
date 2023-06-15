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

#include <stddef.h>

#include <ostream>
#include <vector>
#include <cstdint>
#include <unordered_map>
#include <std/optional>

#include <types/complex.h>

#include <scene/sys_Conf.h>
#include <cphd/Types.h>
#include <cphd/Data.h>
#include <cphd/PVP.h>
#include <cphd/Metadata.h>
#include <cphd/ByteSwap.h>
#include <six/Parameter.h>

namespace cphd
{
    class FileHeader;
/*
 *  \struct AddedPVP
 *  \brief Template Specialization to get additional pvp
 *
 *  Mimics function template specialization
 *
 * \tparam T Desired type to convert to
 */
template<typename T>
struct AddedPVP
{
    T getAddedPVP(const six::Parameter& val) const
    {
        return static_cast<T>(val);
    }
};
template<>
struct AddedPVP<types::zfloat>
{
    auto getAddedPVP(const six::Parameter& val) const
    {
        return val.getComplex<float>();
    }
};
template<>
struct AddedPVP<types::zdouble>
{
    auto getAddedPVP(const six::Parameter& val) const
    {
        return val.getComplex<double> ();
    }
};
template<typename T>
struct AddedPVP<types::zinteger<T> >
{
    auto getAddedPVP(const six::Parameter& val) const
    {
        return val.getComplex<T>();
    }
};
template<>
struct AddedPVP<std::string>
{
    std::string getAddedPVP(const six::Parameter& val) const
    {
        return val.str();
    }
};

/*!
 *  \struct PVPBlock
 *
 *  \brief The PVP Block contains the actual PVP data
 *
 *  PVPBlock handles reading PVPBlock from CPHD file, and loading the data structure
 */
struct PVPBlock
{
    PVPBlock() = default;

    /*
     *  \func PVPBlock
     *  \brief Consturctor sets up the internal structure of a PVPSet based on metadata.
     *
     *  \param pvp A filled out pvp sturcture. This will be used for
     *  information on where to allocate memory and set each
     *  parameter in a PVP set.
     *  \param[in, out] data A filled out data structure. This will be used to
     *  extract the number of channels and vectors of the VBM.
     *  The data metadata will also be updated with the number of pvp bytes required
     *  if provided number of bytes is not sufficient
    */
    PVPBlock(const Pvp& pvp, const Data& data);
    PVPBlock(const Metadata&);

    /*!
     *  \func PVPBlock
     *
     *  \brief Constructor sets up the internal sturcture of a PVPSet without metadata.
     *
     *  Does not update metadata data numBytesPVP with calculated pvp bytes per set
     *
     *  \param numChannels Set the number of PVP Arrays to allocate in the block
     *  \param numVectors Set the number of PVP Sets to allocate per array
     *  \param pvp A filled out pvp sturcture. This will be used for
     *  information on where to allocate memory and set each
     *  parameter in a PVP set.
     */
    PVPBlock(size_t numChannels,
             const std::vector<size_t>& numVectors,
             const Pvp& pvp);

    /*!
     * Constructor.
     * Same as above, and also sets the underlying PVPSets
     * from the provided data
     *
     * \param numChannels Set the number of PVP Arrays to allocate in the block
     * \param numVectors Set the number of PVP Sets to allocate per array
     * \param pvp A filled out pvp sturcture. This will be used for
     * information on where to allocate memory and set each
     * parameter in a PVP set.
     * \param data Actual PVP data, used to set the PVPSet members
     */
    PVPBlock(size_t numChannels,
            const std::vector<size_t>& numVectors,
            const Pvp& pvp,
            const std::vector<const void*>& data);

    /*!
     *  \func verifyChannelVector
     *
     *  \brief Verify channel and vector indexes provided are valid
     *
     *  \param channel A channel number
     *  \param vector A vector number
     *
     *  \throws except::Exception If channel or vector indexes are less than 0,
     *  or greater than available number of channels or vectors
     */
    void verifyChannelVector(size_t channel, size_t vector) const;

    //! Getter functions
    double getTxTime(size_t channel, size_t set) const;
    Vector3 getTxPos(size_t channel, size_t set) const;
    Vector3 getTxVel(size_t channel, size_t set) const;
    double getRcvTime(size_t channel, size_t set) const;
    Vector3 getRcvPos(size_t channel, size_t set) const;
    Vector3 getRcvVel(size_t channel, size_t set) const;
    Vector3 getSRPPos(size_t channel, size_t set) const;
    double getaFDOP(size_t channel, size_t set) const;
    double getaFRR1(size_t channel, size_t set) const;
    double getaFRR2(size_t channel, size_t set) const;
    double getFx1(size_t channel, size_t set) const;
    double getFx2(size_t channel, size_t set) const;
    double getTOA1(size_t channel, size_t set) const;
    double getTOA2(size_t channel, size_t set) const;
    double getTdTropoSRP(size_t channel, size_t set) const;
    double getSC0(size_t channel, size_t set) const;
    double getSCSS(size_t channel, size_t set) const;
    double getAmpSF(size_t channel, size_t set) const;
    double getFxN1(size_t channel, size_t set) const;
    double getFxN2(size_t channel, size_t set) const;
    double getTOAE1(size_t channel, size_t set) const;
    double getTOAE2(size_t channel, size_t set) const;
    double getTdIonoSRP(size_t channel, size_t set) const;
    std::int64_t getSignal(size_t channel, size_t set) const;

    template<typename T>
    T getAddedPVP(size_t channel, size_t set, const std::string& name) const
    {
        verifyChannelVector(channel, set);
        if(mData[channel][set].addedPVP.count(name) == 1)
        {
            AddedPVP<T> aP;
            return aP.getAddedPVP(mData[channel][set].addedPVP.find(name)->second);
            // return AddPVPNamespace::getAddedPVP<T>(mData[channel][set].addedPVP.find(name)->second);
        }
        throw except::Exception(Ctxt(
                "Parameter was not set"));
    }

    //! Setter functions
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
    void setSignal(std::int64_t value, size_t channel, size_t set);

    template<typename T>
    void setAddedPVP(T value, size_t channel, size_t set, const std::string& name)
    {
        verifyChannelVector(channel, set);
        if(mPvp.addedPVP.count(name) != 0)
        {
            if(mData[channel][set].addedPVP.count(name) == 0)
            {
                mData[channel][set].addedPVP[name] = six::Parameter();
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
     *  \param[out] data Will be filled with PVP data. This will
     *  be resized and zeroed internally.
     */
    void getPVPdata(size_t channel,
                    std::vector<sys::ubyte>& data) const;
    void getPVPdata(size_t channel,
                    std::vector<std::byte>& data) const;

    /*
     *  \func getPVPdata
     *  \brief Same as above but uses a void pointer.
     *
     *  \param channel 0 based index
     *  \param[out] data A preallocated buffer for the data.
     */
    void getPVPdata(size_t channel,
                    void*  data) const;

    /*
     *  \func getNumBytesVBP
     *  \brief Number of bytes per PVP seet
     *
     *  Returns the number of bytes of each vector in the PVPBlock.
     *  This does not necessarily match the number of bytes used in
     *  the case where a CPHD file had a larger number allocated than
     *  actually used.
     */
    size_t getNumBytesPVPSet() const;

    /*
     *  \func getPVPsize
     *  \brief Returns the number of bytes in a PVP channel.
     *
     *  \param channel 0 based index
     */
    size_t getPVPsize(size_t channel) const;

    //! Get optional parameter flags
    bool hasAmpSF() const
    {
        return mAmpSFEnabled;
    }
    bool hasFxN1() const
    {
        return mFxN1Enabled;
    }
    bool hasFxN2() const
    {
        return mFxN2Enabled;
    }
    bool hasToaE1() const
    {
        return mToaE1Enabled;
    }
    bool hasToaE2() const
    {
        return mToaE2Enabled;
    }
    bool hasTDIonoSRP() const
    {
        return mTDIonoSRPEnabled;
    }
    bool hasSignal() const
    {
        return mSignalEnabled;
    }

    /*
     *  \func load
     *
     *  \brief Reads in the entire PVP array from an input stream
     *
     *  \param inStream Input stream that contains a valid CPHD file
     *  \param startPVP Offset of start of pvp block
     *  \param sizePVP Size of pvp block
     *  \param numThreads Number of threads desired for parallelism
     *
     *  \throw except::Exception If reach EOF before reading sizePVP bytes
     *
     *  \return Returns the size of the pvp block read in
     */
    // startPVP = cphd header keyword "PVP_BYTE_OFFSET" contains PVP block starting offset
    // sizePVP = cphd header keyword "PVP_DATA_SIZE" contains PVP block size
    int64_t load(io::SeekableInputStream& inStream,
                    int64_t startPVP,
                    int64_t sizePVP,
                    size_t numThreads);
    int64_t load(io::SeekableInputStream& inStream, const FileHeader&, size_t numThreads);

    //! Equality operators
    bool operator==(const PVPBlock& other) const
    {
        return mData == other.mData &&
               mNumBytesPerVector == other.mNumBytesPerVector;
    }

    bool operator!=(const PVPBlock& other) const
    {
        return !((*this) == other);
    }

protected:
    /*!
     *  \struct PVPSet
     *
     *  \brief Parameters for each vector
     *
     *  Stores a single set of parameters
     *  Each channel consists of a PVP Array,
     *  which consists of multiple sets
     */
    struct PVPSet
    {
        /*!
         *  \func PVPSet
         *
         *  \brief Default constructor
         */
        PVPSet();

        /*
         *  \func write
         *
         *  \brief Writes binary data input into PVP Set
         *
         *  \param pvpBlock A pvpBlock struct to access optional parameter flags
         *  \param pvp A filled out pvp sturcture. This will be used for
         *  information on where to allocate memory and set each
         *  parameter in a PVP set.
         *  \param input A pointer to an array of bytes that contains the
         *  parameter data to write into the pvp set
         */
        void write(const PVPBlock& pvpBlock, const Pvp& pvp, const sys::byte* input);
        void write(const PVPBlock& pvpBlock, const Pvp& pvp, const std::byte* input)
        {
            write(pvpBlock, pvp, reinterpret_cast<const sys::byte*>(input));
        }

        /*
         *  \func read
         *
         *  \brief Read PVP set into binary data output
         *
         *  \param pvp A filled out pvp sturcture. This will be used for
         *  information on where to allocate memory and set each
         *  parameter in a PVP set.
         *  \param[out] output A pointer to an array of allocated bytes that
         *  will be written to
         */
        void read(const Pvp& pvp, sys::ubyte* output) const;
        void read(const Pvp& pvp, std::byte* output) const
        {
            read(pvp, reinterpret_cast<sys::ubyte*>(output));
        }

        //! Equality operators
        bool operator==(const PVPSet& other) const
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
                    ampSF == other.ampSF && fxN1 == other.fxN1 &&
                    fxN2 == other.fxN2 && toaE1 == other.toaE1 &&
                    toaE2 == other.toaE2 && tdIonoSRP == other.tdIonoSRP &&
                    signal == other.signal && addedPVP == other.addedPVP;
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
        mem::ScopedCopyablePtr<std::int64_t> signal;

        //! (Optional) Additional parameters
        std::unordered_map<std::string,six::Parameter> addedPVP;
    };
    friend std::ostream& operator<< (std::ostream& os, const PVPSet& p);

private:
    //! The PVP Block [Num Channles][Num Parameters]
    std::vector<std::vector<PVPSet> > mData;
    //! Number of bytes per PVP vector
    size_t mNumBytesPerVector = 0;
    //! PVP block metadata
    Pvp mPvp;

    /*
     *  Optional parameter flags
     */
    bool mAmpSFEnabled;
    bool mFxN1Enabled;
    bool mFxN2Enabled;
    bool mToaE1Enabled;
    bool mToaE2Enabled;
    bool mTDIonoSRPEnabled;
    bool mSignalEnabled;

    //! Ostream operator
    friend std::ostream& operator<< (std::ostream& os, const PVPBlock& p);
};
}
#endif

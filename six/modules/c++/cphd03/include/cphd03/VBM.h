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

#ifndef __CPHD03_VBM_H__
#define __CPHD03_VBM_H__

#include <vector>

#include <sys/Conf.h>
#include <io/SeekableStreams.h>
#include <mem/ScopedCopyablePtr.h>
#include <cphd/Types.h>
#include <cphd03/Data.h>
#include <cphd03/VectorParameters.h>

namespace cphd03
{
//  This class contains the Vector Based Metadata.
//  It contains the cphd03::Data structure (for channel and vector sizes),
//  the cphd03::VectorParameters info (for the map of available VBM entries)
//  and the VBP data itself

class VBM
{
public:
    /*
     *  \func Constructor
     *  \brief Creates a default VBM. While this will create a valid VBM,
     *         it is better to use one of the constructors with params.
     */
    VBM();

    /*
     *  \func Constructor
     *  \brief Sets up the internal structure of a VBM based on metadata.
     *
     *  \param data A filled out data structure. This will be used to
     *         extract the number of channels and vectors of the VBM.
     *         This will also be used to store off the number of bytes
     *         per vector regardless of the optional values.
     *  \param vp A filled out vector parameters struct. This will be
     *         used to populate the correct optional parameters.
    */
    VBM(const Data& data, const VectorParameters &vp);

    /*
     *  \func Constructor
     *  \brief Sets up a VBM without needed a metatdata object.
     *
     *  \param numChannels The number of channels for the CPHD file.
     *  \param numVectors The number of vectors for each image in the
     *         CPHD file. There must be at least numChannels vectors.
     *  \param srpTimeEnabled Will this VBM include SRPTime information?
     *  \param tropoSrpEnabled Will this VBM include TropoSRP information?
     *  \param ampSFEnabled Will this VBM include AmpSF information?
     *  \param domainType What domain is CPHD image?
     */
    VBM(size_t numChannels,
        const std::vector<size_t>& numVectors,
        bool srpTimeEnabled,
        bool tropoSrpEnabled,
        bool ampSFEnabled,
        cphd::DomainType domainType);

    /*
     *  \func Constructor
     *  \brief Same as above but allows the user to pass in the VBM data.
     *
     *  \param data Must contain one void* per channel and each void* must
     *         be sized correctly for that channel
     */
    VBM(size_t numChannels,
        const std::vector<size_t>& numVectors,
        bool srpTimeEnabled,
        bool tropoSrpEnabled,
        bool ampSFEnabled,
        cphd::DomainType domainType,
        const std::vector<const void*>& data);

    // Retrieve the value of a VBM parameter  - request must be valid
    // invalid requests throw exceptions
    double getTxTime(size_t channel, size_t vector) const;
    cphd::Vector3 getTxPos(size_t channel, size_t vector) const;
    double getRcvTime(size_t channel, size_t vector) const;
    cphd::Vector3 getRcvPos(size_t channel, size_t vector) const;
    double getSRPTime(size_t channel, size_t vector) const;
    cphd::Vector3 getSRPPos(size_t channel, size_t vector) const;
    double getTropoSRP(size_t channel, size_t vector) const;
    double getAmpSF(size_t channel, size_t vector) const;
    double getFx0(size_t channel, size_t vector) const;
    double getFxSS(size_t channel, size_t vector) const;
    double getFx1(size_t channel, size_t vector) const;
    double getFx2(size_t channel, size_t vector) const;
    double getDeltaTOA0(size_t channel, size_t vector) const;
    double getTOASS(size_t channel, size_t vector) const;

    void setTxTime(double value, size_t channel, size_t vector);
    void setTxPos(const cphd::Vector3& value, size_t channel, size_t vector);
    void setRcvTime(double value, size_t channel, size_t vector);
    void setRcvPos(const cphd::Vector3& value, size_t channel, size_t vector);
    void setSRPTime(double value, size_t channel, size_t vector);
    void setSRPPos(const cphd::Vector3& value,size_t channel, size_t vector);
    void setTropoSRP(double value, size_t channel, size_t vector);
    void setAmpSF(double value, size_t channel, size_t vector);

    void setFx0(double value, size_t channel, size_t vector);
    void setFxSS(double value, size_t channel, size_t vector);
    void setFx1(double value, size_t channel, size_t vector);
    void setFx2(double value, size_t channel, size_t vector);

    void setDeltaTOA0(double value, size_t channel, size_t vector);
    void setTOASS(double value, size_t channel, size_t vector);

    // More convenience functions

    /*
     *  \func getNumBytesVBP
     *  \brief Returns the number of bytes of each vector in the VBM.
     *         This does not necessarily match the number of bytes used in
     *         the case where a CPHD file had a larger number allocated than
     *         actually used.
     */
    size_t getNumBytesVBP() const
    {
        return mNumBytesPerVector;
    }

    // Returns the number channels for the VBM.
    size_t getNumChannels() const
    {
        return mData.size();
    }

    void clearAmpSF();

    bool haveSRPTime() const
    {
        return mSRPTimeEnabled;
    }

    bool haveTropoSRP() const
    {
        return mTropoSRPEnabled;
    }

    // Return true if Amplitude Scale Factor is available
    bool haveAmpSF() const
    {
        return mAmpSFEnabled;
    }

    /*
     *  \func updateVectorParameters
     *  \brief Updates the offset values of a vector parameter based
     *         on the optional values set in the VBM.
     *
     *  \param [Output]vp  The vector parameters to modify.
     */
    void updateVectorParameters(VectorParameters& vp) const;

    // Read the entire VBM, return number of bytes read or -1 if error
    // startVBM = cphd03 header keyword "VB_BYTE_OFFSET"
    // sizeVBM = cphd03 header keyword "VB_DATA_SIZE"
    ptrdiff_t load(io::SeekableInputStream& inStream,
                    ptrdiff_t startVBM,
                    ptrdiff_t sizeVBM,
                    size_t numThreads);

    /*
     *  \func getVBMdata
     *  \brief This will return a contiguous buffer all the VBM data.
     *
     *  \param channel 0 based index
     *  \param [Output]data Will be filled with VBM data. This will
     *         be resized and zeroed internally.
     */
    void getVBMdata(size_t channel,
                    std::vector<std::byte>& data) const;

    /*
     *  \func getVBMdata
     *  \brief Same as above but uses a void pointer.
     *
     *  \param channel 0 based index
     *  \param [Output]data A preallocated buffer for the data.
     */
    void getVBMdata(size_t channel,
                    void* data) const;

    /*
     *  \func getVBMsize
     *  \brief Returns the number of bytes in a VBM channel.
     *
     *  \param channel 0 based index
     */
    size_t getVBMsize(size_t channel) const;

    bool operator==(const VBM& other) const
    {
        return mSRPTimeEnabled == other.mSRPTimeEnabled &&
               mTropoSRPEnabled == other.mTropoSRPEnabled &&
               mAmpSFEnabled == other.mAmpSFEnabled &&
               mDomainType == other.mDomainType &&
               mNumBytesPerVector == other.mNumBytesPerVector &&
               mData == other.mData;
    }

    bool operator!=(const VBM& vbm) const
    {
        return !((*this) == vbm);
    }

private:
    struct FrequencyParameters
    {
        FrequencyParameters();

        bool operator==(const FrequencyParameters& other) const
        {
            return fx0 == other.fx0 &&
                   fxSS == other.fxSS &&
                   fx1 == other.fx1 &&
                   fx2 == other.fx2;
        }

        bool operator!=(const FrequencyParameters& other) const
        {
            return !((*this) == other);
        }

        double fx0;
        double fxSS;
        double fx1;
        double fx2;
    };

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

        double deltaTOA0;
        double toaSS;
    };

    struct VectorBasedParameters
    {
        VectorBasedParameters(bool srpTimeEnabled,
                              bool tropoSrpEnabled,
                              bool ampSFEnabled,
                              cphd::DomainType domainType);

        size_t getNumBytes() const;

        void getData(std::byte* data) const;

        void setData(const std::byte* data);

        bool operator==(const VectorBasedParameters& other) const;

        bool operator!=(const VectorBasedParameters& other) const
        {
            return !((*this) == other);
        }

        double txTime;
        cphd::Vector3 txPos;
        double rcvTime;
        cphd::Vector3 rcvPos;
        double srpTime;
        cphd::Vector3 srpPos;
        double tropoSrp;
        double ampSF;
        mem::ScopedCopyablePtr<FrequencyParameters> frequencyParameters;
        mem::ScopedCopyablePtr<TOAParameters> toaParameters;
    };

    void verifyChannelVector(size_t channel, size_t vector) const;

    void setupInitialData(size_t numChannels,
                          const std::vector<size_t>& numVectors);

    bool mSRPTimeEnabled;
    bool mTropoSRPEnabled;
    bool mAmpSFEnabled;
    cphd::DomainType mDomainType;
    size_t mNumBytesPerVector;

    // One vector of VectorBasedParameters per channel.
    // Then one VectorBasedParameter per vector.
    std::vector<std::vector<VectorBasedParameters> > mData;

    friend std::ostream& operator<< (std::ostream& os, const VBM& d);
};
}

#endif

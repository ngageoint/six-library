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

#include <sstream>
#include <string.h>

#include <six/Init.h>
#include <cphd/ByteSwap.h>
#include <cphd03/VBM.h>

namespace
{
inline void setData(const sys::byte*& data,
                    double& dest)
{
    memcpy(&dest, data, sizeof(double));
    data += sizeof(double);
}

inline void setData(const sys::byte*& data,
                    cphd::Vector3& dest)
{
    setData(data, dest[0]);
    setData(data, dest[1]);
    setData(data, dest[2]);
}

inline void getData(double value,
                    sys::ubyte*& dest)
{
    memcpy(dest, &value, sizeof(double));
    dest += sizeof(double);
}

inline void getData(const cphd::Vector3& value,
                    sys::ubyte*& dest)
{
    getData(value[0], dest);
    getData(value[1], dest);
    getData(value[2], dest);
}
}

namespace cphd03
{
VBM::FrequencyParameters::FrequencyParameters() :
    fx0(0.0),
    fxSS(0.0),
    fx1(0.0),
    fx2(0.0)
{
}

VBM::TOAParameters::TOAParameters() :
    deltaTOA0(0.0),
    toaSS(0.0)
{
}

VBM::VectorBasedParameters::VectorBasedParameters(
        bool srpTimeEnabled,
        bool tropoSrpEnabled,
        bool ampSFEnabled,
        cphd::DomainType domainType) :
    txTime(0.0),
    txPos(0.0),
    rcvTime(0.0),
    rcvPos(0.0),
    srpTime(srpTimeEnabled ? 0.0 : six::Init::undefined<double>()),
    srpPos(0.0),
    tropoSrp(tropoSrpEnabled ? 0.0 : six::Init::undefined<double>()),
    ampSF(ampSFEnabled ? 0.0 : six::Init::undefined<double>()),
    frequencyParameters(domainType == cphd::DomainType::FX ?
            new FrequencyParameters() : NULL),
    toaParameters(domainType == cphd::DomainType::TOA ?
            new TOAParameters() : NULL)
{
}

size_t VBM::VectorBasedParameters::getNumBytes() const
{
    size_t ret = 11 * sizeof(double);
    if (!six::Init::isUndefined<double>(srpTime))
    {
        ret += sizeof(srpTime);
    }
    if (!six::Init::isUndefined<double>(tropoSrp))
    {
        ret += sizeof(tropoSrp);
    }
    if (!six::Init::isUndefined<double>(ampSF))
    {
        ret += sizeof(ampSF);
    }

    if (frequencyParameters.get())
    {
        ret += sizeof(FrequencyParameters);
    }
    else if (toaParameters.get())
    {
        ret += sizeof(TOAParameters);
    }
    return ret;
}

void VBM::VectorBasedParameters::getData(sys::ubyte* data) const
{
    //! This uses memcpy's here because on Sun these addresses may not be
    //  8 byte aligned. So trying to derefence data as a double results in
    //  a crash.
    ::getData(txTime, data);
    ::getData(txPos, data);
    ::getData(rcvTime, data);
    ::getData(rcvPos, data);
    if (!six::Init::isUndefined<double>(srpTime))
    {
        ::getData(srpTime, data);
    }
    ::getData(srpPos, data);
    if (!six::Init::isUndefined<double>(tropoSrp))
    {
        ::getData(tropoSrp, data);
    }
    if (!six::Init::isUndefined<double>(ampSF))
    {
        ::getData(ampSF, data);
    }
    if (frequencyParameters.get())
    {
        ::getData(frequencyParameters->fx0, data);
        ::getData(frequencyParameters->fxSS, data);
        ::getData(frequencyParameters->fx1, data);
        ::getData(frequencyParameters->fx2, data);
    }
    else if (toaParameters.get())
    {
        ::getData(toaParameters->deltaTOA0, data);
        ::getData(toaParameters->toaSS, data);
    }
}

void VBM::VectorBasedParameters::setData(const sys::byte* data)
{
    //! This uses memcpy's here because on Sun these addresses may not be
    //  8 byte aligned. So trying to derefence data as a double results in
    //  a crash.
    ::setData(data, txTime);
    ::setData(data, txPos);
    ::setData(data, rcvTime);
    ::setData(data, rcvPos);
    if (!six::Init::isUndefined<double>(srpTime))
    {
        ::setData(data, srpTime);
    }
    ::setData(data, srpPos);
    if (!six::Init::isUndefined<double>(tropoSrp))
    {
        ::setData(data, tropoSrp);
    }
    if (!six::Init::isUndefined<double>(ampSF))
    {
        ::setData(data, ampSF);
    }
    if (frequencyParameters.get())
    {
        ::setData(data, frequencyParameters->fx0);
        ::setData(data, frequencyParameters->fxSS);
        ::setData(data, frequencyParameters->fx1);
        ::setData(data, frequencyParameters->fx2);
    }
    else if (toaParameters.get())
    {
        ::setData(data, toaParameters->deltaTOA0);
        ::setData(data, toaParameters->toaSS);
    }
}

bool VBM::VectorBasedParameters::operator==(
        const VBM::VectorBasedParameters& other) const
{
    if (txTime != other.txTime ||
        txPos != other.txPos ||
        rcvTime != other.rcvTime ||
        rcvPos != other.rcvPos ||
        srpPos != other.srpPos)
    {
        return false;
    }

    if (!six::Init::isUndefined<double>(srpTime) ||
        !six::Init::isUndefined<double>(other.srpTime))
    {
        if (srpTime != other.srpTime)
        {
            return false;
        }
    }

    if (!six::Init::isUndefined<double>(tropoSrp) ||
        !six::Init::isUndefined<double>(other.tropoSrp))
    {
        if (tropoSrp != other.tropoSrp)
        {
            return false;
        }
    }

    if (!six::Init::isUndefined<double>(ampSF) ||
        !six::Init::isUndefined<double>(other.ampSF))
    {
        if (ampSF != other.ampSF)
        {
            return false;
        }
    }

    if (frequencyParameters.get() && other.frequencyParameters.get())
    {
        if (*frequencyParameters != *other.frequencyParameters)
        {
            return false;
        }
    }
    else if (frequencyParameters.get() || other.frequencyParameters.get())
    {
        return false;
    }

    if (toaParameters.get() && other.toaParameters.get())
    {
        if (*toaParameters != *other.toaParameters)
        {
            return false;
        }
    }
    else if (toaParameters.get() || other.toaParameters.get())
    {
        return false;
    }
    return true;
}

VBM::VBM() :
    mSRPTimeEnabled(false),
    mTropoSRPEnabled(false),
    mAmpSFEnabled(false),
    mDomainType(cphd::DomainType::NOT_SET),
    mNumBytesPerVector(0)
{
}

VBM::VBM(const Data& data, const VectorParameters &vp) :
    mSRPTimeEnabled(vp.srpTimeOffset() > 0),
    mTropoSRPEnabled(vp.tropoSRPOffset() > 0),
    mAmpSFEnabled(vp.ampSFOffset() > 0),
    mDomainType(vp.fxParameters.get() ? cphd::DomainType::FX :
            vp.toaParameters.get() ? cphd::DomainType::TOA : cphd::DomainType::NOT_SET),
    mNumBytesPerVector(data.getNumBytesVBP()),
    mData(data.numCPHDChannels)
{
    const VectorBasedParameters copy(mSRPTimeEnabled,
                                     mTropoSRPEnabled,
                                     mAmpSFEnabled,
                                     mDomainType);

    for (size_t ii = 0; ii < data.numCPHDChannels; ++ii)
    {
        mData[ii].resize(data.getNumVectors(ii), copy);
    }

    if (!mData.empty() && !mData[0].empty())
    {
        const size_t calculateBytesPerVector =
                mData[0][0].getNumBytes();
        if (six::Init::isUndefined<size_t>(mNumBytesPerVector) ||
            calculateBytesPerVector > mNumBytesPerVector)
        {
            mNumBytesPerVector = calculateBytesPerVector;
        }
    }
}

VBM::VBM(size_t numChannels,
         const std::vector<size_t>& numVectors,
         bool srpTimeEnabled,
         bool tropoSrpEnabled,
         bool ampSFEnabled,
         cphd::DomainType domainType) :
    mSRPTimeEnabled(srpTimeEnabled),
    mTropoSRPEnabled(tropoSrpEnabled),
    mAmpSFEnabled(ampSFEnabled),
    mDomainType(domainType),
    mNumBytesPerVector(0),
    mData(numChannels)
{
    setupInitialData(numChannels, numVectors);
}

VBM::VBM(size_t numChannels,
         const std::vector<size_t>& numVectors,
         bool srpTimeEnabled,
         bool tropoSrpEnabled,
         bool ampSFEnabled,
         cphd::DomainType domainType,
         const std::vector<const void*>& data) :
    mSRPTimeEnabled(srpTimeEnabled),
    mTropoSRPEnabled(tropoSrpEnabled),
    mAmpSFEnabled(ampSFEnabled),
    mDomainType(domainType),
    mNumBytesPerVector(0),
    mData(numChannels)
{
    //! Make sure there is enough data for each channel
    if (numChannels != data.size())
    {
        throw except::Exception(Ctxt(
                "VBM data contains " + str::toString<size_t>(numChannels) +
                " channels, but data has information for " +
                str::toString<size_t>(data.size()) + " channels."));
    }
    setupInitialData(numChannels, numVectors);

    //! For each channel
    for (size_t ii = 0; ii < mData.size(); ++ii)
    {
        const sys::byte* ptr = static_cast<const sys::byte*>(data[ii]);

        //! For each vector
        for (size_t jj = 0;
             jj < mData[ii].size();
             ++jj, ptr += mNumBytesPerVector)
        {
            mData[ii][jj].setData(ptr);
        }
    }
}

void VBM::verifyChannelVector(size_t channel, size_t vector) const
{
    if (channel >= mData.size())
    {
        throw except::Exception(Ctxt(
                "Invalid channel number: " + str::toString<size_t>(channel)));
    }
    if (vector >= mData[channel].size())
    {
        throw except::Exception(Ctxt(
                "Invalid vector number: " + str::toString<size_t>(vector)));
    }
}

void VBM::setupInitialData(size_t numChannels,
                           const std::vector<size_t>& numVectors)
{
    if (numChannels > numVectors.size())
    {
        throw except::Exception(Ctxt("Invalid numVectors parameter: "
                "You must pass a vector sized to the number of channels"));
    }
    const VectorBasedParameters copy(mSRPTimeEnabled,
                                     mTropoSRPEnabled,
                                     mAmpSFEnabled,
                                     mDomainType);

    for (size_t ii = 0; ii < numChannels; ++ii)
    {
        mData[ii].resize(numVectors[ii], copy);
    }

    if (!mData.empty() && !mData[0].empty())
    {
        mNumBytesPerVector = mData[0][0].getNumBytes();
    }
}

double VBM::getTxTime(size_t channel, size_t vector) const
{
    verifyChannelVector(channel, vector);
    return mData[channel][vector].txTime;
}

cphd::Vector3 VBM::getTxPos(size_t channel, size_t vector) const
{
    verifyChannelVector(channel, vector);
    return mData[channel][vector].txPos;
}

double VBM::getRcvTime(size_t channel, size_t vector) const
{
    verifyChannelVector(channel, vector);
    return mData[channel][vector].rcvTime;
}

cphd::Vector3 VBM::getRcvPos(size_t channel, size_t vector) const
{
    verifyChannelVector(channel, vector);
    return mData[channel][vector].rcvPos;
}

double VBM::getSRPTime(size_t channel, size_t vector) const
{
    verifyChannelVector(channel, vector);
    if (!mSRPTimeEnabled)
    {
        throw except::Exception(Ctxt("Invalid SRP time."));
    }
    return mData[channel][vector].srpTime;
}

cphd::Vector3 VBM::getSRPPos(size_t channel, size_t vector) const
{
    verifyChannelVector(channel, vector);
    return mData[channel][vector].srpPos;
}

double VBM::getTropoSRP(size_t channel, size_t vector) const
{
    verifyChannelVector(channel, vector);
    if (!mTropoSRPEnabled)
    {
        throw except::Exception(Ctxt("Invalid TropoSRP."));
    }
    return mData[channel][vector].tropoSrp;
}

double VBM::getAmpSF(size_t channel, size_t vector) const
{
    verifyChannelVector(channel, vector);
    if (!mAmpSFEnabled)
    {
        throw except::Exception(Ctxt("Invalid AmpSF."));
    }
    return mData[channel][vector].ampSF;
}

double VBM::getFx0(size_t channel, size_t vector) const
{
    verifyChannelVector(channel, vector);
    if (mDomainType != cphd::DomainType::FX)
    {
        throw except::Exception(Ctxt("Invalid Fx0."));
    }
    return mData[channel][vector].frequencyParameters->fx0;
}

double VBM::getFxSS(size_t channel, size_t vector) const
{
    verifyChannelVector(channel, vector);
    if (mDomainType != cphd::DomainType::FX)
    {
        throw except::Exception(Ctxt("Invalid FxSS."));
    }
    return mData[channel][vector].frequencyParameters->fxSS;
}

double VBM::getFx1(size_t channel, size_t vector) const
{
    verifyChannelVector(channel, vector);
    if (mDomainType != cphd::DomainType::FX)
    {
        throw except::Exception(Ctxt("Invalid Fx1."));
    }
    return mData[channel][vector].frequencyParameters->fx1;
}

double VBM::getFx2(size_t channel, size_t vector) const
{
    verifyChannelVector(channel, vector);
    if (mDomainType != cphd::DomainType::FX)
    {
        throw except::Exception(Ctxt("Invalid Fx2."));
    }
    return mData[channel][vector].frequencyParameters->fx2;
}

double VBM::getDeltaTOA0(size_t channel, size_t vector) const
{
    verifyChannelVector(channel, vector);
    if (mDomainType != cphd::DomainType::TOA)
    {
        throw except::Exception(Ctxt("Invalid DeltaTOA0."));
    }
    return mData[channel][vector].toaParameters->deltaTOA0;
}

double VBM::getTOASS(size_t channel, size_t vector) const
{
    verifyChannelVector(channel, vector);
    if (mDomainType != cphd::DomainType::TOA)
    {
        throw except::Exception(Ctxt("Invalid TOA_SS."));
    }
    return mData[channel][vector].toaParameters->toaSS;
}

void VBM::setTxTime(double value, size_t channel, size_t vector)
{
    verifyChannelVector(channel, vector);
    mData[channel][vector].txTime = value;
}

void VBM::setTxPos(const cphd::Vector3& value, size_t channel, size_t vector)
{
    verifyChannelVector(channel, vector);
    mData[channel][vector].txPos = value;
}

void VBM::setRcvTime(double value, size_t channel, size_t vector)
{
    verifyChannelVector(channel, vector);
    mData[channel][vector].rcvTime = value;
}

void VBM::setRcvPos(const cphd::Vector3& value, size_t channel, size_t vector)
{
    verifyChannelVector(channel, vector);
    mData[channel][vector].rcvPos = value;
}

void VBM::setSRPTime(double value, size_t channel, size_t vector)
{
    verifyChannelVector(channel, vector);
    if (!mSRPTimeEnabled)
    {
        throw except::Exception(Ctxt("Invalid SRPTime."));
    }
    mData[channel][vector].srpTime = value;
}

void VBM::setSRPPos(const cphd::Vector3& value, size_t channel, size_t vector)
{
    verifyChannelVector(channel, vector);
    mData[channel][vector].srpPos = value;
}

void VBM::setTropoSRP(double value, size_t channel, size_t vector)
{
    verifyChannelVector(channel, vector);
    if (!mTropoSRPEnabled)
    {
        throw except::Exception(Ctxt("Invalid TropoSRP."));
    }
    mData[channel][vector].tropoSrp = value;
}

void VBM::setAmpSF(double value, size_t channel, size_t vector)
{
    verifyChannelVector(channel, vector);
    if (!mAmpSFEnabled)
    {
        throw except::Exception(Ctxt("Invalid AmpSF."));
    }
    mData[channel][vector].ampSF = value;
}

void VBM::setFx0(double value, size_t channel, size_t vector)
{
    verifyChannelVector(channel, vector);
    if (mDomainType != cphd::DomainType::FX)
    {
        throw except::Exception(Ctxt("Invalid Fx0."));
    }
    mData[channel][vector].frequencyParameters->fx0 = value;
}

void VBM::setFxSS(double value, size_t channel, size_t vector)
{
    verifyChannelVector(channel, vector);
    if (mDomainType != cphd::DomainType::FX)
    {
        throw except::Exception(Ctxt("Invalid FxSS."));
    }
    mData[channel][vector].frequencyParameters->fxSS = value;
}

void VBM::setFx1(double value, size_t channel, size_t vector)
{
    verifyChannelVector(channel, vector);
    if (mDomainType != cphd::DomainType::FX)
    {
        throw except::Exception(Ctxt("Invalid Fx1."));
    }
    mData[channel][vector].frequencyParameters->fx1 = value;
}

void VBM::setFx2(double value, size_t channel, size_t vector)
{
    verifyChannelVector(channel, vector);
    if (mDomainType != cphd::DomainType::FX)
    {
        throw except::Exception(Ctxt("Invalid Fx2."));
    }
    mData[channel][vector].frequencyParameters->fx2 = value;
}

void VBM::setDeltaTOA0(double value, size_t channel, size_t vector)
{
    verifyChannelVector(channel, vector);
    if (mDomainType != cphd::DomainType::TOA)
    {
        throw except::Exception(Ctxt("Invalid DeltaTOA0."));
    }
    mData[channel][vector].toaParameters->deltaTOA0 = value;
}

void VBM::setTOASS(double value, size_t channel, size_t vector)
{
    verifyChannelVector(channel, vector);
    if (mDomainType != cphd::DomainType::TOA)
    {
        throw except::Exception(Ctxt("Invalid TOA_SS."));
    }
    mData[channel][vector].toaParameters->toaSS = value;
}

void VBM::clearAmpSF()
{
    if (mAmpSFEnabled)
    {
        // Remove all the data corresponding to ampSF
        for (size_t ii = 0; ii < mData.size(); ++ii)
        {
            for (size_t jj = 0; jj < mData[ii].size(); ++jj)
            {
                mData[ii][jj].ampSF = six::Init::undefined<double>();
            }
        }

        mAmpSFEnabled = false;
        mNumBytesPerVector -= 8;
    }
}

void VBM::getVBMdata(size_t channel,
                     std::vector<sys::ubyte>& data) const
{
    verifyChannelVector(channel, 0);
    data.resize(getVBMsize(channel));
    std::fill(data.begin(), data.end(), 0);

    getVBMdata(channel, &data[0]);
}

void VBM::getVBMdata(size_t channel,
                     void* data) const
{
    verifyChannelVector(channel, 0);
    const size_t numBytes = getNumBytesVBP();
    sys::ubyte* ptr = static_cast<sys::ubyte*>(data);

    for (size_t ii = 0;
         ii < mData[channel].size();
         ++ii, ptr += numBytes)
    {
        mData[channel][ii].getData(ptr);
    }
}

size_t VBM::getVBMsize(size_t channel) const
{
    verifyChannelVector(channel, 0);
    return getNumBytesVBP() * mData[channel].size();
}

void VBM::updateVectorParameters(VectorParameters& vp) const
{
    //! These sized are defined in the CPHD spec. The size of
    //  Vector3 is set to 24 instead of sizeof(Vector3) in case
    //  attributes are ever added to it in the future.
    const size_t doubleSize = sizeof(double);
    const size_t vector3Size = sizeof(double) * 3;

    vp.txTime = doubleSize;
    vp.txPos = vector3Size;
    vp.rcvTime = doubleSize;
    vp.rcvPos = vector3Size;
    vp.srpTime = mSRPTimeEnabled ? doubleSize :
                                   six::Init::undefined<sys::Off_T>();
    vp.srpPos = vector3Size;
    vp.tropoSRP = mTropoSRPEnabled ? doubleSize :
                                     six::Init::undefined<sys::Off_T>();
    vp.ampSF = mAmpSFEnabled ? doubleSize :
                               six::Init::undefined<sys::Off_T>();
    if (mDomainType == cphd::DomainType::FX)
    {
        vp.toaParameters.reset();
        vp.fxParameters.reset(new FxParameters());
        vp.fxParameters->Fx0 = doubleSize;
        vp.fxParameters->FxSS = doubleSize;
        vp.fxParameters->Fx1 = doubleSize;
        vp.fxParameters->Fx2 = doubleSize;
    }
    else if (mDomainType == cphd::DomainType::TOA)
    {
        vp.fxParameters.reset();
        vp.toaParameters.reset(new cphd03::TOAParameters());
        vp.toaParameters->deltaTOA0 = doubleSize;
        vp.toaParameters->toaSS = doubleSize;
    }
}

sys::Off_T VBM::load(io::SeekableInputStream& inStream,
                     sys::Off_T startVBM,
                     sys::Off_T sizeVBM,
                     size_t numThreads)
{
    // Allocate the buffers
    size_t numBytesIn(0);

    // Compute the VBM size per channel (channels aren't necessarily the same size)
    for (size_t ii = 0; ii < mData.size(); ++ii)
    {
        numBytesIn += getVBMsize(ii);
    }

    if (numBytesIn != static_cast<size_t>(sizeVBM))
    {
        std::ostringstream oss;
        oss << "VBM::load: calculated VBM size(" << numBytesIn
            << ") != header VB_DATA_SIZE(" << sizeVBM << ")";
        throw except::Exception(Ctxt(oss.str()));
    }

    const bool swapToLittleEndian = !(sys::isBigEndianSystem());

    // Seek to start of VBM
    size_t totalBytesRead(0);
    inStream.seek(startVBM, io::Seekable::START);
    std::vector<sys::ubyte> data;
    const size_t numBytesPerVector = getNumBytesVBP();

    // Read the data for each channel
    for (size_t ii = 0; ii < mData.size(); ++ii)
    {
        data.resize(getVBMsize(ii));
        //std::vector<sys::ubyte>& data(mVBMdata[ii]);
        if (!data.empty())
        {
            sys::byte* const buf = reinterpret_cast<sys::byte*>(&data[0]);
            sys::SSize_T bytesThisRead = inStream.read(buf, data.size());
            if (bytesThisRead == io::InputStream::IS_EOF)
            {
                std::ostringstream oss;
                oss << "EOF reached during VBM read for channel " << (ii);
                throw except::Exception(Ctxt(oss.str()));
            }
            totalBytesRead += bytesThisRead;

            // Input CPHD is always Big Endian; swap to Little Endian if
            // necessary
            if (swapToLittleEndian)
            {
                cphd::byteSwap(buf,
                         sizeof(double),
                         data.size() / sizeof(double),
                         numThreads);
            }

            sys::byte* ptr = buf;
            for (size_t jj = 0; jj < mData[ii].size(); ++jj, ptr += numBytesPerVector)
            {
                mData[ii][jj].setData(ptr);
            }
        }
    }

    return totalBytesRead;
}

std::ostream& operator<< (std::ostream& os, const VBM& d)
{
    os << "VBM::" << "\n";

    if (d.mData.empty())
    {
        os << "  mData : (empty)\n";
    }
    else
    {
        for (size_t ii = 0; ii < d.mData.size(); ++ii)
        {
            os << "[" << ii << "] mVBMsize: " << d.getVBMsize(ii) << "\n";
        }

        for (size_t ii = 0; ii < d.mData.size(); ++ii)
        {
            if (d.mData[0].empty())
            {
                os << "[" << ii << "] mData: (empty)\n";
            }
            else
            {
                os << "[" << ii << "] mData: " << &d.mData[ii] << "\n";
            }
        }
    }

    return os;
}
}

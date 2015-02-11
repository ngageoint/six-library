/* =========================================================================
 * This file is part of cphd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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

#include <sstream>

#include <algs/ByteSwap.h>
#include <six/Init.h>
#include <cphd/VBM.h>

namespace cphd
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
        DomainType domainType) :
    txTime(0.0),
    txPos(0.0),
    rcvTime(0.0),
    rcvPos(0.0),
    srpTime(srpTimeEnabled ? 0.0 : six::Init::undefined<double>()),
    srpPos(0.0),
    tropoSrp(tropoSrpEnabled ? 0.0 : six::Init::undefined<double>()),
    ampSF(ampSFEnabled ? 0.0 : six::Init::undefined<double>()),
    frequencyParamters(domainType == DomainType::FX ?
            new FrequencyParameters() : NULL),
    toaParameters(domainType == DomainType::TOA ?
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

    if (frequencyParamters.get())
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
    double* doubleData = reinterpret_cast<double*>(data);
    size_t pos = 0;

    doubleData[pos++] = txTime;
    doubleData[pos++] = txPos[0];
    doubleData[pos++] = txPos[1];
    doubleData[pos++] = txPos[2];
    doubleData[pos++] = rcvTime;
    doubleData[pos++] = rcvPos[0];
    doubleData[pos++] = rcvPos[1];
    doubleData[pos++] = rcvPos[2];
    if (!six::Init::isUndefined<double>(srpTime))
    {
        doubleData[pos++] = srpTime;
    }
    doubleData[pos++] = srpPos[0];
    doubleData[pos++] = srpPos[1];
    doubleData[pos++] = srpPos[2];
    if (!six::Init::isUndefined<double>(tropoSrp))
    {
        doubleData[pos++] = tropoSrp;
    }
    if (!six::Init::isUndefined<double>(ampSF))
    {
        doubleData[pos++] = ampSF;
    }
    if (frequencyParamters.get())
    {
        doubleData[pos++] = frequencyParamters->fx0;
        doubleData[pos++] = frequencyParamters->fxSS;
        doubleData[pos++] = frequencyParamters->fx1;
        doubleData[pos++] = frequencyParamters->fx2;
    }
    else if (toaParameters.get())
    {
        doubleData[pos++] = toaParameters->deltaTOA0;
        doubleData[pos++] = toaParameters->toaSS;
    }
}

void VBM::VectorBasedParameters::setData(sys::byte* data)
{
    double* doubleData = reinterpret_cast<double*>(data);
    size_t pos = 0;

    txTime = doubleData[pos++];
    txPos[0] = doubleData[pos++];
    txPos[1] = doubleData[pos++];
    txPos[2] = doubleData[pos++];
    rcvTime = doubleData[pos++];
    rcvPos[0] = doubleData[pos++];
    rcvPos[1] = doubleData[pos++];
    rcvPos[2] = doubleData[pos++];
    if (!six::Init::isUndefined<double>(srpTime))
    {
        srpTime = doubleData[pos++];
    }
    srpPos[0] = doubleData[pos++];
    srpPos[1] = doubleData[pos++];
    srpPos[2] = doubleData[pos++];
    if (!six::Init::isUndefined<double>(tropoSrp))
    {
        tropoSrp = doubleData[pos++];
    }
    if (!six::Init::isUndefined<double>(ampSF))
    {
        ampSF = doubleData[pos++];
    }
    if (frequencyParamters.get())
    {
        frequencyParamters->fx0 = doubleData[pos++];
        frequencyParamters->fxSS = doubleData[pos++];
        frequencyParamters->fx1 = doubleData[pos++];
        frequencyParamters->fx2 = doubleData[pos++];
    }
    else if (toaParameters.get())
    {
        toaParameters->deltaTOA0 = doubleData[pos++];
        toaParameters->toaSS = doubleData[pos++];
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

    if (frequencyParamters.get() && other.frequencyParamters.get())
    {
        if (*frequencyParamters != *other.frequencyParamters)
        {
            return false;
        }
    }
    else if (frequencyParamters.get() || other.frequencyParamters.get())
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
    mDomainType(DomainType::NOT_SET),
    mNumBytesPerVector(0)
{
}

VBM::VBM(const Data& data, const VectorParameters &vp) :
    mSRPTimeEnabled(vp.srpTimeOffset() > 0),
    mTropoSRPEnabled(vp.tropoSRPOffset() > 0),
    mAmpSFEnabled(vp.ampSFOffset() > 0),
    mDomainType(vp.fxParameters.get() ? DomainType::FX :
            vp.toaParameters.get() ? DomainType::TOA : DomainType::NOT_SET),
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
         DomainType domainType) :
    mSRPTimeEnabled(srpTimeEnabled),
    mTropoSRPEnabled(tropoSrpEnabled),
    mAmpSFEnabled(ampSFEnabled),
    mDomainType(domainType),
    mNumBytesPerVector(0),
    mData(numChannels)
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

double VBM::getTxTime(size_t channel, size_t vector) const
{
    verifyChannelVector(channel, vector);
    return mData[channel][vector].txTime;
}

Vector3 VBM::getTxPos(size_t channel, size_t vector) const
{
    verifyChannelVector(channel, vector);
    return mData[channel][vector].txPos;
}

double VBM::getRcvTime(size_t channel, size_t vector) const
{
    verifyChannelVector(channel, vector);
    return mData[channel][vector].rcvTime;
}

Vector3 VBM::getRcvPos(size_t channel, size_t vector) const
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

Vector3 VBM::getSRPPos(size_t channel, size_t vector) const
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
    if (mDomainType != DomainType::FX)
    {
        throw except::Exception(Ctxt("Invalid Fx0."));
    }
    return mData[channel][vector].frequencyParamters->fx0;
}

double VBM::getFxSS(size_t channel, size_t vector) const
{
    verifyChannelVector(channel, vector);
    if (mDomainType != DomainType::FX)
    {
        throw except::Exception(Ctxt("Invalid FxSS."));
    }
    return mData[channel][vector].frequencyParamters->fxSS;
}

double VBM::getFx1(size_t channel, size_t vector) const
{
    verifyChannelVector(channel, vector);
    if (mDomainType != DomainType::FX)
    {
        throw except::Exception(Ctxt("Invalid Fx1."));
    }
    return mData[channel][vector].frequencyParamters->fx1;
}

double VBM::getFx2(size_t channel, size_t vector) const
{
    verifyChannelVector(channel, vector);
    if (mDomainType != DomainType::FX)
    {
        throw except::Exception(Ctxt("Invalid Fx2."));
    }
    return mData[channel][vector].frequencyParamters->fx2;
}

double VBM::getDeltaTOA0(size_t channel, size_t vector) const
{
    verifyChannelVector(channel, vector);
    if (mDomainType != DomainType::TOA)
    {
        throw except::Exception(Ctxt("Invalid DeltaTOA0."));
    }
    return mData[channel][vector].toaParameters->deltaTOA0;
}

double VBM::getTOASS(size_t channel, size_t vector) const
{
    verifyChannelVector(channel, vector);
    if (mDomainType != DomainType::TOA)
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

void VBM::setTxPos(const Vector3& value, size_t channel, size_t vector)
{
    verifyChannelVector(channel, vector);
    mData[channel][vector].txPos = value;
}

void VBM::setRcvTime(double value, size_t channel, size_t vector)
{
    verifyChannelVector(channel, vector);
    mData[channel][vector].rcvTime = value;
}

void VBM::setRcvPos(const Vector3& value, size_t channel, size_t vector)
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

void VBM::setSRPPos(const Vector3& value, size_t channel, size_t vector)
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
    if (mDomainType != DomainType::FX)
    {
        throw except::Exception(Ctxt("Invalid Fx0."));
    }
    mData[channel][vector].frequencyParamters->fx0 = value;
}

void VBM::setFxSS(double value, size_t channel, size_t vector)
{
    verifyChannelVector(channel, vector);
    if (mDomainType != DomainType::FX)
    {
        throw except::Exception(Ctxt("Invalid FxSS."));
    }
    mData[channel][vector].frequencyParamters->fxSS = value;
}

void VBM::setFx1(double value, size_t channel, size_t vector)
{
    verifyChannelVector(channel, vector);
    if (mDomainType != DomainType::FX)
    {
        throw except::Exception(Ctxt("Invalid Fx1."));
    }
    mData[channel][vector].frequencyParamters->fx1 = value;
}

void VBM::setFx2(double value, size_t channel, size_t vector)
{
    verifyChannelVector(channel, vector);
    if (mDomainType != DomainType::FX)
    {
        throw except::Exception(Ctxt("Invalid Fx2."));
    }
    mData[channel][vector].frequencyParamters->fx2 = value;
}

void VBM::setDeltaTOA0(double value, size_t channel, size_t vector)
{
    verifyChannelVector(channel, vector);
    if (mDomainType != DomainType::TOA)
    {
        throw except::Exception(Ctxt("Invalid DeltaTOA0."));
    }
    mData[channel][vector].toaParameters->deltaTOA0 = value;
}

void VBM::setTOASS(double value, size_t channel, size_t vector)
{
    verifyChannelVector(channel, vector);
    if (mDomainType != DomainType::TOA)
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

    const size_t numBytes = getNumBytesVBP();
    sys::ubyte* ptr = &data[0];

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
    if (mDomainType == DomainType::FX)
    {
        vp.toaParameters.reset();
        vp.fxParameters.reset(new FxParameters());
        vp.fxParameters->Fx0 = doubleSize;
        vp.fxParameters->FxSS = doubleSize;
        vp.fxParameters->Fx1 = doubleSize;
        vp.fxParameters->Fx2 = doubleSize;
    }
    else if (mDomainType == DomainType::TOA)
    {
        vp.fxParameters.reset();
        vp.toaParameters.reset(new cphd::TOAParameters());
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
                algs::byteSwap(buf,
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

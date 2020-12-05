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

#include <ostream>
#include <vector>
#include <stddef.h>
#include <typeinfo>

#include <six/Init.h>
#include <sys/Conf.h>
#include <nitf/cstddef.h>

#include <cphd/Types.h>
#include <cphd/PVPBlock.h>
#include <cphd/Metadata.h>
#include <cphd/Utilities.h>

#include <sys/Bit.h>
namespace std
{
    using endian = sys::Endian;
}

namespace
{
// Set data from data block into data struct
template <typename T> inline void setData(const unsigned char* data,
                    T& dest)
{
    memcpy(&dest, data, sizeof(T));
}
template <typename T> inline void setData(const std::byte* data,
    T& dest)
{
    setData(reinterpret_cast<const unsigned char*>(data), dest);
}

inline void setData(const std::byte* data,
                    cphd::Vector3& dest)
{
    setData(data, dest[0]);
    setData(data + sizeof(double), dest[1]);
    setData(data + 2*sizeof(double), dest[2]);
}

// Get data from data struct and put into data block
template <typename T> inline void getData(std::byte* dest,
                    T value)
{
    memcpy(dest, &value, sizeof(T));
}

template <typename T> inline void getData(std::byte* dest,
                    T* value, size_t size)
{
    memcpy(dest, value, size);
}

inline void getData(std::byte* dest,
                    const cphd::Vector3& value)
{
    getData(dest, value[0]);
    getData(dest + sizeof(double), value[1]);
    getData(dest + 2*sizeof(double), value[2]);
}
}

namespace cphd
{

PVPBlock::PVPSet::PVPSet() :
    txTime(six::Init::undefined<double>()),
    txPos(six::Init::undefined<Vector3>()),
    txVel(six::Init::undefined<Vector3>()),
    rcvTime(six::Init::undefined<double>()),
    rcvPos(six::Init::undefined<Vector3>()),
    rcvVel(six::Init::undefined<Vector3>()),
    srpPos(six::Init::undefined<Vector3>()),
    aFDOP(six::Init::undefined<double>()),
    aFRR1(six::Init::undefined<double>()),
    aFRR2(six::Init::undefined<double>()),
    fx1(six::Init::undefined<double>()),
    fx2(six::Init::undefined<double>()),
    toa1(six::Init::undefined<double>()),
    toa2(six::Init::undefined<double>()),
    tdTropoSRP(six::Init::undefined<double>()),
    sc0(six::Init::undefined<double>()),
    scss(six::Init::undefined<double>())
{
}

void PVPBlock::PVPSet::write(const PVPBlock& pvpBlock, const Pvp& p, const std::byte* input)
{
    ::setData(input + p.txTime.getByteOffset(), txTime);
    ::setData(input + p.txPos.getByteOffset(), txPos);
    ::setData(input + p.txVel.getByteOffset(), txVel);
    ::setData(input + p.rcvTime.getByteOffset(), rcvTime);
    ::setData(input + p.rcvPos.getByteOffset(), rcvPos);
    ::setData(input + p.rcvVel.getByteOffset(), rcvVel);
    ::setData(input + p.srpPos.getByteOffset(), srpPos);
    ::setData(input + p.aFDOP.getByteOffset(), aFDOP);
    ::setData(input + p.aFRR1.getByteOffset(), aFRR1);
    ::setData(input + p.aFRR2.getByteOffset(), aFRR2);
    ::setData(input + p.fx1.getByteOffset(), fx1);
    ::setData(input + p.fx2.getByteOffset(), fx2);
    ::setData(input + p.toa1.getByteOffset(), toa1);
    ::setData(input + p.toa2.getByteOffset(), toa2);
    ::setData(input + p.tdTropoSRP.getByteOffset(), tdTropoSRP);
    ::setData(input + p.sc0.getByteOffset(), sc0);
    ::setData(input + p.scss.getByteOffset(), scss);

    if (pvpBlock.hasAmpSF())
    {
        ampSF.reset(new double());
        ::setData(input + p.ampSF.getByteOffset(), *ampSF);
    }
    if (pvpBlock.hasFxN1())
    {
        fxN1.reset(new double());
        ::setData(input + p.fxN1.getByteOffset(), *fxN1);
    }
    if (pvpBlock.hasFxN2())
    {
        fxN2.reset(new double());
        ::setData(input + p.fxN2.getByteOffset(), *fxN2);
    }
    if (pvpBlock.hasToaE1())
    {
        toaE1.reset(new double());
        ::setData(input + p.toaE1.getByteOffset(), *toaE1);
    }
    if (pvpBlock.hasToaE2())
    {
        toaE2.reset(new double());
        ::setData(input + p.toaE2.getByteOffset(), *toaE2);
    }
    if (pvpBlock.hasTDIonoSRP())
    {
        tdIonoSRP.reset(new double());
        ::setData(input + p.tdIonoSRP.getByteOffset(), *tdIonoSRP);
    }
    if (pvpBlock.hasSignal())
    {
        signal.reset(new double());
        ::setData(input + p.signal.getByteOffset(), *signal);
    }
    for (auto it = p.addedPVP.begin(); it != p.addedPVP.end(); ++it)
    {
        if (it->second.getFormat() == "F4" || it->second.getFormat() == "F8")
        {
            double val;
            ::setData(input + it->second.getByteOffset(), val);
            addedPVP[it->first] = six::Parameter();
            addedPVP.find(it->first)->second.setValue(val);
        }
        else if (it->second.getFormat() == "U1" || it->second.getFormat() == "U2" ||
                 it->second.getFormat() == "U4" || it->second.getFormat() == "U8")
        {
            unsigned int val;
            ::setData(input + it->second.getByteOffset(), val);
            addedPVP[it->first] = six::Parameter();
            addedPVP.find(it->first)->second.setValue(val);
        }
        else if (it->second.getFormat() == "I1" || it->second.getFormat() == "I2" ||
                 it->second.getFormat() == "I4" || it->second.getFormat() == "I8")
        {
            int val;
            ::setData(input + it->second.getByteOffset(), val);
            addedPVP[it->first] = six::Parameter();
            addedPVP.find(it->first)->second.setValue(val);
        }
        else if (it->second.getFormat() == "CI2" || it->second.getFormat() == "CI4" ||
                 it->second.getFormat() == "CI8" || it->second.getFormat() == "CI16")
        {
            std::complex<int> val;
            ::setData(input + it->second.getByteOffset(), val);
            addedPVP[it->first] = six::Parameter();
            addedPVP.find(it->first)->second.setValue(val);
        }
        else if (it->second.getFormat() == "CF8" || it->second.getFormat() == "CF16")
        {
            std::complex<double> val;
            ::setData(input + it->second.getByteOffset(), val);
            addedPVP[it->first] = six::Parameter();
            addedPVP.find(it->first)->second.setValue(val);
        }
        else
        {
            const auto pVal = input + it->second.getByteOffset();
            std::string val;
            val.assign(reinterpret_cast<const char*>(pVal), it->second.getByteSize());
            addedPVP[it->first] = six::Parameter();
            addedPVP.find(it->first)->second.setValue(val);
        }
    }
}

void PVPBlock::PVPSet::read(const Pvp& p, std::byte* dest) const
{
    ::getData(dest + p.txTime.getByteOffset(), txTime);
    ::getData(dest + p.txPos.getByteOffset(), txPos);
    ::getData(dest + p.txVel.getByteOffset(), txVel);
    ::getData(dest + p.rcvTime.getByteOffset(), rcvTime);
    ::getData(dest + p.rcvPos.getByteOffset(), rcvPos);
    ::getData(dest + p.rcvVel.getByteOffset(), rcvVel);
    ::getData(dest + p.srpPos.getByteOffset(), srpPos);
    ::getData(dest + p.aFDOP.getByteOffset(), aFDOP);
    ::getData(dest + p.aFRR1.getByteOffset(), aFRR1);
    ::getData(dest + p.aFRR2.getByteOffset(), aFRR2);
    ::getData(dest + p.fx1.getByteOffset(), fx1);
    ::getData(dest + p.fx2.getByteOffset(), fx2);
    ::getData(dest + p.toa1.getByteOffset(), toa1);
    ::getData(dest + p.toa2.getByteOffset(), toa2);
    ::getData(dest + p.tdTropoSRP.getByteOffset(), tdTropoSRP);
    ::getData(dest + p.sc0.getByteOffset(), sc0);
    ::getData(dest + p.scss.getByteOffset(), scss);

    if (ampSF.get())
    {
        ::getData(dest + p.ampSF.getByteOffset(), *ampSF);
    }
    if (fxN1.get())
    {
        ::getData(dest + p.fxN1.getByteOffset(), *fxN1);
    }
    if (fxN2.get())
    {
        ::getData(dest + p.fxN2.getByteOffset(), *fxN2);
    }
    if (toaE1.get())
    {
        ::getData(dest + p.toaE1.getByteOffset(), *toaE1);
    }
    if (toaE2.get())
    {
        ::getData(dest + p.toaE2.getByteOffset(), *toaE2);
    }
    if (tdIonoSRP.get())
    {
        ::getData(dest + p.tdIonoSRP.getByteOffset(), *tdIonoSRP);
    }
    if (signal.get())
    {
        ::getData(dest + p.signal.getByteOffset(), *signal);
    }
    if (addedPVP.size() != p.addedPVP.size())
    {
        throw except::Exception(Ctxt(
            "Incorrect number of additional parameters instantiated"));
    }
    for (auto it = p.addedPVP.begin(); it != p.addedPVP.end(); ++it)
    {
        if (it->second.getFormat() == "F4" || it->second.getFormat() == "F8")
        {
            ::getData(dest + it->second.getByteOffset(), static_cast<double>(addedPVP.find(it->first)->second));
        }
        else if (it->second.getFormat() == "U1" || it->second.getFormat() == "U2" ||
                 it->second.getFormat() == "U4" || it->second.getFormat() == "U8")
        {
            ::getData(dest + it->second.getByteOffset(), static_cast<unsigned int>(addedPVP.find(it->first)->second));
        }
        else if (it->second.getFormat() == "I1" || it->second.getFormat() == "I2" ||
                 it->second.getFormat() == "I4" || it->second.getFormat() == "I8")
        {
            ::getData(dest + it->second.getByteOffset(), static_cast<int>(addedPVP.find(it->first)->second));
        }
        else if (it->second.getFormat() == "CI2" || it->second.getFormat() == "CI4" ||
                 it->second.getFormat() == "CI8" || it->second.getFormat() == "CI16")
        {
            ::getData(dest + it->second.getByteOffset(), addedPVP.find(it->first)->second.getComplex<int>());
        }
        else if (it->second.getFormat() == "CF8" || it->second.getFormat() == "CF16")
        {
            ::getData(dest + it->second.getByteOffset(), addedPVP.find(it->first)->second.getComplex<double>());
        }
        else
        {
            ::getData(dest + it->second.getByteOffset(), addedPVP.find(it->first)->second.str().c_str(), it->second.getByteSize());
        }
    }
}

/*
 * Initialize PVP Array with a data object
 */
PVPBlock::PVPBlock(const Pvp& p, const Data& d) :
    mAmpSFEnabled(!six::Init::isUndefined<size_t>(p.ampSF.getOffset())),
    mFxN1Enabled(!six::Init::isUndefined<size_t>(p.fxN1.getOffset())),
    mFxN2Enabled(!six::Init::isUndefined<size_t>(p.fxN2.getOffset())),
    mToaE1Enabled(!six::Init::isUndefined<size_t>(p.toaE1.getOffset())),
    mToaE2Enabled(!six::Init::isUndefined<size_t>(p.toaE2.getOffset())),
    mTDIonoSRPEnabled(!six::Init::isUndefined<size_t>(p.tdIonoSRP.getOffset())),
    mSignalEnabled(!six::Init::isUndefined<size_t>(p.signal.getOffset()))
{
    mPvp = p;
    mNumBytesPerVector = d.getNumBytesPVPSet();
    mData.resize(d.getNumChannels());
    for (size_t ii = 0; ii < d.getNumChannels(); ++ii)
    {
        mData[ii].resize(d.getNumVectors(ii));
    }
    size_t calculateBytesPerVector = mPvp.getReqSetSize()*sizeof(double);
    if (six::Init::isUndefined<size_t>(mNumBytesPerVector) ||
        calculateBytesPerVector > mNumBytesPerVector)
    {
        std::ostringstream oss;
        oss << "PVP size specified in metadata: " << mNumBytesPerVector
            << " does not match PVP size calculated: " << calculateBytesPerVector;
        throw except::Exception(oss.str());
    }
}

/*
 * Initialize PVP Array with custom parameters
 */
PVPBlock::PVPBlock(size_t numChannels,
                   const std::vector<size_t>& numVectors,
                   const Pvp& p) :
    mNumBytesPerVector(0),
    mPvp(p),
    mAmpSFEnabled(!six::Init::isUndefined<size_t>(p.ampSF.getOffset())),
    mFxN1Enabled(!six::Init::isUndefined<size_t>(p.fxN1.getOffset())),
    mFxN2Enabled(!six::Init::isUndefined<size_t>(p.fxN2.getOffset())),
    mToaE1Enabled(!six::Init::isUndefined<size_t>(p.toaE1.getOffset())),
    mToaE2Enabled(!six::Init::isUndefined<size_t>(p.toaE2.getOffset())),
    mTDIonoSRPEnabled(!six::Init::isUndefined<size_t>(p.tdIonoSRP.getOffset())),
    mSignalEnabled(!six::Init::isUndefined<size_t>(p.signal.getOffset()))
{
    mData.resize(numChannels);
    if(numChannels != numVectors.size())
    {
        throw except::Exception(Ctxt(
                "number of vector dims provided does not match number of channels"));
    }
    for (size_t ii = 0; ii < numChannels; ++ii)
    {
        mData[ii].resize(numVectors[ii]);
    }
    size_t calculateBytesPerVector = mPvp.getReqSetSize()*sizeof(double);
    if (six::Init::isUndefined<size_t>(mNumBytesPerVector) ||
        calculateBytesPerVector > mNumBytesPerVector)
    {
        mNumBytesPerVector = calculateBytesPerVector;
    }
}

PVPBlock::PVPBlock(size_t numChannels,
                   const std::vector<size_t>& numVectors,
                   const Pvp& pvp,
                   const std::vector<const void*>& data) :
    PVPBlock(numChannels, numVectors, pvp)
{
    if (data.size() != numChannels)
    {
        std::ostringstream msg;
        msg << "<" << numChannels << "> channels specified, "
            << "but `data` argument has <" << data.size() << "> channels";
        throw except::Exception(Ctxt(msg.str()));
    }

    for (size_t channel = 0; channel < numChannels; ++channel)
    {
        const std::byte* buf = static_cast<const std::byte*>(data[channel]);

        for (size_t vector = 0; vector < numVectors[channel]; ++vector)
        {
            mData[channel][vector].write(*this, mPvp, buf);
            buf += mPvp.sizeInBytes();
        }
    }
}

size_t PVPBlock::getNumBytesPVPSet() const
{
    return mNumBytesPerVector;
}

void PVPBlock::verifyChannelVector(size_t channel, size_t vector) const
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

size_t PVPBlock::getPVPsize(size_t channel) const
{
    verifyChannelVector(channel, 0);
    return getNumBytesPVPSet() * mData[channel].size();
}

void PVPBlock::getPVPdata(size_t channel,
                          std::vector<std::byte>& data) const
{
    verifyChannelVector(channel, 0);
    data.resize(getPVPsize(channel));
    std::fill(data.begin(), data.end(), static_cast<std::byte>(0));

    getPVPdata(channel, data.data());
}

void PVPBlock::getPVPdata(size_t channel,
                          void* data) const
{
    verifyChannelVector(channel, 0);
    const size_t numBytes = getNumBytesPVPSet();
    std::byte* ptr = static_cast<std::byte*>(data);

    for (size_t ii = 0;
         ii < mData[channel].size();
         ++ii, ptr += numBytes)
    {
        mData[channel][ii].read(mPvp, ptr);
    }
}

int64_t PVPBlock::load(io::SeekableInputStream& inStream,
                     int64_t startPVP,
                     int64_t sizePVP,
                     size_t numThreads)
{
    // Allocate the buffers
    size_t numBytesIn(0);

    // Compute the PVPBlock size per channel
    // (channels aren't necessarily the same size)
    for (size_t ii = 0; ii < mData.size(); ++ii)
    {
        numBytesIn += getPVPsize(ii);
    }

    if (numBytesIn != static_cast<size_t>(sizePVP))
    {
        std::ostringstream oss;
        oss << "PVPBlock::load: calculated PVP size(" << numBytesIn
            << ") != header PVP_DATA_SIZE(" << sizePVP << ")";
        throw except::Exception(Ctxt(oss.str()));
    }

    const bool swapToLittleEndian = std::endian::native == std::endian::little;

    // Seek to start of PVPBlock
    size_t totalBytesRead(0);
    inStream.seek(startPVP, io::Seekable::START);
    std::vector<std::byte> readBuf;
    const size_t numBytesPerVector = getNumBytesPVPSet();

    // Read the data for each channel
    for (size_t ii = 0; ii < mData.size(); ++ii)
    {
        readBuf.resize(getPVPsize(ii));
        if (!readBuf.empty())
        {
            auto const buf = readBuf.data();
            ptrdiff_t bytesThisRead = inStream.read(buf, readBuf.size());
            if (bytesThisRead == io::InputStream::IS_EOF)
            {
                std::ostringstream oss;
                oss << "EOF reached during PVP read for channel " << (ii);
                throw except::Exception(Ctxt(oss.str()));
            }
            totalBytesRead += bytesThisRead;

            // Input CPHD is always Big Endian; swap to Little Endian if
            // necessary
            if (swapToLittleEndian)
            {
                byteSwap(buf,
                         sizeof(double),
                         readBuf.size() / sizeof(double),
                         numThreads);
            }

            std::byte* ptr = buf;
            for (size_t jj = 0; jj < mData[ii].size(); ++jj, ptr += numBytesPerVector)
            {
                mData[ii][jj].write(*this, mPvp, ptr);
            }
        }
    }
    return totalBytesRead;
}

double PVPBlock::getTxTime(size_t channel, size_t set) const
{
    verifyChannelVector(channel, set);
    return mData[channel][set].txTime;
}

Vector3 PVPBlock::getTxPos(size_t channel, size_t set) const
{
    verifyChannelVector(channel, set);
    return mData[channel][set].txPos;
}

Vector3 PVPBlock::getTxVel(size_t channel, size_t set) const
{
    verifyChannelVector(channel, set);
    return mData[channel][set].txVel;
}

double PVPBlock::getRcvTime(size_t channel, size_t set) const
{
    verifyChannelVector(channel, set);
    return mData[channel][set].rcvTime;
}

Vector3 PVPBlock::getRcvPos(size_t channel, size_t set) const
{
    verifyChannelVector(channel, set);
    return mData[channel][set].rcvPos;
}

Vector3 PVPBlock::getRcvVel(size_t channel, size_t set) const
{
    verifyChannelVector(channel, set);
    return mData[channel][set].rcvVel;
}

Vector3 PVPBlock::getSRPPos(size_t channel, size_t set) const
{
    verifyChannelVector(channel, set);
    return mData[channel][set].srpPos;
}

double PVPBlock::getaFDOP(size_t channel, size_t set) const
{
    verifyChannelVector(channel, set);
    return mData[channel][set].aFDOP;
}

double PVPBlock::getaFRR1(size_t channel, size_t set) const
{
    verifyChannelVector(channel, set);
    return mData[channel][set].aFRR1;
}

double PVPBlock::getaFRR2(size_t channel, size_t set) const
{
    verifyChannelVector(channel, set);
    return mData[channel][set].aFRR2;
}

double PVPBlock::getFx1(size_t channel, size_t set) const
{
    verifyChannelVector(channel, set);
    return mData[channel][set].fx1;
}

double PVPBlock::getFx2(size_t channel, size_t set) const
{
    verifyChannelVector(channel, set);
    return mData[channel][set].fx2;
}

double PVPBlock::getTOA1(size_t channel, size_t set) const
{
    verifyChannelVector(channel, set);
    return mData[channel][set].toa1;
}

double PVPBlock::getTOA2(size_t channel, size_t set) const
{
    verifyChannelVector(channel, set);
    return mData[channel][set].toa2;
}

double PVPBlock::getTdTropoSRP(size_t channel, size_t set) const
{
    verifyChannelVector(channel, set);
    return mData[channel][set].tdTropoSRP;
}

double PVPBlock::getSC0(size_t channel, size_t set) const
{
    verifyChannelVector(channel, set);
    return mData[channel][set].sc0;
}

double PVPBlock::getSCSS(size_t channel, size_t set) const
{
    verifyChannelVector(channel, set);
    return mData[channel][set].scss;
}

double PVPBlock::getAmpSF(size_t channel, size_t set) const
{
    verifyChannelVector(channel, set);
    if (mData[channel][set].ampSF.get())
    {
        return *mData[channel][set].ampSF;
    }
    throw except::Exception(Ctxt(
                    "Parameter was not set"));
}

double PVPBlock::getFxN1(size_t channel, size_t set) const
{
    verifyChannelVector(channel, set);
    if (mData[channel][set].fxN1.get())
    {
        return *mData[channel][set].fxN1;
    }
    throw except::Exception(Ctxt(
                    "Parameter was not set"));
}

double PVPBlock::getFxN2(size_t channel, size_t set) const
{
    verifyChannelVector(channel, set);
    if (mData[channel][set].fxN2.get())
    {
        return *mData[channel][set].fxN2;
    }
    throw except::Exception(Ctxt(
                    "Parameter was not set"));
}

double PVPBlock::getTOAE1(size_t channel, size_t set) const
{
    verifyChannelVector(channel, set);
    if (mData[channel][set].toaE1.get())
    {
        return *mData[channel][set].toaE1;
    }
    throw except::Exception(Ctxt(
                    "Parameter was not set"));
}

double PVPBlock::getTOAE2(size_t channel, size_t set) const
{
    verifyChannelVector(channel, set);
    if (mData[channel][set].toaE2.get())
    {
        return *mData[channel][set].toaE2;
    }
    throw except::Exception(Ctxt(
                    "Parameter was not set"));
}

double PVPBlock::getTdIonoSRP(size_t channel, size_t set) const
{
    verifyChannelVector(channel, set);
    if (mData[channel][set].tdIonoSRP.get())
    {
        return *mData[channel][set].tdIonoSRP;
    }
    throw except::Exception(Ctxt(
                    "Parameter was not set"));
}

double PVPBlock::getSignal(size_t channel, size_t set) const
{
    verifyChannelVector(channel, set);
    if (mData[channel][set].signal.get())
    {
        return *mData[channel][set].signal;
    }
    throw except::Exception(Ctxt(
                    "Parameter was not set"));
}

void PVPBlock::setTxTime(double value, size_t channel, size_t vector)
{
    verifyChannelVector(channel, vector);
    mData[channel][vector].txTime = value;
}

void PVPBlock::setTxPos(const cphd::Vector3& value, size_t channel, size_t vector)
{
    verifyChannelVector(channel, vector);
    mData[channel][vector].txPos = value;
}

void PVPBlock::setTxVel(const cphd::Vector3& value, size_t channel, size_t vector)
{
    verifyChannelVector(channel, vector);
    mData[channel][vector].txVel = value;
}

void PVPBlock::setRcvTime(double value, size_t channel, size_t vector)
{
    verifyChannelVector(channel, vector);
    mData[channel][vector].rcvTime = value;
}

void PVPBlock::setRcvPos(const cphd::Vector3& value, size_t channel, size_t vector)
{
    verifyChannelVector(channel, vector);
    mData[channel][vector].rcvPos = value;
}

void PVPBlock::setRcvVel(const cphd::Vector3& value, size_t channel, size_t vector)
{
    verifyChannelVector(channel, vector);
    mData[channel][vector].rcvVel = value;
}

void PVPBlock::setSRPPos(const cphd::Vector3& value, size_t channel, size_t vector)
{
    verifyChannelVector(channel, vector);
    mData[channel][vector].srpPos = value;
}

void PVPBlock::setaFDOP(double value, size_t channel, size_t vector)
{
    verifyChannelVector(channel, vector);
    mData[channel][vector].aFDOP = value;
}

void PVPBlock::setaFRR1(double value, size_t channel, size_t vector)
{
    verifyChannelVector(channel, vector);
    mData[channel][vector].aFRR1 = value;
}

void PVPBlock::setaFRR2(double value, size_t channel, size_t vector)
{
    verifyChannelVector(channel, vector);
    mData[channel][vector].aFRR2 = value;
}

void PVPBlock::setFx1(double value, size_t channel, size_t vector)
{
    verifyChannelVector(channel, vector);
    mData[channel][vector].fx1 = value;
}

void PVPBlock::setFx2(double value, size_t channel, size_t vector)
{
    verifyChannelVector(channel, vector);
    mData[channel][vector].fx2 = value;
}

void PVPBlock::setTOA1(double value, size_t channel, size_t vector)
{
    verifyChannelVector(channel, vector);
    mData[channel][vector].toa1 = value;
}

void PVPBlock::setTOA2(double value, size_t channel, size_t vector)
{
    verifyChannelVector(channel, vector);
    mData[channel][vector].toa2 = value;
}

void PVPBlock::setTdTropoSRP(double value, size_t channel, size_t vector)
{
    verifyChannelVector(channel, vector);
    mData[channel][vector].tdTropoSRP = value;
}

void PVPBlock::setSC0(double value, size_t channel, size_t vector)
{
    verifyChannelVector(channel, vector);
    mData[channel][vector].sc0 = value;
}

void PVPBlock::setSCSS(double value, size_t channel, size_t vector)
{
    verifyChannelVector(channel, vector);
    mData[channel][vector].scss = value;
}

void PVPBlock::setAmpSF(double value, size_t channel, size_t vector)
{
    verifyChannelVector(channel, vector);
    if (hasAmpSF())
    {
        mData[channel][vector].ampSF.reset(new double(value));
        return;
    }
    throw except::Exception(Ctxt(
                            "Parameter was not specified in XML"));
}

void PVPBlock::setFxN1(double value, size_t channel, size_t vector)
{
    verifyChannelVector(channel, vector);
    if (hasFxN1())
    {
        mData[channel][vector].fxN1.reset(new double(value));
        return;
    }
    throw except::Exception(Ctxt(
                            "Parameter was not specified in XML"));
}

void PVPBlock::setFxN2(double value, size_t channel, size_t vector)
{
    verifyChannelVector(channel, vector);
    if (hasFxN2())
    {
        mData[channel][vector].fxN2.reset(new double(value));
        return;
    }
    throw except::Exception(Ctxt(
                            "Parameter was not specified in XML"));
}

void PVPBlock::setTOAE1(double value, size_t channel, size_t vector)
{
    verifyChannelVector(channel, vector);
    if (hasToaE1())
    {
        mData[channel][vector].toaE1.reset(new double(value));
        return;
    }
    throw except::Exception(Ctxt(
                            "Parameter was not specified in XML"));
}

void PVPBlock::setTOAE2(double value, size_t channel, size_t vector)
{
    verifyChannelVector(channel, vector);
    if (hasToaE2())
    {
        mData[channel][vector].toaE2.reset(new double(value));
        return;
    }
    throw except::Exception(Ctxt(
                            "Parameter was not specified in XML"));
}

void PVPBlock::setTdIonoSRP(double value, size_t channel, size_t vector)
{
    verifyChannelVector(channel, vector);
    if (hasTDIonoSRP())
    {
        mData[channel][vector].tdIonoSRP.reset(new double(value));
        return;
    }
    throw except::Exception(Ctxt(
                            "Parameter was not specified in XML"));
}

void PVPBlock::setSignal(double value, size_t channel, size_t vector)
{
    verifyChannelVector(channel, vector);
    if (hasSignal())
    {
        mData[channel][vector].signal.reset(new double(value));
        return;
    }
    throw except::Exception(Ctxt(
                            "Parameter was not specified in XML"));
}

std::ostream& operator<< (std::ostream& os, const PVPBlock::PVPSet& p)
{
    os << "  TxTime         : " << p.txTime << "\n"
        << "  TxPos         : " << p.txPos << "\n"
        << "  TxVel         : " << p.txVel << "\n"
        << "  RcvTime       : " << p.rcvTime << "\n"
        << "  RcvPos        : " << p.rcvPos << "\n"
        << "  RcvVel        : " << p.rcvVel << "\n"
        << "  SRPPos        : " << p.srpPos << "\n"
        << "  aFDOP         : " << p.aFDOP << "\n"
        << "  aFRR1         : " << p.aFRR1 << "\n"
        << "  aFRR2         : " << p.aFRR2 << "\n"
        << "  Fx1           : " << p.fx1 << "\n"
        << "  Fx2           : " << p.fx2 << "\n"
        << "  TOA1          : " << p.toa1 << "\n"
        << "  TOA2          : " << p.toa2 << "\n"
        << "  TdTropoSRP    : " << p.tdTropoSRP << "\n"
        << "  SC0           : " << p.sc0 << "\n"
        << "  SCSS          : " << p.scss << "\n";

    if (p.ampSF.get())
    {
        os << "  AmpSF         : " << *p.ampSF << "\n";
    }
    if (p.fxN1.get())
    {
        os << "  FxN1          : " << *p.fxN1 << "\n";
    }
    if (p.fxN2.get())
    {
        os << "  FxN2          : " << *p.fxN2 << "\n";
    }
    if (p.toaE1.get())
    {
        os << "  TOAE1         : " << *p.toaE1 << "\n";
    }
    if (p.toaE2.get())
    {
        os << "  TOAE2         : " << *p.toaE2 << "\n";
    }
    if (p.tdIonoSRP.get())
    {
        os << "  TdIonoSRP     : " << *p.tdIonoSRP << "\n";
    }
    if (p.signal.get())
    {
        os << "  SIGNAL     : " << *p.signal << "\n";
    }

    for (auto it = p.addedPVP.begin(); it != p.addedPVP.end(); ++it)
    {
        os << "  Additional Parameter : " << it->second.str() << "\n";
    }
    return os;
}


std::ostream& operator<< (std::ostream& os, const PVPBlock& p)
{
    os << "PVPBlock:: \n";

    if (p.mData.empty())
    {
        os << "  mData : (empty) : \n";
    }
    else
    {
        for (size_t ii = 0; ii < p.mData.size(); ++ii)
        {
            os << "[" << ii << "] mPVPsize: " << p.getPVPsize(ii) << "\n";
        }

        for (size_t ii = 0; ii < p.mData.size(); ++ii)
        {
            if (p.mData[ii].empty())
            {
                os << "[" << ii << "] mData: (empty)\n";
            }
            else
            {
                for (size_t jj = 0; jj < p.mData[ii].size(); ++jj)
                {
                    if (p.mData[ii].empty())
                    {
                        os << "[" << ii << "] [" << jj << "] mData: (empty)\n";
                    }
                    else {
                        os << "[" << ii << "] [" << jj << "] mData: " << p.mData[ii][jj] << "\n";
                    }
                }
            }
        }
    }
    return os;
}
}

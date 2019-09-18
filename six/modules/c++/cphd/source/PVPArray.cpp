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
#include <cphd/Types.h>
#include <cphd/PVPArray.h>
#include <cphd/Metadata.h>

namespace
{
// Set data from data block into data struct
template <typename T> inline void setData(const sys::byte* data,
                    T& dest)
{
    memcpy(&dest, data, sizeof(T));
}

inline void setData(const sys::byte* data,
                    cphd::Vector3& dest)
{
    setData(data, dest[0]);
    setData(data, dest[1]);
    setData(data, dest[2]);
}

// Get data from data struct and put into data block
template <typename T> inline void getData(sys::ubyte* dest,
                    T value)
{
    memcpy(dest, &value, sizeof(T));
}

inline void getData(sys::ubyte* dest,
                    const cphd::Vector3& value)
{
    getData(dest, value[0]);
    getData(dest, value[1]);
    getData(dest, value[2]);
}
}

namespace cphd
{

PVPArray::PVPSet::PVPSet() :
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

void PVPArray::PVPSet::write(Pvp& p, const sys::byte* input)
{
    ::setData(input + p.txPos.getOffset()*8, txTime);
    ::setData(input + p.txPos.getOffset()*8, txPos);
    ::setData(input + p.txVel.getOffset()*8, txVel);
    ::setData(input + p.rcvTime.getOffset()*8, rcvTime);
    ::setData(input + p.rcvPos.getOffset()*8, rcvPos);
    ::setData(input + p.rcvVel.getOffset()*8, rcvVel);
    ::setData(input + p.srpPos.getOffset()*8, srpPos);
    ::setData(input + p.aFDOP.getOffset()*8, aFDOP);
    ::setData(input + p.aFRR1.getOffset()*8, aFRR1);
    ::setData(input + p.aFRR2.getOffset()*8, aFRR2);
    ::setData(input + p.fx1.getOffset()*8, fx1);
    ::setData(input + p.fx2.getOffset()*8, fx2);
    ::setData(input + p.toa1.getOffset()*8, toa1);
    ::setData(input + p.toa2.getOffset()*8, toa2);
    ::setData(input + p.tdTropoSRP.getOffset()*8, tdTropoSRP);
    ::setData(input + p.sc0.getOffset()*8, sc0);
    ::setData(input + p.scss.getOffset()*8, scss);

    if (p.ampSF.get())
    {
        ampSF.reset(new double());
        ::setData(input + p.ampSF->getOffset()*8, *ampSF);
    }
    if (p.fxN1.get())
    {
        fxN1.reset(new double());
        ::setData(input + p.fxN1->getOffset()*8, *fxN1);
    }
    if (p.fxN2.get())
    {
        fxN2.reset(new double());
        ::setData(input + p.fxN2->getOffset()*8, *fxN2);
    }
    if (p.toaE1.get())
    {
        toaE1.reset(new double());
        ::setData(input + p.toaE1->getOffset()*8, *toaE1);
    }
    if (p.toaE2.get())
    {
        toaE2.reset(new double());
        ::setData(input + p.toaE2->getOffset()*8, *toaE2);
    }
    if (p.tdIonoSRP.get())
    {
        tdIonoSRP.reset(new double());
        ::setData(input + p.tdIonoSRP->getOffset()*8, *tdIonoSRP);
    }
    if (p.signal.get())
    {
        signal.reset(new double());
        ::setData(input + p.signal->getOffset()*8, *signal);
    }
    if (addedPVP.size() != p.addedPVP.size())
    {
        throw except::Exception(Ctxt(
            "Incorrect number of additional parameters instantiated"));
    }
    for (size_t ii = 0; ii < p.addedPVP.size(); ++ii)
    {
        std::string val;
        val.resize(p.addedPVP.size());
        ::setData(input + p.addedPVP[ii].getOffset()*8, val[0]);
        addedPVP[ii].setValue(val);
    }
}

void PVPArray::PVPSet::read(Pvp& p, sys::ubyte* dest) const
{
    ::getData(dest + p.txPos.getOffset()*8, txTime);
    ::getData(dest + p.txPos.getOffset()*8, txPos);
    ::getData(dest + p.txVel.getOffset()*8, txVel);
    ::getData(dest + p.rcvTime.getOffset()*8, rcvTime);
    ::getData(dest + p.rcvPos.getOffset()*8, rcvPos);
    ::getData(dest + p.rcvVel.getOffset()*8, rcvVel);
    ::getData(dest + p.srpPos.getOffset()*8, srpPos);
    ::getData(dest + p.aFDOP.getOffset()*8, aFDOP);
    ::getData(dest + p.aFRR1.getOffset()*8, aFRR1);
    ::getData(dest + p.aFRR2.getOffset()*8, aFRR2);
    ::getData(dest + p.fx1.getOffset()*8, fx1);
    ::getData(dest + p.fx2.getOffset()*8, fx2);
    ::getData(dest + p.toa1.getOffset()*8, toa1);
    ::getData(dest + p.toa2.getOffset()*8, toa2);
    ::getData(dest + p.tdTropoSRP.getOffset()*8, tdTropoSRP);
    ::getData(dest + p.sc0.getOffset()*8, sc0);
    ::getData(dest + p.scss.getOffset()*8, scss);

    if (ampSF.get())
    {
        ::getData(dest + p.ampSF->getOffset()*8, *ampSF);
    }
    if (fxN1.get())
    {
        ::getData(dest + p.fxN1->getOffset()*8, *fxN1);
    }
    if (fxN2.get())
    {
        ::getData(dest + p.fxN2->getOffset()*8, *fxN2);
    }
    if (toaE1.get())
    {
        ::getData(dest + p.toaE1->getOffset()*8, *toaE1);
    }
    if (toaE2.get())
    {
        ::getData(dest + p.toaE2->getOffset()*8, *toaE2);
    }
    if (tdIonoSRP.get())
    {
        ::getData(dest + p.tdIonoSRP->getOffset()*8, *tdIonoSRP);
    }
    if (signal.get())
    {
        ::getData(dest + p.signal->getOffset()*8, *signal);
    }
    if (addedPVP.size() != p.addedPVP.size())
    {
        throw except::Exception(Ctxt(
            "Incorrect number of additional parameters instantiated"));
    }
    for (size_t ii = 0; ii < p.addedPVP.size(); ++ii)
    {
        ::getData(dest + p.addedPVP[ii].getOffset()*8, addedPVP[ii].str());
    }
}

/*
 * Initialize PVP Array with a data object
 */
PVPArray::PVPArray(Data& d, Pvp& p)
{
    mNumBytesPerVector = d.getNumBytesPVP();
    mData.resize(d.getNumChannels());
    for (size_t ii = 0; ii < d.getNumChannels(); ++ii)
    {
        mData[ii].resize(d.channels[ii].getNumVectors());
        for (size_t jj = 0; jj < mData[ii].size(); ++jj)
        {
            mData[ii][jj].addedPVP.resize(p.addedPVP.size());
        }
    }
}

/*
 * Initialize PVP Array with custom parameters
 */
PVPArray::PVPArray(size_t numBytesPerVector,
                    size_t numChannels,
                    std::vector<size_t> numVectors,
                    size_t numAddedParams) :
    mNumBytesPerVector(numBytesPerVector)
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
        for (size_t jj = 0; jj < mData[ii].size(); ++jj)
        {
            mData[ii][jj].addedPVP.resize(numAddedParams);
        }

    }
}

void PVPArray::verifyChannelVector(size_t channel, size_t vector) const
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

size_t PVPArray::getPVPsize(size_t channel) const
{
    verifyChannelVector(channel, 0);
    return getNumBytesPVP() * mData[channel].size();
}


void PVPArray::getPVPdata(Pvp& p, size_t channel,
                         std::vector<sys::ubyte>& data) const
{
    verifyChannelVector(channel, 0);
    data.resize(getPVPsize(channel));
    std::fill(data.begin(), data.end(), 0);

    getPVPdata(p, channel, &data[0]);
}

void PVPArray::getPVPdata(Pvp& p, size_t channel,
                     void* data) const
{
    verifyChannelVector(channel, 0);
    const size_t numBytes = getNumBytesPVP();
    sys::ubyte* ptr = static_cast<sys::ubyte*>(data);

    for (size_t ii = 0;
         ii < mData[channel].size();
         ++ii, ptr += numBytes)
    {
        mData[channel][ii].read(p, ptr);
    }
}

sys::Off_T PVPArray::load(io::SeekableInputStream& inStream,
                     sys::Off_T startPVP,
                     sys::Off_T sizePVP,
                     size_t numThreads,
                     Pvp& p)
{
    // Allocate the buffers
    size_t numBytesIn(0);

    // Compute the PVPArray size per channel (channels aren't necessarily the same size)
    for (size_t ii = 0; ii < mData.size(); ++ii)
    {
        numBytesIn += getPVPsize(ii);
    }

    if (numBytesIn != static_cast<size_t>(sizePVP))
    {
        std::ostringstream oss;
        oss << "PVPArray::load: calculated PVP size(" << numBytesIn
            << ") != header PVP_DATA_SIZE(" << sizePVP << ")";
        throw except::Exception(Ctxt(oss.str()));
    }

    const bool swapToLittleEndian = !(sys::isBigEndianSystem());

    // Seek to start of PVPArray
    size_t totalBytesRead(0);
    inStream.seek(startPVP, io::Seekable::START);
    std::vector<sys::ubyte> readBuf;
    const size_t numBytesPerVector = getNumBytesPVP();

    // Read the data for each channel
    for (size_t ii = 0; ii < mData.size(); ++ii)
    {
        readBuf.resize(getPVPsize(ii));
        if (!readBuf.empty())
        {
            sys::byte* const buf = reinterpret_cast<sys::byte*>(&readBuf[0]);
            sys::SSize_T bytesThisRead = inStream.read(buf, readBuf.size());
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
                byteSwap(buf,
                         sizeof(double),
                         readBuf.size() / sizeof(double),
                         numThreads);
            }

            sys::byte* ptr = buf;
            for (size_t jj = 0; jj < mData[ii].size(); ++jj, ptr += numBytesPerVector)
            {
                mData[ii][jj].write(p, ptr);
            }
        }
    }

    return totalBytesRead;
}

bool PVPArray::isFormatStr(std::string format)
{
    const char* ptr = format.c_str();
    if(format.size() <= 3)
    {
        if (*ptr == 'S')
        {
            ++ptr;
            if(std::isdigit(*ptr) && std::isdigit(*(ptr+1)))
            {
                return true;
            }
        }
    }
    return false;
}


double PVPArray::getTxTime(size_t channel, size_t set)
{
    verifyChannelVector(channel, set);
    return mData[channel][set].txTime;
}

Vector3 PVPArray::getTxPos(size_t channel, size_t set)
{
    verifyChannelVector(channel, set);
    return mData[channel][set].txPos;
}

Vector3 PVPArray::getTxVel(size_t channel, size_t set)
{
    verifyChannelVector(channel, set);
    return mData[channel][set].txVel;
}

double PVPArray::getRcvTime(size_t channel, size_t set)
{
    verifyChannelVector(channel, set);
    return mData[channel][set].rcvTime;
}

Vector3 PVPArray::getRcvPos(size_t channel, size_t set)
{
    verifyChannelVector(channel, set);
    return mData[channel][set].rcvPos;
}

Vector3 PVPArray::getRcvVel(size_t channel, size_t set)
{
    verifyChannelVector(channel, set);
    return mData[channel][set].rcvVel;
}

Vector3 PVPArray::getSRPPos(size_t channel, size_t set)
{
    verifyChannelVector(channel, set);
    return mData[channel][set].srpPos;
}

double PVPArray::getaFDOP(size_t channel, size_t set)
{
    verifyChannelVector(channel, set);
    return mData[channel][set].aFDOP;
}

double PVPArray::getaFRR1(size_t channel, size_t set)
{
    verifyChannelVector(channel, set);
    return mData[channel][set].aFRR1;
}

double PVPArray::getaFRR2(size_t channel, size_t set)
{
    verifyChannelVector(channel, set);
    return mData[channel][set].aFRR2;
}

double PVPArray::getFx1(size_t channel, size_t set)
{
    verifyChannelVector(channel, set);
    return mData[channel][set].fx1;
}

double PVPArray::getFx2(size_t channel, size_t set)
{
    verifyChannelVector(channel, set);
    return mData[channel][set].fx2;
}

double PVPArray::getTOA1(size_t channel, size_t set)
{
    verifyChannelVector(channel, set);
    return mData[channel][set].toa1;
}

double PVPArray::getTOA2(size_t channel, size_t set)
{
    verifyChannelVector(channel, set);
    return mData[channel][set].toa2;
}

double PVPArray::getTdTropoSRP(size_t channel, size_t set)
{
    verifyChannelVector(channel, set);
    return mData[channel][set].tdTropoSRP;
}

double PVPArray::getSC0(size_t channel, size_t set)
{
    verifyChannelVector(channel, set);
    return mData[channel][set].sc0;
}

double PVPArray::getSCSS(size_t channel, size_t set)
{
    verifyChannelVector(channel, set);
    return mData[channel][set].scss;
}

double PVPArray::getAmpSF(size_t channel, size_t set)
{
    verifyChannelVector(channel, set);
    if (mData[channel][set].ampSF.get())
    {
        return *mData[channel][set].ampSF;
    }
    throw except::Exception(Ctxt(
                    "Parameter was not set"));
}

double PVPArray::getFxN1(size_t channel, size_t set)
{
    verifyChannelVector(channel, set);
    if (mData[channel][set].fxN1.get())
    {
        return *mData[channel][set].fxN1;
    }
    throw except::Exception(Ctxt(
                    "Parameter was not set"));
}

double PVPArray::getFxN2(size_t channel, size_t set)
{
    verifyChannelVector(channel, set);
    if (mData[channel][set].fxN2.get())
    {
        return *mData[channel][set].fxN2;
    }
    throw except::Exception(Ctxt(
                    "Parameter was not set"));
}

double PVPArray::getTOAE1(size_t channel, size_t set)
{
    verifyChannelVector(channel, set);
    if (mData[channel][set].toaE1.get())
    {
        return *mData[channel][set].toaE1;
    }
    throw except::Exception(Ctxt(
                    "Parameter was not set"));
}

double PVPArray::getTOAE2(size_t channel, size_t set)
{
    verifyChannelVector(channel, set);
    if (mData[channel][set].toaE2.get())
    {
        return *mData[channel][set].toaE2;
    }
    throw except::Exception(Ctxt(
                    "Parameter was not set"));
}

double PVPArray::getTdIonoSRP(size_t channel, size_t set)
{
    verifyChannelVector(channel, set);
    if (mData[channel][set].tdIonoSRP.get())
    {
        return *mData[channel][set].tdIonoSRP;
    }
    throw except::Exception(Ctxt(
                    "Parameter was not set"));
}

void PVPArray::setTxTime(double value, size_t channel, size_t vector)
{
    verifyChannelVector(channel, vector);
    mData[channel][vector].txTime = value;
}

void PVPArray::setTxPos(const cphd::Vector3& value, size_t channel, size_t vector)
{
    verifyChannelVector(channel, vector);
    mData[channel][vector].txPos = value;
}

void PVPArray::setTxVel(const cphd::Vector3& value, size_t channel, size_t vector)
{
    verifyChannelVector(channel, vector);
    mData[channel][vector].txVel = value;
}

void PVPArray::setRcvTime(double value, size_t channel, size_t vector)
{
    verifyChannelVector(channel, vector);
    mData[channel][vector].rcvTime = value;
}

void PVPArray::setRcvPos(const cphd::Vector3& value, size_t channel, size_t vector)
{
    verifyChannelVector(channel, vector);
    mData[channel][vector].rcvPos = value;
}

void PVPArray::setRcvVel(const cphd::Vector3& value, size_t channel, size_t vector)
{
    verifyChannelVector(channel, vector);
    mData[channel][vector].rcvVel = value;
}

void PVPArray::setSRPPos(const cphd::Vector3& value, size_t channel, size_t vector)
{
    verifyChannelVector(channel, vector);
    mData[channel][vector].srpPos = value;
}

void PVPArray::setaFDOP(double value, size_t channel, size_t vector)
{
    verifyChannelVector(channel, vector);
    mData[channel][vector].aFDOP = value;
}

void PVPArray::setaFRR1(double value, size_t channel, size_t vector)
{
    verifyChannelVector(channel, vector);
    mData[channel][vector].aFRR1 = value;
}

void PVPArray::setaFRR2(double value, size_t channel, size_t vector)
{
    verifyChannelVector(channel, vector);
    mData[channel][vector].aFRR2 = value;
}

void PVPArray::setFx1(double value, size_t channel, size_t vector)
{
    verifyChannelVector(channel, vector);
    mData[channel][vector].fx1 = value;
}

void PVPArray::setFx2(double value, size_t channel, size_t vector)
{
    verifyChannelVector(channel, vector);
    mData[channel][vector].fx2 = value;
}

void PVPArray::setTOA1(double value, size_t channel, size_t vector)
{
    verifyChannelVector(channel, vector);
    mData[channel][vector].toa1 = value;
}

void PVPArray::setTOA2(double value, size_t channel, size_t vector)
{
    verifyChannelVector(channel, vector);
    mData[channel][vector].toa2 = value;
}

void PVPArray::setTdTropoSRP(double value, size_t channel, size_t vector)
{
    verifyChannelVector(channel, vector);
    mData[channel][vector].tdTropoSRP = value;
}

void PVPArray::setSC0(double value, size_t channel, size_t vector)
{
    verifyChannelVector(channel, vector);
    mData[channel][vector].sc0 = value;
}

void PVPArray::setSCSS(double value, size_t channel, size_t vector)
{
    verifyChannelVector(channel, vector);
    mData[channel][vector].scss = value;
}

void PVPArray::setAmpSF(Pvp& p, double value, size_t channel, size_t vector)
{
    verifyChannelVector(channel, vector);
    if (p.ampSF.get())
    {
        mData[channel][vector].ampSF.reset(new double(value));
        return;
    }
    throw except::Exception(Ctxt(
                            "Parameter was not specified in XML"));
}

void PVPArray::setFxN1(Pvp& p, double value, size_t channel, size_t vector)
{
    verifyChannelVector(channel, vector);
    if (p.fxN1.get())
    {
        mData[channel][vector].fxN1.reset(new double(value));
        return;
    }
    throw except::Exception(Ctxt(
                            "Parameter was not specified in XML"));

}

void PVPArray::setFxN2(Pvp& p, double value, size_t channel, size_t vector)
{
    verifyChannelVector(channel, vector);
    if (p.fxN2.get())
    {
        mData[channel][vector].fxN2.reset(new double(value));
        return;
    }
    throw except::Exception(Ctxt(
                            "Parameter was not specified in XML"));
}

void PVPArray::setTOAE1(Pvp& p, double value, size_t channel, size_t vector)
{
    verifyChannelVector(channel, vector);
    if (p.toaE1.get())
    {
        mData[channel][vector].toaE1.reset(new double(value));
        return;
    }
    throw except::Exception(Ctxt(
                            "Parameter was not specified in XML"));
}

void PVPArray::setTOAE2(Pvp& p, double value, size_t channel, size_t vector)
{
    verifyChannelVector(channel, vector);
    if (p.toaE2.get())
    {
        mData[channel][vector].toaE2.reset(new double(value));
        return;
    }
    throw except::Exception(Ctxt(
                            "Parameter was not specified in XML"));
}

void PVPArray::setTdIonoSRP(Pvp& p, double value, size_t channel, size_t vector)
{
    verifyChannelVector(channel, vector);
    if (p.tdIonoSRP.get())
    {
        mData[channel][vector].tdIonoSRP.reset(new double(value));
        return;
    }
    throw except::Exception(Ctxt(
                            "Parameter was not specified in XML"));
}

void PVPArray::setSignal(Pvp& p, double value, size_t channel, size_t vector)
{
    verifyChannelVector(channel, vector);
    if (p.signal.get())
    {
        mData[channel][vector].signal.reset(new double(value));
        return;
    }
    throw except::Exception(Ctxt(
                            "Parameter was not specified in XML"));
}

std::ostream& operator<< (std::ostream& os, const PVPArray::PVPSet& p)
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

    for (size_t ii = 0; ii < p.addedPVP.size(); ++ii)
    {
        os << "  Additional Parameter : " << p.addedPVP[ii].str() << "\n";
    }
    return os;
}


std::ostream& operator<< (std::ostream& os, const PVPArray& p)
{
    os << "PVPArray:: \n";

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
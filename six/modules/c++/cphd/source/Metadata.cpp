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
#include <cphd/Metadata.h>

namespace cphd
{

Metadata::Metadata()
{
  // Default version defined in cphd::FileHeader
  mVersion = FileHeader::DEFAULT_VERSION;
}

size_t Metadata::getNumChannels() const
{
    return data.getNumChannels();
}

size_t Metadata::getNumVectors(size_t channel_) const
{
    return data.getNumVectors(channel_);
}

size_t Metadata::getNumSamples(size_t channel_) const
{
    return data.getNumSamples(channel_);
}

size_t Metadata::getNumBytesPerSample() const
{
    return data.getNumBytesPerSample();
}

size_t Metadata::getCompressedSignalSize(size_t channel_) const
{
    return data.getCompressedSignalSize(channel_);
}

bool Metadata::isCompressed() const
{
    return data.isCompressed();
}

DomainType Metadata::getDomainType() const
{
    return global.getDomainType();
}

std::string Metadata::getVersion() const
{
    return mVersion;
}
void Metadata::setVersion(const std::string& version)
{
    mVersion = version;
}

bool Metadata::operator==(const Metadata& other) const
{
    return collectionID == other.collectionID &&
           global == other.global &&
           sceneCoordinates == other.sceneCoordinates &&
           data == other.data &&
           channel == other.channel &&
           pvp == other.pvp &&
           dwell == other.dwell &&
           referenceGeometry == other.referenceGeometry &&
           supportArray == other.supportArray &&
           antenna == other.antenna &&
           txRcv == other.txRcv &&
           errorParameters == other.errorParameters &&
           productInfo == other.productInfo &&
           geoInfo == other.geoInfo &&
           matchInfo == other.matchInfo;
}

std::ostream& operator<< (std::ostream& os, const Metadata& d)
{
    os << "Metadata:: \n"
        << d.collectionID << "\n"
        << d.global << "\n"
        << d.sceneCoordinates << "\n"
        << d.data << "\n"
        << d.channel << "\n"
        << d.pvp << "\n"
        << d.dwell << "\n"
        << d.referenceGeometry << "\n";
    if (d.supportArray.get())
    {
        os << *(d.supportArray) << "\n";
    }
    if (d.antenna.get())
    {
        os << *(d.antenna) << "\n";
    }
    if (d.txRcv.get())
    {
        os << *(d.txRcv) << "\n";
    }
    if (d.errorParameters.get())
    {
        os << *(d.errorParameters) << "\n";
    }
    if (d.productInfo.get())
    {
        os << *(d.productInfo) << "\n";
    }
    for (size_t ii = 0; ii < d.geoInfo.size(); ++ii)
    {
        os << d.geoInfo[ii] << "\n";
    }
    if (d.matchInfo.get())
    {
        os << *(d.matchInfo) << "\n";
    }
    return os;
}
}

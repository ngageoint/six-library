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

#include <cphd03/Metadata.h>

namespace cphd03
{
size_t Metadata::getNumChannels() const
{
    return data.getNumChannels();
}

size_t Metadata::getNumVectors(size_t chan) const
{
    return data.getNumVectors(chan);
}

size_t Metadata::getNumSamples(size_t chan) const
{
    return data.getNumSamples(chan);
}

size_t Metadata::getNumBytesPerSample() const
{
    return data.getNumBytesPerSample();
}

std::string Metadata::getDomainTypeString() const
{
    return global.domainType.toString();
}

cphd::DomainType Metadata::getDomainType() const
{
    return global.domainType;
}

bool Metadata::operator==(const Metadata& other) const
{
    if (collectionInformation != other.collectionInformation ||
        data != other.data ||
        global != other.global ||
        channel != other.channel ||
        srp != other.srp ||
        vectorParameters != other.vectorParameters)
    {
        return false;
    }

    if (antenna.get() && other.antenna.get())
    {
        if (*antenna != *other.antenna)
        {
            return false;
        }
    }
    else if (antenna.get() || other.antenna.get())
    {
        return false;
    }
    return true;
}

std::ostream& operator<< (std::ostream& os, const Metadata& d)
{
    os << "Metadata::" << "\n"
       <<  "  " << cphd03::toString(d.collectionInformation) << "\n"
       <<  "  " << d.data << "\n"
       <<  "  " << d.global << "\n"
       <<  "  " << d.channel << "\n"
       <<  "  " << d.srp << "\n";

    if (d.antenna.get())
    {
        os << "  " << *d.antenna << "\n";
    }

    os <<  "  " << d.vectorParameters << "\n";

    return os;
}
}

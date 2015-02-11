/******************************************************************************
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * Developed in the performance of one or more U.S. Government contracts.
 * The U.S. Government has Unlimited Rights in this computer software as set
 * forth in the Rights in Technical Data and Compute Software: Noncommercial
 * Items clauses.
 ******************************************************************************/


#include <cphd/Metadata.h>

namespace cphd
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
       <<  "  " << cphd::toString(d.collectionInformation) << "\n"
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

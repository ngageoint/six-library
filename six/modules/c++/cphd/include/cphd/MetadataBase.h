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
#pragma once
#ifndef __CPHD_METADATA_BASE_H__
#define __CPHD_METADATA_BASE_H__

#include <ostream>
#include <six/Init.h>
#include <cphd/Enums.h>
#include <cphd/Exports.h>

namespace cphd
{
/*!
 *  \class MetadataBase
 *  \brief Abstract base class for the CPHD and CPHD03 Metdata objects.
 *
 *  The metadata object is derived for CPHD and CPHD03
 *. This class provides the interface needed
 *  to interact with the signal block reader Wideband, currently in CPHD
 *
 */
struct SIX_CPHD_API MetadataBase
{
    //! Default constructor
    MetadataBase()
    {
    }

    //! Destructor
    virtual ~MetadataBase()
    {
    }

    /*
     * Getter functions
     */
    virtual size_t getNumChannels() const = 0;
    virtual size_t getNumVectors(size_t channel) const = 0;   // 0-based channel number
    virtual size_t getNumSamples(size_t channel) const = 0;   // 0-based channel number
    virtual size_t getNumBytesPerSample() const = 0;          // 2, 4, or 8 bytes/complex sample

    /*
     * \func getCompressedSignalSize
     * \brief Gets the size of compressed signal array
     *  if applicable
     *
     * This function returns default value. Can be overridden
     * if required (Ex: CPHD::Metadata)
     *
     * \return undefined value by default
     */
    virtual size_t getCompressedSignalSize(size_t /*channel*/) const
    {
        return six::Init::undefined<size_t>();
    }

    /*
     * \func isCompressed
     * \brief Check if signal data is compressed
     *
     * This function returns default value false. Function can
     * be overridden if required (Ex: CPHD::Metadata)
     *
     * \return false by default
     */
    virtual bool isCompressed() const
    {
        return false;
    }

    /*!
     * Get domain type
     * FX for frequency domain,
     * TOA for time-of-arrival domain
     */
    virtual DomainType getDomainType() const = 0;

    //! Is this CPHD formed in the transmit frequency domain?
    bool isFX() const
    {
        return (getDomainType() == cphd::DomainType::FX);
    }

    //! Is this CPHD formed in the Time of Arrival domain?
    bool isTOA() const
    {
        return (getDomainType() == cphd::DomainType::TOA);
    }
};
}

#endif

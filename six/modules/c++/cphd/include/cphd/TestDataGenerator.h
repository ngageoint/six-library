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

#ifndef __CPHD_TEST_DATA_GENERATOR_H__
#define __CPHD_TEST_DATA_GENERATOR_H__

#include <iostream>
#include <string>
#include <vector>
#include <map>

#include <cphd/Metadata.h>
#include <cphd/PVP.h>
#include <cphd/PVPBlock.h>
#include <cphd/Enums.h>
#include <cphd/Types.h>

namespace cphd
{

/*
 *  \func getRandom
 *  \brief randomly generates double between [-1000, 1000]
 *
 * \return Randomly generated double
 */
double getRandom();

/*
 *  \func getRandomVector3
 *  \brief generates vector3D type object with random values
 *
 *  \return Return vector3D object
 */
Vector3 getRandomVector3();

/*
 *  \func setPVPXML
 *  \brief set PVP metadata
 *
 *  Function only sets required parameters metadata
 *
 *  \params[out] pvp A Pvp object for which values will be set
 *
 */
void setPVPXML(Pvp& pvp);

/*
 *  \func setVectorParameters
 *  \brief Sets the per vector parameters of a specific vector
 *
 *  Must be called after setPVPXML
 *  Function only sets required parameters
 *
 *  \params channel The channel number of the vector
 *  \params vector The vector of parameters to be set
 *  \param[in,out] pvpBlock An initialized pvpBlock object,
 *   will be returned with data filled in for specific vector
 *
 */
void setVectorParameters(size_t channel,
                          size_t vector,
                          PVPBlock& pvpBlock);

/*
 *  \func setUpMetadata
 *  \brief Set up bare minimum metadata for valid roundtrip
 *
 *  \param[out] Filled metadata object
 *
 */
void setUpMetadata(Metadata& metadata);

/*
 *  \func setUpData
 *  \brief Sets up data metadata, as well as rest of metadata
 *
 *  \param writeData vector of wideband data
 *  \param dims A RowCol object with number of vectors and number of samples
 *  \param[in, out] metadata cphd::Metadata object that will be filled.
 *  If wideband is compressed, metadata.data.compressedSignalID should be set
 *
 *  \throws except::Exception if wideband data is not an accepted type or size
 */
inline cphd::SignalArrayFormat getSignalArrayFormat(size_t writeDataSize)
{
    if (writeDataSize == 2)
    {
        return cphd::SignalArrayFormat::CI2;
    }
    if (writeDataSize == 4)
    {
        return cphd::SignalArrayFormat::CI4;
    }
    if (writeDataSize == 8)
    {
        return cphd::SignalArrayFormat::CF8;
    }
    
    throw except::Exception(Ctxt("Signal format is incorrect size or type"));
}


template<typename T>
Metadata setUpData(const types::RowCol<size_t>& dims,
               const std::vector<T>& writeData,
               const std::string* pSignalCompressionID = nullptr)
{
    Data data;
    if (pSignalCompressionID != nullptr)
    {
        data.signalCompressionID = *pSignalCompressionID;
    }
    const size_t numChannels = 1;
    for (size_t ii = 0; ii < numChannels; ++ii)
    {
        data.channels.push_back(
                cphd::Data::Channel(dims.row, dims.col));
    }
    Metadata metadata(std::move(data));

    if (!writeData.empty())
    {
        // If compressed data
        if (metadata.data.isCompressed())
        {
            // SignalArrayFormat doesn't matter for storing
            metadata.data.signalArrayFormat = cphd::SignalArrayFormat::CF8;
            for (size_t ii = 0; ii < numChannels; ++ii)
            {
                metadata.data.channels[ii].compressedSignalSize = dims.area();
            }
        }
        // Must set the sample type
        else
        {
            metadata.data.signalArrayFormat = getSignalArrayFormat(sizeof(writeData[0]));
        }
    }
    else
    {
        // Select an arbitrary size so we can test other stuff
        metadata.data.signalArrayFormat = cphd::SignalArrayFormat::CF8;
    }

    setUpMetadata(metadata);

    return metadata;
}
template<typename T>
Metadata setUpData(const std::string& signalCompressionID,
    const types::RowCol<size_t>& dims,
    const std::vector<T>& writeData)
{
    return setUpData(dims, writeData, &signalCompressionID);
}

}
#endif

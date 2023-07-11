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
#include <cphd/CPHDWriter.h>

#include <thread>
#include <std/bit>
#include <std/memory>

#include <types/complex.h>
#include <except/Exception.h>

#include <cphd/ByteSwap.h>
#include <cphd/CPHDXMLControl.h>
#include <cphd/FileHeader.h>
#include <cphd/Utilities.h>
#include <cphd/Wideband.h>

#undef min
#undef max


namespace cphd
{

CPHDWriter::CPHDWriter(const Metadata& metadata,
                       std::shared_ptr<io::SeekableOutputStream> outStream,
                       const std::vector<std::string>& schemaPaths,
                       size_t numThreads,
                       size_t scratchSpaceSize) :
    mMetadata(metadata),
    mElementSize(metadata.data.getNumBytesPerSample()),
    mScratchSpaceSize(scratchSpaceSize),
    mNumThreads(numThreads),
    mSchemaPaths(schemaPaths),
    mStream(outStream)
{
    // Get the correct dataWriter.
    // The CPHD file needs to be big endian.
    mDataWriter = make_DataWriter(*mStream, mNumThreads, mScratchSpaceSize);
}

CPHDWriter::CPHDWriter(const Metadata& metadata,
                       const std::string& pathname,
                       const std::vector<std::string>& schemaPaths,
                       size_t numThreads,
                       size_t scratchSpaceSize) :
    CPHDWriter(metadata,
        std::make_shared<io::FileOutputStream>(pathname), // Initialize output stream
        schemaPaths,
        numThreads,
        scratchSpaceSize)
{
}

void CPHDWriter::writeMetadata(size_t supportSize,
                               size_t pvpSize,
                               size_t cphdSize)
{
    const auto xmlMetadata(CPHDXMLControl().toXMLString(mMetadata, mSchemaPaths));

    Version cphdVersion;
    mMetadata.getVersion(cphdVersion);
    mHeader.setVersion(cphdVersion);

    // update classification and release info
    if (!six::Init::isUndefined(
                mMetadata.collectionID.getClassificationLevel()) &&
        !six::Init::isUndefined(mMetadata.collectionID.releaseInfo))
    {
        mHeader.setClassification(
                mMetadata.collectionID.getClassificationLevel());
        mHeader.setReleaseInfo(mMetadata.collectionID.releaseInfo);
    }
    else
    {
        throw except::Exception(Ctxt("Classification level and Release "
                                     "informaion must be specified"));
    }
    // set header size, final step before write
    mHeader.set(xmlMetadata.size(), supportSize, pvpSize, cphdSize);
    mStream->write(mHeader.toString());
    mStream->write("\f\n");
    mStream->write(xmlMetadata);
    mStream->write("\f\n");
}

void CPHDWriter::writePVPData(const std::byte* pvpBlock, size_t channel)
{
    const size_t size = (mMetadata.data.getNumVectors(channel) *
                         mMetadata.data.getNumBytesPVPSet()) /
            8;

    //! The vector based parameters are always 64 bit
    (*mDataWriter)(pvpBlock, size, 8);
}

void CPHDWriter::writeCPHDDataImpl(const std::byte* data, size_t size)
{
    //! We have to pass in the data as though it was not complex
    //  thus we pass in twice the number of elements at half the size.
    (*mDataWriter)(data, size * 2, mElementSize / 2);
}

void CPHDWriter::writeCompressedCPHDDataImpl(const std::byte* data,
                                             size_t channel)
{
    //! We have to pass in the data as though it was 1 signal array sized
    // element of ubytes
    (*mDataWriter)(data, mMetadata.data.getCompressedSignalSize(channel), 1);
}

void CPHDWriter::writeSupportDataImpl(const std::byte* data,
                                      size_t numElements,
                                      size_t elementSize)
{
    (*mDataWriter)(data, numElements, elementSize);
}

template <typename T>
void CPHDWriter::write(const PVPBlock& pvpBlock,
                       const T* widebandData,
                       const sys::ubyte* supportData)
{
    // Write File header and metadata to file
    // Padding is added in writeMetadata
    writeMetadata(pvpBlock);

    // Write optional support array block
    // Padding is added in writeSupportData
    if (mMetadata.data.getNumSupportArrays() != 0)
    {
        if (supportData == nullptr)
        {
            throw except::Exception(Ctxt("SupportData is not provided"));
        }
        writeSupportData(supportData);
    }

    // Write pvp data block
    // Padding is added in writePVPData
    writePVPData(pvpBlock);

    // Doesn't require pading because pvp block is always 8 bytes words
    // Write wideband (or signal) block
    size_t elementsWritten = 0;  // Used to increment widebandData pointer
    for (size_t ii = 0; ii < mMetadata.data.getNumChannels(); ++ii)
    {
        size_t numElements = mMetadata.data.getNumVectors(ii) *
                mMetadata.data.getNumSamples(ii);
        // writeCPHDData handles compressed data as well
        writeCPHDData<T>(widebandData + elementsWritten, numElements, ii);
        elementsWritten += numElements;
    }
}

// For compressed data
template void CPHDWriter::write<sys::ubyte>(const PVPBlock& pvpBlock,
                                            const sys::ubyte* widebandData,
                                            const sys::ubyte* supportData);
template void CPHDWriter::write<std::byte>(const PVPBlock& pvpBlock,
                                            const std::byte* widebandData,
                                            const std::byte* supportData);

template void CPHDWriter::write<cphd::zint8_t>(
    const PVPBlock& pvpBlock,
    const cphd::zint8_t* widebandData,
    const sys::ubyte* supportData);

template void CPHDWriter::write<cphd::zint16_t>(
    const PVPBlock& pvpBlock,
    const cphd::zint16_t* widebandData,
    const sys::ubyte* supportData);

template void CPHDWriter::write<cphd::zfloat>(
    const PVPBlock& pvpBlock,
    const cphd::zfloat* widebandData,
    const sys::ubyte* supportData);

template void CPHDWriter::write<cphd::zint8_t>(
        const PVPBlock& pvpBlock,
        const cphd::zint8_t* widebandData,
        const std::byte* supportData);

template void CPHDWriter::write<cphd::zint16_t>(
        const PVPBlock& pvpBlock,
        const cphd::zint16_t* widebandData,
        const std::byte* supportData);

template void CPHDWriter::write<cphd::zfloat>(
        const PVPBlock& pvpBlock,
        const cphd::zfloat* widebandData,
        const std::byte* supportData);

void CPHDWriter::writeMetadata(const PVPBlock& pvpBlock)
{
    // Update the number of bytes per PVP
    if (mMetadata.data.numBytesPVP != pvpBlock.getNumBytesPVPSet())
    {
        std::ostringstream ostr;
        ostr << "Number of pvp block bytes in metadata: "
             << mMetadata.data.numBytesPVP
             << " does not match calculated size of pvp block: "
             << pvpBlock.getNumBytesPVPSet();
        throw except::Exception(ostr.str());
    }

    const size_t numChannels = mMetadata.data.getNumChannels();
    size_t totalSupportSize = 0;
    size_t totalPVPSize = 0;
    size_t totalCPHDSize = 0;

    for (auto it = mMetadata.data.supportArrayMap.begin();
         it != mMetadata.data.supportArrayMap.end();
         ++it)
    {
        totalSupportSize += it->second.getSize();
    }

    for (size_t ii = 0; ii < numChannels; ++ii)
    {
        totalPVPSize += pvpBlock.getPVPsize(ii);
        totalCPHDSize += mMetadata.data.getNumVectors(ii) *
                mMetadata.data.getNumSamples(ii) * mElementSize;
    }

    writeMetadata(totalSupportSize, totalPVPSize, totalCPHDSize);
}

void CPHDWriter::writePVPData(const PVPBlock& pvpBlock)
{
    // Add padding
    char zero = 0;
    for (int64_t ii = 0; ii < mHeader.getPvpPadBytes(); ++ii)
    {
        mStream->write(&zero, 1);
    }

    // Write each PVP array
    const size_t numChannels = mMetadata.data.getNumChannels();
    std::vector<std::byte> pvpData;
    for (size_t ii = 0; ii < numChannels; ++ii)
    {
        pvpBlock.getPVPdata(ii, pvpData);
        if (pvpData.empty())
        {
            std::ostringstream ostr;
            ostr << "PVPBlock of channel " << ii << " is empty";
            throw except::Exception(Ctxt(ostr.str()));
        }
        writePVPData(pvpData.data(), ii);
    }
}

template <typename T>
void CPHDWriter::writeCPHDData(const T* data,
                               size_t numElements,
                               size_t channel)
{
    if (mMetadata.data.isCompressed())
    {
        writeCompressedCPHDDataImpl(reinterpret_cast<const std::byte*>(data),
                                    channel);
    }
    else
    {
        if (mElementSize != sizeof(T))
        {
            throw except::Exception(
                    Ctxt("Incorrect buffer data type used for metadata!"));
        }
        writeCPHDDataImpl(reinterpret_cast<const std::byte*>(data),
                          numElements);
    }
}

// For compressed data
template void CPHDWriter::writeCPHDData(const sys::ubyte* data,
                                        size_t numElements,
                                        size_t channel);
template void CPHDWriter::writeCPHDData(const std::byte* data,
                                        size_t numElements,
                                        size_t channel);

template void CPHDWriter::writeCPHDData<cphd::zint8_t>(
        const cphd::zint8_t* data,
        size_t numElements,
        size_t channel);

template void CPHDWriter::writeCPHDData<cphd::zint16_t>(
        const cphd::zint16_t* data,
        size_t numElements,
        size_t channel);

template void CPHDWriter::writeCPHDData<cphd::zfloat>(
        const cphd::zfloat* data, size_t numElements, size_t channel);
}

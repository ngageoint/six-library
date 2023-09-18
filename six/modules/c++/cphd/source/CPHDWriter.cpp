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
static Version getVersion(const Metadata& metadata)
{
    Version retval;
    metadata.getVersion(retval);
    return retval;
}

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
    mStream(outStream),
    mHeader(getVersion(metadata))
{
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

void CPHDWriter::writeMetadata(io::OutputStream& stream,
    size_t supportSize, size_t pvpSize, size_t cphdSize)
{
    mHeader.setVersion(getVersion(mMetadata));

    // update classification and release info
    auto&& collectionID = mMetadata.collectionID;
    if (!six::Init::isUndefined(collectionID.getClassificationLevel()) && !six::Init::isUndefined(collectionID.releaseInfo))
    {
        mHeader.setClassification(collectionID.getClassificationLevel());
        mHeader.setReleaseInfo(collectionID.releaseInfo);
    }
    else
    {
        throw except::Exception(Ctxt("Classification level and Release informaion must be specified"));
    }

    const auto xmlMetadata(CPHDXMLControl().toXMLString(mMetadata, mSchemaPaths));

    // set header size, final step before write
    mHeader.set(xmlMetadata.size(), supportSize, pvpSize, cphdSize);
    stream.write(mHeader.toString());
    stream.write("\f\n");
    stream.write(xmlMetadata);
    stream.write("\f\n");
}

std::unique_ptr<DataWriter> CPHDWriter::make_DataWriter(io::SeekableOutputStream& stream) const
{
    // Get the correct dataWriter.
    // The CPHD file needs to be big endian.
    return cphd::make_DataWriter(stream, mNumThreads, mScratchSpaceSize);
}
std::unique_ptr<DataWriter> CPHDWriter::make_DataWriter() const
{
    return make_DataWriter(*mStream);
}

void CPHDWriter::writeCPHDDataImpl(const std::byte* data, size_t size)
{
    auto dataWriter = make_DataWriter();

    //! We have to pass in the data as though it was not complex
    //  thus we pass in twice the number of elements at half the size.
    (*dataWriter)(data, size * 2, mElementSize / 2);
}
CPHDWriter::WriteImplFunc_t CPHDWriter::getWriteCPHDDataImpl()
{
    return [&](const std::byte* data, size_t channel)
    {
        writeCPHDDataImpl(data, channel);
    };
}

void CPHDWriter::writeCompressedCPHDDataImpl(const std::byte* data, size_t channel)
{
    auto dataWriter = make_DataWriter();

    //! We have to pass in the data as though it was 1 signal array sized
    // element of ubytes
    (*dataWriter)(data, mMetadata.data.getCompressedSignalSize(channel), 1);
}
CPHDWriter::WriteImplFunc_t CPHDWriter::getWriteCompressedCPHDDataImpl()
{
    return [&](const std::byte* data, size_t channel)
    {
        writeCompressedCPHDDataImpl(data, channel);
    };
}

void CPHDWriter::writeSupportDataImpl(std::span<const std::byte> data, size_t elementSize)
{
    auto dataWriter = make_DataWriter();
    (*dataWriter)(data, elementSize);
}

static auto make_span(std::span<const std::byte> data, const cphd::Data::SupportArray& dataArray)
{
    const auto pData = data.data() + dataArray.arrayByteOffset;
    return sys::make_span(pData, dataArray.size_bytes());
}
void CPHDWriter::writeSupportData(io::SeekableOutputStream& stream, std::span<const std::byte> data)
{
    const auto supportBlockByteOffset = mHeader.getSupportBlockByteOffset();

    auto dataWriter = make_DataWriter(stream);
    for (auto&& mapEntry : mMetadata.data.supportArrayMap)
    {
        auto&& dataArray = mapEntry.second;

        // Move inputstream head to offset of particular support array
        stream.seek(supportBlockByteOffset + dataArray.arrayByteOffset, io::SeekableOutputStream::START);

        (*dataWriter)(make_span(data, dataArray), dataArray.bytesPerElement);
    }
    // Move inputstream head to the end of the support block after all supports have been written
    stream.seek(supportBlockByteOffset + mHeader.getSupportBlockSize(), io::SeekableOutputStream::START);
}
void CPHDWriter::writeSupportData(std::span<const std::byte> data)
{
    writeSupportData(*mStream, data);
}

template <typename T>
void CPHDWriter::write(const PVPBlock& pvpBlock,
    const T* widebandData,
    std::span<const std::byte> supportData)
{
    // Write File header and metadata to file
    // Padding is added in writeMetadata
    writeMetadata(pvpBlock);

    // Write optional support array block
    // Padding is added in writeSupportData
    if (mMetadata.data.getNumSupportArrays() != 0)
    {
        if (supportData.empty())
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
template void CPHDWriter::write<std::byte>(const PVPBlock&, const std::byte* widebandData, std::span<const std::byte>); // For compressed data
template void CPHDWriter::write<cphd::zint8_t>(const PVPBlock&, const cphd::zint8_t* widebandData, std::span<const std::byte>);
template void CPHDWriter::write<cphd::zint16_t>(const PVPBlock&, const cphd::zint16_t* widebandData, std::span<const std::byte>);
template void CPHDWriter::write<cphd::zfloat>(const PVPBlock&, const cphd::zfloat* widebandData, std::span<const std::byte>);
//template void CPHDWriter::write<cphd::zint8_t>(const PVPBlock&, const cphd::zint8_t* widebandData, std::span<const std::byte>);
//template void CPHDWriter::write<cphd::zint16_t>(const PVPBlock&, const cphd::zint16_t* widebandData, std::span<const std::byte>);
//template void CPHDWriter::write<cphd::zfloat>(const PVPBlock&, const cphd::zfloat* widebandData, std::span<const std::byte>);

void CPHDWriter::getMetadata(const PVPBlock& pvpBlock,
    size_t& totalSupportSize, size_t& totalPVPSize, size_t& totalCPHDSize) const
{
    auto&& data = mMetadata.data;

    // Update the number of bytes per PVP
    if (data.numBytesPVP != pvpBlock.getNumBytesPVPSet())
    {
        std::ostringstream ostr;
        ostr << "Number of pvp block bytes in metadata: " << data.numBytesPVP
             << " does not match calculated size of pvp block: " << pvpBlock.getNumBytesPVPSet();
        throw except::Exception(ostr.str());
    }

    const size_t numChannels = data.getNumChannels();

    totalSupportSize = 0;
    for (auto&& kv : data.supportArrayMap)
    {
        totalSupportSize += kv.second.getSize();
    }

    totalPVPSize = 0;
    totalCPHDSize = 0;
    for (size_t ii = 0; ii < numChannels; ++ii)
    {
        totalPVPSize += pvpBlock.getPVPsize(ii);
        totalCPHDSize += data.getNumVectors(ii) * data.getNumSamples(ii) * mElementSize;
    }
}
void CPHDWriter::writeMetadata(io::OutputStream& stream, const PVPBlock& pvpBlock)
{
    size_t totalSupportSize;
    size_t totalPVPSize;
    size_t totalCPHDSize;
    getMetadata(pvpBlock, totalSupportSize, totalPVPSize, totalCPHDSize);
    writeMetadata(stream, totalSupportSize, totalPVPSize, totalCPHDSize);
}
void CPHDWriter::writeMetadata(const PVPBlock& pvpBlock)
{
    writeMetadata(*mStream, pvpBlock);
}

void CPHDWriter::writePvpPadBytes(io::OutputStream& stream, int64_t pvpPadBytes)
{
    // Add padding
    char zero = 0;
    for (int64_t ii = 0; ii < pvpPadBytes; ++ii)
    {
        stream.write(&zero, 1);
    }
}

void CPHDWriter::writePVPData(DataWriter& dataWriter, const cphd::Data& data, const PVPBlock& pvpBlock)
{
    // Write each PVP array
    const size_t numChannels = data.getNumChannels();
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

        const size_t size = (data.getNumVectors(ii) * data.getNumBytesPVPSet()) / 8;

        //! The vector based parameters are always 64 bit
        dataWriter(pvpData.data(), size, 8);
    }
}
void CPHDWriter::writePVPData(io::SeekableOutputStream& stream, const PVPBlock& pvpBlock)
{
    // Add padding
    writePvpPadBytes(stream, mHeader.getPvpPadBytes());

    auto dataWriter = make_DataWriter(stream);
    writePVPData(*dataWriter, mMetadata.data, pvpBlock);
}
void CPHDWriter::writePVPData(const PVPBlock& pvpBlock)
{
    writePVPData(*mStream, pvpBlock);
}

template <typename T>
void CPHDWriter::writeCPHDData(const T* data,
                               size_t numElements,
                               size_t channel)
{
    const void* const pData = data;
    if (mMetadata.data.isCompressed())
    {
        writeCompressedCPHDDataImpl(static_cast<const std::byte*>(pData), channel);
    }
    else
    {
        if (mElementSize != sizeof(T))
        {
            throw except::Exception(Ctxt("Incorrect buffer data type used for metadata!"));
        }
        writeCPHDDataImpl(static_cast<const std::byte*>(pData), numElements);
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

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

#include <thread>

#include <except/Exception.h>
#include <sys/Bit.h>

#include <cphd/ByteSwap.h>
#include <cphd/CPHDWriter.h>
#include <cphd/CPHDXMLControl.h>
#include <cphd/FileHeader.h>
#include <cphd/Utilities.h>
#include <cphd/Wideband.h>

#undef min
#undef max


namespace cphd
{
DataWriter::DataWriter(mem::SharedPtr<io::SeekableOutputStream> stream,
                       size_t numThreads) :
    mStream(stream),
    mNumThreads(numThreads == 0 ? std::thread::hardware_concurrency() : numThreads)
{
}

DataWriter::~DataWriter()
{
}

DataWriterLittleEndian::DataWriterLittleEndian(
        mem::SharedPtr<io::SeekableOutputStream> stream,
        size_t numThreads,
        size_t scratchSize) :
    DataWriter(stream, numThreads),
    mScratchSize(scratchSize),
    mScratch(new std::byte[mScratchSize])
{
}

void DataWriterLittleEndian::operator()(const std::byte* data,
                                        size_t numElements,
                                        size_t elementSize)
{
    size_t dataProcessed = 0;
    const size_t dataSize = numElements * elementSize;
    while (dataProcessed < dataSize)
    {
        const size_t dataToProcess =
                std::min(mScratchSize, dataSize - dataProcessed);

        memcpy(mScratch.get(), data + dataProcessed, dataToProcess);

        cphd::byteSwap(mScratch.get(),
                       elementSize,
                       dataToProcess / elementSize,
                       mNumThreads);

        mStream->write(mScratch.get(), dataToProcess);

        dataProcessed += dataToProcess;
    }
}

DataWriterBigEndian::DataWriterBigEndian(
        mem::SharedPtr<io::SeekableOutputStream> stream, size_t numThreads) :
    DataWriter(stream, numThreads)
{
}

void DataWriterBigEndian::operator()(const std::byte* data,
                                     size_t numElements,
                                     size_t elementSize)
{
    mStream->write(reinterpret_cast<const std::byte*>(data),
                   numElements * elementSize);
}

CPHDWriter::CPHDWriter(const Metadata& metadata,
                       mem::SharedPtr<io::SeekableOutputStream> outStream,
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
    if (std::endian::native == std::endian::big)
    {
        mDataWriter.reset(new DataWriterBigEndian(mStream, mNumThreads));
    }
    else
    {
        mDataWriter.reset(new DataWriterLittleEndian(mStream,
                                                     mNumThreads,
                                                     mScratchSpaceSize));
    }
}

CPHDWriter::CPHDWriter(const Metadata& metadata,
                       const std::string& pathname,
                       const std::vector<std::string>& schemaPaths,
                       size_t numThreads,
                       size_t scratchSpaceSize) :
    mMetadata(metadata),
    mElementSize(metadata.data.getNumBytesPerSample()),
    mScratchSpaceSize(scratchSpaceSize),
    mNumThreads(numThreads),
    mSchemaPaths(schemaPaths)
{
    // Initialize output stream
    mStream.reset(new io::FileOutputStream(pathname));

    // Get the correct dataWriter.
    // The CPHD file needs to be big endian.
    if (std::endian::native == std::endian::big)
    {
        mDataWriter.reset(new DataWriterBigEndian(mStream, mNumThreads));
    }
    else
    {
        mDataWriter.reset(new DataWriterLittleEndian(mStream,
                                                     mNumThreads,
                                                     mScratchSpaceSize));
    }
}

void CPHDWriter::writeMetadata(size_t supportSize,
                               size_t pvpSize,
                               size_t cphdSize)
{
    const std::string xmlMetadata(
            CPHDXMLControl().toXMLString(mMetadata, mSchemaPaths));

    // update header version, or remains default if unset
    mHeader.setVersion(mMetadata.getVersion());

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
    mStream->write(mHeader.toString().c_str(), mHeader.size());
    mStream->write("\f\n", 2);
    mStream->write(xmlMetadata.c_str(), xmlMetadata.size());
    mStream->write("\f\n", 2);
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
                       const std::byte* supportData)
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
    for (size_t ii = 0; ii < mMetadata.data.getNumChannels(); ++ii)
    {
        size_t numElements = mMetadata.data.getNumVectors(ii) *
                mMetadata.data.getNumSamples(ii);
        // writeCPHDData handles compressed data as well
        writeCPHDData<T>(widebandData, numElements, ii);
    }
}

// For compressed data
template void CPHDWriter::write<std::byte>(const PVPBlock& pvpBlock,
                                            const std::byte* widebandData,
                                            const std::byte* supportData);

template void CPHDWriter::write<std::complex<int8_t>>(
        const PVPBlock& pvpBlock,
        const std::complex<int8_t>* widebandData,
        const std::byte* supportData);

template void CPHDWriter::write<std::complex<int16_t>>(
        const PVPBlock& pvpBlock,
        const std::complex<int16_t>* widebandData,
        const std::byte* supportData);

template void CPHDWriter::write<std::complex<float>>(
        const PVPBlock& pvpBlock,
        const std::complex<float>* widebandData,
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
template void CPHDWriter::writeCPHDData(const std::byte* data,
                                        size_t numElements,
                                        size_t channel);

template void CPHDWriter::writeCPHDData<std::complex<int8_t>>(
        const std::complex<int8_t>* data,
        size_t numElements,
        size_t channel);

template void CPHDWriter::writeCPHDData<std::complex<int16_t>>(
        const std::complex<int16_t>* data,
        size_t numElements,
        size_t channel);

template void CPHDWriter::writeCPHDData<std::complex<float>>(
        const std::complex<float>* data, size_t numElements, size_t channel);
}

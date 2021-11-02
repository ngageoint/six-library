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
#include <cphd03/CPHDWriter.h>

#include <std/bit>

#include <scene/sys_Conf.h>
#include <except/Exception.h>

#include <cphd03/CPHDXMLControl.h>
#include <cphd03/Utilities.h>
#include <cphd03/FileHeader.h>
#include <cphd/ByteSwap.h>

namespace cphd03
{
CPHDWriter::CPHDWriter(const Metadata& metadata,
                       std::shared_ptr<io::SeekableOutputStream> stream,
                       size_t numThreads,
                       size_t scratchSpaceSize) :
    mMetadata(metadata),
    mElementSize(metadata.data.getNumBytesPerSample()),
    mScratchSpaceSize(scratchSpaceSize),
    mNumThreads(numThreads),
    mStream(stream),
    mCPHDSize(0),
    mVBMSize(0)
{
    //! Get the correct dataWriter.
    //  The CPHD file needs to be big endian.
    auto endianness = std::endian::native; // "conditional expression is constant"
    if (endianness == std::endian::big)
    {
        mDataWriter.reset(new cphd::DataWriterBigEndian(mStream, mNumThreads));
    }
    else
    {
        mDataWriter.reset(new cphd::DataWriterLittleEndian(
                mStream, mNumThreads, mScratchSpaceSize));
    }
}

CPHDWriter::CPHDWriter(const Metadata& metadata,
                       const std::string& pathname,
                       size_t numThreads,
                       size_t scratchSpaceSize) :
    mMetadata(metadata),
    mElementSize(metadata.data.getNumBytesPerSample()),
    mScratchSpaceSize(scratchSpaceSize),
    mNumThreads(numThreads),
    mCPHDSize(0),
    mVBMSize(0)
{
    // Create file stream to write
    mStream.reset(new io::FileOutputStream(pathname));

    //! Get the correct dataWriter.
    //  The CPHD file needs to be big endian.
    auto endianness = std::endian::native; // "conditional expression is constant"
    if (endianness == std::endian::big)
    {
        mDataWriter.reset(new cphd::DataWriterBigEndian(mStream, mNumThreads));
    }
    else
    {
        mDataWriter.reset(new cphd::DataWriterLittleEndian(
                mStream, mNumThreads, mScratchSpaceSize));
    }
}

template <typename T>
void CPHDWriter::addImage(const T* image,
                          const types::RowCol<size_t>& dims,
                          const sys::ubyte* vbmData)
{
    if (mElementSize != sizeof(T))
    {
        throw except::Exception(Ctxt(
                "Incorrect buffer data type used for metadata!"));
    }

    //! If this is the first time you called addImage. We will clear
    //  out the metadata image here and start adding it manually.
    if (mCPHDData.empty())
    {
        mMetadata.data.arraySize.clear();
        mMetadata.data.numCPHDChannels = 0;
    }

    mVBMData.push_back(reinterpret_cast<const std::byte*>(vbmData));
    mCPHDData.push_back(reinterpret_cast<const std::byte*>(image));

    mCPHDSize += dims.area() * mElementSize;
    mVBMSize += dims.row * mMetadata.data.getNumBytesVBP();

    mMetadata.data.numCPHDChannels += 1;
    mMetadata.data.arraySize.push_back(ArraySize(dims.row, dims.col));
}

template
void CPHDWriter::addImage<std::complex<int8_t> >(
    const std::complex<int8_t>* image,
    const types::RowCol<size_t>& dims,
    const sys::ubyte* vbmData);

template
void CPHDWriter::addImage<std::complex<int16_t> >(
    const std::complex<int16_t>* image,
    const types::RowCol<size_t>& dims,
    const sys::ubyte* vbmData);

template
void CPHDWriter::addImage<std::complex<float> >(
    const std::complex<float>* image,
    const types::RowCol<size_t>& dims,
    const sys::ubyte* vbmData);

template
void CPHDWriter::addImage<std::complex<int8_t> >(
        const std::complex<int8_t>* image,
        const types::RowCol<size_t>& dims,
        const std::byte* vbmData);

template
void CPHDWriter::addImage<std::complex<int16_t> >(
        const std::complex<int16_t>* image,
        const types::RowCol<size_t>& dims,
        const std::byte* vbmData);

template
void CPHDWriter::addImage<std::complex<float> >(
        const std::complex<float>* image,
        const types::RowCol<size_t>& dims,
        const std::byte* vbmData);

void CPHDWriter::writeMetadata(size_t vbmSize,
                               size_t cphd03Size,
                               const std::string& classification,
                               const std::string& releaseInfo)
{
    const std::string xmlMetadata(CPHDXMLControl().toXMLString(mMetadata));

    FileHeader header;
    if (!classification.empty())
    {
        header.setClassification(classification);
    }
    if (!releaseInfo.empty())
    {
        header.setReleaseInfo(releaseInfo);
    }

    // set header size, final step before write
    header.set(xmlMetadata.size(), vbmSize, cphd03Size);
    mStream->write(header.toString());
    mStream->write("\f\n");
    mStream->write(xmlMetadata);
    mStream->write("\f\n");

    // Pad bytes
    char zero = 0;
    for (int64_t ii = 0; ii < header.getPadBytes(); ++ii)
    {
        mStream->write(&zero, 1);
    }
}

void CPHDWriter::writeVBMData(const std::byte* vbm,
                              size_t index)
{
    const size_t size = (mMetadata.data.arraySize[index].numVectors *
            mMetadata.data.getNumBytesVBP()) / 8;

    //! The vector based parameters are always 64 bit
    (*mDataWriter)(vbm, size, 8);
}

void CPHDWriter::writeCPHDDataImpl(const std::byte* data,
                                   size_t size)
{
    //! We have to pass in the data as though it was not complex
    //  thus we pass in twice the number of elements at half the size.
    (*mDataWriter)(data, size * 2, mElementSize / 2);
}

void CPHDWriter::writeMetadata(const VBM& vbm,
                               const std::string& classification,
                               const std::string& releaseInfo)
{
    // Update the number of bytes per VBP
    mMetadata.data.numBytesVBP = vbm.getNumBytesVBP();

    const size_t numChannels = vbm.getNumChannels();
    size_t totalVBMSize = 0;
    size_t totalCPHDSize = 0;

    for (size_t ii = 0; ii < numChannels; ++ii)
    {
        totalVBMSize += vbm.getVBMsize(ii);
        totalCPHDSize += mMetadata.data.getNumVectors(ii) *
                mMetadata.data.getNumSamples(ii) * mElementSize;
    }

    writeMetadata(totalVBMSize, totalCPHDSize, classification, releaseInfo);

    std::vector<std::byte> vbmData;
    for (size_t ii = 0; ii < numChannels; ++ii)
    {
        vbm.getVBMdata(ii, vbmData);
        if (!vbmData.empty())
        {
            writeVBMData(vbmData.data(), ii);
        }
    }
}

template <typename T>
void CPHDWriter::writeCPHDData(const T* data,
                               size_t numElements)
{
    if (mElementSize != sizeof(T))
    {
        throw except::Exception(Ctxt(
                "Incorrect buffer data type used for metadata!"));
    }
    writeCPHDDataImpl(reinterpret_cast<const std::byte*>(data), numElements);
}

template
void CPHDWriter::writeCPHDData<std::complex<int8_t> >(
        const std::complex<int8_t>* data,
        size_t numElements);

template
void CPHDWriter::writeCPHDData<std::complex<int16_t> >(
        const std::complex<int16_t>* data,
        size_t numElements);

template
void CPHDWriter::writeCPHDData<std::complex<float> >(
        const std::complex<float>* data,
        size_t numElements);

void CPHDWriter::write(const std::string& classification,
                       const std::string& releaseInfo)
{
    writeMetadata(mVBMSize, mCPHDSize, classification, releaseInfo);

    for (size_t ii = 0; ii < mVBMData.size(); ++ii)
    {
        writeVBMData(mVBMData[ii], ii);
    }

    for (size_t ii = 0; ii < mCPHDData.size(); ++ii)
    {
        const size_t cphd03DataSize = mMetadata.data.arraySize[ii].numVectors *
                mMetadata.data.arraySize[ii].numSamples;
        writeCPHDDataImpl(mCPHDData[ii], cphd03DataSize);
    }
    mStream->close();
}
}

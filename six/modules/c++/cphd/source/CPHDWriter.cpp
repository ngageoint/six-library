/* =========================================================================
 * This file is part of cphd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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

#include <except/Exception.h>
#include <io/FileOutputStream.h>
#include <cphd/CPHDWriter.h>
#include <cphd/CPHDXMLControl.h>
#include <cphd/Utilities.h>
#include <cphd/FileHeader.h>
#include <cphd/ByteSwap.h>

namespace cphd
{
CPHDWriter::DataWriter::DataWriter(mem::SharedPtr<io::OutputStream>& stream,
                                   size_t numThreads) :
    mStream(stream),
    mNumThreads(numThreads == 0 ? sys::OS().getNumCPUs() : numThreads)
{
}

CPHDWriter::DataWriter::~DataWriter()
{
}

CPHDWriter::DataWriterLittleEndian::DataWriterLittleEndian(
        mem::SharedPtr<io::OutputStream>& stream,
        size_t numThreads,
        size_t scratchSize) :
    DataWriter(stream, numThreads),
    mScratchSize(scratchSize),
    mScratch(new sys::byte[mScratchSize])
{
}

void CPHDWriter::DataWriterLittleEndian::operator()(
        const sys::ubyte* data,
        size_t numElements,
        size_t elementSize)
{
    size_t dataProcessed = 0;
    const size_t dataSize = numElements * elementSize;

    while (dataProcessed < dataSize)
    {
        const size_t dataToProcess =
                std::min(mScratchSize, dataSize - dataProcessed);

        memcpy(mScratch.get(),
               data + dataProcessed,
               dataToProcess);

        byteSwap(mScratch.get(),
                 elementSize,
                 dataToProcess / elementSize,
                 mNumThreads);

        mStream->write(mScratch.get(), dataToProcess);

        dataProcessed += dataToProcess;
    }
}

CPHDWriter::DataWriterBigEndian::DataWriterBigEndian(
        mem::SharedPtr<io::OutputStream>& stream,
        size_t numThreads) :
    DataWriter(stream, numThreads)
{
}

void CPHDWriter::DataWriterBigEndian::operator()(const sys::ubyte* data,
                                                 size_t numElements,
                                                 size_t elementSize)
{
    mStream->write(data, numElements * elementSize);
}

CPHDWriter::CPHDWriter(const Metadata& metadata,
                       size_t numThreads,
                       size_t scratchSpaceSize) :
    mMetadata(metadata),
    mElementSize(getNumBytesPerSample(metadata.data.sampleType)),
    mScratchSpaceSize(scratchSpaceSize),
    mNumThreads(numThreads),
    mCPHDSize(0),
    mVBMSize(0)
{
    //! Get the correct dataWriter.
    //  The CPHD file needs to be big endian.
    if (sys::isBigEndianSystem())
    {
        mDataWriter.reset(new DataWriterBigEndian(mOutStream, mNumThreads));
    }
    else
    {
        mDataWriter.reset(new DataWriterLittleEndian(
                mOutStream, mNumThreads, mScratchSpaceSize));
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

    mVBMData.push_back(vbmData);
    mCPHDData.push_back(reinterpret_cast<const sys::ubyte*>(image));

    mCPHDSize += dims.area() * mElementSize;
    mVBMSize += dims.row * mMetadata.data.getNumBytesVBP();

    mMetadata.data.numCPHDChannels += 1;
    mMetadata.data.arraySize.push_back(ArraySize(dims.row, dims.col));
}

template
void CPHDWriter::addImage<std::complex<sys::Int8_T> >(
        const std::complex<sys::Int8_T>* image,
        const types::RowCol<size_t>& dims,
        const sys::ubyte* vbmData);

template
void CPHDWriter::addImage<std::complex<sys::Int16_T> >(
        const std::complex<sys::Int16_T>* image,
        const types::RowCol<size_t>& dims,
        const sys::ubyte* vbmData);

template
void CPHDWriter::addImage<std::complex<float> >(
        const std::complex<float>* image,
        const types::RowCol<size_t>& dims,
        const sys::ubyte* vbmData);

void CPHDWriter::writeMetadata(size_t vbmSize,
                               size_t cphdSize,
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
    header.set(xmlMetadata.size(), vbmSize, cphdSize);
    mOutStream->write(header.toString().c_str(), header.size());
    mOutStream->write("\f\n", 2);
    mOutStream->write(xmlMetadata.c_str(), xmlMetadata.size());
    mOutStream->write("\f\n", 2);

    // Pad bytes
    char zero = 0;
    for (sys::Off_T ii = 0; ii < header.getPadBytes(); ++ii)
    {
        mOutStream->write(&zero, 1);
    }
}

void CPHDWriter::writeVBMData(const sys::ubyte* vbm,
                              size_t index)
{
    const size_t size = (mMetadata.data.arraySize[index].numVectors *
            mMetadata.data.getNumBytesVBP()) / 8;

    //! The vector based parameters are always 64 bit
    (*mDataWriter)(vbm, size, 8);
}

void CPHDWriter::writeCPHDDataImpl(const sys::ubyte* data,
                                   size_t size)
{
    //! We have to pass in the data as though it was not complex
    //  thus we pass in twice the number of elements at half the size.
    (*mDataWriter)(data, size * 2, mElementSize / 2);
}

void CPHDWriter::writeMetadata(const std::string& pathname,
                               const VBM& vbm,
                               const std::string& classification,
                               const std::string& releaseInfo)
{
    mem::SharedPtr<io::OutputStream> outStream(
            new io::FileOutputStream(pathname));
    writeMetadata(outStream, vbm, classification, releaseInfo);
}

void CPHDWriter::writeMetadata(mem::SharedPtr<io::OutputStream> outStream,
                               const VBM& vbm,
                               const std::string& classification,
                               const std::string& releaseInfo)
{
    mOutStream = outStream;

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

    std::vector<sys::ubyte> vbmData;
    for (size_t ii = 0; ii < numChannels; ++ii)
    {
        vbm.getVBMdata(ii, vbmData);
        if (!vbmData.empty())
        {
            writeVBMData(&vbmData[0], ii);
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
    writeCPHDDataImpl(reinterpret_cast<const sys::ubyte*>(data), numElements);
}

template
void CPHDWriter::writeCPHDData<std::complex<sys::Int8_T> >(
        const std::complex<sys::Int8_T>* data,
        size_t numElements);

template
void CPHDWriter::writeCPHDData<std::complex<sys::Int16_T> >(
        const std::complex<sys::Int16_T>* data,
        size_t numElements);

template
void CPHDWriter::writeCPHDData<std::complex<float> >(
        const std::complex<float>* data,
        size_t numElements);

void CPHDWriter::write(const std::string& pathname,
                       const std::string& classification,
                       const std::string& releaseInfo)
{
    mem::SharedPtr<io::OutputStream> outStream(
            new io::FileOutputStream(pathname));
    write(outStream, classification, releaseInfo);
}

void CPHDWriter::write(mem::SharedPtr<io::OutputStream> outStream,
                       const std::string& classification,
                       const std::string& releaseInfo)
{
    mOutStream = outStream;

    writeMetadata(mVBMSize, mCPHDSize, classification, releaseInfo);

    for (size_t ii = 0; ii < mVBMData.size(); ++ii)
    {
        writeVBMData(mVBMData[ii], ii);
    }

    for (size_t ii = 0; ii < mCPHDData.size(); ++ii)
    {
        const size_t cphdDataSize = mMetadata.data.arraySize[ii].numVectors *
                mMetadata.data.arraySize[ii].numSamples;
        writeCPHDDataImpl(mCPHDData[ii], cphdDataSize);
    }

    mOutStream->close();
}
}

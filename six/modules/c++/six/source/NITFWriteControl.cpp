/* =========================================================================
 * This file is part of six-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * six-c++ is free software; you can redistribute it and/or modify
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
#include <six/NITFWriteControl.h>

#include <iomanip>
#include <sstream>
#include <string>

#include <std/bit>
#include <std/memory>

#include <io/ByteStream.h>
#include <math/Round.h>
#include <mem/ScopedArray.h>

#include <six/XMLControlFactory.h>
#include <nitf/IOStreamWriter.hpp>


namespace six
{
NITFWriteControl::NITFWriteControl()
{
    mNITFHeaderCreator.reset(new six::NITFHeaderCreator());
}

NITFWriteControl::NITFWriteControl(std::shared_ptr<Container> container)
{
    mNITFHeaderCreator.reset(new six::NITFHeaderCreator(container));
}

NITFWriteControl::NITFWriteControl(const six::Options& options,
                                   std::shared_ptr<Container> container,
                                   const XMLControlRegistry* xmlRegistry)
{
    mNITFHeaderCreator.reset(new six::NITFHeaderCreator(options, container));
    if (xmlRegistry)
    {
        // Indirecting through *Impl to avoid virtual function call in ctor
        setXMLControlRegistryImpl(xmlRegistry);
    }
}

void NITFWriteControl::setXMLControlRegistryImpl(
        const XMLControlRegistry* xmlRegistry)
{
    mNITFHeaderCreator->setXMLControlRegistry(xmlRegistry);
    WriteControl::setXMLControlRegistryImpl(xmlRegistry);
}

void NITFWriteControl::initialize(const six::Options& options,
                                  std::shared_ptr<Container> container)
{
    mNITFHeaderCreator->initialize(options, container);
}

void NITFWriteControl::initialize(std::shared_ptr<Container> container)
{
    mNITFHeaderCreator->initialize(container);
}

void NITFWriteControl::setNITFHeaderCreator(
        std::unique_ptr<six::NITFHeaderCreator>&& headerCreator)
{
    mNITFHeaderCreator.reset(headerCreator.release());
}
#if !CODA_OSS_cpp17
void NITFWriteControl::setNITFHeaderCreator(
        mem::auto_ptr<six::NITFHeaderCreator> headerCreator)
{
    setNITFHeaderCreator(std::unique_ptr<six::NITFHeaderCreator>(headerCreator.release()));
}
#endif

std::string NITFWriteControl::getComplexIID(size_t segmentNum,
                                            size_t numImageSegments)
{
    return NITFHeaderCreator::getComplexIID(segmentNum, numImageSegments);
}

std::string NITFWriteControl::getDerivedIID(size_t segmentNum,
                                            size_t productNum)
{
    return NITFHeaderCreator::getDerivedIID(segmentNum, productNum);
}

std::string NITFWriteControl::getIID(DataType dataType,
                                     size_t segmentNum,
                                     size_t numImageSegments,
                                     size_t productNum)
{
    return NITFHeaderCreator::getIID(dataType,
                                     segmentNum,
                                     numImageSegments,
                                     productNum);
}

void NITFWriteControl::setBlocking(const std::string& imode,
                                   const types::RowCol<size_t>& segmentDims,
                                   nitf::ImageSubheader& subheader)
{
    mNITFHeaderCreator->setBlocking(imode, segmentDims, subheader);
}

void NITFWriteControl::setImageSecurity(const six::Classification& c,
                                        nitf::ImageSubheader& subheader)
{
    mNITFHeaderCreator->setImageSecurity(c, subheader);
}

void NITFWriteControl::setDESecurity(const six::Classification& c,
                                     nitf::DESubheader& subheader)
{
    mNITFHeaderCreator->setDESecurity(c, subheader);
}

void NITFWriteControl::setSecurity(const six::Classification& c,
                                   nitf::FileSecurity security,
                                   const std::string& prefix)
{
    mNITFHeaderCreator->setSecurity(c, security, prefix);
}

std::string NITFWriteControl::getNITFClassification(const std::string& level)
{
    return NITFHeaderCreator::getNITFClassification(level);
}

void NITFWriteControl::updateFileHeaderSecurity()
{
    mNITFHeaderCreator->updateFileHeaderSecurity();
}

void NITFWriteControl::save(const SourceList& imageData,
                            const std::string& outputFile,
                            const std::vector<std::string>& schemaPaths)
{
    const size_t bufferSize = getOptions().getParameter(
            six::WriteControl::OPT_BUFFER_SIZE,
            Parameter(NITFHeaderCreator::DEFAULT_BUFFER_SIZE));

    nitf::BufferedWriter bufferedIO(outputFile, bufferSize);

    save(imageData, bufferedIO, schemaPaths);
    bufferedIO.close();
}

bool NITFWriteControl::shouldByteSwap() const
{
    const int byteSwapping = (int)getOptions().getParameter(
            six::WriteControl::OPT_BYTE_SWAP,
            Parameter(static_cast<int>(ByteSwapping::SWAP_AUTO)));

    if (byteSwapping == ByteSwapping::SWAP_AUTO)
    {
        // Have to if it's not a BE machine
        static auto endianness = std::endian::native;
        return endianness == std::endian::little;
    }
    else
    {
        // Do what they say.  You really shouldn't do this
        // unless you know what you're doing anyway!
        return byteSwapping ? true : false;
    }
}

void NITFWriteControl::save(const SourceList& imageData,
                            nitf::IOInterface& outputFile,
                            const std::vector<std::string>& schemaPaths)
{
    nitf::Record& record = getRecord();
    mWriter.prepareIO(outputFile, record);
    const bool doByteSwap = shouldByteSwap();

    const auto& infos = getInfos();
    if (infos.size() != imageData.size())
    {
        std::ostringstream ostr;
        ostr << "Require " << infos.size() << " images, received "
             << imageData.size();
        throw except::Exception(Ctxt(ostr.str()));
    }

    const size_t numImages = infos.size();

    //! TODO: This section of code (unlike the memory section below)
    //        does not account for blocked writing or J2K compression.
    //        CODA ticket #443 will update support for this.
    for (size_t i = 0; i < numImages; ++i)
    {
        const NITFImageInfo& info = *(infos[i]);
        std::vector<NITFSegmentInfo> imageSegments = info.getImageSegments();
        const size_t numIS = imageSegments.size();
        size_t pixelSize = info.getData()->getNumBytesPerPixel();
        size_t numCols = info.getData()->getNumCols();
        size_t numChannels = info.getData()->getNumChannels();

        for (size_t j = 0; j < numIS; ++j)
        {
            NITFSegmentInfo segmentInfo = imageSegments[j];

            auto writeHandler(
                std::make_shared<StreamWriteHandler>(segmentInfo,
                                           imageData[i],
                                           numCols,
                                           numChannels,
                                           pixelSize,
                                           doByteSwap));

            mWriter.setImageWriteHandler(static_cast<int>(info.getStartIndex() +
                                                          j),
                                         writeHandler);
        }
    }

    addDataAndWrite(schemaPaths);
}

template<typename TBufferList>
void NITFWriteControl::save_(const TBufferList& imageData,
                            const std::string& outputFile,
                            const std::vector<std::string>& schemaPaths)
{
    const size_t bufferSize = getOptions().getParameter(
            WriteControl::OPT_BUFFER_SIZE,
            Parameter(NITFHeaderCreator::DEFAULT_BUFFER_SIZE));
    nitf::BufferedWriter bufferedIO(outputFile, bufferSize);

    save(imageData, bufferedIO, schemaPaths);
    bufferedIO.close();
}
void NITFWriteControl::save(const BufferList& imageData,
                            const std::string& outputFile,
                            const std::vector<std::string>& schemaPaths)
{
    save_(imageData, outputFile, schemaPaths);
}
void NITFWriteControl::save(const buffer_list& imageData,
                            const std::string& outputFile,
                            const std::vector<std::string>& schemaPaths)
{
    save_(imageData, outputFile, schemaPaths);
}

template<typename TBufferList>
void NITFWriteControl::save_(const TBufferList& imageData,
                            nitf::IOInterface& outputFile,
                            const std::vector<std::string>& schemaPaths)
{
    nitf::Record& record = getRecord();
    mWriter.prepareIO(outputFile, record);
    const bool doByteSwap = shouldByteSwap();

    if (getInfos().size() != imageData.size())
        throw except::Exception(
                Ctxt("Require " + std::to_string(getInfos().size()) +
                     " images, received " + std::to_string(imageData.size())));

    // check to see if J2K compression is enabled
    double j2kCompression = (double)getOptions().getParameter(
            NITFHeaderCreator::OPT_J2K_COMPRESSION_BYTERATE, Parameter(0));

    bool enableJ2K = (getContainer()->getDataType() != DataType::COMPLEX) &&
            (j2kCompression <= 1.0) && j2kCompression > 0.0001;

    // TODO maybe we need to see if the compression plug-in is even available

    size_t numImages = getInfos().size();
    createCompressionOptions(mCompressionOptions);
    for (size_t i = 0; i < numImages; ++i)
    {
        const auto pInfo = getInfo(i);
        const NITFImageInfo& info = *pInfo;
        std::vector<NITFSegmentInfo> imageSegments = info.getImageSegments();
        const size_t numIS = imageSegments.size();
        const size_t numChannels = info.getData()->getNumChannels();
        const auto pixelSize = info.getData()->getNumBytesPerPixel() / numChannels;
        const size_t numCols = info.getData()->getNumCols();

        nitf::ImageSegment imageSegment =
                getRecord().getImages()[info.getStartIndex()];
        nitf::ImageSubheader subheader = imageSegment.getSubheader();

        const bool isBlocking = subheader.numBlocksPerRow() > 1 || subheader.numBlocksPerCol() > 1;

        // The SIDD spec requires that a J2K compressed SIDDs be only a
        // single image segment. However this functionality remains untested.
        if (isBlocking || (enableJ2K && numIS == 1) ||
            !mCompressionOptions.empty())
        {
            if ((isBlocking || (enableJ2K && numIS == 1)) &&
                info.getData()->getDataType() == six::DataType::COMPLEX)
            {
                throw except::Exception(Ctxt("SICD does not support blocked or "
                                             "J2K compressed output"));
            }

            for (size_t jj = 0; jj < numIS; ++jj)
            {
                // We will use the ImageWriter provided by NITRO so that we can
                // take advantage of the built-in compression capabilities
                nitf::ImageWriter iWriter = mWriter.newImageWriter(
                        static_cast<int>(info.getStartIndex() + jj),
                        mCompressionOptions);
                iWriter.setWriteCaching(1);

                nitf::ImageSource iSource;
                const NITFSegmentInfo segmentInfo = imageSegments[jj];
                const size_t bandSize =
                        pixelSize * numCols * segmentInfo.getNumRows();

                for (size_t chan = 0; chan < numChannels; ++chan)
                {
                    // Assume that the bands are interleaved in memory.  This
                    // makes sense for 24-bit 3-color data.
                    const auto data = imageData[i] + pixelSize *  segmentInfo.getFirstRow() * numCols;
                    const auto start = gsl::narrow<nitf::Off>(chan);
                    const auto numBytesPerPixel = gsl::narrow<int>(pixelSize);
                    const auto pixelSkip = gsl::narrow<int>(numChannels - 1);
                    nitf::MemorySource ms(data, bandSize, start, numBytesPerPixel, pixelSkip);
                    iSource.addBand(ms);
                }
                iWriter.attachSource(iSource);
            }
        }
        else
        {
            // this bypasses the normal NITF ImageWriter and streams directly
            // to the output
            for (size_t jj = 0; jj < numIS; ++jj)
            {
                const NITFSegmentInfo segmentInfo = imageSegments[jj];

                auto writeHandler(
                        std::make_shared<MemoryWriteHandler>(segmentInfo,
                                               imageData[i],
                                               segmentInfo.getFirstRow(),
                                               numCols,
                                               numChannels,
                                               pixelSize * numChannels,
                                               doByteSwap));
                // Could set start index here
                mWriter.setImageWriteHandler(static_cast<int>(
                                                     info.getStartIndex() + jj),
                                             writeHandler);
            }
        }

        const Legend* const legend = getContainer()->getLegend(i);
        if (legend)
        {
            if (legend->mDims.row * legend->mDims.col != legend->mImage.size())
            {
                throw except::Exception(Ctxt("Legend dimensions don't match"));
            }

            if (legend->mImage.empty())
            {
                throw except::Exception(Ctxt("Empty legend"));
            }

            nitf::ImageSource iSource;

            nitf::MemorySource memSource(legend->mImage.data(),
                                         legend->mImage.size(),
                                         0,
                                         sizeof(std::byte),
                                         0);

            iSource.addBand(memSource);

            nitf::ImageWriter iWriter = mWriter.newImageWriter(
                    static_cast<int>(info.getStartIndex() + numIS));
            iWriter.setWriteCaching(1);
            iWriter.attachSource(iSource);
        }
    }

    addDataAndWrite(schemaPaths);
}
void NITFWriteControl::save(const BufferList& imageData,
                            nitf::IOInterface& outputFile,
                            const std::vector<std::string>& schemaPaths)
{
    save_(imageData, outputFile, schemaPaths);
}
void NITFWriteControl::save(const buffer_list& imageData,
    nitf::IOInterface& outputFile,
    const std::vector<std::string>& schemaPaths)
{
    save_(imageData, outputFile, schemaPaths);
}

void NITFWriteControl::addDataAndWrite(
        const std::vector<std::string>& schemaPaths)
{
    const size_t numDES = getContainer()->getNumData();

    // These must stick around until mWriter.write() is called since the
    // SegmentMemorySource's will be pointing to them
    const auto desStrs = std::make_unique<std::string[]>(numDES);

    for (size_t ii = 0; ii < getContainer()->getNumData(); ++ii)
    {
        const Data* data = getContainer()->getData(ii);
        std::string& desStr(desStrs[ii]);

        desStr = six::toValidXMLString(data, schemaPaths, mLog, mXMLRegistry);
        nitf::SegmentWriter deWriter =
                mWriter.newDEWriter(static_cast<int>(ii));
        nitf::SegmentMemorySource segSource(desStr, 0, 0, false);
        deWriter.attachSource(segSource);
    }

    int deWriterIndex = static_cast<int>(getContainer()->getNumData());
    for (size_t ii = 0; ii < getSegmentWriters().size(); ++ii)
    {
        mWriter.setDEWriteHandler(deWriterIndex++, getSegmentWriters()[ii]);
    }
    mWriter.write();
}

std::string NITFWriteControl::getDesTypeID(const six::Data& data)
{
    return NITFHeaderCreator::getDesTypeID(data);
}

bool NITFWriteControl::needUserDefinedSubheader(const six::Data& data)
{
    return NITFHeaderCreator::needUserDefinedSubheader(data);
}

void NITFWriteControl::setOrganizationId(const std::string& organizationId)
{
    mNITFHeaderCreator->setOrganizationId(organizationId);
}

void NITFWriteControl::setLocationIdentifier(
        const std::string& locationId, const std::string& locationIdNamespace)
{
    mNITFHeaderCreator->setLocationIdentifier(locationId, locationIdNamespace);
}

void NITFWriteControl::setAbstract(const std::string& abstract)
{
    mNITFHeaderCreator->setAbstract(abstract);
}

void NITFWriteControl::addUserDefinedSubheader(
        const six::Data& data, nitf::DESubheader& subheader) const
{
    mNITFHeaderCreator->addUserDefinedSubheader(data, subheader);
}

void NITFWriteControl::addAdditionalDES(
       std::shared_ptr<nitf::SegmentWriter> segmentWriter)
{
    mNITFHeaderCreator->addAdditionalDES(segmentWriter);
}
}

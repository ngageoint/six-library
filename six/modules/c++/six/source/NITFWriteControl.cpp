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
#include <algorithm>
#include <type_traits>
#include <stdexcept>

#include <io/ByteStream.h>
#include <math/Round.h>
#include <mem/ScopedArray.h>
#include <gsl/gsl.h>

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
NITFWriteControl::NITFWriteControl(std::unique_ptr<Data>&& pData)
    : NITFWriteControl(std::make_shared<six::Container>(std::move(pData)))
{
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

inline size_t getBandSize(const NITFSegmentInfo& segmentInfo, const Data& data)
{
    const auto pixelSize = data.getNumBytesPerPixel() / data.getNumChannels();
    const auto numCols = data.getNumCols();
    const auto bandSize = pixelSize * numCols * segmentInfo.getNumRows();
    return bandSize;
}

inline std::span<const std::byte> as_bytes(BufferList::value_type pImageData,
    const NITFSegmentInfo& segmentInfo, const Data& data)
{
    const auto bandSize = getBandSize(segmentInfo, data);
    const void* pImageData_ = pImageData;
    auto size_in_bytes = bandSize * data.getNumChannels();

    // At this point, we've lost information about the ACTUAL size of the buffer. Normally, the computation above will be correct.
    // But in the case of AMP8I_PHS8I (now supported), the buffer is actually RE32F_IM32F as the data is converted to
    // std::complex<float> when read-in, and converted to std::pair<uint8_t, uint8_t> when written-out.
    if (data.getPixelType() == six::PixelType::AMP8I_PHS8I)
    {
        size_in_bytes *= sizeof(float);
    }

    return std::span<const std::byte>(static_cast<const std::byte*>(pImageData_), size_in_bytes);
}

// this bypasses the normal NITF ImageWriter and streams directly to the output
template<typename T>
inline std::shared_ptr<NewMemoryWriteHandler> makeWriteHandler(const NITFSegmentInfo& segmentInfo,
    std::span<const T> imageData, const Data& data, bool doByteSwap)
{
    return std::make_shared<NewMemoryWriteHandler>(segmentInfo,
        imageData, segmentInfo.getFirstRow(), data, doByteSwap);
}
inline std::shared_ptr<NewMemoryWriteHandler> makeWriteHandler(const NITFSegmentInfo& segmentInfo,
    BufferList::value_type pImageData, const Data& data, bool doByteSwap)
{
    const auto pImageData_ = as_bytes(pImageData, segmentInfo, data);
    return makeWriteHandler(segmentInfo, pImageData_, data, doByteSwap);
}

inline std::shared_ptr<StreamWriteHandler> makeWriteHandler(NITFSegmentInfo segmentInfo,
io::InputStream* imageData, const Data& data, bool doByteSwap)
{
//! TODO: This section of code (unlike the memory section above)
//        does not account for blocked writing or J2K compression.
//        CODA ticket #443 will update support for this.
return std::make_shared<StreamWriteHandler>(segmentInfo, imageData, data, doByteSwap);
}

template<typename TImageData>
void writeWithoutNitro(nitf::Writer& mWriter, const TImageData& imageData,
    const std::vector<NITFSegmentInfo>& imageSegments, size_t startIndex, const Data& data, bool doByteSwap)
{
    for (size_t j = 0; j < imageSegments.size(); ++j)
    {
        auto writeHandler = makeWriteHandler(imageSegments[j], imageData, data, doByteSwap);
        mWriter.setImageWriteHandler(static_cast<int>(startIndex + j), writeHandler);
    }
}

bool NITFWriteControl::do_prepareIO(size_t imageDataSize, nitf::IOInterface& outputFile)
{
    const auto& infos = getInfos();
    if (infos.size() != imageDataSize)
    {
        std::ostringstream ostr;
        ostr << "Require " << infos.size() << " images, received " << imageDataSize;
        throw except::Exception(Ctxt(ostr.str()));
    }

    nitf::Record& record = getRecord();
    mWriter.prepareIO(outputFile, record);
    return shouldByteSwap();
}

void NITFWriteControl::save(const SourceList& imageData,
    nitf::IOInterface& outputFile,
    const std::vector<std::string>& schemaPaths)
{
    const bool doByteSwap = do_prepareIO(imageData.size(), outputFile);
    const auto& infos = getInfos();

    //! TODO: This section of code (unlike the memory section below)
    //        does not account for blocked writing or J2K compression.
    //        CODA ticket #443 will update support for this.
    for (size_t i = 0; i < infos.size(); ++i)
    {
        const NITFImageInfo& info = *(infos[i]);
        const std::vector<NITFSegmentInfo> imageSegments = info.getImageSegments();
        const auto startIndex = info.getStartIndex();
        const six::Data* const pData = info.getData();

        writeWithoutNitro(mWriter, imageData[i], imageSegments, startIndex, *pData, doByteSwap);
    }

    addDataAndWrite(schemaPaths);
}

static nitf::ImageSource make_ImageSource_(std::span<const std::byte> pData, size_t numChannels, size_t pixelSize)
{
    const auto numBytesPerPixel = gsl::narrow<int>(pixelSize);

    nitf::ImageSource retval;
    for (size_t chan = 0; chan < numChannels; ++chan)
    {
        // Assume that the bands are interleaved in memory.  This
        // makes sense for 24-bit 3-color data.
        const auto start = gsl::narrow<nitf::Off>(chan);
        const auto pixelSkip = gsl::narrow<int>(numChannels - 1);
        nitf::MemorySource ms(pData, start, numBytesPerPixel, pixelSkip);
        retval.addBand(ms);
    }
    return retval;
}

template<typename T>
static nitf::ImageSource make_ImageSource(std::span<const T> pImageData_, const NITFSegmentInfo& segmentInfo, const Data& data)
{
    constexpr auto numBytesPerPixel = sizeof(pImageData_[0]);

    const auto numChannels = data.getNumChannels();
    const auto pixelSize = data.getNumBytesPerPixel() / numChannels;
    if (pixelSize != numBytesPerPixel)
    {
        throw std::invalid_argument("numBytesPerPixel mis-match!");
    }

    const auto bandSize = getBandSize(segmentInfo, data);
    const auto pImageData = six::as_bytes(pImageData_);
    if (pImageData.size() != bandSize)
    {
        throw std::invalid_argument("bandSize mis-match!");
    }

    const auto numCols = data.getNumCols();
    const auto pData_ = pImageData.data() + pixelSize * segmentInfo.getFirstRow() * numCols;
    const  std::span<const std::byte> pData(pData_, bandSize);
    return make_ImageSource_(pData, numChannels, pixelSize);
}

// Existing code that uses BufferList needs raw "std::byte*" instead of std::span<std::byte>
static nitf::ImageSource make_ImageSource(const BufferList::value_type pImageData, const NITFSegmentInfo& segmentInfo, const Data& data)
{
    const auto pImageData_ = as_bytes(pImageData, segmentInfo, data);
    return make_ImageSource(pImageData_, segmentInfo, data);
}

template<typename TImageData>
void writeWithNitro(nitf::Writer& mWriter, const std::map<std::string, void*>& mCompressionOptions,
    const TImageData& imageData, const std::vector<NITFSegmentInfo>& imageSegments, size_t startIndex, const Data& data)
{
    const auto numChannels = data.getNumChannels();
    const auto pixelSize = data.getNumBytesPerPixel() / numChannels;

    for (size_t jj = 0; jj < imageSegments.size(); ++jj)
    {
        // We will use the ImageWriter provided by NITRO so that we can
        // take advantage of the built-in compression capabilities
        nitf::ImageWriter iWriter = mWriter.newImageWriter(static_cast<int>(startIndex + jj), mCompressionOptions);

        // We will use the ImageWriter provided by NITRO so that we can
        // take advantage of the built-in compression capabilities
        iWriter.setWriteCaching(1);

        const NITFSegmentInfo segmentInfo = imageSegments[jj];
        auto iSource = make_ImageSource(imageData, segmentInfo, data);
        iWriter.attachSource(iSource);
    }
}

static const Legend* getLegend(const six::Container* container, size_t i)
{
    const auto legend = container->getLegend(i);
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
    }
    return legend;
}
void NITFWriteControl::addLegend(const Legend& legend, int imageNumber)
{
    nitf::ImageSource iSource;
    nitf::MemorySource memSource(legend.mImage, 0 /*start*/, 0 /*pixelSkip*/);
    iSource.addBand(memSource);

    nitf::ImageWriter iWriter = mWriter.newImageWriter(imageNumber);
    iWriter.setWriteCaching(1);
    iWriter.attachSource(iSource);
}

template<typename T>
inline size_t get_imageDataSize(const T&) 
{
    return 1;
}
inline size_t get_imageDataSize(const BufferList& imageData)
{
    return imageData.size();
}
template<typename T>
bool NITFWriteControl::prepareIO(const T& imageData, nitf::IOInterface& outputFile)
{
    return do_prepareIO(get_imageDataSize(imageData), outputFile);
}

template<typename T>
void NITFWriteControl::write_imageData(const T& imageData, const NITFImageInfo& info, const Legend* const legend,
    bool doByteSwap, bool enableJ2K)
{
    const std::vector<NITFSegmentInfo> imageSegments = info.getImageSegments();
    const six::Data* const pData = info.getData();

    const auto startIndex = info.getStartIndex();
    nitf::ImageSegment imageSegment = getRecord().getImages()[startIndex];
    nitf::ImageSubheader subheader = imageSegment.getSubheader();

    const bool isBlocking = subheader.numBlocksPerRow() > 1 || subheader.numBlocksPerCol() > 1;

    // The SIDD spec requires that a J2K compressed SIDDs be only a
    // single image segment. However this functionality remains untested.
    const auto numIS = imageSegments.size();
    if (isBlocking || (enableJ2K && numIS == 1) || !mCompressionOptions.empty())
    {
        if ((isBlocking || (enableJ2K && numIS == 1)) && (pData->getDataType() == six::DataType::COMPLEX))
        {
            throw except::Exception(Ctxt("SICD does not support blocked or J2K compressed output"));
        }

        writeWithNitro(mWriter, mCompressionOptions, imageData, imageSegments, startIndex, *pData);
    }
    else
    {
        writeWithoutNitro(mWriter, imageData, imageSegments, startIndex, *pData, doByteSwap);
    }

    if (legend)
    {
        addLegend(*legend, static_cast<int>(startIndex + numIS));
    }
}

template<typename T>
void NITFWriteControl::do_save(const T& imageData, nitf::IOInterface& outputFile, const std::vector<std::string>& schemaPaths)
{
    const bool doByteSwap = prepareIO(imageData, outputFile);

    // check to see if J2K compression is enabled
    double j2kCompression = (double)getOptions().getParameter(
        NITFHeaderCreator::OPT_J2K_COMPRESSION_BYTERATE, Parameter(0));

    bool enableJ2K = (getContainer()->getDataType() != DataType::COMPLEX) &&
        (j2kCompression <= 1.0) && j2kCompression > 0.0001;

    // TODO maybe we need to see if the compression plug-in is even available
    createCompressionOptions(mCompressionOptions);

    do_save_(imageData, doByteSwap, enableJ2K);

    addDataAndWrite(schemaPaths);
}

template<>
void NITFWriteControl::do_save_(const BufferList& list, bool doByteSwap, bool enableJ2K)
{
    size_t numImages = getInfos().size();
    for (size_t i = 0; i < numImages; ++i)
    {
        const auto pInfo = getInfo(i);
        const Legend* const legend = getLegend(getContainer().get(), i);

        write_imageData(list[i], *pInfo, legend, doByteSwap, enableJ2K);
    }
}
void NITFWriteControl::save_buffer_list(const BufferList& imageData, nitf::IOInterface& outputFile, const std::vector<std::string>& schemaPaths)
{
    do_save(imageData, outputFile, schemaPaths);
}

template<typename T>
void NITFWriteControl::do_save_(const T& imageData, bool doByteSwap, bool enableJ2K)
{
    size_t numImages = getInfos().size();
    if (numImages > 1)
    {
        throw std::invalid_argument("Should only have one image!"); // we would be in do_save_(BufferList)
    }
    if (numImages == 1) // can we really have 0 images?
    {
        constexpr size_t i = 0; // keep code consistent with do_save_(BufferList)

        const auto pInfo = getInfo(i);
        const Legend* const legend = getLegend(getContainer().get(), i);

        write_imageData(imageData, *pInfo, legend, doByteSwap, enableJ2K);
    }
}
template<>
void NITFWriteControl::save_image(std::span<const std::complex<float>> imageData,
                            nitf::IOInterface& outputFile,
                            const std::vector<std::string>& schemaPaths)
{
    do_save(imageData, outputFile, schemaPaths);
}
template<>
void NITFWriteControl::save_image(std::span<const std::complex<short>> imageData,
    nitf::IOInterface& outputFile,
    const std::vector<std::string>& schemaPaths)
{
    do_save(imageData, outputFile, schemaPaths);
}
template<>
void NITFWriteControl::save_image(std::span<const std::pair<uint8_t, uint8_t>> imageData,
                            nitf::IOInterface& outputFile,
                            const std::vector<std::string>& schemaPaths)
{
    do_save(imageData, outputFile, schemaPaths);
}
template<>
void NITFWriteControl::save_image(std::span<const uint8_t> imageData,
    nitf::IOInterface& outputFile,
    const std::vector<std::string>& schemaPaths)
{
    do_save(imageData, outputFile, schemaPaths);
}
template<>
void NITFWriteControl::save_image(std::span<const uint16_t> imageData,
    nitf::IOInterface& outputFile,
    const std::vector<std::string>& schemaPaths)
{
    do_save(imageData, outputFile, schemaPaths);
}

void NITFWriteControl::save(const BufferList& list, const std::string& outputFile, const std::vector<std::string>& schemaPaths)
{
    save_buffer_list_to_file(list, outputFile, schemaPaths);
}
void NITFWriteControl::save(const std::complex<float>* image, const std::string& outputFile, const std::vector<std::string>& schemaPaths)
{
    // Keeping this code-path in place as it's an easy way to test legacy BufferList functionality.
    const void* pImage = image;
    const BufferList list{ static_cast<const UByte*>(pImage) };
    save(list, outputFile, schemaPaths);
}

void NITFWriteControl::addDataAndWrite(const std::vector<std::string>& schemaPaths)
{
    const auto numDES = getContainer()->size();

    // These must stick around until mWriter.write() is called since the
    // SegmentMemorySource's will be pointing to them
    const auto desStrs = std::make_unique<std::string[]>(numDES);

    for (size_t ii = 0; ii < numDES; ++ii)
    {
        const Data* data = getContainer()->getData(ii);

        desStrs[ii] = six::toValidXMLString(data, schemaPaths, mLog, mXMLRegistry);
        nitf::SegmentWriter deWriter = mWriter.newDEWriter(gsl::narrow<int>(ii));
        nitf::SegmentMemorySource segSource(desStrs[ii], 0, 0, false);
        deWriter.attachSource(segSource);
    }

    auto deWriterIndex = gsl::narrow<int>(numDES);
    for (auto segmentWriter : getSegmentWriters())
    {
        mWriter.setDEWriteHandler(deWriterIndex++, segmentWriter);
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


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

#include <iomanip>
#include <sstream>

#include <math/Round.h>
#include <mem/ScopedArray.h>
#include <io/ByteStream.h>
#include <nitf/IOStreamWriter.hpp>
#include <six/NITFWriteControl.h>
#include <six/XMLControlFactory.h>

namespace six
{
NITFWriteControl::NITFWriteControl()
{
    mNITFHeaderCreator.reset(new six::NITFHeaderCreator());
}

NITFWriteControl::NITFWriteControl(mem::SharedPtr<Container> container)
{
    mNITFHeaderCreator.reset(new six::NITFHeaderCreator(container));
}

NITFWriteControl::NITFWriteControl(const six::Options& options,
                                   mem::SharedPtr<Container> container,
                                   const XMLControlRegistry* xmlRegistry)
{
    mNITFHeaderCreator.reset(new six::NITFHeaderCreator(options, container));
    if (xmlRegistry)
    {
        // Indirecting through *Impl to avoid virtual function call in ctor
        setXMLControlRegistryImpl(xmlRegistry);
    }
}

void NITFWriteControl::setXMLControlRegistryImpl(const XMLControlRegistry* xmlRegistry)
{
    mNITFHeaderCreator->setXMLControlRegistry(xmlRegistry);
    WriteControl::setXMLControlRegistryImpl(xmlRegistry);
}

void NITFWriteControl::initialize(const six::Options& options,
                                  mem::SharedPtr<Container> container)
{
    mNITFHeaderCreator->initialize(options, container);
}

void NITFWriteControl::initialize(mem::SharedPtr<Container> container)
{
    mNITFHeaderCreator->initialize(container);
}

void NITFWriteControl::setNITFHeaderCreator(
    std::auto_ptr<six::NITFHeaderCreator> headerCreator)
{
    mNITFHeaderCreator.reset(headerCreator.release());
}

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
    return NITFHeaderCreator::getIID(dataType, segmentNum, numImageSegments,
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
    const size_t bufferSize =
            getOptions().getParameter(six::WriteControl::OPT_BUFFER_SIZE,
                                      Parameter(NITFHeaderCreator::DEFAULT_BUFFER_SIZE));

    nitf::BufferedWriter bufferedIO(outputFile, bufferSize);

    save(imageData, bufferedIO, schemaPaths);
    bufferedIO.close();
}

bool NITFWriteControl::shouldByteSwap() const
{
    bool doByteSwap;

    const int byteSwapping =
        (int) getOptions().getParameter(six::WriteControl::OPT_BYTE_SWAP,
                                        Parameter((int) ByteSwapping::SWAP_AUTO));

    if (byteSwapping == ByteSwapping::SWAP_AUTO)
    {
        // Have to if it's not a BE machine
        doByteSwap = !sys::isBigEndianSystem();
    }
    else
    {
        // Do what they say.  You really shouldn't do this
        // unless you know what you're doing anyway!
        doByteSwap = byteSwapping ? true : false;
    }

    return doByteSwap;
}

void NITFWriteControl::save(
        const SourceList& imageData,
        nitf::IOInterface& outputFile,
        const std::vector<std::string>& schemaPaths)
{
    nitf::Record& record = getRecord();
    mWriter.prepareIO(outputFile, record);
    const bool doByteSwap = shouldByteSwap();

    const std::vector<mem::SharedPtr<NITFImageInfo> >& infos = getInfos();
    if (infos.size() != imageData.size())
    {
        std::ostringstream ostr;
        ostr << "Require " << infos.size() << " images, received "
             << imageData.size();
        throw except::Exception(Ctxt(ostr.str()));
    }

    size_t numImages = infos.size();

    //! TODO: This section of code (unlike the memory section below)
    //        does not account for blocked writing or J2K compression.
    //        CODA ticket #443 will update support for this.
    for (size_t i = 0; i < numImages; ++i)
    {
        const NITFImageInfo& info = *(infos[i]);
        std::vector < NITFSegmentInfo > imageSegments
                = info.getImageSegments();
        size_t numIS = imageSegments.size();
        size_t pixelSize = info.getData()->getNumBytesPerPixel();
        size_t numCols = info.getData()->getNumCols();
        size_t numChannels = info.getData()->getNumChannels();

        for (size_t j = 0; j < numIS; ++j)
        {
            NITFSegmentInfo segmentInfo = imageSegments[j];

            mem::SharedPtr< ::nitf::WriteHandler> writeHandler(
                new StreamWriteHandler (segmentInfo, imageData[i], numCols,
                                        numChannels, pixelSize, doByteSwap));

            mWriter.setImageWriteHandler(
                    static_cast<int>(info.getStartIndex() + j),
                    writeHandler);
        }
    }

    addDataAndWrite(schemaPaths);
}

void NITFWriteControl::save(const BufferList& imageData,
                            const std::string& outputFile,
                            const std::vector<std::string>& schemaPaths)
{
    const size_t bufferSize =
            getOptions().getParameter(WriteControl::OPT_BUFFER_SIZE,
                                      Parameter(NITFHeaderCreator::DEFAULT_BUFFER_SIZE));
    nitf::BufferedWriter bufferedIO(outputFile, bufferSize);

    save(imageData, bufferedIO, schemaPaths);
    bufferedIO.close();
}

void NITFWriteControl::save(
        const BufferList& imageData,
        nitf::IOInterface& outputFile,
        const std::vector<std::string>& schemaPaths)
{
    nitf::Record& record = getRecord();
    mWriter.prepareIO(outputFile, record);
    const bool doByteSwap = shouldByteSwap();

    if (getInfos().size() != imageData.size())
        throw except::Exception(Ctxt("Require " +
                str::toString(getInfos().size())
                + " images, received " + str::toString(imageData.size())));

    // check to see if J2K compression is enabled
    double j2kCompression = (double)getOptions().getParameter(
            NITFHeaderCreator::OPT_J2K_COMPRESSION_BYTERATE, Parameter(0));

    bool enableJ2K = (getContainer()->getDataType() != DataType::COMPLEX) &&
            (j2kCompression <= 1.0) && j2kCompression > 0.0001;

    //TODO maybe we need to see if the compression plug-in is even available

    size_t numImages = getInfos().size();
    createCompressionOptions(mCompressionOptions);
    for (size_t i = 0; i < numImages; ++i)
    {
        const NITFImageInfo& info = *(getInfos()[i]);
        std::vector < NITFSegmentInfo > imageSegments
                = info.getImageSegments();
        const size_t numIS = imageSegments.size();
        const int pixelSize = static_cast<int>(
                info.getData()->getNumBytesPerPixel());
        const size_t numCols = info.getData()->getNumCols();
        const size_t numChannels = info.getData()->getNumChannels();

        nitf::ImageSegment imageSegment =
                getRecord().getImages()[info.getStartIndex()];
        nitf::ImageSubheader subheader = imageSegment.getSubheader();

        const bool isBlocking =
            static_cast<nitf::Uint32>(subheader.getNumBlocksPerRow()) > 1 ||
            static_cast<nitf::Uint32>(subheader.getNumBlocksPerCol()) > 1;

        // The SIDD spec requires that a J2K compressed SIDDs be only a
        // single image segment. However this functionality remains untested.
        if (isBlocking || (enableJ2K && numIS == 1) ||
            !mCompressionOptions.empty())
        {
            if ((isBlocking || (enableJ2K && numIS == 1)) &&
                info.getData()->getDataType() == six::DataType::COMPLEX)
            {
                throw except::Exception(Ctxt(
                    "SICD does not support blocked or J2K compressed output"));
            }

            for (size_t jj = 0; jj < numIS; ++jj)
            {
                // We will use the ImageWriter provided by NITRO so that we can
                // take advantage of the built-in compression capabilities
                nitf::ImageWriter iWriter =
                    mWriter.newImageWriter(
                            static_cast<int>(info.getStartIndex() + jj),
                            mCompressionOptions);
                iWriter.setWriteCaching(1);

                nitf::ImageSource iSource;
                const NITFSegmentInfo segmentInfo = imageSegments[jj];
                const size_t bandSize =
                        pixelSize * numCols * segmentInfo.numRows;

                for (size_t chan = 0; chan < numChannels; ++chan)
                {
                    nitf::MemorySource ms(imageData[i] +
                            pixelSize * segmentInfo.firstRow * numCols,
                            bandSize,
                            bandSize * chan,
                            pixelSize, 0);
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

                mem::SharedPtr< ::nitf::WriteHandler> writeHandler(
                    new MemoryWriteHandler(segmentInfo, imageData[i],
                                           segmentInfo.firstRow, numCols,
                                           numChannels, pixelSize, doByteSwap));
                // Could set start index here
                mWriter.setImageWriteHandler(
                        static_cast<int>(info.getStartIndex() + jj),
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

            nitf::MemorySource memSource(&legend->mImage[0],
                                         legend->mImage.size(),
                                         0,
                                         sizeof(sys::ubyte),
                                         0);

            iSource.addBand(memSource);

            nitf::ImageWriter iWriter =
                mWriter.newImageWriter(static_cast<int>(
                        info.getStartIndex() + numIS));
            iWriter.setWriteCaching(1);
            iWriter.attachSource(iSource);
        }
    }

    addDataAndWrite(schemaPaths);
}

void NITFWriteControl::addDataAndWrite(
        const std::vector<std::string>& schemaPaths)
{
    const size_t numDES = getContainer()->getNumData();

    // These must stick around until mWriter.write() is called since the
    // SegmentMemorySource's will be pointing to them
    const mem::ScopedArray<std::string> desStrs(new std::string[numDES]);

    for (size_t ii = 0; ii < getContainer()->getNumData(); ++ii)
    {
        const Data* data = getContainer()->getData(ii);
        std::string& desStr(desStrs[ii]);

        desStr = six::toValidXMLString(data, schemaPaths, mLog, mXMLRegistry);
        nitf::SegmentWriter deWriter = mWriter.newDEWriter(static_cast<int>(ii));
        nitf::SegmentMemorySource segSource(desStr.c_str(),
                                            desStr.length(),
                                            0,
                                            0,
                                            false);
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
    const std::string& locationId,
    const std::string& locationIdNamespace)
{
    mNITFHeaderCreator->setLocationIdentifier(locationId, locationIdNamespace);
}

void NITFWriteControl::setAbstract(const std::string& abstract)
{
    mNITFHeaderCreator->setAbstract(abstract);
}

void NITFWriteControl::addUserDefinedSubheader(
        const six::Data& data,
        nitf::DESubheader& subheader) const
{
    mNITFHeaderCreator->addUserDefinedSubheader(data, subheader);
}

void NITFWriteControl::addAdditionalDES(mem::SharedPtr<nitf::SegmentWriter> segmentWriter)
{
    mNITFHeaderCreator->addAdditionalDES(segmentWriter);
}
}

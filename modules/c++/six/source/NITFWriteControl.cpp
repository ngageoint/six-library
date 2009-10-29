/* =========================================================================
 * This file is part of six-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
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
#include "six/NITFWriteControl.h"
#include "six/XMLControlFactory.h"

using namespace six;

const char NITFWriteControl::OPT_MAX_PRODUCT_SIZE[] = "MaxProductSize";
const char NITFWriteControl::OPT_MAX_ILOC_ROWS[] = "MaxILOCRows";

void NITFWriteControl::initialize(Container* container)
{
    mContainer = container;

    sys::Uint32_T ilocMax = Constants::ILOC_MAX;
    // TODO: Validate they are below this!
    sys::Uint32_T maxRows = mOptions.getParameter(OPT_MAX_ILOC_ROWS, Parameter(
            ilocMax));

    sys::Uint64_T maxSize = (sys::Uint64_T) mOptions.getParameter(
            OPT_MAX_PRODUCT_SIZE, Parameter(six::Constants::IS_SIZE_MAX));

    /*
     *  If our container is SICD, must have only one image.
     *
     *  However, if our container is SIDD, we can have more than one
     *  image, and not every Data necessarily needs an image
     *
     */
    if (mContainer->getDataType() == TYPE_COMPLEX)
    {
        mInfos.push_back(new NITFImageInfo(mContainer->getData(0), maxRows,
                maxSize, true));
    }
    else
    {
        for (unsigned int i = 0; i < mContainer->getNumData(); ++i)
        {
            Data* ith = mContainer->getData(i);
            if (ith->getDataClass() == DATA_DERIVED)
                mInfos.push_back(new NITFImageInfo(ith, maxRows, maxSize, true));
        }
    }
    mRecord = nitf::Record(NITF_VER_21);

    DataType dataType = mContainer->getDataType();
    std::string name = mInfos[0]->getData()->getName();
    std::string fileTitle = FmtX("%s: %s", str::toString(dataType).c_str(),
            name.c_str());

    mRecord.getHeader().getFileTitle().set(fileTitle);

    int startIndex = 0;
    for (unsigned int i = 0; i < mInfos.size(); ++i)
    {
        NITFImageInfo* info = mInfos[i];

        std::vector<NITFSegmentInfo> imageSegments = info->getImageSegments();

        size_t numIS = imageSegments.size();
        int nbpp = info->getNumBitsPerPixel();
        int numCols = info->getData()->getNumCols();
        std::string irep = info->getRepresentation();
        std::string imode = info->getMode();
        std::string pvtype = info->getPixelValueType();
        // NITRO wants to see this, not our corners object
        double corners[4][2];

        // Update this info's startIndex
        info->setStartIndex(startIndex);
        startIndex += numIS;

        for (unsigned int j = 0; j < numIS; j++)
        {
            NITFSegmentInfo segmentInfo = imageSegments[j];

            nitf::ImageSegment imageSegment = mRecord.newImageSegment();
            nitf::ImageSubheader subheader = imageSegment.getSubheader();

            //numIS > 1?i+1:i));
            subheader.getImageTitle().set(fileTitle);
            DateTime creationTime = info->getData()->getCreationTime();
            subheader.getImageDateAndTime().set(creationTime);

            std::string iid = str::toString(dataType);
            if (dataType == TYPE_COMPLEX)
            {
                iid = FmtX("%s%03d", iid.c_str(), numIS > 1 ? j + 1 : j);
            }
            else
            {
                iid = FmtX("%s%03d%03d", iid.c_str(), i + 1, j + 1);
            }
            subheader.getImageId().set(iid);
            subheader.getImageLocation().set(FmtX("%05d00000",
                    segmentInfo.rowOffset));

            std::vector<nitf::BandInfo> bandInfo = info->getBandInfo();

            subheader.setPixelInformation(pvtype, nbpp, nbpp, "R", irep, "SAR",
                    bandInfo);

            subheader.setBlocking(segmentInfo.numRows, numCols,
                    segmentInfo.numRows > 8192 ? 0 : segmentInfo.numRows,
                    numCols > 8192 ? 0 : numCols, imode);
            subheader.getImageSyncCode().set(0);
            if (j == 0)
            {
                // Need to attach to CCS
                subheader.getImageAttachmentLevel().set(0);
            }
            else
            {
                // Need to attach to last segment
                subheader.getImageAttachmentLevel().set((nitf::Uint16)(
                        info->getStartIndex() + j));
            }
            corners[0][0] = segmentInfo.corners.getLat(0);
            corners[0][1] = segmentInfo.corners.getLon(0);

            corners[1][0] = segmentInfo.corners.getLat(1);
            corners[1][1] = segmentInfo.corners.getLon(1);

            corners[2][0] = segmentInfo.corners.getLat(2);
            corners[2][1] = segmentInfo.corners.getLon(2);

            corners[3][0] = segmentInfo.corners.getLat(3);
            corners[3][1] = segmentInfo.corners.getLon(3);

            subheader.setCornersFromLatLons(NITF_CORNERS_GEO, corners);
        }
    }
    for (unsigned int i = 0; i < mContainer->getNumData(); ++i)
    {
        // Write out a DES
        nitf::DESegment seg = mRecord.newDataExtensionSegment();
        nitf::DESubheader subheader = seg.getSubheader();
        std::string desid = str::toString(
                mContainer->getData(i)->getDataClass()) + "_XML";
        subheader.getTypeID().set(desid);
        // BUG? Is DESVER BCS A or N?
        subheader.getVersion().set("01");
        // Forget about the security record for a minute
    }

}

void NITFWriteControl::save(SourceList& imageData, std::string outputFile)
{
    //     int bufferSize = mOptions.getParameter(OPT_BUFFER_SIZE,
    //                                            Parameter((int)DEFAULT_BUFFER_SIZE));

    int bufferSize = DEFAULT_BUFFER_SIZE;
    nitf::BufferedWriter bufferedIO(outputFile, bufferSize);

    saveIO(imageData, bufferedIO);
    //     std::cout << "Write block info: " << std::endl;
    //     std::cout << "------------------------------------" << std::endl;
    //     std::cout << "Total number of blocks written: "
    //             << bufferedIO.getNumBlocksWritten() << std::endl;
    //     std::cout << "Of those, " << bufferedIO.getNumPartialBlocksWritten()
    //             << " were less than buffer size " << bufferSize << std::endl;
    bufferedIO.close();
}

void NITFWriteControl::saveIO(SourceList& imageData,
        nitf::IOInterface& outputFile)
{

    mWriter.prepareIO(outputFile, mRecord);
    bool doByteSwap;

    int byteSwapping = (int) mOptions.getParameter(OPT_BYTE_SWAP, Parameter(
            (int) BYTE_SWAP_AUTO));

    if (byteSwapping == BYTE_SWAP_AUTO)
    {
        // Have to if its not a BE machine
        doByteSwap = !sys::isBigEndianSystem();
    }
    else
    {
        // Do what they say.  You really shouldnt do this
        // unless you know what you're doing anyway!
        doByteSwap = (bool) byteSwapping;
    }

    if (mInfos.size() != imageData.size())
        throw except::Exception(Ctxt(FmtX("Require %d images, received %s",
                mInfos.size(), imageData.size())));

    size_t numImages = mInfos.size();

    for (unsigned int i = 0; i < numImages; ++i)
    {
        NITFImageInfo* info = mInfos[i];
        std::vector<NITFSegmentInfo> imageSegments = info->getImageSegments();
        size_t numIS = imageSegments.size();
        unsigned long pixelSize = info->getData()->getNumBytesPerPixel();
        unsigned long numCols = info->getData()->getNumCols();
        unsigned long numChannels = info->getData()->getNumChannels();

        for (unsigned int j = 0; j < numIS; ++j)
        {
            NITFSegmentInfo segmentInfo = imageSegments[j];

            nitf::WriteHandler* writeHandler = new StreamWriteHandler(
                    segmentInfo, imageData[i], numCols, numChannels, pixelSize,
                    doByteSwap);

            mWriter.setImageWriteHandler(info->getStartIndex() + j,
                    writeHandler);
        }
    }

    addDataAndWrite();

}

void NITFWriteControl::save(io::InputStream& imageData, std::string toFile)
{
    int bufferSize = DEFAULT_BUFFER_SIZE;
    nitf::BufferedWriter outputFile(toFile, bufferSize);

    mWriter.prepareIO(outputFile, mRecord);
    bool doByteSwap;

    int byteSwapping = (int) mOptions.getParameter(OPT_BYTE_SWAP, Parameter(
            (int) BYTE_SWAP_AUTO));

    if (byteSwapping == BYTE_SWAP_AUTO)
    {
        // Have to if its not a BE machine
        doByteSwap = !sys::isBigEndianSystem();
    }
    else
    {
        // Do what they say.  You really shouldnt do this
        // unless you know what you're doing anyway!
        doByteSwap = (bool) byteSwapping;
    }

    if (mInfos.size() != 1)
        throw except::Exception(Ctxt(FmtX("Require %d images, received 1",
                mInfos.size())));

    NITFImageInfo* info = mInfos[0];
    std::vector<NITFSegmentInfo> imageSegments = info->getImageSegments();
    size_t numIS = imageSegments.size();
    unsigned long pixelSize = info->getData()->getNumBytesPerPixel();
    unsigned long numCols = info->getData()->getNumCols();
    unsigned long numChannels = info->getData()->getNumChannels();

    for (unsigned int j = 0; j < numIS; ++j)
    {
        NITFSegmentInfo segmentInfo = imageSegments[j];

        nitf::WriteHandler* writeHandler = new StreamWriteHandler(
                segmentInfo, &imageData, numCols, numChannels, pixelSize,
                doByteSwap);

        mWriter.setImageWriteHandler(info->getStartIndex() + j,
                writeHandler);
    }

    addDataAndWrite();

    outputFile.close();
}

void NITFWriteControl::save(BufferList& imageData, std::string outputFile)
{

    //     int bufferSize = mOptions.getParameter(OPT_BUFFER_SIZE,
    //                                            Parameter((int)DEFAULT_BUFFER_SIZE));
    int bufferSize = DEFAULT_BUFFER_SIZE;
    nitf::BufferedWriter bufferedIO(outputFile, bufferSize);

    saveIO(imageData, bufferedIO);
    //     std::cout << "Write block info: " << std::endl;
    //     std::cout << "------------------------------------" << std::endl;
    //     std::cout << "Total number of blocks written: "
    //             << bufferedIO.getNumBlocksWritten() << std::endl;
    //     std::cout << "Of those, " << bufferedIO.getNumPartialBlocksWritten()
    //             << " were less than buffer size " << bufferSize << std::endl;
    bufferedIO.close();
}

void NITFWriteControl::saveIO(BufferList& imageData,
        nitf::IOInterface& outputFile)
{

    mWriter.prepareIO(outputFile, mRecord);

    // Have to if its not a BE machine
    bool doByteSwap = !sys::isBigEndianSystem();

    if (mInfos.size() != imageData.size())
        throw except::Exception(Ctxt(FmtX("Require %d images, received %s",
                mInfos.size(), imageData.size())));

    size_t numImages = mInfos.size();
    for (unsigned int i = 0; i < numImages; ++i)
    {
        NITFImageInfo* info = mInfos[i];
        std::vector<NITFSegmentInfo> imageSegments = info->getImageSegments();
        size_t numIS = imageSegments.size();
        unsigned long pixelSize = info->getData()->getNumBytesPerPixel();
        unsigned long numCols = info->getData()->getNumCols();
        unsigned long numChannels = info->getData()->getNumChannels();

        for (unsigned int j = 0; j < numIS; ++j)
        {
            NITFSegmentInfo segmentInfo = imageSegments[j];

            nitf::WriteHandler* writeHandler = new MemoryWriteHandler(
                    segmentInfo, imageData[i], segmentInfo.firstRow, numCols,
                    numChannels, pixelSize, doByteSwap);
            // Could set start index here
            mWriter.setImageWriteHandler(info->getStartIndex() + j,
                    writeHandler);
        }
    }

    addDataAndWrite();
}

void NITFWriteControl::save(UByte* imageData, std::string toFile)
{
    int bufferSize = DEFAULT_BUFFER_SIZE;
    nitf::BufferedWriter outputFile(toFile, bufferSize);

    mWriter.prepareIO(outputFile, mRecord);

    // Have to if its not a BE machine
    bool doByteSwap = !sys::isBigEndianSystem();

    if (mInfos.size() != 1)
        throw except::Exception(Ctxt(FmtX("Require %d images, received 1",
                mInfos.size())));

    NITFImageInfo* info = mInfos[0];
    std::vector<NITFSegmentInfo> imageSegments = info->getImageSegments();
    size_t numIS = imageSegments.size();
    unsigned long pixelSize = info->getData()->getNumBytesPerPixel();
    unsigned long numCols = info->getData()->getNumCols();
    unsigned long numChannels = info->getData()->getNumChannels();

    for (unsigned int j = 0; j < numIS; ++j)
    {
        NITFSegmentInfo segmentInfo = imageSegments[j];

        nitf::WriteHandler* writeHandler = new MemoryWriteHandler(
                segmentInfo, imageData, segmentInfo.firstRow, numCols,
                numChannels, pixelSize, doByteSwap);
        // Could set start index here
        mWriter.setImageWriteHandler(info->getStartIndex() + j,
                writeHandler);
    }

    addDataAndWrite();

    outputFile.close();
}

void NITFWriteControl::addDataAndWrite()
{
    size_t numDES = mContainer->getNumData();

    char **raw = new char*[numDES];

    for (unsigned int i = 0; i < mContainer->getNumData(); ++i)
    {
        Data* data = mContainer->getData(i);

        raw[i] = six::toXMLCharArray(data);
        nitf::SegmentWriter* deWriter = mWriter.newDEWriter(i);
        nitf::SegmentSource* segSource = new nitf::SegmentMemorySource(raw[i],
                strlen(raw[i]), 0, 0);
        deWriter->attachSource(segSource, true);
    }
    mWriter.write();

    for (unsigned int i = 0; i < numDES; ++i)
    {
        delete[] raw[i];
    }
    delete[] raw;

}

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
    sys::Uint32_T maxRows = mOptions.getParameter(OPT_MAX_ILOC_ROWS,
                                                  Parameter(ilocMax));

    sys::Uint64_T
            maxSize =
                    (sys::Uint64_T) mOptions.getParameter(
                                                          OPT_MAX_PRODUCT_SIZE,
                                                          Parameter(
                                                                    six::Constants::IS_SIZE_MAX));

    /*
     *  If our container is SICD, must have only one image.
     *
     *  However, if our container is SIDD, we can have more than one
     *  image, and not every Data necessarily needs an image
     *
     */
    if (mContainer->getDataType() == DataType::COMPLEX)
    {
        mInfos.push_back(new NITFImageInfo(mContainer->getData(0), maxRows,
                                           maxSize, true));
    }
    else
    {
        for (unsigned int i = 0; i < mContainer->getNumData(); ++i)
        {
            Data* ith = mContainer->getData(i);
            if (ith->getDataType() == DataType::DERIVED)
                mInfos.push_back(new NITFImageInfo(ith, maxRows, maxSize, true));
        }
    }
    mRecord = nitf::Record(NITF_VER_21);

    DataType dataType = mContainer->getDataType();
    std::string name = mInfos[0]->getData()->getName();
    std::string fileTitle = FmtX("%s: %s", six::toString(dataType).c_str(),
                                 name.c_str());
    fileTitle = fileTitle.substr(0, NITF_FTITLE_SZ); //truncate past 80
    mRecord.getHeader().getFileTitle().set(fileTitle);

    int startIndex = 0;
    for (unsigned int i = 0; i < mInfos.size(); ++i)
    {
        NITFImageInfo* info = mInfos[i];

        std::vector < NITFSegmentInfo > imageSegments
                = info->getImageSegments();

        size_t numIS = imageSegments.size();
        int nbpp = info->getNumBitsPerPixel();
        int numCols = info->getData()->getNumCols();
        std::string irep = info->getRepresentation();
        std::string imode = info->getMode();
        std::string pvtype = info->getPixelValueType();
        // NITRO wants to see this, not our corners object
        double corners[4][2];

        std::string targetId = "";

        // TODO: Subclass to get this?
        // if (info->getData()->getDataType() == DataType::DERIVED)
        //         {
        //             DerivedData* derived = (DerivedData*)info->getData();
        //             if(derived->geographicAndTarget && derived->geographicAndTarget->targetInformation.size() > 0)
        //             {
        //                 six::TargetInformation* target = derived->geographicAndTarget->targetInformation[0];
        //                 if (target->identifiers.size() > 0)
        //                 {
        //                     targetId = target->identifiers[0].str();
        //                 }
        //             }
        //         }

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

            std::string iid = six::toString(dataType);
            if (dataType == DataType::COMPLEX)
            {
                iid = FmtX("%s%03d", iid.c_str(), numIS > 1 ? j + 1 : j);
            }
            else
            {
                iid = FmtX("%s%03d%03d", iid.c_str(), i + 1, j + 1);
            }
            subheader.getImageId().set(iid);

            std::string isorce = info->getData()->getSource();
            subheader.getImageSource().set(isorce);

            subheader.getImageLocation().set(FmtX("%05d00000",
                                                  segmentInfo.rowOffset));

            subheader.getTargetId().set(targetId);

            std::vector < nitf::BandInfo > bandInfo = info->getBandInfo();

            subheader.setPixelInformation(pvtype, nbpp, nbpp, "R", irep, "SAR",
                                          bandInfo);

            subheader.setBlocking(
                                  segmentInfo.numRows,
                                  numCols,
                                  segmentInfo.numRows > 8192 ? 0
                                                             : segmentInfo.numRows,
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
                subheader.getImageAttachmentLevel().set(
                                                        (nitf::Uint16)(
                                                                       info->getStartIndex()
                                                                               + j));
            }
            corners[0][0] = segmentInfo.corners[0].getLat();
            corners[0][1] = segmentInfo.corners[0].getLon();

            corners[1][0] = segmentInfo.corners[1].getLat();
            corners[1][1] = segmentInfo.corners[1].getLon();

            corners[2][0] = segmentInfo.corners[2].getLat();
            corners[2][1] = segmentInfo.corners[2].getLon();

            corners[3][0] = segmentInfo.corners[3].getLat();
            corners[3][1] = segmentInfo.corners[3].getLon();

            subheader.setCornersFromLatLons(NITF_CORNERS_GEO, corners);

            setImageSecurity(info->getData()->getClassification(), subheader);
        }
    }
    for (unsigned int i = 0; i < mContainer->getNumData(); ++i)
    {
        // Write out a DES
        nitf::DESegment seg = mRecord.newDataExtensionSegment();
        nitf::DESubheader subheader = seg.getSubheader();
        std::string desid =
                six::toString(mContainer->getData(i)->getDataType()) + "_XML";
        subheader.getTypeID().set(desid);
        // BUG? Is DESVER BCS A or N?
        subheader.getVersion().set("01");

        setDESecurity(mContainer->getData(i)->getClassification(), subheader);
    }

    updateFileHeaderSecurity();
}

void NITFWriteControl::setImageSecurity(six::Classification& c,
        nitf::ImageSubheader& subheader)
{
    //This requires a normalized name to get set correctly
    subheader.getImageSecurityClass().set(getNITFClassification(c.level));
    setSecurity(c, subheader.getSecurityGroup(), "IS");
}

void NITFWriteControl::setDESecurity(six::Classification& c,
        nitf::DESubheader& subheader)
{
    subheader.getSecurityClass().set(getNITFClassification(c.level));
    setSecurity(c, subheader.getSecurityGroup(), "DES");
}

void NITFWriteControl::setSecurity(six::Classification& c,
        nitf::FileSecurity security, std::string prefix)
{

    std::string k;
    Options& ops = c.fileOptions;

    k = NITFImageInfo::generateFieldKey(NITFImageInfo::CLSY, prefix);
    if (ops.hasParameter(k))
    {
        Parameter p = ops.getParameter(k);
        security.getClassificationSystem().set(p.str());
        mLog->debug(Ctxt(FmtX("Using NITF security option: [%s]->[%s]",
                              k.c_str(), (const char*) p)));
    }
    k = NITFImageInfo::generateFieldKey(NITFImageInfo::CODE, prefix);
    if (ops.hasParameter(k))
    {
        Parameter p = ops.getParameter(k);
        security.getCodewords().set(p.str());
        mLog->debug(Ctxt(FmtX("Using NITF security option: [%s]->[%s]",
                              k.c_str(), (const char*) p)));
    }
    k = NITFImageInfo::generateFieldKey(NITFImageInfo::CTLH, prefix);
    if (ops.hasParameter(k))
    {
        Parameter p = ops.getParameter(k);
        security.getControlAndHandling().set(p.str());
        mLog->debug(Ctxt(FmtX("Using NITF security option: [%s]->[%s]",
                              k.c_str(), (const char*) p)));
    }
    k = NITFImageInfo::generateFieldKey(NITFImageInfo::REL, prefix);
    if (ops.hasParameter(k))
    {
        Parameter p = ops.getParameter(k);
        security.getReleasingInstructions().set(p.str());
        mLog->debug(Ctxt(FmtX("Using NITF security option: [%s]->[%s]",
                              k.c_str(), (const char*) p)));
    }
    k = NITFImageInfo::generateFieldKey(NITFImageInfo::DCTP, prefix);
    if (ops.hasParameter(k))
    {
        Parameter p = ops.getParameter(k);
        security.getDeclassificationType().set(p.str());
        mLog->debug(Ctxt(FmtX("Using NITF security option: [%s]->[%s]",
                              k.c_str(), (const char*) p)));
    }
    k = NITFImageInfo::generateFieldKey(NITFImageInfo::DCDT, prefix);
    if (ops.hasParameter(k))
    {
        Parameter p = ops.getParameter(k);
        security.getDeclassificationDate().set(p.str());
        mLog->debug(Ctxt(FmtX("Using NITF security option: [%s]->[%s]",
                              k.c_str(), (const char*) p)));
    }
    k = NITFImageInfo::generateFieldKey(NITFImageInfo::DCXM, prefix);
    if (ops.hasParameter(k))
    {
        Parameter p = ops.getParameter(k);
        security.getDeclassificationExemption().set(p.str());
        mLog->debug(Ctxt(FmtX("Using NITF security option: [%s]->[%s]",
                              k.c_str(), (const char*) p)));
    }
    k = NITFImageInfo::generateFieldKey(NITFImageInfo::DG, prefix);
    if (ops.hasParameter(k))
    {
        Parameter p = ops.getParameter(k);
        security.getDowngrade().set(p.str());
        mLog->debug(Ctxt(FmtX("Using NITF security option: [%s]->[%s]",
                              k.c_str(), (const char*) p)));
    }
    k = NITFImageInfo::generateFieldKey(NITFImageInfo::DGDT, prefix);
    if (ops.hasParameter(k))
    {
        Parameter p = ops.getParameter(k);
        security.getDowngradeDateTime().set(p.str());
        mLog->debug(Ctxt(FmtX("Using NITF security option: [%s]->[%s]",
                              k.c_str(), (const char*) p)));
    }
    k = NITFImageInfo::generateFieldKey(NITFImageInfo::CLTX, prefix);
    if (ops.hasParameter(k))
    {
        Parameter p = ops.getParameter(k);
        security.getClassificationText().set(p.str());
        mLog->debug(Ctxt(FmtX("Using NITF security option: [%s]->[%s]",
                              k.c_str(), (const char*) p)));
    }
    k = NITFImageInfo::generateFieldKey(NITFImageInfo::CATP, prefix);
    if (ops.hasParameter(k))
    {
        Parameter p = ops.getParameter(k);
        security.getClassificationAuthorityType().set(p.str());
        mLog->debug(Ctxt(FmtX("Using NITF security option: [%s]->[%s]",
                              k.c_str(), (const char*) p)));
    }
    k = NITFImageInfo::generateFieldKey(NITFImageInfo::CAUT, prefix);
    if (ops.hasParameter(k))
    {
        Parameter p = ops.getParameter(k);
        security.getClassificationAuthority().set(p.str());
        mLog->debug(Ctxt(FmtX("Using NITF security option: [%s]->[%s]",
                              k.c_str(), (const char*) p)));
    }
    k = NITFImageInfo::generateFieldKey(NITFImageInfo::CRSN, prefix);
    if (ops.hasParameter(k))
    {
        Parameter p = ops.getParameter(k);
        security.getClassificationReason().set(p.str());
        mLog->debug(Ctxt(FmtX("Using NITF security option: [%s]->[%s]",
                              k.c_str(), (const char*) p)));
    }
    k = NITFImageInfo::generateFieldKey(NITFImageInfo::SRDT, prefix);
    if (ops.hasParameter(k))
    {
        Parameter p = ops.getParameter(k);
        security.getSecuritySourceDate().set(p.str());
        mLog->debug(Ctxt(FmtX("Using NITF security option: [%s]->[%s]",
                              k.c_str(), (const char*) p)));
    }
    k = NITFImageInfo::generateFieldKey(NITFImageInfo::CTLN, prefix);
    if (ops.hasParameter(k))
    {
        Parameter p = ops.getParameter(k);
        security.getSecurityControlNumber().set(p.str());
        mLog->debug(Ctxt(FmtX("Using NITF security option: [%s]->[%s]",
                              k.c_str(), (const char*) p)));
    }

    // Now, do some specific overrides
    if (security.getClassificationSystem().toString().empty())
    {
        security.getClassificationSystem().set("US");
        mLog->debug(Ctxt("Forcing NITF Classification System to [US]"));
    }

    std::string code;
    for (unsigned int j = 0; j < c.securityMarkings.size(); j++)
    {
        if (j)
            code += " ";
        code += c.securityMarkings[j];
    }
    str::trim(code);
    if (!code.empty())
    {
        security.getCodewords().set(code);
        k = NITFImageInfo::generateFieldKey(NITFImageInfo::CODE, prefix);
        mLog->debug(Ctxt(FmtX("Setting NITF [%s] from sicd/sidd: [%s]",
                              k.c_str(), code.c_str())));
    }

    std::string srdt;
    if (c.level != "UNCLASSIFIED")
    {
        security.getDeclassificationType().set("X");
        k = NITFImageInfo::generateFieldKey(NITFImageInfo::DCTP, prefix);
        mLog->debug(Ctxt(FmtX("Setting NITF [%s] as [X]", k.c_str())));

        security.getClassificationAuthorityType().set("D");
        k = NITFImageInfo::generateFieldKey(NITFImageInfo::CATP, prefix);
        mLog->debug(Ctxt(FmtX("Setting NITF [%s] as [D]", k.c_str())));

        if (c.guidance)
        {
            security.getClassificationAuthority().set(c.guidance->authority);
            k = NITFImageInfo::generateFieldKey(NITFImageInfo::CAUT, prefix);
            mLog->debug(Ctxt(FmtX("Setting NITF [%s] from sicd/sidd: [%s]",
                                  k.c_str(), c.guidance->authority.c_str())));

            char raw[256];
            raw[255] = 0;
            c.guidance->date.format("%Y%m%d", raw, 255);
            srdt.assign(raw);

            str::trim(srdt);
        }
    }
    if (!srdt.empty())
    {
        k = NITFImageInfo::generateFieldKey(NITFImageInfo::SRDT, prefix);
        security.getSecuritySourceDate().set(srdt);
        mLog->debug(Ctxt(FmtX("Setting NITF [%s] from sicd/sidd: [%s]",
                              k.c_str(), srdt.c_str())));
    }
}

std::string NITFWriteControl::getNITFClassification(std::string level)
{
    if (str::startsWith(level, "UNCLASSIFIED"))
    {
        return "U";
    }
    else if (str::startsWith(level, "CLASSIFIED"))
    {
        return "C";
    }
    else if (str::startsWith(level, "RESTRICTED"))
    {
        return "R";
    }
    else if (str::startsWith(level, "SECRET"))
    {
        return "S";
    }
    else if (str::startsWith(level, "TOP SECRET"))
    {
        return "T";
    }

    // They get one last chance to fix this using
    // fileOptions.getParameter(Classification::OPT_*CLAS)
    return "";
}

void NITFWriteControl::updateFileHeaderSecurity()
{
    bool changed = false;
    std::string classOrder = "URCST";
    int classIndex =
            classOrder.find(mRecord.getHeader().getClassification().toString());
    classIndex = classIndex != std::string::npos ? classIndex : -1;

    nitf::FileSecurity highest = mRecord.getHeader().getSecurityGroup();

    for (int i = 0, size = mRecord.getImages().getSize(); i < size; i++)
    {
        nitf::ImageSubheader subheader =
                nitf::ImageSegment(mRecord.getImages()[i]).getSubheader();
        int idx = classOrder.find(subheader.getImageSecurityClass().toString());
        idx = idx != std::string::npos ? idx : -1;

        if (idx > classIndex)
        {
            highest = subheader.getSecurityGroup();
            classIndex = idx;
            changed = true;
        }
    }

    for (int i = 0, size = mRecord.getDataExtensions().getSize(); i < size; i++)
    {
        nitf::DESubheader subheader =
                nitf::DESegment(mRecord.getDataExtensions()[i]).getSubheader();
        int idx = classOrder.find(subheader.getSecurityClass().toString());
        idx = idx != std::string::npos ? idx : -1;

        if (idx > classIndex)
        {
            highest = subheader.getSecurityGroup();
            classIndex = idx;
            changed = true;
        }
    }

    if (changed)
    {
        mRecord.getHeader().getClassification() = classOrder.substr(classIndex,
                                                                    1);
        mRecord.getHeader().setSecurityGroup(highest.clone());
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

    int
            byteSwapping =
                    (int) mOptions.getParameter(
                                                OPT_BYTE_SWAP,
                                                Parameter(
                                                          (int) ByteSwapping::SWAP_AUTO));

    if (byteSwapping == ByteSwapping::SWAP_AUTO)
    {
        // Have to if its not a BE machine
        doByteSwap = !sys::isBigEndianSystem();
    }
    else
    {
        // Do what they say.  You really shouldnt do this
        // unless you know what you're doing anyway!
        doByteSwap = byteSwapping ? true : false;
    }

    if (mInfos.size() != imageData.size())
        throw except::Exception(Ctxt(FmtX("Require %d images, received %s",
                                          mInfos.size(), imageData.size())));

    size_t numImages = mInfos.size();

    for (unsigned int i = 0; i < numImages; ++i)
    {
        NITFImageInfo* info = mInfos[i];
        std::vector < NITFSegmentInfo > imageSegments
                = info->getImageSegments();
        size_t numIS = imageSegments.size();
        unsigned long pixelSize = info->getData()->getNumBytesPerPixel();
        unsigned long numCols = info->getData()->getNumCols();
        unsigned long numChannels = info->getData()->getNumChannels();

        for (unsigned int j = 0; j < numIS; ++j)
        {
            NITFSegmentInfo segmentInfo = imageSegments[j];

            StreamWriteHandler writeHandler(segmentInfo, imageData[i], numCols,
                                            numChannels, pixelSize, doByteSwap);

            mWriter.setImageWriteHandler(info->getStartIndex() + j,
                                         writeHandler);
        }
    }

    addDataAndWrite();

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
        std::vector < NITFSegmentInfo > imageSegments
                = info->getImageSegments();
        size_t numIS = imageSegments.size();
        unsigned long pixelSize = info->getData()->getNumBytesPerPixel();
        unsigned long numCols = info->getData()->getNumCols();
        unsigned long numChannels = info->getData()->getNumChannels();

        for (unsigned int j = 0; j < numIS; ++j)
        {
            NITFSegmentInfo segmentInfo = imageSegments[j];

            MemoryWriteHandler writeHandler(segmentInfo, imageData[i],
                                            segmentInfo.firstRow, numCols,
                                            numChannels, pixelSize, doByteSwap);
            // Could set start index here
            mWriter.setImageWriteHandler(info->getStartIndex() + j,
                                         writeHandler);
        }
    }

    addDataAndWrite();
}

void NITFWriteControl::addDataAndWrite()
{
    size_t numDES = mContainer->getNumData();

    char **raw = new char*[numDES];

    for (unsigned int i = 0; i < mContainer->getNumData(); ++i)
    {
        Data* data = mContainer->getData(i);

        raw[i] = six::toXMLCharArray(data);
        nitf::SegmentWriter deWriter = mWriter.newDEWriter(i);
        nitf::SegmentMemorySource segSource(raw[i], strlen(raw[i]), 0, 0);
        deWriter.attachSource(segSource);
    }
    mWriter.write();

    for (unsigned int i = 0; i < numDES; ++i)
    {
        delete[] raw[i];
    }
    delete[] raw;

}

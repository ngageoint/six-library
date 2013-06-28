/* =========================================================================
 * This file is part of six-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2013, General Dynamics - Advanced Information Systems
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

#include <sstream>

#include <mem/ScopedArray.h>
#include <six/NITFWriteControl.h>
#include <six/XMLControlFactory.h>

using namespace six;

const char NITFWriteControl::OPT_MAX_PRODUCT_SIZE[] = "MaxProductSize";
const char NITFWriteControl::OPT_MAX_ILOC_ROWS[] = "MaxILOCRows";
const char NITFWriteControl::OPT_J2K_COMPRESSION[] = "J2KCompression";

namespace
{
// Just using this to provide a more useful exception message
// TODO: Consider adding this as a utility method in NITRO.  I don't think it
//       can just move into a method in the TRE class easily and still provide
//       the field name in the exception message.
void setField(const std::string& field,
              const std::string& value,
              nitf::TRE& tre)
{
    nitf::Field treField(tre[field]);
    if (value.length() > treField.getLength())
    {
        std::ostringstream ostr;
        ostr << "Tried to set field '" << field << "' to '" << value
             << "' but this is " << value.length() << " characters when the "
             << "field can only contain " << treField.getLength()
             << " characters";

        throw except::Exception(Ctxt(ostr.str()));
    }

    treField = value;
}
}

NITFWriteControl::~NITFWriteControl()
{
    for (size_t ii = 0; ii < mInfos.size(); ++ii)
    {
        delete mInfos[ii];
    }
}

void NITFWriteControl::initialize(Container* container)
{
    // Clean up
    // NOTE  We do not own the container, so we don't delete 'mContainer' here
    for (size_t ii = 0; ii < mInfos.size(); ++ii)
    {
        delete mInfos[ii];
    }
    mInfos.clear();

    mContainer = container;

    sys::Uint32_T ilocMax = Constants::ILOC_MAX;
    // TODO: Validate they are below this!
    sys::Uint32_T maxRows = mOptions.getParameter(OPT_MAX_ILOC_ROWS,
                                                  Parameter(ilocMax));

    sys::Uint64_T maxSize =
            (sys::Uint64_T) mOptions.getParameter(
                    OPT_MAX_PRODUCT_SIZE,
                    Parameter(six::Constants::IS_SIZE_MAX));


    double j2kCompression = 0;
    bool enableJ2K = false;

    /*
     *  If our container is SICD, must have only one image.
     *
     *  However, if our container is SIDD, we can have more than one
     *  image, and not every Data necessarily needs an image
     *
     */
    if (mContainer->getDataType() == DataType::COMPLEX)
    {
        std::auto_ptr<NITFImageInfo>
            info(new NITFImageInfo(mContainer->getData(0),
                                   maxRows,
                                   maxSize,
                                   true));

        mInfos.resize(1);
        mInfos[0] = info.release();
    }
    else
    {
        // J2K only available for derived data
        j2kCompression = (double)mOptions.getParameter(
                OPT_J2K_COMPRESSION, Parameter(0));
        enableJ2K = (j2kCompression <= 1.0) && j2kCompression > 0.0001;

        for (size_t ii = 0; ii < mContainer->getNumData(); ++ii)
        {
            Data* const ith = mContainer->getData(ii);
            if (ith->getDataType() == DataType::DERIVED)
            {
                std::auto_ptr<NITFImageInfo>
                    info(new NITFImageInfo(ith, maxRows, maxSize, true));

                mInfos.resize(mInfos.size() + 1);
                mInfos.back() = info.release();
            }
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
    for (size_t ii = 0; ii < mInfos.size(); ++ii)
    {
        NITFImageInfo* info = mInfos[ii];

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

        for (size_t jj = 0; jj < numIS; ++jj)
        {
            NITFSegmentInfo segmentInfo = imageSegments[jj];

            nitf::ImageSegment imageSegment = mRecord.newImageSegment();
            nitf::ImageSubheader subheader = imageSegment.getSubheader();

            //numIS > 1?i+1:i));
            subheader.getImageTitle().set(fileTitle);
            DateTime creationTime = info->getData()->getCreationTime();
            subheader.getImageDateAndTime().set(creationTime);

            std::string iid = six::toString(dataType);
            if (dataType == DataType::COMPLEX)
            {
                iid = FmtX("%s%03d", iid.c_str(), numIS > 1 ? jj + 1 : jj);
            }
            else
            {
                iid = FmtX("%s%03d%03d", iid.c_str(), ii + 1, jj + 1);
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

            subheader.setBlocking(segmentInfo.numRows,
                                  numCols,
                                  segmentInfo.numRows > 8192 ? 0
                                                             : segmentInfo.numRows,
                                  numCols > 8192 ? 0 : numCols, imode);
            subheader.getImageSyncCode().set(0);
            if (jj == 0)
            {
                // Need to attach to CCS
                subheader.getImageAttachmentLevel().set(0);
            }
            else
            {
                // Need to attach to last segment
                subheader.getImageAttachmentLevel().set(
                        (nitf::Uint16)(info->getStartIndex() + jj));
            }

            for (size_t kk = 0; kk < LatLonCorners::NUM_CORNERS; ++kk)
            {
                corners[kk][0] = segmentInfo.corners.getCorner(kk).getLat();
                corners[kk][1] = segmentInfo.corners.getCorner(kk).getLon();
            }
            subheader.setCornersFromLatLons(NITF_CORNERS_GEO, corners);

            setImageSecurity(info->getData()->getClassification(), subheader);

            if (enableJ2K)
            {
                subheader.getImageCompression().set("C8");

                //calculate comrat
                int comratInt = (int)((j2kCompression * nbpp * 10.0) + 0.5);
                std::string comrat = FmtX("N%03d", comratInt);
                subheader.getCompressionRate().set(comrat);
            }
        }
    }
    for (size_t ii = 0; ii < mContainer->getNumData(); ++ii)
    {
        const six::Data& data(*mContainer->getData(ii));

        // Write out a DES
        nitf::DESegment seg = mRecord.newDataExtensionSegment();
        nitf::DESubheader subheader = seg.getSubheader();
        subheader.getTypeID().set(getDesTypeID(data));
        subheader.getVersion().set(Constants::DES_VERSION_STR);

        if (needUserDefinedSubheader(data))
        {
            addUserDefinedSubheader(data, subheader);
        }

        setDESecurity(data.getClassification(), subheader);
    }

    updateFileHeaderSecurity();
}

void NITFWriteControl::setImageSecurity(const six::Classification& c,
        nitf::ImageSubheader& subheader)
{
    //This requires a normalized name to get set correctly
    subheader.getImageSecurityClass().set(
        getNITFClassification(c.getLevel()));
    setSecurity(c, subheader.getSecurityGroup(), "IS");
}

void NITFWriteControl::setDESecurity(const six::Classification& c,
        nitf::DESubheader& subheader)
{
    subheader.getSecurityClass().set(getNITFClassification(c.getLevel()));
    setSecurity(c, subheader.getSecurityGroup(), "DES");
}

void NITFWriteControl::setSecurity(const six::Classification& c,
        nitf::FileSecurity security, const std::string& prefix)
{
    std::string k;
    const Options& ops = c.fileOptions;

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

    c.setSecurity(prefix, *mLog, security);
}

std::string NITFWriteControl::getNITFClassification(const std::string& level)
{
    // If the string starts with UCRST (after any leading whitespaces),
    // return the correct identifier
    for (size_t ii = 0; ii < level.length(); ++ii)
    {
        const char ch(::toupper(level[ii]));
        switch (ch)
        {
        case 'U':
        case 'C':
        case 'R':
        case 'S':
        case 'T':
            return std::string(1, ch);
        case ' ':
            // Keep going
            break;
        default:
            return "";
        }
    }

    // The user gets one last chance to fix this using
    // fileOptions.getParameter(Classification::OPT_*CLAS)
    return "";
}

void NITFWriteControl::updateFileHeaderSecurity()
{
    bool changed = false;
    std::string classOrder = "URCST";
    size_t foundLoc =
            classOrder.find(mRecord.getHeader().getClassification().toString());
    int classIndex = foundLoc != std::string::npos ? foundLoc : -1;

    nitf::FileSecurity highest = mRecord.getHeader().getSecurityGroup();

    for (int i = 0, size = mRecord.getImages().getSize(); i < size; i++)
    {
        nitf::ImageSubheader subheader =
                nitf::ImageSegment(mRecord.getImages()[i]).getSubheader();
        foundLoc = classOrder.find(subheader.getImageSecurityClass().toString());
        int idx = foundLoc != std::string::npos ? foundLoc : -1;

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
        foundLoc = classOrder.find(subheader.getSecurityClass().toString());
        int idx = foundLoc != std::string::npos ? foundLoc : -1;

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

void NITFWriteControl::save(SourceList& imageData,
                            const std::string& outputFile,
                            const std::vector<std::string>& schemaPaths)
{
    //     int bufferSize = mOptions.getParameter(OPT_BUFFER_SIZE,
    //                                            Parameter((int)DEFAULT_BUFFER_SIZE));

    int bufferSize = DEFAULT_BUFFER_SIZE;
    nitf::BufferedWriter bufferedIO(outputFile, bufferSize);

    save(imageData, bufferedIO, schemaPaths);
    bufferedIO.close();
}

void NITFWriteControl::save(
        SourceList& imageData,
        nitf::IOInterface& outputFile,
        const std::vector<std::string>& schemaPaths)
{

    mWriter.prepareIO(outputFile, mRecord);
    bool doByteSwap;

    int byteSwapping =
        (int) mOptions.getParameter(OPT_BYTE_SWAP,
                                    Parameter((int) ByteSwapping::SWAP_AUTO));

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

    addDataAndWrite(schemaPaths);

}

void NITFWriteControl::save(BufferList& imageData,
                            const std::string& outputFile,
                            const std::vector<std::string>& schemaPaths)
{

    //     int bufferSize = mOptions.getParameter(OPT_BUFFER_SIZE,
    //                                            Parameter((int)DEFAULT_BUFFER_SIZE));
    int bufferSize = DEFAULT_BUFFER_SIZE;
    nitf::BufferedWriter bufferedIO(outputFile, bufferSize);

    save(imageData, bufferedIO, schemaPaths);
    bufferedIO.close();
}

void NITFWriteControl::save(
        BufferList& imageData,
        nitf::IOInterface& outputFile,
        const std::vector<std::string>& schemaPaths)
{

    mWriter.prepareIO(outputFile, mRecord);

    // Have to if its not a BE machine
    bool doByteSwap = !sys::isBigEndianSystem();

    if (mInfos.size() != imageData.size())
        throw except::Exception(Ctxt(FmtX("Require %d images, received %s",
                                          mInfos.size(), imageData.size())));

    // check to see if J2K compression is enabled
    double j2kCompression = (double)mOptions.getParameter(
            OPT_J2K_COMPRESSION, Parameter(0));

    bool enableJ2K = (mContainer->getDataType() != DataType::COMPLEX) &&
            (j2kCompression <= 1.0) && j2kCompression > 0.0001;

    //TODO maybe we need to see if the compression plug-in is even available

    size_t numImages = mInfos.size();
    createCompressionOptions(mCompressionOptions);
    for (unsigned int i = 0; i < numImages; ++i)
    {
        NITFImageInfo* info = mInfos[i];
        std::vector < NITFSegmentInfo > imageSegments
                = info->getImageSegments();
        size_t numIS = imageSegments.size();
        size_t pixelSize = info->getData()->getNumBytesPerPixel();
        size_t numCols = info->getData()->getNumCols();
        size_t numChannels = info->getData()->getNumChannels();

        // The SIDD spec requires that a J2K compressed SIDDs be only a 
        // single image segment. However this functionality remains untested.
        if (enableJ2K && numIS == 1 || !mCompressionOptions.empty())
        {
            // We will use the ImageWriter provided by NITRO so that we can
            // take advantage of the built-in compression capabilities
            nitf::ImageWriter iWriter = mWriter.newImageWriter(i, mCompressionOptions);
            nitf::ImageSource iSource;
            size_t bandSize = numCols * info->getData()->getNumRows();

            for (size_t j = 0; j < numChannels; ++j)
            {
                nitf::MemorySource ms((char *)imageData[i], bandSize, bandSize * j,
                                      pixelSize, 0);
                iSource.addBand(ms);
            }
            iWriter.attachSource(iSource);
        }
        else
        {
            // this bypasses the normal NITF ImageWriter and streams directly
            // to the output
            for (size_t j = 0; j < numIS; ++j)
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
    }

    addDataAndWrite(schemaPaths);
}

void NITFWriteControl::addDataAndWrite(
        const std::vector<std::string>& schemaPaths)
{
    const size_t numDES = mContainer->getNumData();

    // These must stick around until mWriter.write() is called since the
    // SegmentMemorySource's will be pointing to them
    const mem::ScopedArray<std::string> desStrs(new std::string[numDES]);

    for (size_t ii = 0; ii < mContainer->getNumData(); ++ii)
    {
        const Data* data = mContainer->getData(ii);
        std::string& desStr(desStrs[ii]);

        desStr = six::toValidXMLString(data, schemaPaths, mLog, mXMLRegistry);
        nitf::SegmentWriter deWriter = mWriter.newDEWriter(ii);
        nitf::SegmentMemorySource segSource(desStr.c_str(),
                                            desStr.length(),
                                            0,
                                            0,
                                            false);
        deWriter.attachSource(segSource);
    }
    mWriter.write();
}

std::string NITFWriteControl::getDesTypeID(const six::Data& data)
{
    // TODO: This requires some knowledge of the SICD/SIDD versions, but not
    //       sure where else this logic should live.

    const std::string version(data.getVersion());
    const std::vector<std::string> versionParts(str::split(version, "."));

    if (versionParts.size() < 2)
    {
        throw except::Exception(Ctxt("Invalid version string: " + version));
    }

    // Name of DES changed for version 1.0+
    const std::string& majorVersion = versionParts[0];
    if (majorVersion == "0")
    {
        return XMLControl::dataTypeToString(data.getDataType());
    }
    else
    {
        return "XML_DATA_CONTENT";
    }
}

bool NITFWriteControl::needUserDefinedSubheader(const six::Data& data)
{
    // TODO: This requires some knowledge of the SICD/SIDD versions, but not
    //       sure where else this logic should live.

    const std::string version(data.getVersion());
    const std::vector<std::string> versionParts(str::split(version, "."));

    if (versionParts.size() < 2)
    {
        throw except::Exception(Ctxt("Invalid version string: " + version));
    }

    // Starting with 1.0, the user-defined subheader is required
    const std::string& majorVersion = versionParts[0];
    return (majorVersion != "0");
}

void NITFWriteControl::setOrganizationId(const std::string& organizationId)
{
    mOrganizationId = organizationId;
}

void NITFWriteControl::setLocationIdentifier(
    const std::string& locationId,
    const std::string& locationIdNamespace)
{
    mLocationId = locationId;
    mLocationIdNamespace = locationIdNamespace;
}

void NITFWriteControl::setAbstract(const std::string& abstract)
{
    mAbstract = abstract;
}

void NITFWriteControl::addUserDefinedSubheader(
        const six::Data& data,
        nitf::DESubheader& subheader) const
{
    // If NITRO doesn't know this TRE (because our plugin path isn't set),
    // we won't actually get an exception in the TRE constructor because it'll
    // use a default handler for the TRE.  Instead, we'll get a no such key
    // exception when it doesn't know what the fields below are.  try/catch
    // this so we can supply a more obvious error message.
    nitf::TRE tre(Constants::DES_USER_DEFINED_SUBHEADER_TAG,
                  Constants::DES_USER_DEFINED_SUBHEADER_ID);

    try
    {
        tre["DESCRC"] = "99999";
        tre["DESSHFT"] = "XML";
        tre["DESSHDT"] = data.getCreationTime().format("%Y-%m-%dT%H:%M:%SZ");
        setField("DESSHRP", mOrganizationId, tre);

        const std::string dataType =
                (data.getDataType() == DataType::COMPLEX) ? "SICD" : "SIDD";
        tre["DESSHSI"] = dataType +
                " Volume 1 Design & Implementation Description Document";

        // This is the publication date and version of the
        // Design and Implementation Description Document 
        // for the specification -- Add to this list as more
        // versions are published
        const std::string version(data.getVersion());
        std::string specVers;
        std::string specDT;
        if (dataType == "SICD")
        {    
            if (version == "1.0.0" || version == "1.0.1")
            {
                specVers = "1.0";
                specDT = "2011-09-28T00:00:00Z";
            }
        }
        else if (dataType == "SIDD")
        {
            if (version == "1.0.0")
            {
                specVers = "1.0";
                specDT = "2011-08-01T00:00:00Z";
            }
        }

        // spec version
        if (specVers.empty())
        {
            throw except::Exception(Ctxt(
                "DESSHSV Failure - Unsupported in " + dataType +
                " version: " + version));
        }
        tre["DESSHSV"] = specVers;

        // spec publication Date/Time
        if (specDT.empty())
        {
            throw except::Exception(Ctxt(
                "DESSHSD Failure - Unsupported in " + dataType +
                " version: " + version));
        }
        tre["DESSHSD"] = specDT;

        tre["DESSHTN"] = "urn:" + dataType + ":" + version;
        tre["DESSHLPG"] = toString(data.getImageCorners());

        // Spec specifies leaving this blank
        tre["DESSHLPT"] = "";

        setField("DESSHLI", mLocationId, tre);
        setField("DESSHLIN", mLocationIdNamespace, tre);
        setField("DESSHABS", mAbstract, tre);
    }
    catch (const except::NoSuchKeyException& )
    {
        throw except::NoSuchKeyException(Ctxt(
                "Must have '" +
                std::string(Constants::DES_USER_DEFINED_SUBHEADER_TAG) +
                "' plugin on the plugin path.  Either set the "
                "NITF_PLUGIN_PATH environment variable or use "
                "six::loadPluginDir()"));
    }
    subheader.setSubheaderFields(tre);
}


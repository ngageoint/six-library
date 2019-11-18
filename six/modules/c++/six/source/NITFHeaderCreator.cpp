/* =========================================================================
 * This file is part of six-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2018, MDA Information Systems LLC
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
#include <six/NITFHeaderCreator.h>
#include <six/XMLControlFactory.h>
#include <six/WriteControl.h>

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

std::string generateILOC(size_t row, size_t col)
{
    std::ostringstream ostr;
    ostr.fill('0');
    ostr << std::setw(5) << row << std::setw(5) << col;

    return ostr.str();
}

std::string generateILOC(const types::RowCol<size_t>& dims)
{
    return generateILOC(dims.row, dims.col);
}

class GetDisplayLutFromLegend
{
public:
    GetDisplayLutFromLegend(const six::Legend& legend) :
        mLegend(legend)
    {
    }

    const six::LUT* operator()() const
    {
        return mLegend.mLUT.get();
    }

private:
    const six::Legend& mLegend;
};
}

namespace six
{
const char NITFHeaderCreator::OPT_MAX_PRODUCT_SIZE[] = "MaxProductSize";
const char NITFHeaderCreator::OPT_MAX_ILOC_ROWS[] = "MaxILOCRows";
const char NITFHeaderCreator::OPT_J2K_COMPRESSION_BYTERATE[] = "J2KCompressionByterate";
const char NITFHeaderCreator::OPT_J2K_COMPRESSION_LOSSLESS[] = "J2KCompressionLossless";
const char NITFHeaderCreator::OPT_NUM_ROWS_PER_BLOCK[] = "NumRowsPerBlock";
const char NITFHeaderCreator::OPT_NUM_COLS_PER_BLOCK[] = "NumColsPerBlock";
const size_t NITFHeaderCreator::DEFAULT_BUFFER_SIZE = 8 * 1024 * 1024;

NITFHeaderCreator::NITFHeaderCreator() : 
    mRecord(NITF_VER_21),
    mXMLRegistry(NULL),
    mLog(NULL),
    mOwnLog(false)
{
    // Make sure that if we use XML_DATA_CONTENT that we've loaded it into the
    // singleton PluginRegistry
    loadXmlDataContentHandler();
}

NITFHeaderCreator::NITFHeaderCreator(mem::SharedPtr<Container> container) : 
    mRecord(NITF_VER_21),
    mXMLRegistry(NULL),
    mLog(NULL),
    mOwnLog(false)
{
    loadXmlDataContentHandler();
    initialize(container);
}

NITFHeaderCreator::NITFHeaderCreator(const six::Options& options,
                                     mem::SharedPtr<Container> container) : 
    mRecord(NITF_VER_21),
    mXMLRegistry(NULL),
    mLog(NULL),
    mOwnLog(false)
{
    loadXmlDataContentHandler();
    initialize(options, container);
}

std::string NITFHeaderCreator::getComplexIID(size_t segmentNum,
                                             size_t numImageSegments)
{
    // SICD###
    std::ostringstream ostr;
    ostr << six::toString(DataType(DataType::COMPLEX))
         << std::setfill('0') << std::setw(3)
         << ((numImageSegments > 1) ? segmentNum + 1 : segmentNum);

    return ostr.str();
}

std::string NITFHeaderCreator::getDerivedIID(size_t segmentNum,
                                          size_t productNum)
{
    // SIDD######
    std::ostringstream ostr;
    ostr << six::toString(DataType(DataType::DERIVED))
         << std::setfill('0')
         << std::setw(3) << (productNum + 1)
         << std::setw(3) << (segmentNum + 1);

    return ostr.str();
}

std::string NITFHeaderCreator::getIID(DataType dataType,
                                      size_t segmentNum,
                                      size_t numImageSegments,
                                      size_t productNum)
{
    if (dataType == DataType::COMPLEX)
    {
        return getComplexIID(segmentNum, numImageSegments);
    }
    else
    {
        return getDerivedIID(segmentNum, productNum);
    }
}

void NITFHeaderCreator::setBlocking(const std::string& imode,
                                    const types::RowCol<size_t>& segmentDims,
                                    nitf::ImageSubheader& subheader)
{
    const bool isSICD = (mContainer->getDataType() == DataType::COMPLEX);

    nitf::Uint32 numRowsPerBlock;
    if (mOptions.hasParameter(OPT_NUM_ROWS_PER_BLOCK))
    {
        if (isSICD)
        {
            throw except::Exception(Ctxt("SICDs do not support blocking"));
        }

        const size_t optNumRowsPerBlock = static_cast<size_t>(
                mOptions.getParameter(OPT_NUM_ROWS_PER_BLOCK));

        numRowsPerBlock = static_cast<sys::Uint32_T>(
                std::min(optNumRowsPerBlock, segmentDims.row));
    }
    else
    {
        // Unblocked (per 2500C, if > 8192, should be set to 0)
        numRowsPerBlock = (segmentDims.row > 8192) ?
                0 : static_cast<nitf::Uint32>(segmentDims.row);
    }

    nitf::Uint32 numColsPerBlock;
    if (mOptions.hasParameter(OPT_NUM_COLS_PER_BLOCK))
    {
        if (isSICD)
        {
            throw except::Exception(Ctxt("SICDs do not support blocking"));
        }

        const size_t optNumColsPerBlock = static_cast<size_t>(
                mOptions.getParameter(OPT_NUM_COLS_PER_BLOCK));

        numColsPerBlock = static_cast<sys::Uint32_T>(
                std::min(optNumColsPerBlock, segmentDims.col));
    }
    else
    {
        // Unblocked (per 2500C, if > 8192, should be set to 0)
        numColsPerBlock = (segmentDims.col > 8192) ? 0 :
                static_cast<nitf::Uint32>(segmentDims.col);
    }

    subheader.setBlocking(static_cast<nitf::Uint32>(segmentDims.row),
                          static_cast<nitf::Uint32>(segmentDims.col),
                          numRowsPerBlock,
                          numColsPerBlock,
                          imode);
}

void NITFHeaderCreator::setImageSecurity(
    const six::Classification& classification,
    nitf::ImageSubheader& subheader)
{
    //This requires a normalized name to get set correctly
    subheader.getImageSecurityClass().set(
        getNITFClassification(classification.getLevel()));
    setSecurity(classification, subheader.getSecurityGroup(), "IS");
}

void NITFHeaderCreator::setDESecurity(
    const six::Classification& classification,
    nitf::DESubheader& subheader)
{
    subheader.getSecurityClass().set(
        getNITFClassification(classification.getLevel()));
    setSecurity(classification, subheader.getSecurityGroup(), "DES");
}

void NITFHeaderCreator::setSecurity(
    const six::Classification& classification,
    nitf::FileSecurity security,
    const std::string& prefix)
{
    std::string k;
    const Options& ops = classification.fileOptions;

    k = NITFImageInfo::generateFieldKey(NITFImageInfo::CLSY, prefix);
    if (ops.hasParameter(k))
    {
        Parameter p = ops.getParameter(k);
        security.getClassificationSystem().set(p.str());
    }
    k = NITFImageInfo::generateFieldKey(NITFImageInfo::CODE, prefix);
    if (ops.hasParameter(k))
    {
        Parameter p = ops.getParameter(k);
        security.getCodewords().set(p.str());
    }
    k = NITFImageInfo::generateFieldKey(NITFImageInfo::CTLH, prefix);
    if (ops.hasParameter(k))
    {
        Parameter p = ops.getParameter(k);
        security.getControlAndHandling().set(p.str());
    }
    k = NITFImageInfo::generateFieldKey(NITFImageInfo::REL, prefix);
    if (ops.hasParameter(k))
    {
        Parameter p = ops.getParameter(k);
        security.getReleasingInstructions().set(p.str());
    }
    k = NITFImageInfo::generateFieldKey(NITFImageInfo::DCTP, prefix);
    if (ops.hasParameter(k))
    {
        Parameter p = ops.getParameter(k);
        security.getDeclassificationType().set(p.str());
    }
    k = NITFImageInfo::generateFieldKey(NITFImageInfo::DCDT, prefix);
    if (ops.hasParameter(k))
    {
        Parameter p = ops.getParameter(k);
        security.getDeclassificationDate().set(p.str());
    }
    k = NITFImageInfo::generateFieldKey(NITFImageInfo::DCXM, prefix);
    if (ops.hasParameter(k))
    {
        Parameter p = ops.getParameter(k);
        security.getDeclassificationExemption().set(p.str());
    }
    k = NITFImageInfo::generateFieldKey(NITFImageInfo::DG, prefix);
    if (ops.hasParameter(k))
    {
        Parameter p = ops.getParameter(k);
        security.getDowngrade().set(p.str());
    }
    k = NITFImageInfo::generateFieldKey(NITFImageInfo::DGDT, prefix);
    if (ops.hasParameter(k))
    {
        Parameter p = ops.getParameter(k);
        security.getDowngradeDateTime().set(p.str());
    }
    k = NITFImageInfo::generateFieldKey(NITFImageInfo::CLTX, prefix);
    if (ops.hasParameter(k))
    {
        Parameter p = ops.getParameter(k);
        security.getClassificationText().set(p.str());
    }
    k = NITFImageInfo::generateFieldKey(NITFImageInfo::CATP, prefix);
    if (ops.hasParameter(k))
    {
        Parameter p = ops.getParameter(k);
        security.getClassificationAuthorityType().set(p.str());
    }
    k = NITFImageInfo::generateFieldKey(NITFImageInfo::CAUT, prefix);
    if (ops.hasParameter(k))
    {
        Parameter p = ops.getParameter(k);
        security.getClassificationAuthority().set(p.str());
    }
    k = NITFImageInfo::generateFieldKey(NITFImageInfo::CRSN, prefix);
    if (ops.hasParameter(k))
    {
        Parameter p = ops.getParameter(k);
        security.getClassificationReason().set(p.str());
    }
    k = NITFImageInfo::generateFieldKey(NITFImageInfo::SRDT, prefix);
    if (ops.hasParameter(k))
    {
        Parameter p = ops.getParameter(k);
        security.getSecuritySourceDate().set(p.str());
    }
    k = NITFImageInfo::generateFieldKey(NITFImageInfo::CTLN, prefix);
    if (ops.hasParameter(k))
    {
        Parameter p = ops.getParameter(k);
        security.getSecurityControlNumber().set(p.str());
    }

    // Now, do some specific overrides
    if (security.getClassificationSystem().toString().empty())
    {
        security.getClassificationSystem().set("US");
    }

    if (mLog != NULL)
    {
        classification.setSecurity(prefix, *mLog, security);
    }
}

std::string NITFHeaderCreator::getNITFClassification(const std::string& level)
{
    // If the string starts with UCRST (after any leading whitespaces),
    // return the correct identifier
    for (size_t ii = 0; ii < level.length(); ++ii)
    {
        const char ch(static_cast<char>(::toupper(level[ii])));
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

void NITFHeaderCreator::updateFileHeaderSecurity()
{
    nitf::Record& record = getRecord();

    bool changed = false;
    std::string classOrder = "URCST";
    size_t foundLoc =
            classOrder.find(record.getHeader().getClassification().toString());
    int classIndex = foundLoc != std::string::npos ?
            static_cast<int>(foundLoc) : -1;

    nitf::FileSecurity highest = record.getHeader().getSecurityGroup();

    for (size_t i = 0, size = record.getImages().getSize(); i < size; i++)
    {
        nitf::ImageSubheader subheader =
                nitf::ImageSegment(record.getImages()[i]).getSubheader();
        foundLoc = classOrder.find(subheader.getImageSecurityClass().toString());
        int idx = foundLoc != std::string::npos ?
                static_cast<int>(foundLoc) : -1;

        if (idx > classIndex)
        {
            highest = subheader.getSecurityGroup();
            classIndex = idx;
            changed = true;
        }
    }

    for (size_t i = 0, size = record.getDataExtensions().getSize(); i < size; i++)
    {
        nitf::DESubheader subheader =
                nitf::DESegment(record.getDataExtensions()[i]).getSubheader();
        foundLoc = classOrder.find(subheader.getSecurityClass().toString());
        int idx = foundLoc != std::string::npos ?
            static_cast<int>(foundLoc) : -1;

        if (idx > classIndex)
        {
            highest = subheader.getSecurityGroup();
            classIndex = idx;
            changed = true;
        }
    }

    if (changed)
    {
        record.getHeader().getClassification() = classOrder.substr(classIndex,
                                                                   1);
        record.getHeader().setSecurityGroup(highest.clone());
    }
}

std::string NITFHeaderCreator::getDesTypeID(const six::Data& data)
{
    // TODO: This requires some knowledge of the SICD/SIDD versions, but not
    //       sure where else this logic should live.

    const std::string version(data.getVersion());

    std::vector<std::string> versionParts;
    XMLControl::splitVersion(version, versionParts);

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

bool NITFHeaderCreator::needUserDefinedSubheader(const six::Data& data)
{
    // TODO: This requires some knowledge of the SICD/SIDD versions, but not
    //       sure where else this logic should live.

    const std::string version(data.getVersion());

    std::vector<std::string> versionParts;
    XMLControl::splitVersion(version, versionParts);

    // Starting with 1.0, the user-defined subheader is required
    const std::string& majorVersion = versionParts[0];
    return (majorVersion != "0");
}

void NITFHeaderCreator::setOrganizationId(const std::string& organizationId)
{
    mOrganizationId = organizationId;
}

void NITFHeaderCreator::setLocationIdentifier(
    const std::string& locationId,
    const std::string& locationIdNamespace)
{
    mLocationId = locationId;
    mLocationIdNamespace = locationIdNamespace;
}

void NITFHeaderCreator::setAbstract(const std::string& abstract)
{
    mAbstract = abstract;
}

void NITFHeaderCreator::addUserDefinedSubheader(
    const six::Data& data,
    nitf::DESubheader& subheader) const
{
    nitf::TRE tre(Constants::DES_USER_DEFINED_SUBHEADER_TAG,
                  Constants::DES_USER_DEFINED_SUBHEADER_ID);

    tre["DESCRC"] = "99999";
    tre["DESSHFT"] = "XML";
    tre["DESSHDT"] = data.getCreationTime().format("%Y-%m-%dT%H:%M:%SZ");
    setField("DESSHRP", mOrganizationId, tre);

    const std::string dataType =
            (data.getDataType() == DataType::COMPLEX) ? "SICD" : "SIDD";

    if (dataType == "SICD")
    {
        tre["DESSHSI"] = Constants::SICD_DESSHSI;
    }
    else
    {
        tre["DESSHSI"] = Constants::SIDD_DESSHSI;
    }

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
        else if (version == "1.1.0")
        {
            specVers = "1.1";
            specDT = "2014-07-08T00:00:00Z";
        }
        else if (version == "1.2.0")
        {
            specVers = "1.2";
            // TODO: Update specDT once final document is posted
            specDT = "2016-04-12T00:00:00Z";
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

    subheader.setSubheaderFields(tre);
}

void NITFHeaderCreator::addAdditionalDES(mem::SharedPtr<nitf::SegmentWriter> segmentWriter)
{
    if (segmentWriter.get() == NULL)
    {
        throw except::Exception(Ctxt("segmentWriter is NULL"));
    }

    mSegmentWriters.push_back(segmentWriter);
}

void NITFHeaderCreator::initialize(mem::SharedPtr<Container> container)
{
    mContainer = container;

    if (container->getNumData() == 0)
    {
        return;
    }

    // Clean up
    mInfos.clear();

    sys::Uint32_T ilocMax = Constants::ILOC_MAX;
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
    if (container->getDataType() == DataType::COMPLEX)
    {
        mem::SharedPtr<NITFImageInfo>
            info(new NITFImageInfo(container->getData(0),
                                   maxRows,
                                   maxSize,
                                   true,
                                   0,
                                   0));

        mInfos.push_back(info);
    }
    else
    {
        // J2K only available for derived data
        j2kCompression = (double) mOptions.getParameter(
                OPT_J2K_COMPRESSION_BYTERATE, Parameter(0));
        enableJ2K = (j2kCompression <= 1.0) && j2kCompression > 0.0001;

        // get row blocking parameters
        nitf::Uint32 optNumRowsPerBlock = 0;
        if (mOptions.hasParameter(OPT_NUM_ROWS_PER_BLOCK)) {
            optNumRowsPerBlock =
                static_cast<nitf::Uint32>(mOptions.getParameter(OPT_NUM_ROWS_PER_BLOCK));
        }

        // get column blocking parameters
        nitf::Uint32 optNumColsPerBlock = 0;
        if (mOptions.hasParameter(OPT_NUM_COLS_PER_BLOCK)) {
            optNumColsPerBlock =
                static_cast<nitf::Uint32>(mOptions.getParameter(OPT_NUM_COLS_PER_BLOCK));
        }

        for (size_t ii = 0; ii < container->getNumData(); ++ii)
        {
            Data* const ith = container->getData(ii);
            if (ith->getDataType() == DataType::DERIVED)
            {
                const nitf::Uint32 numRowsPerBlock = std::min(
                        optNumRowsPerBlock,
                        static_cast<nitf::Uint32>(ith->getNumRows()));

                const nitf::Uint32 numColsPerBlock = std::min(
                        optNumColsPerBlock,
                        static_cast<nitf::Uint32>(ith->getNumCols()));

                mem::SharedPtr<NITFImageInfo>
                    info(new NITFImageInfo(ith,
                                           maxRows,
                                           maxSize,
                                           true,
                                           numRowsPerBlock,
                                           numColsPerBlock));

                mInfos.push_back(info);
            }
        }
    }

    nitf::Record& record = getRecord();

    DataType dataType = container->getDataType();
    std::string name = mInfos[0]->getData()->getName();
    std::string fileTitle = six::toString(dataType) + ": " + name;
    fileTitle = fileTitle.substr(0, NITF_FTITLE_SZ); //truncate past 80
    record.getHeader().getFileTitle().set(fileTitle);

    // Make sure the date/time exactly matches what's in the XML rather than
    // letting NITRO set it automatically at write time
    const DateTime dt(container->getData(0)->getCreationTime());
    record.getHeader().getFileDateTime().set(dt.format(NITF_DATE_FORMAT_21));

    size_t startIndex = 0;
    for (size_t ii = 0; ii < mInfos.size(); ++ii)
    {
        NITFImageInfo& info = *mInfos[ii];

        const std::vector <NITFSegmentInfo> imageSegments
                = info.getImageSegments();

        size_t numIS = imageSegments.size();
        nitf::Uint32 nbpp =
                static_cast<nitf::Uint32>(info.getNumBitsPerPixel());
        size_t numCols = info.getData()->getNumCols();
        std::string irep = info.getRepresentation();
        std::string imode = info.getMode();
        std::string pvtype = info.getPixelValueType();
        // NITRO wants to see this, not our corners object
        double corners[4][2];

        std::string targetId;

        // TODO: Subclass to get this?
        // if (info.getData()->getDataType() == DataType::DERIVED)
        //         {
        //             DerivedData* derived = (DerivedData*)info.getData();
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
        info.setStartIndex(startIndex);
        startIndex += numIS;

        // Images
        const std::string imageSource = info.getData()->getSource();
        for (size_t jj = 0; jj < numIS; ++jj)
        {
            NITFSegmentInfo segmentInfo = imageSegments[jj];

            nitf::ImageSegment imageSegment = record.newImageSegment();
            nitf::ImageSubheader subheader = imageSegment.getSubheader();

            subheader.getImageTitle().set(fileTitle);
            const DateTime collectionDT =
                    info.getData()->getCollectionStartDateTime();
            subheader.getImageDateAndTime().set(collectionDT);
            subheader.getImageId().set(getIID(dataType, jj, numIS, ii));
            subheader.getImageSource().set(imageSource);

            // Fill out ILOC with the row offset, making sure it's in range
            if (segmentInfo.rowOffset > maxRows)
            {
                std::ostringstream ostr;
                ostr << "Row offset cannot exceed " << maxRows
                     << ", but for image segment " << jj << " it is "
                     << segmentInfo.firstRow;

                throw except::Exception(Ctxt(ostr.str()));
            }

            subheader.getImageLocation().set(generateILOC(segmentInfo.rowOffset,
                                                          0));

            subheader.getTargetId().set(targetId);

            std::vector<nitf::BandInfo> bandInfo = info.getBandInfo();

            subheader.setPixelInformation(pvtype, nbpp, nbpp, "R", irep, "SAR",
                                          bandInfo);

            setBlocking(imode,
                        types::RowCol<size_t>(segmentInfo.numRows, numCols),
                        subheader);

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
                        (nitf::Uint16)(info.getStartIndex() + jj));
            }

            for (size_t kk = 0; kk < LatLonCorners::NUM_CORNERS; ++kk)
            {
                corners[kk][0] = segmentInfo.corners.getCorner(kk).getLat();
                corners[kk][1] = segmentInfo.corners.getCorner(kk).getLon();
            }
            subheader.setCornersFromLatLons(NITF_CORNERS_GEO, corners);

            setImageSecurity(info.getData()->getClassification(), subheader);

            if (enableJ2K)
            {
                subheader.getImageCompression().set("C8");

                //calculate comrat
                const int comratInt = static_cast<int>(
                        math::round((j2kCompression * nbpp * 10.0)));
                const bool isNumericallyLossless = static_cast<bool>(
                        mOptions.getParameter(
                        OPT_J2K_COMPRESSION_LOSSLESS, Parameter(false)));
                // We are assuming the image is not compressed so heavily as
                // to be visually lossy
                const char comratChar = isNumericallyLossless ? 'N' : 'V';
                std::ostringstream comratStream;
                comratStream << comratChar;
                comratStream << std::setw(3) << std::setfill('0') << comratInt;
                subheader.getCompressionRate().set(comratStream.str());

            }
        }

        // Optional legend
        const Legend* const legend = container->getLegend(ii);
        if (legend)
        {
            nitf::ImageSegment imageSegment = record.newImageSegment();
            nitf::ImageSubheader subheader = imageSegment.getSubheader();

            subheader.getImageTitle().set(fileTitle);
            const DateTime collectionDT =
                    info.getData()->getCollectionStartDateTime();
            subheader.getImageDateAndTime().set(collectionDT);
            subheader.getImageId().set(getDerivedIID(numIS, ii));
            subheader.getImageSource().set(imageSource);

            subheader.getImageLocation().set(generateILOC(legend->mLocation));

            // Set NBPP and sanity check if LUT is set appropriately
            nitf::Uint32 legendNbpp;
            switch (legend->mType)
            {
            case PixelType::MONO8I:
                // We shouldn't have a LUT
                if (legend->mLUT.get())
                {
                    throw except::Exception(Ctxt(
                            "LUT shouldn't be present for mono legend"));
                }
                legendNbpp = 8;
                break;

            case PixelType::RGB8LU:
                // We should have a legend
                if (legend->mLUT.get() == NULL)
                {
                    throw except::Exception(Ctxt(
                            "LUT should be present for indexed RGB legend"));
                }
                legendNbpp = 8;
                break;

            default:
                throw except::Exception(Ctxt("Unsupported legend pixel type"));
            }

            const GetDisplayLutFromLegend getLUT(*legend);
            std::vector<nitf::BandInfo> bandInfo =
                    NITFImageInfo::getBandInfoImpl(legend->mType, getLUT);

            subheader.setPixelInformation(
                    NITFImageInfo::getPixelValueType(legend->mType),
                    legendNbpp,
                    legendNbpp,
                    "R",
                    NITFImageInfo::getRepresentation(legend->mType),
                    "LEG",
                    bandInfo);

            subheader.setBlocking(static_cast<nitf::Uint32>(legend->mDims.row),
                                  static_cast<nitf::Uint32>(legend->mDims.col),
                                  0, 0, NITFImageInfo::getMode(legend->mType));

            // While we never set IDLVL explicitly in here, NITRO will
            // kindly do that for us (incrementing it once for each segment).
            // We want to set the legend's IALVL to the IDLVL we want to attach
            // to (which is the first image segment for this product which is
            // conveniently at info.getStartIndex()... but IDLVL is 1-based).
            subheader.getImageAttachmentLevel().set(static_cast<nitf::Uint16>(
                       info.getStartIndex() + 1));

            setImageSecurity(info.getData()->getClassification(), subheader);

            ++startIndex;
        }
    }

    for (size_t ii = 0; ii < container->getNumData(); ++ii)
    {
        const six::Data& data(*container->getData(ii));

        // Write out a DES
        nitf::DESegment seg = record.newDataExtensionSegment();
        nitf::DESubheader subheader = seg.getSubheader();
        subheader.getTypeID().set(getDesTypeID(data));
        subheader.getVersion().set(Constants::DES_VERSION_STR);

        if (needUserDefinedSubheader(data))
        {
            addUserDefinedSubheader(data,
                                    subheader);
        }

        setDESecurity(data.getClassification(), subheader);
    }

    updateFileHeaderSecurity();
}

void NITFHeaderCreator::initialize(
    const six::Options& options,
    mem::SharedPtr<Container> container)
{
    mOptions = options;
    initialize(container);
}

void NITFHeaderCreator::loadMeshSegment(const std::string& meshName,
                                        const std::vector<sys::byte>& meshBuffer,
                                        const six::Classification& classification)
{
    nitf::Record& record = getRecord();

    // Populate the data extension subheader
    nitf::DESegment des = record.newDataExtensionSegment();
    nitf::DESubheader subheader = des.getSubheader();
    nitf::FileSecurity security = record.getHeader().getSecurityGroup();
    subheader.getFilePartType().set("DE");
    subheader.getTypeID().set(meshName);
    subheader.getVersion().set(six::Constants::DES_VERSION_STR);
    subheader.getSecurityClass().set(getNITFClassification(
                                        classification.getLevel()));
    subheader.setSecurityGroup(security.clone());

    // Add the data and writer for this segment
    nitf::SegmentMemorySource dataSource(
        meshBuffer.data(), meshBuffer.size(), 0, 0, true);
    mem::SharedPtr<nitf::SegmentWriter> desWriter(
        new nitf::SegmentWriter(dataSource));
    addAdditionalDES(desWriter);
}
}

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

#include <sstream>

#include <mem/ScopedArray.h>
#include <six/NITFWriteControl.h>
#include <six/XMLControlFactory.h>

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
const char NITFWriteControl::OPT_MAX_PRODUCT_SIZE[] = "MaxProductSize";
const char NITFWriteControl::OPT_MAX_ILOC_ROWS[] = "MaxILOCRows";
const char NITFWriteControl::OPT_J2K_COMPRESSION[] = "J2KCompression";
const char NITFWriteControl::OPT_NUM_ROWS_PER_BLOCK[] = "NumRowsPerBlock";
const char NITFWriteControl::OPT_NUM_COLS_PER_BLOCK[] = "NumColsPerBlock";

void NITFWriteControl::initialize(Container* container)
{
    // Clean up
    // NOTE  We do not own the container, so we don't delete 'mContainer' here
    mInfos.clear();

    mContainer = container;

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
    if (mContainer->getDataType() == DataType::COMPLEX)
    {
        mem::SharedPtr<NITFImageInfo>
            info(new NITFImageInfo(mContainer->getData(0),
                                   maxRows,
                                   maxSize,
                                   true));

        mInfos.push_back(info);
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
                mem::SharedPtr<NITFImageInfo>
                    info(new NITFImageInfo(ith, maxRows, maxSize, true));

                mInfos.push_back(info);
            }
        }
    }
    mRecord = nitf::Record(NITF_VER_21);

    DataType dataType = mContainer->getDataType();
    std::string name = mInfos[0]->getData()->getName();
    std::string fileTitle = six::toString(dataType) + ": " + name;
    fileTitle = fileTitle.substr(0, NITF_FTITLE_SZ); //truncate past 80
    mRecord.getHeader().getFileTitle().set(fileTitle);

    // Make sure the date/time exactly matches what's in the XML rather than
    // letting NITRO set it automatically at write time
    const DateTime dt(mContainer->getData(0)->getCreationTime());
    mRecord.getHeader().getFileDateTime().set(dt.format(NITF_DATE_FORMAT_21));

    int startIndex = 0;
    for (size_t ii = 0; ii < mInfos.size(); ++ii)
    {
        NITFImageInfo& info = *mInfos[ii];

        const std::vector <NITFSegmentInfo> imageSegments
                = info.getImageSegments();

        size_t numIS = imageSegments.size();
        int nbpp = info.getNumBitsPerPixel();
        int numCols = info.getData()->getNumCols();
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

            nitf::ImageSegment imageSegment = mRecord.newImageSegment();
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
                int comratInt = (int)((j2kCompression * nbpp * 10.0) + 0.5);
                std::string comrat = FmtX("N%03d", comratInt);
                subheader.getCompressionRate().set(comrat);
            }
        }

        // Optional legend
        const Legend* const legend = mContainer->getLegend(ii);
        if (legend)
        {
            nitf::ImageSegment imageSegment = mRecord.newImageSegment();
            nitf::ImageSubheader subheader = imageSegment.getSubheader();

            subheader.getImageTitle().set(fileTitle);
            const DateTime collectionDT =
                    info.getData()->getCollectionStartDateTime();
            subheader.getImageDateAndTime().set(collectionDT);
            subheader.getImageId().set(getDerivedIID(numIS, ii));
            subheader.getImageSource().set(imageSource);

            subheader.getImageLocation().set(generateILOC(legend->mLocation));

            // Set NBPP and sanity check if LUT is set appropriately
            size_t legendNbpp;
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

            subheader.setBlocking(legend->mDims.row, legend->mDims.col, 0, 0,
                                  NITFImageInfo::getMode(legend->mType));

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

std::string NITFWriteControl::getComplexIID(size_t segmentNum,
											size_t numImageSegments)
{
	// SICD###
	std::ostringstream ostr;
	ostr << six::toString(DataType(DataType::COMPLEX))
	     << std::setfill('0') << std::setw(3)
	     << ((numImageSegments > 1) ? segmentNum + 1 : segmentNum);

	return ostr.str();
}

std::string NITFWriteControl::getDerivedIID(size_t segmentNum,
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

std::string NITFWriteControl::getIID(DataType dataType,
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

void NITFWriteControl::setBlocking(const std::string& imode,
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

        numRowsPerBlock = static_cast<sys::Uint32_T>(
                mOptions.getParameter(OPT_NUM_ROWS_PER_BLOCK));
    }
    else
    {
        // Unblocked (per 2500C, if > 8192, should be set to 0)
        numRowsPerBlock = (segmentDims.row > 8192) ?
                0 : segmentDims.row;
    }

    nitf::Uint32 numColsPerBlock;
    if (mOptions.hasParameter(OPT_NUM_COLS_PER_BLOCK))
    {
        if (isSICD)
        {
            throw except::Exception(Ctxt("SICDs do not support blocking"));
        }

        numColsPerBlock = static_cast<sys::Uint32_T>(
                mOptions.getParameter(OPT_NUM_COLS_PER_BLOCK));
    }
    else
    {
        // Unblocked (per 2500C, if > 8192, should be set to 0)
        numColsPerBlock = (segmentDims.col > 8192) ? 0 : segmentDims.col;
    }

    subheader.setBlocking(segmentDims.row,
                          segmentDims.col,
                          numRowsPerBlock,
                          numColsPerBlock,
                          imode);
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
        // Do what they say.  You really shouldn't do this
        // unless you know what you're doing anyway!
        doByteSwap = byteSwapping ? true : false;
    }

    if (mInfos.size() != imageData.size())
    {
        std::ostringstream ostr;
        ostr << "Require " << mInfos.size() << " images, received "
             << imageData.size();
        throw except::Exception(Ctxt(ostr.str()));
    }

    size_t numImages = mInfos.size();

    //! TODO: This section of code (unlike the memory section below)
    //        does not account for blocked writing or J2K compression.
    //        CODA ticket #443 will update support for this.
    for (size_t i = 0; i < numImages; ++i)
    {
        const NITFImageInfo& info = *mInfos[i];
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

            mWriter.setImageWriteHandler(info.getStartIndex() + j,
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
    for (size_t i = 0; i < numImages; ++i)
    {
        const NITFImageInfo& info = *mInfos[i];
        std::vector < NITFSegmentInfo > imageSegments
                = info.getImageSegments();
        const size_t numIS = imageSegments.size();
        const size_t pixelSize = info.getData()->getNumBytesPerPixel();
        const size_t numCols = info.getData()->getNumCols();
        const size_t numChannels = info.getData()->getNumChannels();

        nitf::ImageSegment imageSegment = mRecord.getImages()[i];
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
                    mWriter.newImageWriter(info.getStartIndex() + jj,
                                           mCompressionOptions);
                iWriter.setWriteCaching(1);

                nitf::ImageSource iSource;
                const size_t bandSize = numCols * info.getData()->getNumRows();

                for (size_t chan = 0; chan < numChannels; ++chan)
                {
                    nitf::MemorySource ms(imageData[i], bandSize,
                                          bandSize * chan, pixelSize, 0);
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
                mWriter.setImageWriteHandler(info.getStartIndex() + jj,
                                             writeHandler);
            }
        }

        const Legend* const legend = mContainer->getLegend(i);
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
                mWriter.newImageWriter(info.getStartIndex() + numIS);
            iWriter.setWriteCaching(1);
            iWriter.attachSource(iSource);
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
            else if (version == "1.1.0")
            {
                specVers = "1.1";
                specDT = "2014-07-08T00:00:00Z";
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
}

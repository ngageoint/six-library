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

#include <six/NITFReadControl.h>
#include <six/XMLControlFactory.h>
#include <six/Utilities.h>

using namespace six;

DataType NITFReadControl::getDataType(const std::string& fromFile) const
{
    // Could cache this
    if (mReader.getNITFVersion(fromFile) != NITF_VER_UNKNOWN)
    {
        nitf::IOHandle inFile(fromFile);
        nitf::Record rec = mReader.read(inFile);
        inFile.close();
        std::string title = rec.getHeader().getFileTitle().toString();
        if (str::startsWith(title, "SICD"))
            return DataType::COMPLEX;
        else if (str::startsWith(title, "SIDD"))
            return DataType::DERIVED;
    }
    return DataType::NOT_SET;
}

void NITFReadControl::validateSegment(nitf::ImageSubheader subheader,
                                      const NITFImageInfo* info)
{
    const size_t numBandsSeg =
            static_cast<nitf::Uint32>(subheader.getNumImageBands());

    const std::string pjust = subheader.getPixelJustification().toString();
    // TODO: More validation in here!
    if (pjust != "R")
    {
        throw except::Exception(Ctxt("Expected right pixel justification"));
    }

    // The number of bytes per pixel, which we count to be 3 in the
    // case of 24-bit true color and 8 in the case of complex float
    // and 1 in the case of most SIDD data
    const size_t numBitsPerPixel =
            static_cast<nitf::Uint32>(subheader.getNumBitsPerPixel());
    const size_t numBytesPerPixel = (numBitsPerPixel + 7) / 8;
    const size_t numBytesSeg = numBytesPerPixel * numBandsSeg;

    const size_t nbpp = info->getData()->getNumBytesPerPixel();
    if (numBytesSeg != nbpp)
    {
        std::ostringstream ostr;
        ostr << "Expected [" << nbpp << "] bytes per pixel, found ["
             << numBytesSeg << "]";
        throw except::Exception(Ctxt(ostr.str()));
    }

    const size_t numCols = info->getData()->getNumCols();
    const size_t numColsSubheader =
            static_cast<nitf::Uint32>(subheader.getNumCols());

    if (numColsSubheader != numCols)
    {
        std::ostringstream ostr;
        ostr << "Invalid column width: was expecting [" << numCols
             << "], got [" << numColsSubheader << "]";
        throw except::Exception(Ctxt(ostr.str()));
    }

}

void NITFReadControl::load(const std::string& fromFile,
                           const std::vector<std::string>& schemaPaths)
{
    nitf::IOHandle handle(fromFile);
    load(handle, schemaPaths);
}

void NITFReadControl::load(nitf::IOInterface& interface)
{
    load(interface, std::vector<std::string>());
}

void NITFReadControl::load(nitf::IOInterface& interface,
                           const std::vector<std::string>& schemaPaths)
{
    reset();

    mRecord = mReader.readIO(interface);
    const std::string title = mRecord.getHeader().getFileTitle().toString();

    DataType dataType;
    if (str::startsWith(title, "SICD"))
        dataType = DataType::COMPLEX;
    else if (str::startsWith(title, "SIDD"))
        dataType = DataType::DERIVED;
    else
        throw except::Exception(Ctxt("Unexpected file type"));

    mContainer = new Container(dataType);

    // First, read in the DE segments, and organize them
    nitf::List des = mRecord.getDataExtensions();
    nitf::ListIterator desIter = des.begin();

    for (size_t i = 0; desIter != des.end(); ++desIter, ++i)
    {
        // Get a segment ref
        nitf::DESegment segment = (nitf::DESegment) * desIter;
        nitf::DESubheader subheader = segment.getSubheader();
        std::string desid = subheader.getTypeID().toString();
        str::trim(desid);

        // SICD/SIDD 1.0 specify DESID as XML_DATA_CONTENT
        // Older versions of the spec specified it as SICD_XML/SIDD_XML
        // Here we'll accept any of these under the assumption that it's not
        // such an old version of the spec that the XML layout itself has
        // changed (if it did, XMLControl will end up throwing anyway)
        if (desid == "XML_DATA_CONTENT" ||
            desid == "SICD_XML" ||
            desid == "SIDD_XML")
        {
            if (desid == "XML_DATA_CONTENT")
            {
                // We better have a user-defined subheader.  For now we
                // don't actually store off the values, but we do check its
                // tag and size.  Note that we need to check the subheader
                // length first rather than calling tre.getCurrentSize() in
                // case there is no subheader (in which case
                // subheader.getSubheaderFields() will throw with a
                // NITRO-specific message).
                const size_t
                        subheaderLength(subheader.getSubheaderFieldsLength());
                if (subheaderLength !=
                            Constants::DES_USER_DEFINED_SUBHEADER_LENGTH)
                {
                    std::ostringstream ostr;
                    ostr << "Expected a user-defined subheader size of "
                         << Constants::DES_USER_DEFINED_SUBHEADER_LENGTH
                         << " but got " << subheaderLength;
                    throw except::Exception(Ctxt(ostr.str()));
                }

                nitf::TRE tre = subheader.getSubheaderFields();
                if (tre.getTag() !=
                        Constants::DES_USER_DEFINED_SUBHEADER_TAG)
                {
                    throw except::Exception(Ctxt(
                        "Expected a user-defined subheader tag of " +
                        std::string(Constants::DES_USER_DEFINED_SUBHEADER_TAG) +
                        " but got " + tre.getTag()));
                }
            }

            nitf::SegmentReader deReader = mReader.newDEReader(i);
            SegmentInputStreamAdapter ioAdapter(deReader);
            std::auto_ptr<Data> data(parseData(*mXMLRegistry, 
                                               ioAdapter, 
                                               dataType,
                                               schemaPaths,
                                               *mLog));
            if (data.get() == NULL)
            {
                throw except::Exception(Ctxt("Unable to transform XML DES"));
            }

            // Note that DE override data never should clash, there
            // is one DES per data, so it's safe to do this
            addDEClassOptions(subheader, data->getClassification());

            mContainer->addData(data);
        }
    }

    // Get the total number of images in the NITF
    nitf::Uint32 numImages = mRecord.getNumImages();

    if (numImages == 0)
    {
        throw except::Exception(Ctxt(
                "SICD/SIDD files must have at least one image"));
    }

    // For SICD, we'll have exactly one DES
    // For SIDD, we'll have one SIDD DES per image product
    // We may also have some SICD DES's (this occurs if the SIDD was generated
    // from SICD input(s) - these serve as a reference), so we want to skip
    // over these when saving off NITFImageInfo's
    if (mContainer->getDataType() == DataType::COMPLEX)
    {
        if (mContainer->getNumData() != 1)
        {
            throw except::Exception(Ctxt(
                    "SICD file must have exactly 1 SICD DES but got " +
                    str::toString(mContainer->getNumData())));
        }

        mInfos.push_back(new NITFImageInfo(mContainer->getData(0)));
    }
    else
    {
        // We will validate that we got a SIDD DES per image product in the
        // for loop below
        for (size_t ii = 0; ii < mContainer->getNumData(); ++ii)
        {
            Data* const data = mContainer->getData(ii);
            if (data->getDataType() == DataType::DERIVED)
            {
                mInfos.push_back(new NITFImageInfo(data));
            }
        }
    }

    double corners[4][2];

    nitf::List images = mRecord.getImages();
    nitf::ListIterator imageIter = images.begin();

    // Now go through every image and figure out what clump its attached
    // to and use that for the measurements
    for (size_t i = 0; imageIter != images.end(); ++imageIter, ++i)
    {
        // Get a segment ref
        nitf::ImageSegment segment = (nitf::ImageSegment) * imageIter;

        // Get the subheader out
        nitf::ImageSubheader subheader = segment.getSubheader();

        // The number of rows in the segment (actual)
        size_t numRowsSeg = (nitf::Uint32) subheader.getNumRows();

        // This function should throw if the data does not exist
        std::pair<int, int> imageAndSegment = getIndices(subheader);
        if (static_cast<size_t>(imageAndSegment.first) >= mInfos.size())
        {
            throw except::Exception(Ctxt(
                    "Image " + str::toString(imageAndSegment.first) +
                    " is out of bounds"));
        }

        NITFImageInfo* const currentInfo = mInfos[imageAndSegment.first];
        int j = imageAndSegment.second;

        // We have to enforce a number of rules, namely that the #
        // columns match, and the pixel type, etc.
        validateSegment(subheader, currentInfo);

        // We are propagating the last segment's
        // security markings through.  This should be okay, since, if you
        // segment, you have the same security markings
        addImageClassOptions(subheader,
                             currentInfo->getData()->getClassification());

        NITFSegmentInfo si;
        si.numRows = numRowsSeg;

        std::vector < NITFSegmentInfo > imageSegments
                = currentInfo->getImageSegments();

        if (j != 0)
        {
            si.rowOffset = imageSegments[j - 1].numRows;
            si.firstRow = imageSegments[j - 1].firstRow + si.rowOffset;
        }
        else
        {
            si.rowOffset = 0;
            si.firstRow = 0;
        }
        subheader.getCornersAsLatLons(corners);
        for (size_t kk = 0; kk < LatLonCorners::NUM_CORNERS; ++kk)
        {
            si.corners.getCorner(kk).setLat(corners[kk][0]);
            si.corners.getCorner(kk).setLon(corners[kk][1]);
        }
        currentInfo->addSegment(si);
    }
}

void NITFReadControl::addImageClassOptions(nitf::ImageSubheader& subheader,
        six::Classification& c) const
{

    Parameter p;
    p = subheader.getImageSecurityClass().toString();
    c.fileOptions.setParameter("ISCLAS", p);
    addSecurityOptions(subheader.getSecurityGroup(), "IS", c.fileOptions);
}

void NITFReadControl::addDEClassOptions(nitf::DESubheader& subheader,
        six::Classification& c) const
{
    Parameter p;
    p = subheader.getSecurityClass().toString();
    c.fileOptions.setParameter("DECLAS", p);
    addSecurityOptions(subheader.getSecurityGroup(), "DES", c.fileOptions);
}

void NITFReadControl::addSecurityOptions(nitf::FileSecurity security,
        const std::string& prefix, six::Options& options) const
{
    Parameter p;
    std::string k;

    p = security.getClassificationSystem().toString();
    k = NITFImageInfo::generateFieldKey(NITFImageInfo::CLSY, prefix);
    options.setParameter(k, p);
    mLog->debug(Ctxt(FmtX("Added NITF security option: [%s]->[%s]", k.c_str(),
                          (const char*) p)));

    p = security.getCodewords().toString();
    k = NITFImageInfo::generateFieldKey(NITFImageInfo::CODE, prefix);
    options.setParameter(k, p);
    mLog->debug(Ctxt(FmtX("Added NITF security option: [%s]->[%s]", k.c_str(),
                          (const char*) p)));

    p = security.getControlAndHandling().toString();
    k = NITFImageInfo::generateFieldKey(NITFImageInfo::CTLH, prefix);
    options.setParameter(k, p);
    mLog->debug(Ctxt(FmtX("Added NITF security option: [%s]->[%s]", k.c_str(),
                          (const char*) p)));

    p = security.getReleasingInstructions().toString();
    k = NITFImageInfo::generateFieldKey(NITFImageInfo::REL, prefix);
    options.setParameter(k, p);
    mLog->debug(Ctxt(FmtX("Added NITF security option: [%s]->[%s]", k.c_str(),
                          (const char*) p)));

    p = security.getDeclassificationType().toString();
    k = NITFImageInfo::generateFieldKey(NITFImageInfo::DCTP, prefix);
    options.setParameter(k, p);
    mLog->debug(Ctxt(FmtX("Added NITF security option: [%s]->[%s]", k.c_str(),
                          (const char*) p)));

    p = security.getDeclassificationDate().toString();
    k = NITFImageInfo::generateFieldKey(NITFImageInfo::DCDT, prefix);
    options.setParameter(k, p);
    mLog->debug(Ctxt(FmtX("Added NITF security option: [%s]->[%s]", k.c_str(),
                          (const char*) p)));

    p = security.getDeclassificationExemption().toString();
    k = NITFImageInfo::generateFieldKey(NITFImageInfo::DCXM, prefix);
    options.setParameter(k, p);
    mLog->debug(Ctxt(FmtX("Added NITF security option: [%s]->[%s]", k.c_str(),
                          (const char*) p)));

    p = security.getDowngrade().toString();
    k = NITFImageInfo::generateFieldKey(NITFImageInfo::DG, prefix);
    options.setParameter(k, p);
    mLog->debug(Ctxt(FmtX("Added NITF security option: [%s]->[%s]", k.c_str(),
                          (const char*) p)));

    p = security.getDowngradeDateTime().toString();
    k = NITFImageInfo::generateFieldKey(NITFImageInfo::DGDT, prefix);
    options.setParameter(k, p);
    mLog->debug(Ctxt(FmtX("Added NITF security option: [%s]->[%s]", k.c_str(),
                          (const char*) p)));

    p = security.getClassificationText().toString();
    k = NITFImageInfo::generateFieldKey(NITFImageInfo::CLTX, prefix);
    options.setParameter(k, p);
    mLog->debug(Ctxt(FmtX("Added NITF security option: [%s]->[%s]", k.c_str(),
                          (const char*) p)));

    p = security.getClassificationAuthorityType().toString();
    k = NITFImageInfo::generateFieldKey(NITFImageInfo::CATP, prefix);
    options.setParameter(k, p);
    mLog->debug(Ctxt(FmtX("Added NITF security option: [%s]->[%s]", k.c_str(),
                          (const char*) p)));

    p = security.getClassificationAuthority().toString();
    k = NITFImageInfo::generateFieldKey(NITFImageInfo::CAUT, prefix);
    options.setParameter(k, p);
    mLog->debug(Ctxt(FmtX("Added NITF security option: [%s]->[%s]", k.c_str(),
                          (const char*) p)));

    p = security.getClassificationReason().toString();
    k = NITFImageInfo::generateFieldKey(NITFImageInfo::CRSN, prefix);
    options.setParameter(k, p);
    mLog->debug(Ctxt(FmtX("Added NITF security option: [%s]->[%s]", k.c_str(),
                          (const char*) p)));

    p = security.getSecuritySourceDate().toString();
    k = NITFImageInfo::generateFieldKey(NITFImageInfo::SRDT, prefix);
    options.setParameter(k, p);
    mLog->debug(Ctxt(FmtX("Added NITF security option: [%s]->[%s]", k.c_str(),
                          (const char*) p)));

    p = security.getSecurityControlNumber().toString();
    k = NITFImageInfo::generateFieldKey(NITFImageInfo::CTLN, prefix);
    options.setParameter(k, p);
    mLog->debug(Ctxt(FmtX("Added NITF security option: [%s]->[%s]", k.c_str(),
                          (const char*) p)));
}

std::pair<int, int> NITFReadControl::getIndices(nitf::ImageSubheader& subheader)
{

    std::string imageID = subheader.getImageId().toString();
    str::trim(imageID);
    // There is definitely something in here
    std::pair<int, int> imageAndSegment;
    imageAndSegment.first = 0;
    imageAndSegment.second = 0;

    int iid = str::toType<int>(imageID.substr(4, 3));

    /*
     *  Always first = 0, second = N - 1 (where N is numSegments)
     *
     */
    if (mContainer->getDataType() == DataType::COMPLEX)
    {
        // We need to find the SICD data here, and there is
        // only one
        if (iid != 0)
        {
            imageAndSegment.second = iid - 1;
        }
    }
    /*
     *  First is always iid - 1
     *  Second is always str::toType<int>(substr(7)) - 1
     */
    else
    {
        // If its SIDD, we need to check the first three
        // digits within the IID
        imageAndSegment.first = iid - 1;
        imageAndSegment.second = str::toType<int>(imageID.substr(7)) - 1;
    }
    return imageAndSegment;
}

UByte* NITFReadControl::interleaved(Region& region, size_t imageNumber)
{

    NITFImageInfo* thisImage = mInfos[imageNumber];

    size_t numRowsTotal = thisImage->getData()->getNumRows();
    size_t numColsTotal = thisImage->getData()->getNumCols();

    if (region.getNumRows() == -1)
    {
        region.setNumRows(numRowsTotal);
    }
    if (region.getNumCols() == -1)
    {
        region.setNumCols(numColsTotal);
    }

    size_t numRowsReq = region.getNumRows();
    size_t numColsReq = region.getNumCols();

    size_t startRow = region.getStartRow();
    size_t startCol = region.getStartCol();

    size_t extentRows = startRow + numRowsReq;
    size_t extentCols = startCol + numColsReq;

    if (extentRows > numRowsTotal || startRow > numRowsTotal)
        throw except::Exception(Ctxt(FmtX("Too many rows requested [%d]",
                                          numRowsReq)));

    if (extentCols > numColsTotal || startCol > numColsTotal)
        throw except::Exception(Ctxt(FmtX("Too many cols requested [%d]",
                                          numColsReq)));

    // Allocate one band
    nitf::Uint32 bandList(0);

    nitf::Uint8* buffer = region.getBuffer();

    size_t subWindowSize = numRowsReq * numColsReq
            * thisImage->getData()->getNumBytesPerPixel();

    if (buffer == NULL)
    {
        buffer = new nitf::Uint8[subWindowSize];
        region.setBuffer(buffer);
    }

    // Do segmenting here
    nitf::SubWindow sw;
    sw.setStartCol(startCol);
    sw.setNumCols(numColsReq);
    sw.setNumBands(1);
    sw.setBandList(&bandList);

    std::vector < NITFSegmentInfo > imageSegments
            = thisImage->getImageSegments();
    size_t numIS = imageSegments.size();
    size_t startOff = 0;

    size_t i;
    for (i = 0; i < numIS; i++)
    {
        size_t firstRowSeg = imageSegments[i].firstRow;

        if (firstRowSeg <= startRow)
        {
            // It could be in this segment
            startOff = firstRowSeg;
        }
        else
        {
            break;
        }

    }
    --i; // Need to get rid of the last one
    size_t totalRead = 0;
    size_t numRowsLeft = numRowsReq;
    sw.setStartRow(startRow - startOff);
#if DEBUG_OFFSETS
    std::cout << "startRow: " << startRow
    << " startOff: " << startOff
    << " sw.startRow: " << sw.getStartRow()
    << " i: " << i << std::endl;
#endif
    int nbpp = thisImage->getData()->getNumBytesPerPixel();
    int startIndex = thisImage->getStartIndex();
    createCompressionOptions(mCompressionOptions);
    for (; i < numIS && totalRead < subWindowSize; i++)
    {
        size_t numRowsReqSeg =
                std::min<size_t>(numRowsLeft, imageSegments[i].numRows
                        - sw.getStartRow());

        sw.setNumRows(numRowsReqSeg);
        nitf::ImageReader imageReader = mReader.newImageReader(startIndex + i,
                                                               mCompressionOptions);

        nitf::Uint8* bufferPtr = buffer + totalRead;

        int padded;
        imageReader.read(sw, &bufferPtr, &padded);
        totalRead += numColsReq * nbpp * numRowsReqSeg;
        sw.setStartRow(0);
        numRowsLeft -= numRowsReqSeg;
    }

    return buffer;
}

void NITFReadControl::reset()
{
    for (size_t ii = 0; ii < mInfos.size(); ++ii)
    {
        delete mInfos[ii];
    }
    mInfos.clear();

    delete mContainer;
    mContainer = NULL;
}


six::ReadControl* NITFReadControlCreator::newReadControl() const
{
    return new NITFReadControl();
}

bool NITFReadControlCreator::supports(const std::string& filename) const
{
    try
    {
        NITFReadControl control;
        return control.getDataType(filename) != DataType::NOT_SET;
    }
    catch(except::Exception&)
    {
        return false;
    }
}

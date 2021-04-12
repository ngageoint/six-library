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

#include <assert.h>

#include <sstream>
#include <stdexcept>
#include <string>

#include <six/NITFReadControl.h>
#include <six/XMLControlFactory.h>
#include <six/Utilities.h>

#ifndef SIX_ENABLE_DED
    // set to 1 for DEM support
    #define SIX_ENABLE_DED 0
#endif
namespace six
{
    constexpr auto enable_ded = SIX_ENABLE_DED ? true : false;
}

namespace
{
types::RowCol<size_t> parseILOC(const std::string& str)
{
    // First 5 digits are the row
    // Next 5 are the column
    types::RowCol<size_t> iLoc;
    iLoc.row = str::toType<size_t>(str.substr(0, 5));
    iLoc.col = str::toType<size_t>(str.substr(5, 5));
    return iLoc;
}

void assignLUT(const nitf::ImageSubheader& subheader, six::Legend& legend)
{
    nitf::LookupTable lut =
            subheader.getBandInfo(0).getLookupTable();

    legend.mLUT.reset(new six::LUT(lut.getEntries(), lut.getTables()));

    const unsigned char* const table = lut.getTable();
    assert(table != nullptr);

    for (size_t ii = 0, kk = 0; ii < lut.getEntries(); ++ii)
    {
        for (size_t jj = 0; jj < lut.getTables(); ++jj, ++kk)
        {
            // Need to transpose the lookup table entries
            legend.mLUT->getTable()[kk] =
                    table[jj * lut.getEntries() + ii];
        }
    }
}

six::PixelType getPixelType(const nitf::ImageSubheader& subheader)
{
    const auto iRep = subheader.imageRepresentation();
    if (iRep == "MONO")
    {
        return six::PixelType::MONO8I;
    }
    else if (iRep == "RGB/LUT")
    {
        return six::PixelType::RGB8LU;
    }
    else
    {
        throw except::Exception(Ctxt(
                "Unexpected image representation '" + iRep + "'"));
    }
}
}

namespace six
{
NITFReadControl::NITFReadControl()
{
    // Make sure that if we use XML_DATA_CONTENT that we've loaded it into the
    // singleton PluginRegistry
    loadXmlDataContentHandler();
}

DataType NITFReadControl::getDataType(const nitf::Record& record)
{
    nitf::List des = record.getDataExtensions();
    if (des.isEmpty())
    {
        return DataType::NOT_SET;
    }

    // SICD spec guarantees that the first DES will be the SICD/SIDD DES
    nitf::DESegment segment = (nitf::DESegment) des.getFirst().getData();
    return getDataType(segment);
}

DataType NITFReadControl::getDataType(const std::string& desid,
        uint64_t subheaderLength, const std::string& desshsiField,
        const std::string& treTag)
{
    // SICD/SIDD 1.0 specify DESID as XML_DATA_CONTENT
    // Older versions of the spec specified it as SICD_XML/SIDD_XML
    // Here we'll accept any of these under the assumption that it's not
    // such an old version of the spec that the XML layout itself has
    // changed (if it did, XMLControl will end up throwing anyway)
    if (desid == "SICD_XML")
    {
        return DataType::COMPLEX;
    }
    else if (desid == "SIDD_XML")
    {
        return DataType::DERIVED;
    }
    else if (desid == Constants::DES_USER_DEFINED_SUBHEADER_TAG)
    {
        // Check whether DES is SICD/SIDD, or something of a different sort.
        if (subheaderLength !=
            Constants::DES_USER_DEFINED_SUBHEADER_LENGTH)
        {
            return DataType::NOT_SET;
        }
        if (treTag != Constants::DES_USER_DEFINED_SUBHEADER_TAG)
        {
            return DataType::NOT_SET;
        }
        if (desshsiField == Constants::SICD_DESSHSI)
        {
            return DataType::COMPLEX;
        }
        else if (desshsiField == Constants::SIDD_DESSHSI)
        {
            return DataType::DERIVED;
        }
    }
    return DataType::NOT_SET;
}

DataType NITFReadControl::getDataType(const nitf::DESegment& segment)
{
    // NOTE: Versions of SICD <= 1.1 and SIDD <= 1.0 prefixed FTITLE with
    //       SICD or SIDD, so for old files we could key off of that. Since
    //       that's not guaranteed for newer versions though, we now use the
    //       DESID for really old versions and the DESSHSI from
    //       XML_DATA_CONTENT for newer versions.
    nitf::DESubheader subheader = segment.getSubheader();
    const auto desid = subheader.typeID();

    // Note that we need to check the subheader
    // length first rather than calling tre.getCurrentSize() in
    // case there is no subheader (in which case
    // subheader.getSubheaderFields() will throw with a
    // NITRO-specific message).
    const uint64_t subheaderLength(subheader.getSubheaderFieldsLength());
    std::string treTag;
    std::string desshsiField;
    if (subheaderLength != 0)
    {
        nitf::TRE tre = subheader.getSubheaderFields();
        treTag = tre.getTag();

        if (tre.exists("DESSHSI"))
        {
            desshsiField = tre.getField("DESSHSI").toTrimString();
        }
    }

    return getDataType(desid, subheaderLength, desshsiField, treTag);
}

DataType NITFReadControl::getDataType(const std::string& fromFile) const
{
    // Could cache this
    if (mReader.getNITFVersion(fromFile) != NITF_VER_UNKNOWN)
    {
        nitf::IOHandle inFile(fromFile);
        nitf::Record record = mReader.read(inFile);

        return getDataType(record);
    }
    else
    {
        return DataType::NOT_SET;
    }
}

void NITFReadControl::validateSegment(nitf::ImageSubheader subheader,
                                      const NITFImageInfo* info)
{
    assert(info != nullptr);
    validateSegment(subheader, *info);
}
void NITFReadControl::validateSegment(nitf::ImageSubheader subheader,
                                      const NITFImageInfo& info)
{
    const size_t numBandsSeg = subheader.numImageBands();

    const std::string pjust = subheader.pixelJustification();
    // TODO: More validation in here!
    if (pjust != "R")
    {
        throw except::Exception(Ctxt("Expected right pixel justification"));
    }

    // The number of bytes per pixel, which we count to be 3 in the
    // case of 24-bit true color and 8 in the case of complex float
    // and 1 in the case of most SIDD data
    const size_t numBitsPerPixel = subheader.numBitsPerPixel();
    const size_t numBytesPerPixel = (numBitsPerPixel + 7) / 8;
    const size_t numBytesSeg = numBytesPerPixel * numBandsSeg;

    const size_t nbpp = info.getData()->getNumBytesPerPixel();
    if (numBytesSeg != nbpp)
    {
        std::ostringstream ostr;
        ostr << "Expected [" << nbpp << "] bytes per pixel, found ["
             << numBytesSeg << "]";
        throw except::Exception(Ctxt(ostr.str()));
    }

    const size_t numCols = info.getData()->getNumCols();
    const size_t numColsSubheader = subheader.numCols();

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
    auto handle(std::make_shared<nitf::IOHandle>(fromFile));
    load(handle, schemaPaths);
}

void NITFReadControl::load(io::SeekableInputStream& stream,
                           const std::vector<std::string>& schemaPaths)
{
    auto handle(std::make_shared<nitf::IOStreamReader>(stream));
    load(handle, schemaPaths);
}

void NITFReadControl::load(mem::SharedPtr<nitf::IOInterface> ioInterface)
{
    load(ioInterface, std::vector<std::string>());
}

void NITFReadControl::load(mem::SharedPtr<nitf::IOInterface> ioInterface,
                           const std::vector<std::string>& schemaPaths)
{
    reset();
    mInterface = ioInterface;

    mRecord = mReader.readIO(*ioInterface);
    const DataType dataType = getDataType(mRecord);
    mContainer.reset(new Container(dataType));

    // First, read in the DE segments, and organize them
    nitf::List des = mRecord.getDataExtensions();
    nitf::ListIterator desIter = des.begin();

    for (int i = 0, productNum = 0; desIter != des.end(); ++desIter, ++i)
    {
        nitf::DESegment segment = (nitf::DESegment) *desIter;
        nitf::DESubheader subheader = segment.getSubheader();
        nitf::SegmentReader deReader = mReader.newDEReader(i);

        if (getDataType(segment) == DataType::NOT_SET)
        {
            mContainer->addDESSource(nitf::SegmentReaderSource(deReader));
        }
        else
        {
            SegmentInputStreamAdapter ioAdapter(deReader);
            std::unique_ptr<Data> data(parseData(*mXMLRegistry,
                                               ioAdapter,
                                               dataType,
                                               schemaPaths,
                                               *mLog));
            if (data.get() == nullptr)
            {
                throw except::Exception(Ctxt("Unable to transform XML DES"));
            }

            // Note that DE override data never should clash, there
            // is one DES per data, so it's safe to do this
            addDEClassOptions(subheader, data->getClassification());

            if (data->getDataType() == six::DataType::DERIVED)
            {
                mContainer->addData(std::move(data), findLegend(productNum));
            }
            else if (data->getDataType() == six::DataType::COMPLEX)
            {
                mContainer->addData(std::move(data));
            }
            else
            {
                throw except::Exception(Ctxt("Unknown 'getDataType()' value."));
            }

            ++productNum;
        }
    }

    // Get the total number of images in the NITF
    const uint32_t numImages = mRecord.getNumImages();
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
                    std::to_string(mContainer->getNumData())));
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

    nitf::List images = mRecord.getImages();
    nitf::ListIterator imageIter = images.begin();

    // Now go through every image and figure out what clump it's attached
    // to and use that for the measurements
    for (size_t nitfSegmentIdx = 0;
         imageIter != images.end();
         ++imageIter, ++nitfSegmentIdx)
    {
        // Get a segment ref
        nitf::ImageSegment segment = (nitf::ImageSegment) * imageIter;

        // Get the subheader out
        nitf::ImageSubheader subheader = segment.getSubheader();

        // The number of rows in the segment (actual)
        const size_t numRowsSeg = subheader.numRows();

        // This function should throw if the data does not exist
        ImageAndSegment imageAndSegment;
        getIndices(subheader, imageAndSegment);
        if (imageAndSegment.image >= mInfos.size())
        {
            throw except::Exception(Ctxt(
                    "Image " + std::to_string(imageAndSegment.image) +
                    " is out of bounds"));
        }

        const auto& currentInfo = mInfos[imageAndSegment.image];
        const size_t productSegmentIdx = imageAndSegment.segment;

        // We have to enforce a number of rules, namely that the #
        // columns match, and the pixel type, etc.
        // But, we don't do this for legends since their size has nothing to
        // do with the size of the pixel data
        const bool segIsLegend = isLegend(subheader);
        const bool segIsDed = isDed(subheader);

        const auto do_validateSegment = six::enable_ded ? !segIsLegend && !segIsDed : !segIsLegend;
        if (do_validateSegment)
        {
            validateSegment(subheader, *currentInfo);
        }

        // We are propagating the last segment's
        // security markings through.  This should be okay, since, if you
        // segment, you have the same security markings
        addImageClassOptions(subheader,
                             currentInfo->getData()->getClassification());

        NITFSegmentInfo si;
        si.numRows = numRowsSeg;

        std::vector < NITFSegmentInfo > imageSegments
                = currentInfo->getImageSegments();

        if (productSegmentIdx == 0)
        {
            si.rowOffset = 0;
            si.firstRow = 0;
            currentInfo->setStartIndex(nitfSegmentIdx);
        }
        else
        {
            si.rowOffset = imageSegments[productSegmentIdx - 1].getNumRows();
            si.firstRow = imageSegments[productSegmentIdx - 1].getFirstRow() +
                    si.getRowOffset();
        }

        // Legends don't set lat/lons
        const auto do_setLatLon = six::enable_ded ? !segIsLegend && !segIsDed : !segIsLegend;
        if (do_setLatLon)
        {
            double corners[4][2]{};
            subheader.getCornersAsLatLons(corners);
            for (size_t kk = 0; kk < LatLonCorners::NUM_CORNERS; ++kk)
            {
                si.corners.getCorner(kk).setLat(corners[kk][0]);
                si.corners.getCorner(kk).setLon(corners[kk][1]);
            }
        }

        // SIDD 2.0 needs to read LUT directly from NITF
        if (currentInfo->getData()->getDataType() == DataType::DERIVED &&
            currentInfo->getData()->getVersion() == "2.0.0")
        {
            nitf::LookupTable nitfLut =
                    subheader.getBandInfo(0).getLookupTable();

            currentInfo->getData()->getDisplayLUT().reset(new LUT(nitfLut));
        }
        currentInfo->addSegment(si);
    }
}

void NITFReadControl::addImageClassOptions(const nitf::ImageSubheader& subheader,
        six::Classification& c) const
{

    Parameter p;
    p = subheader.imageSecurityClass();
    c.fileOptions.setParameter("ISCLAS", p);
    addSecurityOptions(subheader.getSecurityGroup(), "IS", c.fileOptions);
}

void NITFReadControl::addDEClassOptions(const nitf::DESubheader& subheader,
        six::Classification& c) const
{
    Parameter p;
    p = subheader.securityClass();
    c.fileOptions.setParameter("DECLAS", p);
    addSecurityOptions(subheader.getSecurityGroup(), "DES", c.fileOptions);
}

class AddSecurityOption final
{
    const std::string& prefix_;
    six::Options& options_;
    logging::Logger& log_;
public:
    AddSecurityOption(const std::string& prefix, six::Options& options, logging::Logger& log)
        : prefix_(prefix), options_(options), log_(log) {}
    void operator()(const nitf::Field& parameter, const std::string& field)
    {
        Parameter p = parameter.toString();
        const auto k = NITFImageInfo::generateFieldKey(field, prefix_);
        options_.setParameter(k, p);
        log_.debug(Ctxt(FmtX("Added NITF security option: [%s]->[%s]", k.c_str(),
            static_cast<const char*>(p))));
    }
};

void NITFReadControl::addSecurityOptions(nitf::FileSecurity security,
        const std::string& prefix, six::Options& options) const
{
    AddSecurityOption addSecurityOption(prefix, options, *mLog);

    addSecurityOption(security.getClassificationSystem(), NITFImageInfo::CLSY);
    addSecurityOption(security.getCodewords(), NITFImageInfo::CODE);
    addSecurityOption(security.getControlAndHandling(), NITFImageInfo::CTLH);
    addSecurityOption(security.getReleasingInstructions(), NITFImageInfo::REL);
    addSecurityOption(security.getDeclassificationType(), NITFImageInfo::DCTP);
    addSecurityOption(security.getDeclassificationDate(), NITFImageInfo::DCDT);
    addSecurityOption(security.getDeclassificationExemption(), NITFImageInfo::DCXM);
    addSecurityOption(security.getDowngrade(), NITFImageInfo::DG);
    addSecurityOption(security.getDowngradeDateTime(), NITFImageInfo::DGDT);
    addSecurityOption(security.getClassificationText(), NITFImageInfo::CLTX);
    addSecurityOption(security.getClassificationAuthorityType(), NITFImageInfo::CATP);
    addSecurityOption(security.getClassificationAuthority(), NITFImageInfo::CAUT);
    addSecurityOption(security.getClassificationReason(), NITFImageInfo::CRSN);
    addSecurityOption(security.getSecuritySourceDate(), NITFImageInfo::SRDT);
    addSecurityOption(security.getSecurityControlNumber(), NITFImageInfo::CTLN);
}

std::pair<size_t, size_t>
NITFReadControl::getIndices(const nitf::ImageSubheader& subheader) const
{
    ImageAndSegment result;
    getIndices(subheader, result);
    return std::make_pair(result.image, result.segment);
}
void NITFReadControl::getIndices(const nitf::ImageSubheader& subheader, ImageAndSegment& result) const
{
    const auto imageID = subheader.imageId();

    // There is definitely something in here
    auto& image = result.image;
    auto& segment = result.segment;

    const auto digit_pos = imageID.find_first_of("0123456789");
    if (digit_pos == std::string::npos)
    {
        throw except::Exception(Ctxt("Can't parse 'iid' from '" + imageID + "'"));
    }
    const auto str_image = imageID.substr(digit_pos, 3);
    const size_t iid = str::toType<size_t>(str_image);

    /*
     *  Always first = 0, second = N - 1 (where N is numSegments)
     *
     */
    const auto dataType = mContainer->getDataType();
    if (dataType == DataType::COMPLEX)
    {
        // We need to find the SICD data here, and there is
        // only one
        if (iid != 0)
        {
            segment = iid - 1;
        }
    }
    /*
     *  First is always iid - 1
     *  Second is always str::toType<int>(substr(7)) - 1
     */
    else if (dataType == DataType::DERIVED)
    {
        // If it's SIDD, we need to check the first three digits within the IID
        image = iid - 1;
        std::string str_segment;
        const auto segment_pos = digit_pos + 3;
        if (segment_pos < imageID.length())
        {
            str_segment = imageID.substr(segment_pos);
        }
        else
        {
            // 'imageID' might also be "DED001"
            if (six::enable_ded)
            {
                str_segment = imageID.substr(digit_pos);
            }
            else
            {
                throw except::Exception(Ctxt("Can't extract segment # from: " + imageID));
            }
        }
        segment = str::toType<size_t>(str_segment) - 1;
    }
    else
    {
        throw except::Exception(Ctxt("Unknown 'DataType' value: " + dataType.toString()));
    }
}

UByte* NITFReadControl::interleaved(Region& region, size_t imageNumber)
{
    const NITFImageInfo& thisImage = *(mInfos[imageNumber]);

    const size_t numRowsTotal = thisImage.getData()->getNumRows();
    const size_t numColsTotal = thisImage.getData()->getNumCols();

    if (region.getNumRows() == -1)
    {
        region.setNumRows(numRowsTotal);
    }
    if (region.getNumCols() == -1)
    {
        region.setNumCols(numColsTotal);
    }

    const size_t numRowsReq = region.getNumRows();
    const size_t numColsReq = region.getNumCols();

    const size_t startRow = region.getStartRow();
    const size_t startCol = region.getStartCol();

    const size_t extentRows = startRow + numRowsReq;
    const size_t extentCols = startCol + numColsReq;

    if (extentRows > numRowsTotal || startRow > numRowsTotal)
        throw except::Exception(Ctxt(FmtX("Too many rows requested [%d]",
                                          numRowsReq)));

    if (extentCols > numColsTotal || startCol > numColsTotal)
        throw except::Exception(Ctxt(FmtX("Too many cols requested [%d]",
                                          numColsReq)));

    // Allocate one band
    uint32_t bandList(0);

    auto buffer = region.getBuffer();

    const size_t subWindowSize = numRowsReq * numColsReq
            * thisImage.getData()->getNumBytesPerPixel();

    if (buffer == nullptr)
    {
        buffer = region.setBuffer(subWindowSize).release();
    }

    // Do segmenting here
    nitf::SubWindow sw;
    sw.setStartCol(static_cast<uint32_t>(startCol));
    sw.setNumCols(static_cast<uint32_t>(numColsReq));
    sw.setNumBands(1);
    sw.setBandList(&bandList);

    std::vector < NITFSegmentInfo > imageSegments
            = thisImage.getImageSegments();
    const size_t numIS = imageSegments.size();
    size_t startOff = 0;

    size_t i;
    for (i = 0; i < numIS; i++)
    {
        const auto firstRowSeg = imageSegments[i].getFirstRow();

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
    sw.setStartRow(static_cast<uint32_t>(startRow - startOff));
#if DEBUG_OFFSETS
    std::cout << "startRow: " << startRow
    << " startOff: " << startOff
    << " sw.startRow: " << sw.getStartRow()
    << " i: " << i << std::endl;
#endif

    const size_t nbpp = thisImage.getData()->getNumBytesPerPixel();
    const size_t startIndex = thisImage.getStartIndex();
    createCompressionOptions(mCompressionOptions);
    for (; i < numIS && totalRead < subWindowSize; i++)
    {
        const size_t numRowsReqSeg =
                std::min<size_t>(numRowsLeft, imageSegments[i].getNumRows()
                        - sw.getStartRow());

        sw.setNumRows(static_cast<uint32_t>(numRowsReqSeg));
        nitf::ImageReader imageReader = mReader.newImageReader(
                static_cast<int>(startIndex + i),
                mCompressionOptions);

        auto bufferPtr = buffer + totalRead;

        int padded;
        imageReader.read(sw, &bufferPtr, &padded);
        totalRead += numColsReq * nbpp * numRowsReqSeg;
        sw.setStartRow(0);
        numRowsLeft -= numRowsReqSeg;
    }

    return buffer;
}

std::unique_ptr<Legend> NITFReadControl::findLegend(size_t productNum)
{
    std::unique_ptr<Legend> legend;

    nitf::List images = mRecord.getImages();
    nitf::ListIterator imageIter = images.begin();

    for (size_t imageSeg = 0;
         imageIter != images.end();
         ++imageIter, ++imageSeg)
    {
        nitf::ImageSegment segment = (nitf::ImageSegment) *imageIter;
        nitf::ImageSubheader subheader = segment.getSubheader();

        if (productNum == getIndices(subheader).first && isLegend(subheader))
        {
            // It's an image segment associated with the product we care
            // about and it's a legend (for simplicity right now we're going
            // to just assume there are not multiple legends associated with
            // one product, though the spec does allow for this)

            legend.reset(new Legend());
            legend->mType = getPixelType(subheader);

            legend->mLocation = parseILOC(subheader.imageLocation());
            if (legend->mType == PixelType::RGB8LU)
            {
                assignLUT(subheader, *legend);
            }

            readLegendPixelData(subheader, imageSeg, *legend);

            break;
        }
    }

    return legend;
}

void NITFReadControl::readLegendPixelData(const nitf::ImageSubheader& subheader,
                                          size_t imageSeg,
                                          Legend& legend)
{
    const types::RowCol<uint32_t> dims(
            static_cast<uint32_t>(subheader.numRows()),
            static_cast<uint32_t>(subheader.numCols()));

    legend.setDims(dims);

    uint32_t bandList(0);
    nitf::SubWindow sw;
    sw.setNumRows(dims.row);
    sw.setNumCols(dims.col);
    sw.setNumBands(1);
    sw.setBandList(&bandList);

    if (!legend.mImage.empty())
    {
        auto bufferPtr = legend.mImage.data();
        nitf::ImageReader imageReader = mReader.newImageReader(
                static_cast<int>(imageSeg));
        int padded = 0;
        imageReader.read(sw, &bufferPtr, &padded);
    }
}

void NITFReadControl::reset()
{
    for (size_t ii = 0; ii < mInfos.size(); ++ii)
    {
        delete mInfos[ii];
    }
    mInfos.clear();
    mInterface.reset();
}


void NITFReadControlCreator::newReadControl(std::unique_ptr<six::ReadControl>& result) const
{
    result.reset(new NITFReadControl());
}

bool NITFReadControlCreator::supports(const std::string& filename) const
{
    try
    {
        NITFReadControl control;
        return control.getDataType(filename) != DataType::NOT_SET;
    }
    catch(const except::Exception&)
    {
        return false;
    }
}
}

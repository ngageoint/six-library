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
#include <std/memory>

#include <gsl/gsl.h>

#include <six/NITFReadControl.h>
#include <six/XMLControlFactory.h>
#include <six/Utilities.h>

#undef min
#undef max

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
    if (iRep == nitf::ImageRepresentation::MONO)
    {
        return six::PixelType::MONO8I;
    }
    if (iRep == nitf::ImageRepresentation::RGB_LUT)
    {
        return six::PixelType::RGB8LU;
    }
    throw except::Exception(Ctxt("Unexpected image representation '" + to_string(iRep) + "'"));
}
}

namespace six
{
NITFReadControl::NITFReadControl(FILE* log)
{
    // Make sure that if we use XML_DATA_CONTENT that we've loaded it into the
    // singleton PluginRegistry
    loadXmlDataContentHandler(log);
}
// This can generate output from implicitConstruct() complaining about NITF_PLUGIN_PATH
// not being set; often the warning is benign and is just confusing.  Provide a way to turn
// it off (FILE* log = NULL) without upsetting existing code.
NITFReadControl::NITFReadControl()
    : NITFReadControl(stderr) // existing/legacy behavior
{
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
    if (desid == "SIDD_XML")
    {
        return DataType::DERIVED;
    }
    if (desid == Constants::DES_USER_DEFINED_SUBHEADER_TAG)
    {
        // Check whether DES is SICD/SIDD, or something of a different sort.
        if (subheaderLength != Constants::DES_USER_DEFINED_SUBHEADER_LENGTH)
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
        if (desshsiField == Constants::SIDD_DESSHSI)
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
        const auto tre = subheader.getSubheaderFields();
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
        const auto record = mReader.read(inFile);
        return getDataType(record);
    }
    return DataType::NOT_SET;
}

void NITFReadControl::validateSegment(const nitf::ImageSubheader& subheader,
                                      const NITFImageInfo* info) const
{
    assert(info != nullptr);
    validateSegment(subheader, *info);
}
void NITFReadControl::validateSegment(const nitf::ImageSubheader& subheader,
                                      const NITFImageInfo& info) const
{
    const auto pjust = subheader.pixelJustification();
    // TODO: More validation in here!
    if (pjust != "R")
    {
        throw except::Exception(Ctxt("Expected right pixel justification"));
    }

    // The number of bytes per pixel, which we count to be 3 in the
    // case of 24-bit true color and 8 in the case of complex float
    // and 1 in the case of most SIDD data
    const auto numBandsSeg = subheader.numImageBands();

    const auto numBitsPerPixel = subheader.numBitsPerPixel();
    const auto numBytesPerPixel = (numBitsPerPixel + 7) / 8;
    const auto numBytesSeg = numBytesPerPixel * numBandsSeg;
    //const auto numChannels = info.getData()->getNumChannels();
    const auto foundBytesPerPixel = numBytesSeg;

    const auto expectedBytesPerPixel = info.getData()->getNumBytesPerPixel();
    if (foundBytesPerPixel != expectedBytesPerPixel)
    {
        std::ostringstream ostr;
        ostr << "Expected [" << expectedBytesPerPixel << "] bytes per pixel, found [" << foundBytesPerPixel << "]";
        throw except::Exception(Ctxt(ostr.str()));
    }

    const auto numCols = info.getData()->getNumCols();
    const auto numColsSubheader = subheader.numCols();
    if (numColsSubheader != numCols)
    {
        std::ostringstream ostr;
        ostr << "Invalid column width: was expecting [" << numCols << "], got [" << numColsSubheader << "]";
        throw except::Exception(Ctxt(ostr.str()));
    }
}

void NITFReadControl::load(const std::string& fromFile, const std::vector<std::string>* pSchemaPaths)
{
    auto handle(std::make_shared<nitf::IOHandle>(fromFile));
    load(handle, pSchemaPaths);
}
void NITFReadControl::load(const std::filesystem::path& fromFile, const std::vector<std::filesystem::path>* pSchemaPaths)
{
    std::shared_ptr<nitf::IOInterface> handle(std::make_shared<nitf::IOHandle>(fromFile.string()));
    load(handle, pSchemaPaths);
}

void NITFReadControl::load(std::shared_ptr<nitf::IOInterface> ioInterface)
{
    load(ioInterface, std::vector<std::string>());
}

static std::vector<six::NITFImageInfo*> getImageInfos(six::Container& container)
{
    // For SICD, we'll have exactly one DES
    // For SIDD, we'll have one SIDD DES per image product
    // We may also have some SICD DES's (this occurs if the SIDD was generated
    // from SICD input(s) - these serve as a reference), so we want to skip
    // over these when saving off NITFImageInfo's
    if (container.getDataType() == DataType::COMPLEX)
    {
        if (container.size() != 1)
        {
            throw except::Exception(Ctxt("SICD file must have exactly 1 SICD DES but got " + std::to_string(container.size())));
        }

        return std::vector<six::NITFImageInfo*> { new NITFImageInfo(container.getData(0)) };
    }

    // We will validate that we got a SIDD DES per image product in the
    // for loop below
    assert(container.getDataType() == DataType::DERIVED);
    std::vector<six::NITFImageInfo*> retval;
    for (size_t ii = 0; ii < container.size(); ++ii)
    {
        Data* const data = container.getData(ii);
        if (data->getDataType() == DataType::DERIVED)
        {
            retval.push_back(new NITFImageInfo(data));
        }
    }
    return retval;
}

void NITFReadControl::load(std::shared_ptr<nitf::IOInterface> ioInterface, const std::vector<std::string>* pSchemaPaths)
{
    const std::vector<std::string> schemaPaths_;
    const std::vector<std::string>& schemaPaths = (pSchemaPaths != nullptr) ? *pSchemaPaths : schemaPaths_;

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
                mContainer->addData(std::move(data), findLegend(gsl::narrow<size_t>(productNum)));
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

    if (mRecord.getNumImages() == 0) // Get the total number of images in the NITF
    {
        throw except::Exception(Ctxt("SICD/SIDD files must have at least one image"));
    }

    mInfos = getImageInfos(*mContainer);
    auto images_ = mRecord.getImages();
    const auto& images = images_;

    // Now go through every image and figure out what clump it's attached
    // to and use that for the measurements
    for (size_t nitfSegmentIdx = 0; nitfSegmentIdx < images.getSize(); ++nitfSegmentIdx)
    {
        // Get a segment ref
       auto segment_ = static_cast<nitf::ImageSegment>(images_[nitfSegmentIdx]);
       const auto& segment = segment_;

        // Get the subheader out
        const auto subheader = segment.getSubheader();

        // The number of rows in the segment (actual)
        const auto numRowsSeg = subheader.numRows();

        // This function should throw if the data does not exist
        ImageAndSegment imageAndSegment;
        getIndices(subheader, imageAndSegment);
        if (imageAndSegment.image >= mInfos.size())
        {
            throw except::Exception(Ctxt("Image " + std::to_string(imageAndSegment.image) + " is out of bounds"));
        }

        const auto& currentInfo = mInfos[imageAndSegment.image];
        const auto productSegmentIdx = imageAndSegment.segment;

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
        addImageClassOptions(subheader, currentInfo->getData_()->getClassification());

        NITFSegmentInfo si;
        si.numRows = numRowsSeg;
        if (productSegmentIdx == 0)
        {
            si.rowOffset = 0;
            si.firstRow = 0;
            currentInfo->setStartIndex(nitfSegmentIdx);
        }
        else
        {
            const auto imageSegments = currentInfo->getImageSegments();
            si.rowOffset = imageSegments[productSegmentIdx - 1].getNumRows();
            si.firstRow = imageSegments[productSegmentIdx - 1].getFirstRow() + si.getRowOffset();
        }

        // Legends don't set lat/lons
        const auto do_setLatLon = six::enable_ded ? !segIsLegend && !segIsDed : !segIsLegend;
        if (do_setLatLon)
        {
            double corners[4][2]{};
            subheader.getCornersAsLatLons(corners);
            for (size_t kk = 0; kk < LatLonCorners::NUM_CORNERS; ++kk)
            {
                si.corners.getCorner(kk).setLatLon(corners[kk]);
            }
        }

        // SIDD 2.0 needs to read LUT directly from NITF
        const auto pData = currentInfo->getData();
        if (pData->getDataType() == DataType::DERIVED && pData->getVersion() == "2.0.0")
        {
            const auto nitfLut = subheader.getBandInfo(0).getLookupTable();
            currentInfo->getData_()->setDisplayLUT(std::make_unique<AmplitudeTable>(nitfLut));
        }
        currentInfo->addSegment(si);
    }
}
void NITFReadControl::load(std::shared_ptr<nitf::IOInterface> ioInterface, const std::vector<std::filesystem::path>* pSchemaPaths)
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
        nitf::DESegment segment = (nitf::DESegment)*desIter;
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
                pSchemaPaths,
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
                mContainer->addData(std::move(data), findLegend(gsl::narrow<size_t>(productNum)));
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

    if (mRecord.getNumImages() == 0) // Get the total number of images in the NITF
    {
        throw except::Exception(Ctxt("SICD/SIDD files must have at least one image"));
    }

    mInfos = getImageInfos(*mContainer);
    auto images_ = mRecord.getImages();
    const auto& images = images_;

    // Now go through every image and figure out what clump it's attached
    // to and use that for the measurements
    for (size_t nitfSegmentIdx = 0; nitfSegmentIdx < images.getSize(); ++nitfSegmentIdx)
    {
        // Get a segment ref
        auto segment_ = static_cast<nitf::ImageSegment>(images_[nitfSegmentIdx]);
        const auto& segment = segment_;

        // Get the subheader out
        const auto subheader = segment.getSubheader();

        // The number of rows in the segment (actual)
        const auto numRowsSeg = subheader.numRows();

        // This function should throw if the data does not exist
        ImageAndSegment imageAndSegment;
        getIndices(subheader, imageAndSegment);
        if (imageAndSegment.image >= mInfos.size())
        {
            throw except::Exception(Ctxt("Image " + std::to_string(imageAndSegment.image) + " is out of bounds"));
        }

        const auto& currentInfo = mInfos[imageAndSegment.image];
        const auto productSegmentIdx = imageAndSegment.segment;

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
        addImageClassOptions(subheader, currentInfo->getData_()->getClassification());

        NITFSegmentInfo si;
        si.numRows = numRowsSeg;
        if (productSegmentIdx == 0)
        {
            si.rowOffset = 0;
            si.firstRow = 0;
            currentInfo->setStartIndex(nitfSegmentIdx);
        }
        else
        {
            const auto imageSegments = currentInfo->getImageSegments();
            si.rowOffset = imageSegments[productSegmentIdx - 1].getNumRows();
            si.firstRow = imageSegments[productSegmentIdx - 1].getFirstRow() + si.getRowOffset();
        }

        // Legends don't set lat/lons
        const auto do_setLatLon = six::enable_ded ? !segIsLegend && !segIsDed : !segIsLegend;
        if (do_setLatLon)
        {
            double corners[4][2]{};
            subheader.getCornersAsLatLons(corners);
            for (size_t kk = 0; kk < LatLonCorners::NUM_CORNERS; ++kk)
            {
                si.corners.getCorner(kk).setLatLon(corners[kk]);
            }
        }

        // SIDD 2.0 needs to read LUT directly from NITF
        const auto pData = currentInfo->getData();
        if (pData->getDataType() == DataType::DERIVED && pData->getVersion() == "2.0.0")
        {
            const auto nitfLut = subheader.getBandInfo(0).getLookupTable();
            currentInfo->getData_()->setDisplayLUT(std::make_unique<AmplitudeTable>(nitfLut));
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

    AddSecurityOption(const AddSecurityOption&) = delete;
    AddSecurityOption& operator=(const AddSecurityOption&) = delete;
    AddSecurityOption& operator=(AddSecurityOption&&) = delete;

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

    const types::RowCol<ptrdiff_t> imageExtent(getExtent(thisImage.getData()));
    const auto numRowsTotal = imageExtent.row;
    const auto numColsTotal = imageExtent.col;

    if (region.getNumRows() == -1)
    {
        region.setNumRows(numRowsTotal);
    }
    if (region.getNumCols() == -1)
    {
        region.setNumCols(numColsTotal);
    }

    const auto regionExtent = getExtent(region);
    const auto numRowsReq = regionExtent.row;
    const auto numColsReq = regionExtent.col;

    const auto startRow = region.getStartRow();
    const auto startCol = region.getStartCol();

    const auto extentRows = startRow + numRowsReq;
    const auto extentCols = startCol + numColsReq;

    if (extentRows > numRowsTotal || startRow > numRowsTotal)
        throw except::Exception(Ctxt(FmtX("Too many rows requested [%d]", numRowsReq)));

    if (extentCols > numColsTotal || startCol > numColsTotal)
        throw except::Exception(Ctxt(FmtX("Too many cols requested [%d]", numColsReq)));

    // Allocate one band
    uint32_t bandList(0);

    const auto subWindowSize = regionExtent.area() * thisImage.getData()->getNumBytesPerPixel();

    auto buffer = region.getBuffer();
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

    std::vector < NITFSegmentInfo > imageSegments = thisImage.getImageSegments();
    const size_t numIS = imageSegments.size();
    size_t startOff = 0;

    size_t i;
    for (i = 0; i < numIS; i++)
    {
        const auto firstRowSeg = gsl::narrow<ptrdiff_t>(imageSegments[i].getFirstRow());

        if (firstRowSeg <= startRow)
        {
            // It could be in this segment
            startOff = gsl::narrow<decltype(startOff)>(firstRowSeg);
        }
        else
        {
            break;
        }

    }
    --i; // Need to get rid of the last one
    size_t totalRead = 0;
    auto numRowsLeft = numRowsReq;
    sw.setStartRow(static_cast<uint32_t>(startRow - startOff));
#if DEBUG_OFFSETS
    std::cout << "startRow: " << startRow
    << " startOff: " << startOff
    << " sw.startRow: " << sw.getStartRow()
    << " i: " << i << std::endl;
#endif

    const auto nbpp = thisImage.getData()->getNumBytesPerPixel();
    const auto startIndex = thisImage.getStartIndex();
    createCompressionOptions(mCompressionOptions);
    for (; i < numIS && totalRead < subWindowSize; i++)
    {
        const auto numRowsReqSeg =
                std::min(gsl::narrow<size_t>(numRowsLeft), imageSegments[i].getNumRows() - sw.getStartRow());

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
    const types::RowCol<uint32_t> dims(subheader.dims());
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

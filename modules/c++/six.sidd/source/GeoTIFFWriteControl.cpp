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

#include "six/sidd/GeoTIFFWriteControl.h"

#if !defined(SIX_TIFF_DISABLED)

using namespace six;
using namespace six::sidd;

GeoTIFFWriteControl::GeoTIFFWriteControl()
{
    tiff::KnownTagsRegistry::getInstance().addEntry(Constants::GT_XML_KEY,
                                                    tiff::Const::Type::ASCII,
                                                    Constants::GT_XML_TAG);
}
void GeoTIFFWriteControl::initialize(Container* container)
{

    if (container->getDataType() != DataType::DERIVED)
        throw except::Exception(Ctxt("SICD spec. does not support GeoTIFF"));

    // There still could be complex data in the container though, so we
    // will keep those around for later

    sys::Uint64_T length = 0;
    for (size_t ii = 0; ii < container->getNumData(); ++ii)
    {
        Data* data = container->getData(ii);
        if (data->getDataType() == DataType::COMPLEX)
            mComplexData.push_back(data);
        else if (data->getDataType() == DataType::DERIVED)
        {
            //length = ??
            length += (sys::Uint64_T) data->getNumBytesPerPixel()
                    * (sys::Uint64_T) data->getNumRows()
                    * (sys::Uint64_T) data->getNumCols();

            if (length > Constants::GT_SIZE_MAX)
                throw except::Exception(
                                        Ctxt(
                                             "Data segments are too large to be stored in GeoTIFF format"));

            mDerivedData.push_back(data);
        }
        else
            throw except::Exception(
                                    Ctxt(
                                         FmtX(
                                              "Data element at position [%d] in container is undefined",
                                              ii)));
    }

}

void GeoTIFFWriteControl::save(SourceList& sources, const std::string& toFile)
{
    tiff::FileWriter tiffWriter(toFile);

    tiffWriter.writeHeader();
    if (sources.size() != mDerivedData.size())
        throw except::Exception(
                                Ctxt(
                                     FmtX(
                                          "Meta-data count [%d] does not match source list [%d]",
                                          mDerivedData.size(), sources.size())));

    std::vector<unsigned char> buf;
    for (size_t ii = 0; ii < sources.size(); ++ii)
    {
        tiff::ImageWriter* const imageWriter = tiffWriter.addImage();
        const DerivedData* const data =
            reinterpret_cast<DerivedData*>(mDerivedData[ii]);
        const unsigned long oneRow =
            data->getNumCols() * data->getNumBytesPerPixel();
        tiff::IFD* ifd = imageWriter->getIFD();
        setupIFD(data, ifd);
        buf.resize(oneRow);
        const unsigned long numRows = data->getNumRows();
        const unsigned long numCols = data->getNumCols();

        for (unsigned long row = 0; row < numRows; ++row)
        {
            sources[ii]->read((sys::byte*)&buf[0], oneRow);
            imageWriter->putData(&buf[0], numCols);
        }
        imageWriter->writeIFD();
    }

    tiffWriter.close();

}

void GeoTIFFWriteControl::setupIFD(const DerivedData* data, tiff::IFD* ifd)
{
    PixelType pixelType = data->getPixelType();
    sys::Uint32_T numRows = (sys::Uint32_T) data->getNumRows();
    sys::Uint32_T numCols = (sys::Uint32_T) data->getNumCols();

    // Start by initializing the TIFF info
    ifd->addEntry(tiff::KnownTags::IMAGE_WIDTH, numCols);

    ifd->addEntry(tiff::KnownTags::IMAGE_LENGTH, numRows);
    ifd->addEntry(tiff::KnownTags::BITS_PER_SAMPLE);

    tiff::IFDEntry* bitsPerSample = (*ifd)[tiff::KnownTags::BITS_PER_SAMPLE];

    unsigned short numBands = data->getNumChannels();
    unsigned short bitDepth = data->getNumBytesPerPixel() * 8 / numBands;

    for (unsigned int j = 0; j < numBands; ++j)
    {
        bitsPerSample->addValue(
                                tiff::TypeFactory::create(
                                                          (unsigned char*) &bitDepth,
                                                          tiff::Const::Type::SHORT));
    }

    unsigned short photoInterp(1);

    if (pixelType == PixelType::RGB8LU)
    {
        ifd->addEntry("ColorMap");
        tiff::IFDEntry* lutEntry = (*ifd)["ColorMap"];
        LUT& lut = *data->display->remapInformation->remapLUT;

        for (unsigned int j = 0; j < 3; ++j)
        {
            for (unsigned int i = 0; i < lut.numEntries; ++i)
            {
                unsigned short lutij = lut[i][j];
                lutEntry->addValue(
                                   tiff::TypeFactory::create(
                                                             (unsigned char*) &lutij,
                                                             tiff::Const::Type::SHORT));
            }
        }

        photoInterp = 3;
    }
    else if (pixelType == PixelType::RGB24I)
    {
        short spp(3);
        ifd->addEntry(tiff::KnownTags::SAMPLES_PER_PIXEL, spp);
        photoInterp = 2;
    }
    ifd->addEntry(tiff::KnownTags::PHOTOMETRIC_INTERPRETATION, photoInterp);

    addStringArray(ifd,
                   "ImageDescription",
                   FmtX("SIDD: %s", data->getName().c_str()));

    unsigned short orientation(1);
    ifd->addEntry("Orientation", orientation);
    unsigned short planarConf(1);
    ifd->addEntry("PlanarConfiguration", planarConf);

    addStringArray(ifd,
                   "Software",
                   data->productCreation->processorInformation->application);

    char date[256];
    date[255] = 0;
    data->getCreationTime().format("%Y:%m:%d %H:%M:%S", date, 255);

    addCharArray(ifd, "DateTime", date);

    addStringArray(ifd,
                   "Artist",
                   data->productCreation->processorInformation->site);

    ifd->addEntry(tiff::KnownTags::COMPRESSION,
                  (unsigned short) tiff::Const::CompressionType::NO_COMPRESSION);

    addGeoTIFFKeys(ifd, data->getImageCorners(), numRows, numCols);

    // Add in the SIDD and SICD xml in a single IFDEntry
    // Each XML section is separated by a null character
    if (!ifd->exists(Constants::GT_XML_TAG))
    {
        ifd->addEntry(Constants::GT_XML_TAG);
    }
    tiff::IFDEntry* const xmlEntry = (*ifd)[Constants::GT_XML_TAG];

    addStringArray(xmlEntry, toXMLString(data));

    for (size_t jj = 0; jj < mComplexData.size(); ++jj)
    {
        addStringArray(xmlEntry, toXMLString(mComplexData[jj]));
    }
}

void GeoTIFFWriteControl::save(BufferList& sources, const std::string& toFile)
{

    tiff::FileWriter tiffWriter(toFile);

    tiffWriter.writeHeader();
    if (sources.size() != mDerivedData.size())
        throw except::Exception(
                                Ctxt(
                                     FmtX(
                                          "Meta-data count [%d] does not match source list [%d]",
                                          mDerivedData.size(), sources.size())));

    for (size_t ii = 0; ii < sources.size(); ++ii)
    {

        tiff::ImageWriter* imageWriter = tiffWriter.addImage();
        tiff::IFD* ifd = imageWriter->getIFD();

        DerivedData* data = (DerivedData*) mDerivedData[ii];
        setupIFD(data, ifd);
        // Now we hack to write

        imageWriter->putData(sources[ii], data->getNumRows()
                * data->getNumCols());

        imageWriter->writeIFD();
    }

    tiffWriter.close();
}

void GeoTIFFWriteControl::addCharArray(tiff::IFDEntry *entry,
                                       const char *cstr,
                                       int tiffType)
{
    const unsigned char * const cstr_ptr =
        reinterpret_cast<const unsigned char *>(cstr);

    for (size_t ii = 0, len = ::strlen(cstr) + 1; ii < len; ++ii)
    {
        std::auto_ptr<tiff::TypeInterface>
            value(tiff::TypeFactory::create(cstr_ptr + ii, tiffType));
        entry->addValue(value);
    }
}

void GeoTIFFWriteControl::addCharArray(tiff::IFD* ifd, const std::string &tag,
                                       const char* cstr, int tiffType)
{
    if (!ifd->exists(tag.c_str()))
    {
        ifd->addEntry(tag);
    }

    addCharArray((*ifd)[tag.c_str()], cstr, tiffType);
}

void GeoTIFFWriteControl::addStringArray(tiff::IFDEntry* entry,
                                         const std::string &str,
                                         int tiffType)
{
    addCharArray(entry, str.c_str(), tiffType);
}

void GeoTIFFWriteControl::addStringArray(tiff::IFD* ifd,
                                         const std::string &tag,
                                         const std::string &str,
                                         int tiffType)
{
    addCharArray(ifd, tag, str.c_str(), tiffType);
}

void GeoTIFFWriteControl::addGeoTIFFKeys(tiff::IFD* ifd, const std::vector<
        LatLon>& c, unsigned long numRows, unsigned long numCols)
{
    ifd->addEntry("GeoKeyDirectoryTag");
    tiff::IFDEntry* entry = (*ifd)["GeoKeyDirectoryTag"];

    // Write the gt header
    // The first four elements are...
    // keyDirVersion, keyRevision, keyRevisionMinor, numKeys
    // If keys are added/removed from this header, numKeys must be kept in
    // sync
    const short header[] = {
    1,    1, 2, 4,
    1024, 0, 1, 2, /* GTModelTypeGeoKey - Geographic */
    1025, 0, 1, 1, /* GTRasterTypeGeoKey - RasterPixelIsArea */
    2048, 0, 1, 4326, /* GeographicTypeGeoKey - (GCSE_WGS84) 4030 */
    3072, 0, 1, 0 }; /* ProjectedCSTypeGeoKey */
    for (unsigned int i = 0; i < sizeof(header) / sizeof(short); ++i)
    {
        entry->addValue(tiff::TypeFactory::create((unsigned char*) &header[i],
                                                  tiff::Const::Type::SHORT));
    }

    ifd->addEntry("ModelTiepointTag");
    entry = (*ifd)["ModelTiepointTag"];

    double ties[6];
    memset(ties, 0, sizeof(double) * 6);

    // SIDD footprint starts at upper left corner, then clockwise

    size_t numPoints = c.size();

    if (numPoints > 0)
    {
        // Upper Left
        ties[0] = 0;
        ties[1] = 0;
        ties[3] = c[0].getLon();
        ties[4] = c[0].getLat();
        addTiepoint(entry, ties);
    }

    if (numPoints > 1)
    {
        // Upper Right
        ties[0] = numCols;
        ties[1] = 0;
        ties[3] = c[1].getLon();
        ties[4] = c[1].getLat();
        addTiepoint(entry, ties);
    }

    if (numPoints > 2)
    {
        // Lower Right
        ties[0] = numCols;
        ties[1] = numRows;
        ties[3] = c[2].getLon();
        ties[4] = c[2].getLat();
        addTiepoint(entry, ties);
    }

    if (numPoints > 3)
    {
        // Lower Left
        ties[0] = 0;
        ties[1] = numRows;
        ties[3] = c[3].getLon();
        ties[4] = c[3].getLat();
        addTiepoint(entry, ties);
    }

}

#endif

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
#include "six/GeoTIFFWriteControl.h"

#if !defined(SIX_TIFF_DISABLED)

using namespace six;

GeoTIFFWriteControl::GeoTIFFWriteControl()
{
    // Fix us, we are broken
    tiff::KnownTagsRegistry::getInstance().addEntry(
        Constants::GT_SICD_KEY,
        tiff::Const::Type::ASCII, "SICDXMLTag"
        );
    tiff::KnownTagsRegistry::getInstance().addEntry(
        Constants::GT_SIDD_KEY,
        tiff::Const::Type::ASCII, "SIDDXMLTag"
        );
}
void GeoTIFFWriteControl::initialize(Container* container)
{

    if (container->getDataType() != TYPE_DERIVED)
        throw except::Exception(Ctxt("SICD spec. does not support GeoTIFF"));

    // There still could be complex data in the container though, so we
    // will keep those around for later

    sys::Uint64_T length = 0;
    for (unsigned int i = 0; i < container->getNumData(); ++i)
    {
        Data* data = container->getData(i);
        if (data->getDataClass() == DATA_COMPLEX)
            mComplexData.push_back(data);
        else if (data->getDataClass() == DATA_DERIVED)
        {
            //length = ??
            length += (sys::Uint64_T)data->getNumBytesPerPixel() *
                (sys::Uint64_T)data->getNumRows() * (sys::Uint64_T)data->getNumCols();

            if (length > Constants::GT_SIZE_MAX)
                throw except::Exception(Ctxt("Data segments are too large to be stored in GeoTIFF format"));

            mDerivedData.push_back(data);
        }
        else
            throw except::Exception(Ctxt(FmtX("Data element at position [%d] in container is undefined", i)));
    }


}

void GeoTIFFWriteControl::save(SourceList& sources, std::string toFile)
{
    tiff::FileWriter tiffWriter(toFile);

    tiffWriter.writeHeader();
    if (sources.size() != mDerivedData.size())
        throw except::Exception(
            Ctxt(
                FmtX("Meta-data count [%d] does not match source list [%d]",
                     mDerivedData.size(), sources.size())
                )
            );

    for (unsigned int i = 0; i < sources.size(); ++i)
    {
        tiff::ImageWriter* imageWriter = tiffWriter.addImage();
        DerivedData* data = (DerivedData*)mDerivedData[i];
        unsigned long oneRow = data->getNumCols() * data->getNumBytesPerPixel();
        tiff::IFD* ifd = imageWriter->getIFD();

        std::vector<char*> allocated = setupIFD(data, ifd);
        unsigned char * buf = new unsigned char[oneRow];
        unsigned long numRows = data->getNumRows();
        unsigned long numCols = data->getNumCols();

        for (unsigned int j = 0; j < numRows; ++j)
        {
            sources[i]->read((sys::byte*)buf, oneRow);
            imageWriter->putData(buf, numCols);
        }
        delete [] buf;
        imageWriter->writeIFD();

        for (unsigned int j = 0; j < allocated.size(); ++j)
            delete allocated[j];

    }

    tiffWriter.close();



}

void GeoTIFFWriteControl::save(io::InputStream& source, std::string toFile)
{
    tiff::FileWriter tiffWriter(toFile);

    tiffWriter.writeHeader();
    if (mDerivedData.size() != 1)
        throw except::Exception(
            Ctxt(
                FmtX("Meta-data count [%d] does not match source [1]",
                     mDerivedData.size())
                )
            );

    tiff::ImageWriter* imageWriter = tiffWriter.addImage();
    DerivedData* data = (DerivedData*)mDerivedData[0];
    unsigned long oneRow = data->getNumCols() * data->getNumBytesPerPixel();        tiff::IFD* ifd = imageWriter->getIFD();

    std::vector<char*> allocated = setupIFD(data, ifd);
    unsigned char * buf = new unsigned char[oneRow];
    unsigned long numRows = data->getNumRows();
    unsigned long numCols = data->getNumCols();

    for (unsigned int j = 0; j < numRows; ++j)
    {
        source.read((sys::byte*)buf, oneRow);
        imageWriter->putData(buf, numCols);
    }
    delete [] buf;
    imageWriter->writeIFD();

    for (unsigned int j = 0; j < allocated.size(); ++j)
        delete allocated[j];

    tiffWriter.close();
}

std::vector<char*> GeoTIFFWriteControl::setupIFD(DerivedData* data, tiff::IFD* ifd)
{
    std::vector<char*> allocated;
    PixelType pixelType = data->getPixelType();
    unsigned long numRows = data->getNumRows();
    unsigned long numCols = data->getNumCols();

    // Start by initializing the TIFF info
    ifd->addEntry(tiff::KnownTags::IMAGE_WIDTH, numCols);
    ifd->addEntry(tiff::KnownTags::IMAGE_LENGTH, numRows);
    ifd->addEntry(tiff::KnownTags::BITS_PER_SAMPLE);


    tiff::IFDEntry* bitsPerSample =
        (*ifd)[tiff::KnownTags::BITS_PER_SAMPLE];

    unsigned short numBands = data->getNumChannels();
    unsigned short eight(8);

    for (unsigned int j = 0; j < numBands; ++j)
    {
        bitsPerSample->addValue(
            tiff::TypeFactory::create((unsigned char*)&eight,
                                      tiff::Const::Type::SHORT)
            );
    }

    unsigned int photoInterp(1);

    if (pixelType == RGB8LU)
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
                    tiff::TypeFactory::create((unsigned char*)&lutij,
                                              tiff::Const::Type::SHORT)
                    );
            }
        }

        photoInterp = 3;
    }
    else if (pixelType == RGB24I)
    {
        ifd->addEntry(tiff::KnownTags::SAMPLES_PER_PIXEL, 3);
        photoInterp = 2;
    }
    ifd->addEntry(tiff::KnownTags::PHOTOMETRIC_INTERPRETATION,
                  photoInterp);

    allocated.push_back(
        addStringArray(ifd, "ImageDescription",
                       FmtX("SIDD: %s", data->getName().c_str())
            )
        );

    ifd->addEntry("Orientation", 1);
    ifd->addEntry("XResolution", 1);
    ifd->addEntry("YResolution", 1);
    ifd->addEntry("PlanarConfiguration", 1);

    allocated.push_back(
        addStringArray(
            ifd, "Software",
            data->productCreation->processorInformation->application)
        );

    char date[256];
    date[255] = 0;
    data->getCreationTime().format("%Y:%m:%d %H:%M:%S", date, 255);
    std::string dateTime(date);

    allocated.push_back(addStringArray(ifd, "DateTime", dateTime));

    allocated.push_back(
        addStringArray(
            ifd, "Artist", data->productCreation->processorInformation->site)
        );

    ifd->addEntry(tiff::KnownTags::COMPRESSION,
                  (unsigned short)tiff::Const::CompressionType::NO_COMPRESSION);
    char* siddData = toXMLCharArray(data);
    allocated.push_back(siddData);
    addGeoTIFFKeys(ifd, data->getImageCorners(), numRows, numCols);
    addCharArray(ifd, "SIDDXMLTag", siddData, tiff::Const::Type::UNDEFINED);

    for (unsigned int j = 0; j < mComplexData.size(); ++j)
    {
        char* sicdData = toXMLCharArray(mComplexData[j]);
        addCharArray(ifd, "SICDXMLTag", sicdData, tiff::Const::Type::UNDEFINED);
        delete sicdData;
    }


    return allocated;
}

void GeoTIFFWriteControl::save(BufferList& sources, std::string toFile)
{

    tiff::FileWriter tiffWriter(toFile);

    tiffWriter.writeHeader();
    if (sources.size() != mDerivedData.size())
        throw except::Exception(
            Ctxt(
                FmtX("Meta-data count [%d] does not match source list [%d]",
                     mDerivedData.size(), sources.size())
                )
            );

    for (unsigned int i = 0; i < sources.size(); ++i)
    {

        tiff::ImageWriter* imageWriter = tiffWriter.addImage();
        tiff::IFD* ifd = imageWriter->getIFD();
        DerivedData* data = (DerivedData*)mDerivedData[i];
        std::vector<char*> allocated = setupIFD(data, ifd);
        // Now we hack to write
        imageWriter->putData(sources[i], data->getNumRows() * data->getNumCols());
        imageWriter->writeIFD();
        for (unsigned int j = 0; j < allocated.size(); ++j)
            delete allocated[j];

    }

    tiffWriter.close();
}

void GeoTIFFWriteControl::save(UByte* source, std::string toFile)
{
    tiff::FileWriter tiffWriter(toFile);

    tiffWriter.writeHeader();
    if (1 != mDerivedData.size())
        throw except::Exception(
            Ctxt(
                FmtX("Meta-data count [%d] does not match source list [1]",
                     mDerivedData.size())
                )
            );


    tiff::ImageWriter* imageWriter = tiffWriter.addImage();
    tiff::IFD* ifd = imageWriter->getIFD();
    DerivedData* data = (DerivedData*)mDerivedData[0];
    std::vector<char*> allocated = setupIFD(data, ifd);
    // Now we hack to write
    imageWriter->putData(source, data->getNumRows() * data->getNumCols());
    imageWriter->writeIFD();
    for (unsigned int j = 0; j < allocated.size(); ++j)
        delete allocated[j];

    tiffWriter.close();
}

void GeoTIFFWriteControl::addCharArray(tiff::IFD* ifd,
                                       std::string tag,
                                       char* cstr,
                                       int tiffType)
{
    if (!ifd->exists(tag.c_str()))
        ifd->addEntry(tag);

    tiff::IFDEntry* entry = (*ifd)[tag.c_str()];
    size_t len = strlen(cstr) + 1;
    for (unsigned int i = 0; i < len; ++i)
    {
        entry->addValue(tiff::TypeFactory::create((unsigned char*)&cstr[i],
                                                  tiffType));
    }

}
char* GeoTIFFWriteControl::addStringArray(tiff::IFD* ifd,
                                          std::string tag,
                                          std::string str,
                                          int tiffType)
{
    char *copy = new char[ str.length() + 1];
    strcpy(copy, str.c_str());
    addCharArray(ifd, tag, copy, tiffType);
    return copy;
}

void GeoTIFFWriteControl::addGeoTIFFKeys(tiff::IFD* ifd,
                                         Corners c,
                                         unsigned long numRows,
                                         unsigned long numCols)
{
    ifd->addEntry("GeoKeyDirectoryTag");
    tiff::IFDEntry* entry = (*ifd)["GeoKeyDirectoryTag"];

    // Write the gt header
    const short header[] =
    {
        1, 1, 2, 6,
        1024, 0, 1, 2,  /* GTModelTypeGeoKey - Geographic */
        1025, 0, 1, 1,  /* GTRasterTypeGeoKey - RasterPixelIsArea */
        2048, 0, 1, 4326, /* GeographicTypeGeoKey - (GCSE_WGS84) 4030 */
        2052, 0, 1, 9001, /* GeogLinearUnitsGeoKey - Linear_Meter */
        2054, 0, 1, 9102, /* GeogAngularUnitsGeoKey - Angular Degrees */
        2056, 0, 1, 7030, /* GeogEllipsoidGeoKey - Ellipse_WGS_84 */
        3072, 0, 1, 0
    };
    for (unsigned int i = 0; i < sizeof(header)/sizeof(short); ++i)
    {
        entry->addValue(
            tiff::TypeFactory::create((unsigned char*)&header[i],
                                      tiff::Const::Type::SHORT));
    }

    ifd->addEntry("ModelTiepointTag");
    entry = (*ifd)["ModelTiepointTag"];

    double ties[6];
    memset(ties, 0, sizeof(double)*6);

    // 0, 0, lat(0), lon(0)
    ties[3] = c.getLat(0);
    ties[4] = c.getLon(0);
    addTiepoint(entry, ties);

    // 0, NC, lat(1), lon(1)
    ties[1] = numCols;
    ties[3] = c.getLat(1);
    ties[4] = c.getLon(1);
    addTiepoint(entry, ties);

    // NR, NC, lat(2), lon(2)
    ties[0] = numRows;
    ties[3] = c.getLat(2);
    ties[4] = c.getLon(2);
    addTiepoint(entry, ties);
}

#endif

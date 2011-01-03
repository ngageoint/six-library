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
#include "six/sidd/GeoTIFFReadControl.h"
#include "six/XMLControlFactory.h"

namespace six
{
namespace sidd
{
std::vector<std::string> _parseSIXEntry(tiff::IFDEntry *entry)
{
    if (!entry)
        return std::vector<std::string>();
    std::vector<tiff::TypeInterface *> values = entry->getValues();
    std::ostringstream s;
    for (std::vector<tiff::TypeInterface *>::iterator it = values.begin(); it
            != values.end(); ++it)
    {
        s << (*it)->toString();
    }
    return str::split(s.str(), "|");
}

}
}

six::DataType six::sidd::GeoTIFFReadControl::getDataType(std::string fromFile)
{
    try
    {
        tiff::FileReader reader(fromFile);
        if (reader.getImageCount() > 0)
        {
            if (reader[0]->getIFD()->exists(
                                            (unsigned short) six::Constants::GT_SIDD_KEY))
                return six::DataType::DERIVED;
        }
    }
    catch (...)
    {
    }
    return six::DataType::NOT_SET;
}

void six::sidd::GeoTIFFReadControl::load(std::string fromFile,
                                         six::XMLControlRegistry* xmlRegistry)
{
    mReader.openFile(fromFile);

    if (mReader.getImageCount() <= 0
            || !mReader[0]->getIFD()->exists(
                                             (unsigned short) six::Constants::GT_SIDD_KEY))
        throw except::Exception(Ctxt("Unexpected file type"));

    std::vector < std::string > sidds
            = _parseSIXEntry(
                             (*mReader[0]->getIFD())[(unsigned short) six::Constants::GT_SIDD_KEY]);
    std::vector < std::string > sicds;

    if (mReader[0]->getIFD()->exists(
                                     (unsigned short) six::Constants::GT_SICD_KEY))
        sicds
                = _parseSIXEntry(
                                 (*mReader[0]->getIFD())[(unsigned short) six::Constants::GT_SICD_KEY]);

    mContainer = new six::Container(six::DataType::DERIVED);

    if (!xmlRegistry)
        xmlRegistry = &six::XMLControlFactory::getInstance();

    for (size_t i = 0, n = sidds.size(); i < n; ++i)
    {
        six::XMLControl * xmlControl = xmlRegistry->newXMLControl("SIDD_XML");

        std::string xmlString = sidds[i];
        str::trim(xmlString);
        while ((int) xmlString[xmlString.size() - 1] == 0)
            xmlString = xmlString.substr(0, xmlString.size() - 1);

        io::ByteStream stream;
        stream.write(xmlString);
        stream.seek(0, io::Seekable::START);
        xml::lite::MinidomParser xmlParser;
        xmlParser.preserveCharacterData(true);
        xmlParser.parse(stream);
        xml::lite::Document* doc = xmlParser.getDocument();
        six::Data* data = xmlControl->fromXML(doc);
        delete xmlControl;

        if (!data)
            throw except::Exception(Ctxt("Unable to transform SIDD XML"));
        mContainer->addData(data);
    }
    for (size_t i = 0, n = sicds.size(); i < n; ++i)
    {
        six::XMLControl * xmlControl = xmlRegistry->newXMLControl("SICD_XML");

        std::string xmlString = sicds[i];
        str::trim(xmlString);
        while ((int) xmlString[xmlString.size() - 1] == 0)
            xmlString = xmlString.substr(0, xmlString.size() - 1);

        io::ByteStream stream;
        stream.write(xmlString);
        stream.seek(0, io::Seekable::START);
        xml::lite::MinidomParser xmlParser;
        xmlParser.preserveCharacterData(true);
        xmlParser.parse(stream);
        xml::lite::Document* doc = xmlParser.getDocument();
        six::Data* data = xmlControl->fromXML(doc);
        delete xmlControl;

        if (!data)
            throw except::Exception(Ctxt("Unable to transform SICD XML"));
        mContainer->addData(data);
    }

}

six::UByte* six::sidd::GeoTIFFReadControl::interleaved(six::Region& region,
                                                       int imIndex)
{
    if (mReader.getImageCount() <= imIndex || imIndex < 0)
        throw except::IndexOutOfRangeException(Ctxt(FmtX("Invalid index: %d",
                                                         imIndex)));

    tiff::ImageReader *imReader = mReader[imIndex];
    tiff::IFD *ifd = imReader->getIFD();

    size_t numRowsTotal = ifd->getImageLength();
    size_t numColsTotal = ifd->getImageWidth();
    size_t elemSize = ifd->getElementSize();

    if (region.getNumRows() == -1)
        region.setNumRows(numRowsTotal);
    if (region.getNumCols() == -1)
        region.setNumCols(numColsTotal);

    unsigned long numRowsReq = region.getNumRows();
    unsigned long numColsReq = region.getNumCols();

    unsigned long startRow = region.getStartRow();
    unsigned long startCol = region.getStartCol();

    unsigned long extentRows = startRow + numRowsReq;
    unsigned long extentCols = startCol + numColsReq;

    if (extentRows > numRowsTotal || startRow > numRowsTotal)
        throw except::Exception(Ctxt(FmtX("Too many rows requested [%d]",
                                          numRowsReq)));

    if (extentCols > numColsTotal || startCol > numColsTotal)
        throw except::Exception(Ctxt(FmtX("Too many cols requested [%d]",
                                          numColsReq)));

    six::UByte* buffer = region.getBuffer();

    if (buffer == NULL)
    {
        buffer = new six::UByte[numRowsReq * numColsReq * elemSize];
        region.setBuffer(buffer);
    }

    if (numRowsReq == numRowsTotal && numColsReq == numColsTotal)
    {
        // one read
        imReader->getData(buffer, numRowsReq * numColsReq);
    }
    else
    {
        six::UByte* rowBuf = new six::UByte[numColsTotal * elemSize];

        //        // skip past rows
        //        for (size_t i = 0; i < startRow; ++i)
        //            imReader->getData(rowBuf, numColsTotal);

        size_t offset = 0;
        // this is not the most efficient, but it works
        for (size_t i = 0; i < numRowsReq; ++i)
        {
            // possibly skip past some cols
            if (startCol > 0)
                imReader->getData(rowBuf, startCol);
            imReader->getData(rowBuf, numColsReq);
            memcpy(buffer + offset, rowBuf, numColsReq * elemSize);
            offset += numColsReq * elemSize;
            // more skipping..
            if (extentCols < numColsTotal)
                imReader->getData(rowBuf, numColsTotal - extentCols);
        }
        delete[] rowBuf;
    }
    return buffer;
}

six::ReadControl* six::sidd::GeoTIFFReadControlCreator::newReadControl() const
{
    return new six::sidd::GeoTIFFReadControl();
}

bool six::sidd::GeoTIFFReadControlCreator::supports(const std::string& filename) const
{
    try
    {
        six::sidd::GeoTIFFReadControl control;
        return control.getDataType(filename) != DataType::NOT_SET;
    }
    catch (except::Exception&)
    {
        return false;
    }
}

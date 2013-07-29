/* =========================================================================
 * This file is part of six.sidd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2013, General Dynamics - Advanced Information Systems
 *
 * six.sidd-c++ is free software; you can redistribute it and/or modify
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
#include <mem/ScopedArray.h>
#include "six/sidd/GeoTIFFReadControl.h"
#include "six/XMLControlFactory.h"

namespace
{
// This entry should contain XML entries as strings.  Each separate entry is
// NULL-terminated, so we split on this.
void parseXMLEntry(const tiff::IFDEntry *entry,
                   std::vector<std::string> &entries)
{
    entries.clear();

    if (entry)
    {
        const std::vector<tiff::TypeInterface *> values(entry->getValues());

        std::string curStr;
        for (std::vector<tiff::TypeInterface *>::const_iterator iter =
                 values.begin();
             iter != values.end();
             ++iter)
        {
            const tiff::TypeInterface * const value = *iter;
            const char * const data =
                reinterpret_cast<const char *>(value->data());
            const size_t size(value->size());

            for (size_t ii = 0; ii < size; ++ii)
            {
                const char ch(data[ii]);
                if (ch == '\0')
                {
                    str::trim(curStr);
                    if (!curStr.empty())
                    {
                        entries.push_back(curStr);
                        curStr.clear();
                    }
                }
                else
                {
                    curStr += ch;
                }
            }
        }

        // TODO: Should we treat this as an error instead?  We expect the
        //       string to be NULL-terminated according to the TIFF spec.
        str::trim(curStr);
        if (!curStr.empty())
        {
           entries.push_back(curStr);
        }
    }
}
}

six::DataType
six::sidd::GeoTIFFReadControl::getDataType(const std::string& fromFile) const
{
    try
    {
        tiff::FileReader reader(fromFile);
        if (reader.getImageCount() > 0)
        {
            if (reader[0]->getIFD()->exists(six::Constants::GT_XML_KEY))
            {
                std::vector<std::string> xmlStrs;
                parseXMLEntry(
                    (*(reader[0]->getIFD()))[six::Constants::GT_XML_KEY],
                    xmlStrs);

                // If any of the XML strings is a SIDD xml, the data type is
                // DERIVED
                for (size_t ii = 0; ii < xmlStrs.size(); ++ii)
                {
                    // Parse it into an XML document
                    io::StringStream stream;
                    stream.write(xmlStrs[ii]);
                    stream.seek(0, io::Seekable::START);
                    xml::lite::MinidomParser xmlParser;
                    xmlParser.preserveCharacterData(true);
                    xmlParser.parse(stream);

                    const std::string rootName =
                        xmlParser.getDocument()->getRootElement()->getQName();
                    if (rootName == "SIDD")
                    {
                        return six::DataType::DERIVED;
                    }
                }
            }
        }
    }
    catch (...)
    {
    }
    return six::DataType::NOT_SET;
}

void six::sidd::GeoTIFFReadControl::load(
        const std::string& fromFile,
        const std::vector<std::string>& schemaPaths)
{
    // Clean up
    delete mContainer;
    mContainer = NULL;

    mReader.openFile(fromFile);

    if (mReader.getImageCount() <= 0 ||
        !mReader[0]->getIFD()->exists(six::Constants::GT_XML_KEY))
    {
        throw except::Exception(Ctxt(fromFile + ": unexpected file type"));
    }

    std::vector<std::string> xmlStrs;
    parseXMLEntry((*(mReader[0]->getIFD()))[six::Constants::GT_XML_KEY],
                  xmlStrs);

    mContainer = new six::Container(six::DataType::DERIVED);

    std::auto_ptr<six::XMLControl> siddXMLControl;
    std::auto_ptr<six::XMLControl> sicdXMLControl;

    for (size_t ii = 0; ii < xmlStrs.size(); ++ii)
    {
        // Parse it into an XML document
        io::StringStream stream;
        stream.write(xmlStrs[ii]);
        stream.seek(0, io::Seekable::START);
        xml::lite::MinidomParser xmlParser;
        xmlParser.preserveCharacterData(true);
        xmlParser.parse(stream);
        const xml::lite::Document* const doc = xmlParser.getDocument();

        // Get the associated XML control
        const std::string rootName(doc->getRootElement()->getQName());
        six::XMLControl *xmlControl;
        if (rootName == "SIDD")
        {
            if (siddXMLControl.get() == NULL)
            {
                siddXMLControl.reset(
                    mXMLRegistry->newXMLControl(DataType::DERIVED, mLog));
            }
            xmlControl = siddXMLControl.get();
        }
        else if (rootName == "SICD")
        {
            if (sicdXMLControl.get() == NULL)
            {
                sicdXMLControl.reset(
                    mXMLRegistry->newXMLControl(DataType::COMPLEX, mLog));
            }
            xmlControl = sicdXMLControl.get();
        }
        else
        {
            // If it's something we don't know about, just skip it
            xmlControl = NULL;
        }

        if (xmlControl)
        {
            std::auto_ptr<six::Data> data(xmlControl->fromXML(doc, 
                                                              schemaPaths));

            if (!data.get())
            {
                throw except::Exception(Ctxt(
                          "Unable to transform " + rootName + " XML"));
            }
            mContainer->addData(data);
        }
    }
}

six::UByte* six::sidd::GeoTIFFReadControl::interleaved(six::Region& region,
                                                       size_t imIndex)
{
    if (mReader.getImageCount() <= imIndex)
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
        const mem::ScopedArray<six::UByte>
            scopedRowBuf(new six::UByte[numColsTotal * elemSize]);
        six::UByte* const rowBuf(scopedRowBuf.get());

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


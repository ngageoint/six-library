/* =========================================================================
 * This file is part of six.sidd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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

#include <string>
#include <vector>

#include <str/Convert.h>
#include <gsl/gsl.h>
#include <mem/ScopedArray.h>
#include "six/sidd/GeoTIFFReadControl.h"
#include "six/XMLControlFactory.h"

namespace
{
// This entry should contain XML entries as strings.  Each separate entry is
// nullptr-terminated, so we split on this.
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
        //       string to be nullptr-terminated according to the TIFF spec.
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
    mReader.openFile(fromFile);

    if (mReader.getImageCount() <= 0 ||
        !mReader[0]->getIFD()->exists(six::Constants::GT_XML_KEY))
    {
        throw except::Exception(Ctxt(fromFile + ": unexpected file type"));
    }

    std::vector<std::string> xmlStrs;
    parseXMLEntry((*(mReader[0]->getIFD()))[six::Constants::GT_XML_KEY],
                  xmlStrs);

    mContainer.reset(new six::Container(six::DataType::DERIVED));

    std::unique_ptr<six::XMLControl> siddXMLControl;
    std::unique_ptr<six::XMLControl> sicdXMLControl;

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
        six::XMLControl* xmlControl = nullptr;
        if (rootName == "SIDD")
        {
            if (siddXMLControl.get() == nullptr)
            {
                siddXMLControl.reset(
                    mXMLRegistry->newXMLControl(DataType::DERIVED, mLog));
            }
            xmlControl = siddXMLControl.get();
        }
        else if (rootName == "SICD")
        {
            if (sicdXMLControl.get() == nullptr)
            {
                sicdXMLControl.reset(
                    mXMLRegistry->newXMLControl(DataType::COMPLEX, mLog));
            }
            xmlControl = sicdXMLControl.get();
        }
        else
        {
            // If it's something we don't know about, just skip it
            xmlControl = nullptr;
        }

        if (xmlControl)
        {
            std::unique_ptr<six::Data> data(xmlControl->fromXML(doc,
                                                              schemaPaths));

            if (!data.get())
            {
                throw except::Exception(Ctxt(
                          "Unable to transform " + rootName + " XML"));
            }
            mContainer->addData(std::move(data));
        }
    }
}

six::UByte* six::sidd::GeoTIFFReadControl::interleaved(six::Region& region,
                                                       size_t imIndex)
{
    if (mReader.getImageCount() <= imIndex)
    {
        throw except::IndexOutOfRangeException(Ctxt(
                "Invalid index: " + std::to_string(imIndex)));
    }

    tiff::ImageReader *imReader = mReader[static_cast<uint32_t>(imIndex)];
    tiff::IFD *ifd = imReader->getIFD();

    const auto numRowsTotal = ifd->getImageLength();
    const auto numColsTotal = ifd->getImageWidth();
    const auto elemSize = ifd->getElementSize();

    if (region.getNumRows() == -1)
        region.setNumRows(numRowsTotal);
    if (region.getNumCols() == -1)
        region.setNumCols(numColsTotal);

    const auto numRowsReq = region.getNumRows();
    const auto numColsReq = region.getNumCols();

    const auto startRow = region.getStartRow();
    const auto startCol = region.getStartCol();

    const auto extentRows = startRow + numRowsReq;
    const auto extentCols = startCol + numColsReq;

    if (extentRows > numRowsTotal || startRow > numRowsTotal)
    {
        throw except::Exception(Ctxt("Too many rows requested [" +
                std::to_string(numRowsReq) + "]"));
    }

    if (extentCols > numColsTotal || startCol > numColsTotal)
    {
        throw except::Exception(Ctxt("Too many cols requested [" +
                std::to_string(numColsReq) + "]"));
    }

    auto buffer = region.getBuffer();

    if (buffer == nullptr)
    {
        const types::RowCol<size_t> regionExtent(getExtent(region));
        buffer = region.setBuffer(regionExtent.area() * elemSize).release();
    }

    if (numRowsReq == numRowsTotal && numColsReq == numColsTotal)
    {
        // one read
        imReader->getData(reinterpret_cast<unsigned char*>(buffer), static_cast<uint32_t>(numRowsReq * numColsReq));
    }
    else
    {
        std::vector<std::byte> scopedRowBuf(gsl::narrow<size_t>(numColsTotal) * elemSize);
        std::byte* const rowBuf(scopedRowBuf.data());

        //        // skip past rows
        //        for (size_t i = 0; i < startRow; ++i)
        //            imReader->getData(rowBuf, numColsTotal);

        size_t offset = 0;
        // this is not the most efficient, but it works
        for (size_t i = 0; i < static_cast<size_t>(numRowsReq); ++i)
        {
            auto rowBuf_ = reinterpret_cast<unsigned char*>(rowBuf);
            // possibly skip past some cols
            if (startCol > 0)
                imReader->getData(rowBuf_, static_cast<uint32_t>(startCol));
            imReader->getData(rowBuf_, static_cast<uint32_t>(numColsReq));
            memcpy(buffer + offset, rowBuf, static_cast<size_t>(numColsReq * elemSize));
            offset += numColsReq * elemSize;
            // more skipping..
            if (extentCols < numColsTotal)
                imReader->getData(rowBuf_, static_cast<uint32_t>(numColsTotal - extentCols));
        }
    }
    return buffer;
}
void six::sidd::GeoTIFFReadControl::interleaved(six::Region& region,
                                                       size_t imIndex, std::byte* &result)
{
    result = reinterpret_cast<std::byte*>(interleaved(region, imIndex));
}

six::ReadControl* six::sidd::GeoTIFFReadControlCreator::newReadControl() const
{
    std::unique_ptr<six::ReadControl> retval;
    newReadControl(retval);
    return retval.release();
}
void six::sidd::GeoTIFFReadControlCreator::newReadControl(std::unique_ptr<six::ReadControl>& result) const
{
    result = std::unique_ptr<six::ReadControl>(new six::sidd::GeoTIFFReadControl());
}


bool six::sidd::GeoTIFFReadControlCreator::supports(const std::string& filename) const
{
    try
    {
        six::sidd::GeoTIFFReadControl control;
        return control.getDataType(filename) != DataType::NOT_SET;
    }
    catch (const except::Exception&)
    {
        return false;
    }
}


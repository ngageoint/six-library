/* =========================================================================
 * This file is part of six.sicd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 * (C) Copyright 2023, Maxar Technologies, Inc.
 *
 * six.sicd-c++ is free software; you can redistribute it and/or modify
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
#include "six/sicd/DataParser.h"

#include <assert.h>

#include <string>
#include <memory>

#include <io/StringStream.h>
#include <gsl/gsl.h>

#include "six/Utilities.h"
#include "six/XMLControlFactory.h"

#include "six/sicd/ComplexXMLControl.h"
#include "six/sicd/ImageData.h"
#include "six/sicd/NITFReadComplexXMLControl.h"

namespace fs = std::filesystem;

six::sicd::DataParser::DataParser(const std::vector<std::filesystem::path>* pSchemaPaths, logging::Logger* pLog)
    : mDataParser(pSchemaPaths, pLog)
{
    mXmlRegistry.addCreator<ComplexXMLControl>();
}

std::unique_ptr<six::sicd::ComplexData> six::sicd::DataParser::DataParser::fromXML(::io::InputStream& xmlStream) const
{
    auto pData = mDataParser.fromXML(xmlStream, mXmlRegistry, DataType::NOT_SET);
    return std::unique_ptr<ComplexData>(static_cast<ComplexData*>(pData.release()));
}

std::unique_ptr<six::sicd::ComplexData> six::sicd::DataParser::DataParser::fromXML(const std::filesystem::path& pathname) const
{
    io::FileInputStream inStream(pathname.string());
    return fromXML(inStream);
}

std::unique_ptr<six::sicd::ComplexData> six::sicd::DataParser::DataParser::fromXML(const std::u8string& xmlStr) const
{
    io::U8StringStream inStream;
    inStream.write(xmlStr);
    return fromXML(inStream);
}

std::u8string six::sicd::DataParser::DataParser::toXML(const six::sicd::ComplexData& data) const
{
    return mDataParser.toXML(data, mXmlRegistry);
}

void six::sicd::DataParser::DataParser::preserveCharacterData(bool preserve)
{
    mDataParser.preserveCharacterData(preserve);
}
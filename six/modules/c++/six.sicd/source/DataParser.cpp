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

six::sicd::DataParser::DataParser(const std::vector<std::filesystem::path>* pSchemaPaths, logging::Logger* pLog, bool preserveCharacterData)
	: mpSchemaPaths(pSchemaPaths),
	mLog(pLog == nullptr ? mNullLogger : *pLog),
	mPreserveCharacterData(preserveCharacterData)
{
}

std::unique_ptr<six::sicd::ComplexData> six::sicd::DataParser::DataParser::parseData(::io::InputStream& xmlStream)
{
    XMLControlRegistry xmlRegistry;
    xmlRegistry.addCreator<ComplexXMLControl>();

    auto pData = six::parseData(xmlRegistry, xmlStream, mpSchemaPaths, mLog, mPreserveCharacterData);
    return std::unique_ptr<ComplexData>(static_cast<ComplexData*>(pData.release()));
}

std::unique_ptr<six::sicd::ComplexData> six::sicd::DataParser::DataParser::parseData(const std::filesystem::path& pathname)
{
    io::FileInputStream inStream(pathname.string());
    return parseData(inStream);
}

std::unique_ptr<six::sicd::ComplexData> six::sicd::DataParser::DataParser::parseData(const std::u8string& xmlStr)
{
    io::U8StringStream inStream;
    inStream.write(xmlStr);
    return parseData(inStream);
}
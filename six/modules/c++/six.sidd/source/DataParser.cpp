/* =========================================================================
 * This file is part of six.sidd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 * (C) Copyright 2023, Maxar Technologies, Inc.
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
#include "six/sidd/DataParser.h"

#include <string>
#include <memory>
#include <set>

#include <io/StringStream.h>

#include "six/Utilities.h"
#include "six/XMLControlFactory.h"

#include "six/sidd/DerivedXMLControl.h"
#include "six/sidd/DerivedDataBuilder.h"

namespace fs = std::filesystem;

static void prependISMSchemaPaths(const std::vector<std::filesystem::path>*& pSchemaPaths,
    std::vector<std::filesystem::path>& adjustedSchemaPaths)
{
    if (pSchemaPaths == nullptr)
    {
        return;
    }

    // Get directories for XSDs that appear to be SIDD schemas
    const auto xsd_files = six::sidd300::find_SIDD_schema_V_files(*pSchemaPaths);
    std::set<std::string> xsd_dirs; // easy way to make directories unique
    for (auto&& xsd : xsd_files)
    {
        xsd_dirs.insert(xsd.parent_path().string());
    }
    for (const auto& dir : xsd_dirs)
    {
        adjustedSchemaPaths.push_back(dir);
    }

    // Include all the original schema paths; these will be AFTER the adjusted paths, above
    adjustedSchemaPaths.insert(adjustedSchemaPaths.end(), pSchemaPaths->begin(), pSchemaPaths->end());

    pSchemaPaths = &adjustedSchemaPaths;
}
static auto adjustSchemaPaths(const std::vector<std::filesystem::path>* pSchemaPaths, std::vector<std::filesystem::path>& adjustedSchemaPaths)
{
    prependISMSchemaPaths(pSchemaPaths, adjustedSchemaPaths);
    return pSchemaPaths;
}

six::sidd::DataParser::DataParser(const std::vector<std::filesystem::path>* pSchemaPaths, logging::Logger* pLog)
    : mDataParser(adjustSchemaPaths(pSchemaPaths, mAdjustedSchemaPaths), pLog)
{
    mDataParser.addCreator<DerivedXMLControl>();
}

std::unique_ptr<six::sidd::DerivedData> six::sidd::DataParser::DataParser::fromXML(::io::InputStream& xmlStream) const
{
    return mDataParser.fromXML<DerivedData>(xmlStream);
}

std::unique_ptr<six::sidd::DerivedData> six::sidd::DataParser::DataParser::fromXML(const std::filesystem::path& pathname) const
{
    io::FileInputStream inStream(pathname.string());
    return fromXML(inStream);
}

std::unique_ptr<six::sidd::DerivedData> six::sidd::DataParser::DataParser::fromXML(const std::u8string& xmlStr) const
{
    io::U8StringStream inStream;
    inStream.write(xmlStr);
    return fromXML(inStream);
}

std::u8string six::sidd::DataParser::DataParser::toXML(const six::sidd::DerivedData& data) const
{
    return mDataParser.toXML(data);
}

void six::sidd::DataParser::DataParser::preserveCharacterData(bool preserve)
{
    mDataParser.preserveCharacterData(preserve);
}
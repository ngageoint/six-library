/* =========================================================================
 * This file is part of six.sicd-c++
 * =========================================================================
 *
 * (C) Copyright 2021 Maxar Technologies, Inc.
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
#include "six/sicd/NITFReadComplexXMLControl.h"

#include <algorithm>
#include <iterator>

#include "six/sicd/ComplexXMLControl.h"
#include "six/sicd/Utilities.h"

namespace fs = std::filesystem;

six::sicd::NITFReadComplexXMLControl::NITFReadComplexXMLControl()
{
    xmlRegistry.addCreator<six::sicd::ComplexXMLControl>();
    reader.setXMLControlRegistry(&xmlRegistry);
}

void six::sicd::NITFReadComplexXMLControl::load(const std::string& fromFile,
    const std::vector<std::string>& schemaPaths)
{
    reader.load(fromFile, schemaPaths);
}
void six::sicd::NITFReadComplexXMLControl::load(const std::filesystem::path& fromFile,
    const std::vector<std::filesystem::path>& schemaPaths)
{
    std::vector<std::string> schemaPaths_;
    std::transform(schemaPaths.begin(), schemaPaths.end(), std::back_inserter(schemaPaths_), [](const fs::path& p) { return p.string(); });
    load(fromFile.string(), schemaPaths_);
}

const six::Container& six::sicd::NITFReadComplexXMLControl::getContainer() const
{
    return *(reader.getContainer());

}
six::Container& six::sicd::NITFReadComplexXMLControl::getContainer()
{
    return *(reader.getContainer());
}

void six::sicd::NITFReadComplexXMLControl::setXMLControlRegistry()
{
    // This tells the reader that it doesn't own an XMLControlRegistry
    reader.setXMLControlRegistry(nullptr);
}

std::unique_ptr<six::sicd::ComplexData> six::sicd::NITFReadComplexXMLControl::getComplexData()
{
    auto result = Utilities::getComplexData(reader);
    return std::unique_ptr< six::sicd::ComplexData>(result.release());
}

std::vector<std::complex<float>>  six::sicd::NITFReadComplexXMLControl::getWidebandData(const ComplexData& complexData)
{
    std::vector<std::complex<float>> retval;
    Utilities::getWidebandData(reader, complexData, retval);
    return retval;
}

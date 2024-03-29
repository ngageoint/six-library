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

#include <assert.h>

#include <std/memory>

#include <six/Enums.h>

#include <six/sidd/DerivedXMLControl.h>
#include <six/sidd/DerivedData.h>
#include <six/sidd/DerivedXMLParser100.h>
#include <six/sidd/DerivedXMLParser200.h>
#include <six/sidd/DerivedXMLParser300.h>

namespace
{
std::string normalizeVersion(const std::string& strVersion)
{
    std::vector<std::string> versionParts;
    six::XMLControl::splitVersion(strVersion, versionParts);
    if (versionParts.size() != 3)
    {
        throw except::Exception(
            Ctxt("Unsupported SIDD Version: " + strVersion));
    }

    #if _MSC_VER
    #pragma warning(push)
    #pragma warning(disable: 4365) // '...': conversion from '...' to '...', signed/unsigned mismatch
    #endif
    return str::join(versionParts, "");
    #if _MSC_VER
    #pragma warning(pop)
    #endif
}
}

namespace six
{
namespace sidd
{
    const six::DataType DerivedXMLControl::dataType = six::DataType::DERIVED;

DerivedXMLControl::DerivedXMLControl(logging::Logger* log, bool ownLog) : XMLControl(log, ownLog) { }
DerivedXMLControl::DerivedXMLControl(std::unique_ptr<logging::Logger>&& log) : XMLControl(std::move(log)) { }
DerivedXMLControl::DerivedXMLControl(logging::Logger& log) : XMLControl(log) { }

Data* DerivedXMLControl::fromXMLImpl(const xml::lite::Document* doc)
{
    assert(doc != nullptr);
    return fromXMLImpl(*doc).release();
}
std::unique_ptr<Data> DerivedXMLControl::fromXMLImpl(const xml::lite::Document& doc) const
{
    return getParser(getVersionFromURI(&doc))->fromXML(doc);
}

xml::lite::Document* DerivedXMLControl::toXMLImpl(const Data* data)
{
    assert(data != nullptr);
    return toXMLImpl(*data).release();
}
std::unique_ptr<xml::lite::Document> DerivedXMLControl::toXMLImpl(const Data& data) const
{
    if (data.getDataType() != DataType::DERIVED)
    {
        throw except::Exception(Ctxt("Data must be SIDD"));
    }

    auto parser = getParser(data.getVersion());
    return parser->toXML(dynamic_cast<const DerivedData&>(data));
}

std::unique_ptr<DerivedXMLParser>
DerivedXMLControl::getParser(const std::string& strVersion) const
{
    const std::string normalizedVersion = normalizeVersion(strVersion);

    // six.sidd only currently supports --
    //   SIDD 1.0.0
    //   SIDD 2.0.0
    //   SIDD 3.0.0
    if (normalizedVersion == "100")
    {
        return std::make_unique<DerivedXMLParser100>(mLog);
    }
    if (normalizedVersion == "200")
    {
        return std::make_unique<DerivedXMLParser200>(mLog);
    }
    if (normalizedVersion == "300")
    {
        return std::make_unique<DerivedXMLParser300>(getLogger());
    }

    if (normalizedVersion == "110")
    {
        throw except::Exception(Ctxt(
            "SIDD Version 1.1.0 does not exist. "
            "Did you mean 2.0.0 instead?"
        ));
    }

    throw except::Exception(Ctxt("Unsupported SIDD Version: " + strVersion));
}

std::unique_ptr<DerivedXMLParser> DerivedXMLControl::getParser_(const std::string& strVersion)
{
    return DerivedXMLControl().getParser(strVersion);
}


}
}

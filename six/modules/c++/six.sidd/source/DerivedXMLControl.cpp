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
#include <stdexcept>
#include <tuple>
#include <algorithm>

#include <sys/OS.h>

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

DerivedXMLControl::DerivedXMLControl(logging::Logger* log, bool ownLog) : XMLControl(log, ownLog) {}
DerivedXMLControl::DerivedXMLControl(std::unique_ptr<logging::Logger>&& log) : XMLControl(std::move(log)) {}
DerivedXMLControl::DerivedXMLControl(logging::Logger& log) : XMLControl(log) {}

Data* DerivedXMLControl::fromXMLImpl(const xml::lite::Document* doc)
{
    assert(doc != nullptr);
    return fromXMLImpl(*doc).release();
}
std::unique_ptr<Data> DerivedXMLControl::fromXMLImpl(const xml::lite::Document& doc) const
{
    return getParser(getVersionFromURI(&doc))->fromXML(doc);
}

// Is this SIDD 3.0 XML?
static bool has_sidd300_attribute(const xml::lite::Element& rootElement)
{
    static const xml::lite::Uri sidd300("urn:SIDD:3.0.0");
    const auto predicate = [&](const auto& attribute) {
        const xml::lite::Uri uriValue(attribute.getValue());
        return (uriValue == sidd300) && (attribute.getLocalName() == "xmlns");
    };
    auto&& attributes = rootElement.getAttributes();
    return std::any_of(attributes.begin(), attributes.end(), predicate);
}

static const xml::lite::Uri xmlns("http://www.w3.org/2000/xmlns/");

// Return the ISM Uri, if any
static const xml::lite::Uri ism_201609("urn:us:gov:ic:ism:201609");
static const xml::lite::Uri ism_13("urn:us:gov:ic:ism:13");
static auto has_ism_attribute(const xml::lite::Element& rootElement)
{
    xml::lite::Uri retval;
    const auto predicate = [&](const auto& attribute) {
        xml::lite::Uri uri;
        attribute.getUri(uri);
        if (uri != xmlns)
            return false;

        const xml::lite::Uri uriValue(attribute.getValue());
        if ((uriValue == ism_201609) || (uriValue == ism_13))
        {
            retval = uriValue;
            return true;
        }
        return false;
    };

    auto&& attributes = rootElement.getAttributes();
    std::ignore = std::any_of(attributes.begin(), attributes.end(), predicate); // using `retval`, not the result of any_of()
    return retval;
}

static auto get_SIDD300_schema_path(const xml::lite::Document& doc, const std::vector<std::filesystem::path>& schemaPaths)
{
    using retval_t = std::filesystem::path;

    auto&& rootElement = getRootElement(doc);
    if (!has_sidd300_attribute(rootElement))
    {
        return retval_t{};
    }

    const auto xml_ism = has_ism_attribute(rootElement);
    if (xml_ism.empty())
    {
        return retval_t{};
    }

    const auto xsd_files = xml::lite::Validator::loadSchemas(schemaPaths);
    for (auto&& xsd : xsd_files)
    {
        io::FileInputStream xsdStream(xsd);
        six::MinidomParser xsdParser;
        xsdParser.parse(xsdStream);

        const auto& doc = getDocument(xsdParser);
        const auto& root = getRootElement(doc);
        if (!has_sidd300_attribute(root))
        {
            continue;
        }

        const auto predicate = [&](const auto& attribute) {
            xml::lite::Uri uri;
            attribute.getUri(uri);
            if (uri != xmlns)
                return false;

            const xml::lite::Uri uriValue(attribute.getValue());
            return (uriValue == xml_ism) && (attribute.getLocalName() == "ism");
        };

        auto&& attributes = root.getAttributes();
        if (std::any_of(attributes.begin(), attributes.end(), predicate))
        {
            return xsd;
        }
    }

    return retval_t{};
}


std::unique_ptr<Data> DerivedXMLControl::validateXMLImpl(const xml::lite::Document& doc,
    const std::vector<std::filesystem::path>& schemaPaths, logging::Logger& log) const
{
    const auto path = get_SIDD300_schema_path(doc, schemaPaths);

    return validateXMLImpl_(doc, schemaPaths, log);
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
        const auto ismVersion = six::sidd300::get(six::sidd300::ISMVersion::current);
        return std::make_unique<DerivedXMLParser300>(getLogger(), ismVersion);
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

// Percolating ISMVersion everywhere is a nusiance as several APIs will be touched, even if it is
// to add a default parameter.  Doing that doesn't make much sense for an arbitrary
// `XMLControl` class (the base class for `DerivedXMLControl`)), and even here (in SIDD) it
// only makes sense for  SIDD 3.0. Yes, this is a bit messy too; but it stops (or slows)
// ISMVersion from spreading too much.
//
// Try to make this easy for clients by providing several ways to access this functionality ...
// while trying not to make things messier than they already are.

static std::optional<six::sidd300::ISMVersion> getISMVersionFromEnv()
{
    static const std::string envName = "SIX_SIDD300_ISM_VERSION"; // set to `201609` or `13`
    static const sys::OS os;

    // Don't cache this result; it could change while running.
    if (os.isEnvSet(envName))
    {
        const auto result = os.getEnv(envName);
        if (result == "13")
        {
            return six::sidd300::ISMVersion::v13;
        }
        if (result == "201609")
        {
            return six::sidd300::ISMVersion::v201609;
        }
    }

    return std::optional<six::sidd300::ISMVersion>{};
}

static std::optional<six::sidd300::ISMVersion> s_setISMVersion;
std::optional<six::sidd300::ISMVersion> six::sidd300::getISMVersion()
{
    // Try the environment variable first as that can be managed outside of C++
    const auto ismVersionFromEnv = getISMVersionFromEnv();
    if (ismVersionFromEnv.has_value())
    {
        return *ismVersionFromEnv;
    }

    // Then our global (static) variable; normally this won't be set
    return s_setISMVersion;
}
six::sidd300::ISMVersion six::sidd300::get(ISMVersion defaultIfNotSet)
{
    // Try getting something from the runtime enviroment ...
    const auto ismVersion = getISMVersion();
    if (ismVersion.has_value())
    {
        return *ismVersion;
    }

    // ... nothing; use the the default value
    return defaultIfNotSet;
}
std::optional<six::sidd300::ISMVersion> six::sidd300::set(ISMVersion value) // returns previous value, if any
{
    auto retval = s_setISMVersion;
    s_setISMVersion = value;
    return retval;
}
std::optional<six::sidd300::ISMVersion> six::sidd300::clearISMVersion() // returns previous value, if any
{
    auto retval = s_setISMVersion;
    s_setISMVersion.reset();
    return retval;
}

std::string six::sidd300::to_string(ISMVersion value)
{
    switch (value)
    {
    case ISMVersion::v201609: return "v201609";
    case ISMVersion::v13: return "v13";
    default: break;
    }
    throw std::invalid_argument("Unknown 'ISMVersion' value.");
};
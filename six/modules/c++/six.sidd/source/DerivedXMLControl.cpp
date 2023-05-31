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
six::sidd::Version normalizeVersion(const std::string& strVersion)
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
    const auto normalizedVersion = str::join(versionParts, "");
    #if _MSC_VER
    #pragma warning(pop)
    #endif

    // six.sidd only currently supports --
    //   SIDD 1.0.0
    //   SIDD 2.0.0
    //   SIDD 3.0.0
    if (normalizedVersion == "100")
    {
        return six::sidd::Version::v100;
    }
    if (normalizedVersion == "200")
    {
        return six::sidd::Version::v200;
    }
    if (normalizedVersion == "300")
    {
        return six::sidd::Version::v300;
    }

    if (normalizedVersion == "110")
    {
        throw except::Exception(Ctxt(
            "SIDD Version 1.1.0 does not exist. Did you mean 2.0.0 instead?"
        ));
    }

    throw except::Exception(Ctxt("Unsupported SIDD Version: " + strVersion));
}
}

namespace six
{
namespace sidd
{
    std::string to_string(Version version)
    {
        switch (version)
        {
        case Version::v100: return "v100";
        case Version::v200: return "v200";
        case Version::v300: return "v300";
        default: break;
        }
        throw std::logic_error("Unkown 'Version' value.");
    }

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
    const auto version = normalizeVersion(getVersionFromURI(&doc));
    return getParser(version)->fromXML(doc);
}

// Is this SIDD 3.0 XML?
static bool has_sidd300_attribute(const xml::lite::Element& element)
{
    static const xml::lite::Uri sidd300("urn:SIDD:3.0.0");
    const auto predicate = [&](const auto& attribute) {
        const xml::lite::Uri uriValue(attribute.getValue());
        return (uriValue == sidd300) && (attribute.getLocalName() == "xmlns");
    };
    auto&& attributes = element.getAttributes();
    return std::any_of(attributes.begin(), attributes.end(), predicate);
}

static bool is_sidd300(const xml::lite::Document& doc)
{
    auto&& rootElement = getRootElement(doc);

    // In the XML: <SIDD xmlns="urn:SIDD:3.0.0" ... >
    if (rootElement.getLocalName() != "SIDD")
    {
        return false;
    }

    return has_sidd300_attribute(rootElement);
}

// Return the ISM Uri, if any
static auto has_ism_attribute(const xml::lite::Element& element)
{
    static const xml::lite::Uri xmlns("http://www.w3.org/2000/xmlns/");
    static const xml::lite::Uri ism_201609("urn:us:gov:ic:ism:201609");
    static const xml::lite::Uri ism_13("urn:us:gov:ic:ism:13");

    // In the XML (SIDD or XSD): <... xmlns:ism="urn:us:gov:ic:ism:201609" ...>
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

    auto&& attributes = element.getAttributes();
    std::ignore = std::any_of(attributes.begin(), attributes.end(), predicate); // using `retval`, not the result of any_of()
    return retval;
}

// Is this the XSD for the ISM of interest?
static auto xsd_has_ism(const std::filesystem::path& xsd, const xml::lite::Uri& xml_ism)
{
    if (xml_ism.empty())
    {
        throw std::invalid_argument("'xml_ism' is empty()");
    }

    io::FileInputStream xsdStream(xsd);
    six::MinidomParser xsdParser;
    xsdParser.parse(xsdStream);
    auto&& rootElement = getRootElement(getDocument(xsdParser));

    // In the XSD: <xs:schema xmlns:xs="http://www.w3.org/2001/XMLSchema" ... xmlns="urn:SIDD:3.0.0" ... >
    if (!has_sidd300_attribute(rootElement))
    {
        return false;
    }

    // In the XSD: <xs:schema xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:ism="urn:us:gov:ic:ism:201609" ...>
    const auto uriValue = has_ism_attribute(rootElement);
    return uriValue == xml_ism;
}

static auto find_SIDD_xsd_files(const std::vector<std::filesystem::path>& xsdFiles)
{
    // Parsing each XSD can be time-consuming; use a few heuristics to try to minmize.
    std::vector<std::filesystem::path> retval;
    for (auto&& xsd : xsdFiles)
    {
        static const std::string SIDD_schema_prefix = "SIDD_schema_V"; // e.g., "SIDD_schema_V3.0.0"
        if (str::starts_with(xsd.stem().string(), SIDD_schema_prefix))
        {
            // We could remove this entry from `xsdFiles` too, but 1) it's an added complication, and
            // 2) make this routine less thread-friendly. <shrug>
            retval.push_back(xsd);
        }
    }
    return retval;
}

static auto get_SIX_SIDD300_schema_dir()
{
    // If this enviroment variable is set, assume the caller as worked everything out.
    static const std::string envName = "SIX_SIDD300_SCHEMA_DIR"; // a single directory, not a search path

    static const sys::OS os;
    std::string result;
    os.getEnvIfSet(envName, result); // Don't cache this result; it could change while running.
    return std::filesystem::path(result);
}

// Try to find a XSD which can validate this XML; this is needed for SIDD 3.0
// because we have to support two versions of ISM.
static auto find_xsd_path(const xml::lite::Element& rootElement, const std::vector<std::filesystem::path>& schemaPaths)
{
    // In the XML: <SIDD xmlns="urn:SIDD:3.0.0" ... xmlns:ism="urn:us:gov:ic:ism:201609">
    const auto xml_ism = has_ism_attribute(rootElement);
    if (xml_ism.empty())
    {
        return std::filesystem::path{};
    }

    // Process the schemaPaths one at a time to avoid traversing so many directories at once.
    for (auto&& schemaPath : schemaPaths)
    {
        const std::vector<std::filesystem::path> schemaPaths_{ schemaPath }; // use one path at a time
        const auto xsd_files = xml::lite::Validator::loadSchemas(schemaPaths_);
        
        // Try anything that looks like a SIDD schema first; this is to avoid
        // loading XSDs that "obviously" won't work.
        const auto sidd_xsd_files = find_SIDD_xsd_files(xsd_files);
        for (auto&& xsd : sidd_xsd_files)
        {
            if (xsd_has_ism(xsd, xml_ism))
            {
                return xsd;
            }
        }

        // Yeah, this will do some of the above over again ... does it matter?
        for (auto&& xsd : xsd_files)
        {
            if (xsd_has_ism(xsd, xml_ism))
            {
                return xsd;
            }
        }
    }

    return std::filesystem::path{};
}

std::unique_ptr<Data> DerivedXMLControl::validateXMLImpl(const xml::lite::Document& doc,
    const std::vector<std::filesystem::path>& schemaPaths_, logging::Logger& log) const
{
    auto schemaPaths = schemaPaths_;

    // If this enviroment variable is set, assume the caller as worked everything out ...
    auto result = get_SIX_SIDD300_schema_dir();
    if (!result.empty() && is_sidd300(doc))
    {
        // ... but only for SIDD 3.0 XML
        schemaPaths.clear();
        schemaPaths.emplace_back(std::move(result));
        return validateXMLImpl_(doc, schemaPaths, log);
    }

    // Otherwise (i.e., not very special SIDD 3.0 case, above), try to find the XSD which will vallidate this XML.
    // This is needed because downstream code finds all the XSDs in the schemaPaths; that normally wouldn't
    // be a problem but there are now two SIDD 3.0 XSDs: one for ISM-v13 and another for ISM-v201609.
    // Both claim to be SIDD 3.0, but that "can't" be the case; by finding a corresponding XSD up-front
    // errors from validateXMLImpl_() are (hopefully) eliminated (other than real validation errors, of course).
    const auto path = find_xsd_path(getRootElement(doc), schemaPaths_);
    if (!path.empty())
    {
        // We now know this is a good path, put it at the beginning of the search path so that
        // subsequent validation will use it first.
        schemaPaths.insert(schemaPaths.begin(), path.parent_path());
    }
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

    const auto version = normalizeVersion(data.getVersion());
    auto parser = getParser(version);
    return parser->toXML(dynamic_cast<const DerivedData&>(data));
}

std::unique_ptr<DerivedXMLParser>
DerivedXMLControl::getParser(Version normalizedVersion/*, std::optional<six::sidd300::ISMVersion> ismVersion*/) const
{   
    // six.sidd only currently supports --
    //   SIDD 1.0.0
    //   SIDD 2.0.0
    //   SIDD 3.0.0
    if (normalizedVersion == Version::v100)
    {
        return std::make_unique<DerivedXMLParser100>(mLog);
    }
    if (normalizedVersion == Version::v200)
    {
        return std::make_unique<DerivedXMLParser200>(mLog);
    }
    if (normalizedVersion == Version::v300)
    {
        //if (!ismVersion.has_value())
        //{
        //    throw except::Exception(Ctxt("Must specify ISMVersion for SIDD 3.0.0"));
        //}
        const auto ismVersion = six::sidd300::get(six::sidd300::ISMVersion::current);
        return std::make_unique<DerivedXMLParser300>(getLogger(), ismVersion);
    }

    throw except::Exception(Ctxt("Unsupported SIDD Version: " + to_string(normalizedVersion)));
}

std::unique_ptr<DerivedXMLParser> DerivedXMLControl::getParser_(const std::string& strVersion)
{
    const auto version = normalizeVersion(strVersion);
    return DerivedXMLControl().getParser(version);
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
    std::string result;
    if (os.getEnvIfSet(envName, result)) // Don't cache this result; it could change while running.
    {
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

// Find all the XSDs that look like they may be SIDD schemas, e.g., SIDD_schema_V3.0.0.xsd
std::vector<std::filesystem::path> six::sidd300::find_SIDD_schema_V_files(const std::vector<std::filesystem::path>& schemaPaths_)
{
    auto schemaPaths = schemaPaths_;
    // If this enviroment variable is set, assume the caller as worked everything out ...
    auto result = six::sidd::get_SIX_SIDD300_schema_dir();
    if (!result.empty())
    {
        schemaPaths.clear();
        schemaPaths.push_back(std::move(result));
    }

    const auto xsd_files = xml::lite::Validator::loadSchemas(schemaPaths);
    return six::sidd::find_SIDD_xsd_files(xsd_files);
}

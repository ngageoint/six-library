/* =========================================================================
 * This file is part of cphd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2019, MDA Information Systems LLC
 *
 * cphd-c++ is free software; you can redistribute it and/or modify
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
#include <cphd/CPHDXMLControl.h>

#include <set>
#include <unordered_map>
#include <algorithm>
#include <std/memory>
#include <iterator>

#include <io/StringStream.h>
#include <logging/NullLogger.h>
#include <xml/lite/MinidomParser.h>
#include <str/EncodedStringView.h>

#include <six/XMLControl.h>
#include <six/XmlLite.h>
#include <cphd/CPHDXMLParser.h>
#include <cphd/Enums.h>
#include <cphd/Metadata.h>
#include <cphd/Types.h>


#define ENFORCESPEC 0

namespace cphd
{

CPHDXMLControl::CPHDXMLControl(logging::Logger* log, bool ownLog) :
    mLogger(mLog, mOwnLog, nullptr)
{
    setLogger(log, ownLog);
}

/* TO XML */
std::u8string CPHDXMLControl::toXMLString(
    const Metadata& metadata,
    const std::vector<std::filesystem::path>* pSchemaPaths,
    bool prettyPrint)
{
    std::vector<std::string> schemaPaths;
    if (pSchemaPaths != nullptr)
    {
        std::transform(pSchemaPaths->begin(), pSchemaPaths->end(), std::back_inserter(schemaPaths),
            [](const std::filesystem::path& p) { return p.string(); });
    }

    std::unique_ptr<xml::lite::Document> doc(toXML(metadata, schemaPaths));
    io::U8StringStream ss;
    (prettyPrint) ?
        doc->getRootElement()->prettyPrint(ss) :
        doc->getRootElement()->print(ss);
    return ss.stream().str();
}
std::string CPHDXMLControl::toXMLString(
        const Metadata& metadata,
        const std::vector<std::string>& schemaPaths_,
        bool prettyPrint)
{
    std::vector<std::filesystem::path> schemaPaths;
    std::transform(schemaPaths_.begin(), schemaPaths_.end(), std::back_inserter(schemaPaths),
        [](const std::string& s) { return s; });

    const auto result = toXMLString(metadata, &schemaPaths, prettyPrint);
    return str::EncodedStringView(result).native();
}

mem::auto_ptr<xml::lite::Document> CPHDXMLControl::toXML(
        const Metadata& metadata,
        const std::vector<std::string>& schemaPaths)
{
    mem::auto_ptr<xml::lite::Document> doc(toXMLImpl(metadata).release());
    if(!schemaPaths.empty())
    {
        six::XMLControl::validate(doc.get(), schemaPaths, mLog);
    }
    return doc;
}

std::unordered_map<std::string, xml::lite::Uri> CPHDXMLControl::getVersionUriMap()
{
    return {
        {"1.0.0", xml::lite::Uri("urn:CPHD:1.0.0")},
        {"1.0.1", xml::lite::Uri("http://api.nsgreg.nga.mil/schema/cphd/1.0.1")},
        {"1.1.0", xml::lite::Uri("http://api.nsgreg.nga.mil/schema/cphd/1.1.0")}
    };
}

std::unique_ptr<xml::lite::Document> CPHDXMLControl::toXMLImpl(const Metadata& metadata)
{
    const auto versionUriMap = getVersionUriMap();
    if (versionUriMap.find(metadata.getVersion()) != versionUriMap.end())
    {
      return getParser(versionUriMap.find(metadata.getVersion())->second)->toXML(metadata);
    }
    std::ostringstream ostr;
    ostr << "The version " << metadata.getVersion() << " is invalid. "
         << "Check if version is valid or "
         << "add a <version, URI> entry to versionUriMap";
    throw except::Exception(Ctxt(ostr.str()));
}

/* FROM XML */
std::unique_ptr<Metadata> CPHDXMLControl::fromXML(const std::string& xmlString,
                                     const std::vector<std::string>& schemaPaths_)
{
    std::vector<std::filesystem::path> schemaPaths;
    std::transform(schemaPaths_.begin(), schemaPaths_.end(), std::back_inserter(schemaPaths),
        [](const std::string& s) { return s; });

    return fromXML(str::EncodedStringView(xmlString).u8string(), schemaPaths);
}
std::unique_ptr<Metadata> CPHDXMLControl::fromXML(const std::u8string& xmlString,
    const std::vector<std::filesystem::path>& schemaPaths)
{
    io::U8StringStream stringStream;
    stringStream.write(xmlString);
    six::MinidomParser parser;
    parser.parse(stringStream);
    auto result = fromXML(parser.getDocument(), schemaPaths);
    return std::make_unique<Metadata>(std::move(result));
}

std::unique_ptr<Metadata> CPHDXMLControl::fromXML(const xml::lite::Document* doc,
                                     const std::vector<std::string>& schemaPaths)
{
    if(!schemaPaths.empty())
    {
        six::XMLControl::validate(doc, schemaPaths, mLog);
    }
    std::unique_ptr<Metadata> metadata = fromXMLImpl(doc);
    const xml::lite::Uri uri(doc->getRootElement()->getUri());
    metadata->setVersion(uriToVersion(uri));
    return metadata;
}
Metadata CPHDXMLControl::fromXML(const xml::lite::Document& doc, const std::vector<std::filesystem::path>& schemaPaths)
{
    std::vector<std::string> schemaPaths_;
    std::transform(schemaPaths.begin(), schemaPaths.end(), std::back_inserter(schemaPaths_),
        [](const std::filesystem::path& p) { return p.string(); });
    auto result = fromXML(&doc, schemaPaths_);
    return *(result.release());
}

std::unique_ptr<Metadata> CPHDXMLControl::fromXMLImpl(const xml::lite::Document* doc)
{
    const xml::lite::Uri uri(doc->getRootElement()->getUri());
    return getParser(uri)->fromXML(doc);
}

std::unique_ptr<CPHDXMLParser>
CPHDXMLControl::getParser(const xml::lite::Uri& uri) const
{
    return std::make_unique<CPHDXMLParser>(uri.value, false, mLog);
}

std::string CPHDXMLControl::uriToVersion(const xml::lite::Uri& uri) const
{
    const auto versionUriMap = getVersionUriMap();
    for (const auto& p : versionUriMap)
    {
        if (p.second == uri)
        {
            return p.first;
        }
    }
    std::ostringstream ostr;
    ostr << "The URI " << uri << " is invalid. "
         << "Either input a valid URI or "
         << "add a <version, URI> entry to versionUriMap";
    throw except::Exception(Ctxt(ostr.str()));
}

}

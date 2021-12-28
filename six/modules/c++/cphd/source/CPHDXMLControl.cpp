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

#include <io/StringStream.h>
#include <logging/NullLogger.h>
#include <xml/lite/MinidomParser.h>

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
std::string CPHDXMLControl::toXMLString(
        const Metadata& metadata,
        const std::vector<std::string>& schemaPaths,
        bool prettyPrint)
{
    std::unique_ptr<six::xml_lite::Document> doc(toXML(metadata, schemaPaths));
    io::StringStream ss;
    (prettyPrint) ?
            doc->getRootElement()->prettyPrint(ss) :
            doc->getRootElement()->print(ss);
    return ss.stream().str();
}

mem::auto_ptr<six::xml_lite::Document> CPHDXMLControl::toXML(
        const Metadata& metadata,
        const std::vector<std::string>& schemaPaths)
{
    mem::auto_ptr<six::xml_lite::Document> doc(toXMLImpl(metadata).release());
    if(!schemaPaths.empty())
    {
        six::XMLControl::validate(doc.get(), schemaPaths, mLog);
    }
    return doc;
}

std::unordered_map<std::string, six::xml_lite::Uri> CPHDXMLControl::getVersionUriMap()
{
    return {
        {"1.0.0", six::xml_lite::Uri("urn:CPHD:1.0.0")},
        {"1.0.1", six::xml_lite::Uri("http://api.nsgreg.nga.mil/schema/cphd/1.0.1")}
    };
}

std::unique_ptr<six::xml_lite::Document> CPHDXMLControl::toXMLImpl(const Metadata& metadata)
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
                                     const std::vector<std::string>& schemaPaths)
{
    io::StringStream stringStream;
    stringStream.write(xmlString);
    six::MinidomParser parser;
    parser.parse(stringStream);
    return fromXML(&parser.getDocument(), schemaPaths);
}

std::unique_ptr<Metadata> CPHDXMLControl::fromXML(const six::xml_lite::Document* doc,
                                     const std::vector<std::string>& schemaPaths)
{
    if(!schemaPaths.empty())
    {
        six::XMLControl::validate(doc, schemaPaths, mLog);
    }
    std::unique_ptr<Metadata> metadata = fromXMLImpl(doc);
    metadata->setVersion(uriToVersion(doc->getRootElement()->getUri()));
    return metadata;
}
Metadata CPHDXMLControl::fromXML(const six::xml_lite::Document& doc, const std::vector<std::filesystem::path>& schemaPaths)
{
    std::vector<std::string> schemaPaths_;
    std::transform(schemaPaths.begin(), schemaPaths.end(), std::back_inserter(schemaPaths_),
        [](const std::filesystem::path& p) { return p.string(); });
    auto result = fromXML(&doc, schemaPaths_);
    return *(result.release());
}

std::unique_ptr<Metadata> CPHDXMLControl::fromXMLImpl(const six::xml_lite::Document* doc)
{
  return getParser(doc->getRootElement()->getUri())->fromXML(doc);
}

std::unique_ptr<CPHDXMLParser>
CPHDXMLControl::getParser(const six::xml_lite::Uri& uri) const
{
    return std::make_unique<CPHDXMLParser>(uri.value, false, mLog);
}

std::string CPHDXMLControl::uriToVersion(const six::xml_lite::Uri& uri) const
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

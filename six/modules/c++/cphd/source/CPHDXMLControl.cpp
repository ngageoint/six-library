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
#include <set>
#include <unordered_map>
#include <io/StringStream.h>
#include <logging/NullLogger.h>
#include <xml/lite/MinidomParser.h>

#include <six/XMLControl.h>
#include <cphd/CPHDXMLControl.h>
#include <cphd/CPHDXMLParser.h>
#include <cphd/Enums.h>
#include <cphd/Metadata.h>
#include <cphd/Types.h>


#define ENFORCESPEC 0

namespace cphd
{

CPHDXMLControl::CPHDXMLControl(logging::Logger* log, bool ownLog) :
    mLog(NULL),
    mOwnLog(false)
{
    setLogger(log, ownLog);
}

CPHDXMLControl::~CPHDXMLControl()
{
    if (mLog && mOwnLog)
    {
        delete mLog;
    }
}

void CPHDXMLControl::setLogger(logging::Logger* log, bool own)
{
    if (mLog && mOwnLog && log != mLog)
    {
        delete mLog;
        mLog = NULL;
    }

    if (log)
    {
        mLog = log;
        mOwnLog = own;
    }
    else
    {
        mLog = new logging::NullLogger;
        mOwnLog = true;
    }
}

/* TO XML */
std::string CPHDXMLControl::toXMLString(
        const Metadata& metadata,
        const std::vector<std::string>& schemaPaths,
        bool prettyPrint)
{
    std::unique_ptr<xml::lite::Document> doc(toXML(nullptr, metadata, schemaPaths));
    io::StringStream ss;
    (prettyPrint) ?
            doc->getRootElement()->prettyPrint(ss) :
            doc->getRootElement()->print(ss);
    return ss.stream().str();
}

std::auto_ptr<xml::lite::Document> CPHDXMLControl::toXML(
        const Metadata& metadata,
        const std::vector<std::string>& schemaPaths)
{
  auto result = toXML(nullptr, metadata, schemaPaths);
  return std::auto_ptr<xml::lite::Document>(result.release());
}
std::unique_ptr<xml::lite::Document> CPHDXMLControl::toXML(std::nullptr_t,
        const Metadata& metadata,
        const std::vector<std::string>& schemaPaths)
{
    std::unique_ptr<xml::lite::Document> doc = toXMLImpl(nullptr, metadata);
    if(!schemaPaths.empty())
    {
        six::XMLControl::validate(doc.get(), schemaPaths, mLog);
    }
    return doc;
}

std::unordered_map<std::string, std::string> CPHDXMLControl::getVersionUriMap()
{
    return {
        {"1.0.0", "urn:CPHD:1.0.0"},
        {"1.0.1", "http://api.nsgreg.nga.mil/schema/cphd/1.0.1"}
    };
}

std::auto_ptr<xml::lite::Document> CPHDXMLControl::toXMLImpl(const Metadata& metadata)
{
  auto result = toXMLImpl(nullptr, metadata);
  return std::auto_ptr<xml::lite::Document>(result.release());
}
std::unique_ptr<xml::lite::Document> CPHDXMLControl::toXMLImpl(std::nullptr_t, const Metadata& metadata)
{
    const auto versionUriMap = getVersionUriMap();
    if (versionUriMap.find(metadata.getVersion()) != versionUriMap.end())
    {
      return getParser(nullptr, versionUriMap.find(metadata.getVersion())->second)->toXML(nullptr, metadata);
    }
    std::ostringstream ostr;
    ostr << "The version " << metadata.getVersion() << " is invalid. "
         << "Check if version is valid or "
         << "add a <version, URI> entry to versionUriMap";
    throw except::Exception(Ctxt(ostr.str()));
}

/* FROM XML */
std::auto_ptr<Metadata> CPHDXMLControl::fromXML(const std::string& xmlString,
                                     const std::vector<std::string>& schemaPaths)
{
  auto result = fromXML(nullptr, xmlString, schemaPaths);
  return std::auto_ptr<Metadata>(result.release());
}
std::unique_ptr<Metadata> CPHDXMLControl::fromXML(std::nullptr_t, const std::string& xmlString,
                                     const std::vector<std::string>& schemaPaths)
{
    io::StringStream stringStream;
    stringStream.write(xmlString.c_str(), xmlString.size());
    xml::lite::MinidomParser parser;
    parser.parse(stringStream);
    return fromXML(nullptr, parser.getDocument(), schemaPaths);
}

std::auto_ptr<Metadata> CPHDXMLControl::fromXML(const xml::lite::Document* doc,
                                     const std::vector<std::string>& schemaPaths)
{
  auto result = fromXML(nullptr, doc, schemaPaths);
  return std::auto_ptr<Metadata>(result.release());
}
std::unique_ptr<Metadata> CPHDXMLControl::fromXML(std::nullptr_t, const xml::lite::Document* doc,
                                     const std::vector<std::string>& schemaPaths)
{
    if(!schemaPaths.empty())
    {
        six::XMLControl::validate(doc, schemaPaths, mLog);
    }
    std::unique_ptr<Metadata> metadata = fromXMLImpl(nullptr, doc);
    metadata->setVersion(uriToVersion(doc->getRootElement()->getUri()));
    return metadata;
}

std::auto_ptr<Metadata> CPHDXMLControl::fromXMLImpl(const xml::lite::Document* doc)
{
  auto result = fromXMLImpl(nullptr, doc);
  return std::auto_ptr<Metadata>(result.release());
}
std::unique_ptr<Metadata> CPHDXMLControl::fromXMLImpl(std::nullptr_t, const xml::lite::Document* doc)
{
  return getParser(nullptr, doc->getRootElement()->getUri())->fromXML(nullptr, doc);
}

std::unique_ptr<CPHDXMLParser>
CPHDXMLControl::getParser(std::nullptr_t, const std::string& uri) const
{
    std::unique_ptr<CPHDXMLParser> parser;
    parser.reset(new CPHDXMLParser(uri, false, mLog));
    return parser;
}

std::string CPHDXMLControl::uriToVersion(const std::string& uri) const
{
    const auto versionUriMap = getVersionUriMap();
    for (auto it = versionUriMap.begin(); it != versionUriMap.end(); ++it)
    {
        if (it->second == uri)
        {
            return it->first;
        }
    }
    std::ostringstream ostr;
    ostr << "The URI " << uri << " is invalid. "
         << "Either input a valid URI or "
         << "add a <version, URI> entry to versionUriMap";
    throw except::Exception(Ctxt(ostr.str()));
}

}

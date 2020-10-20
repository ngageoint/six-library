/* =========================================================================
 * This file is part of six-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * six-c++ is free software; you can redistribute it and/or modify
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

#include <io/OutputStream.h>

#include "six/XMLControlFactory.h"
#include <str/Convert.h>
#include <logging/NullLogger.h>

using namespace six;

XMLControlRegistry::~XMLControlRegistry()
{
    for (RegistryMap::const_iterator iter = mRegistry.begin();
         iter != mRegistry.end();
         ++iter)
    {
        delete iter->second;
    }
}

void XMLControlRegistry::addCreator(const std::string& identifier,
                                    std::unique_ptr<XMLControlCreator>&& creator)
{
    const RegistryMap::iterator iter(mRegistry.lower_bound(identifier));
    if (iter == mRegistry.end() || iter->first != identifier)
    {
        // Don't have an entry for this identifier yet
        mRegistry.insert(iter,
                         RegistryMap::value_type(identifier, creator.get()));
    }
    else if (iter->second != creator.get())
    {
        // We already have an entry for this identifier - overwrite it
        delete iter->second;
        iter->second = creator.get();
    }

    // At this point we've taken ownership
    creator.release();
}

XMLControl*
XMLControlRegistry::newXMLControl(const std::string& identifier,
                                  logging::Logger* log) const
{
    RegistryMap::const_iterator const iter = mRegistry.find(identifier);
    if (iter == mRegistry.end())
    {
        throw except::NoSuchKeyException(Ctxt("No data class creator " +
                                              identifier));
    }
    return iter->second->newXMLControl(log);
}

std::string six::toXMLString(const Data* data,
                             const six::XMLControlRegistry *xmlRegistry)
{
    std::unique_ptr<logging::Logger> log (new logging::NullLogger());
    return toValidXMLString(data, std::vector<std::string>(),
                            log.get(), xmlRegistry);
}

static const io::TextEncoding* getEncoding(const Data& data)
{
    // If we passed "true" to xml::lite::MinidomParser::preserveCharacterData(),
    // then we want encoding to be UTF-8 so preserve any UTF-8 characers in
    // the input.  Note that if preserveCharacterData()==false, an exceptiion
    // will be thrown during parsing if there is UTF-8 data in the XML.
    if (data.getPreserveCharacterData())
    {
        static const auto utf8 = io::TextEncoding::Utf8;
        return &utf8;
    }

    return nullptr;
}

std::string six::toValidXMLString(const Data* data,
                                  const std::vector<std::string>& schemaPaths,
                                  logging::Logger* log,
                                  const six::XMLControlRegistry *xmlRegistry)
{
    if (!xmlRegistry)
    {
        xmlRegistry = &XMLControlFactory::getInstance();
    }

    const std::unique_ptr<XMLControl>
        xmlControl(xmlRegistry->newXMLControl(data->getDataType(), log));

    // this will validate if SIX_SCHEMA_PATH EnvVar is set
    const std::unique_ptr<xml::lite::Document> doc(
        xmlControl->toXML(data, schemaPaths));

    io::StringStream oss;
    doc->getRootElement()->print(oss, getEncoding(*data));

    return oss.stream().str();
}


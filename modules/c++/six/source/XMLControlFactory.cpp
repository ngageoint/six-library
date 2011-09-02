/* =========================================================================
 * This file is part of six-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
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

#include <str/Convert.h>

#include "six/XMLControlFactory.h"

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
                                    std::auto_ptr<XMLControlCreator> creator)
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
XMLControlRegistry::newXMLControl(const std::string& identifier) const
{
    RegistryMap::const_iterator const iter = mRegistry.find(identifier);
    if (iter == mRegistry.end())
    {
        throw except::NoSuchKeyException(Ctxt("No data class creator " +
                                              identifier));
    }
    return iter->second->newXMLControl();
}

std::string XMLControlRegistry::dataTypeToString(DataType dataType)
{
    switch (dataType)
    {
    case DataType::COMPLEX:
        return "SICD_XML";
    case DataType::DERIVED:
        return "SIDD_XML";
    default:
        throw except::Exception(Ctxt("Invalid data type " +
                                         str::toString(dataType)));
    }
}

char* six::toXMLCharArray(const Data* data,
                          const six::XMLControlRegistry *xmlRegistry)
{
    const std::string xml = toXMLString(data, xmlRegistry);
    char* const raw = new char[xml.length() + 1];
    strcpy(raw, xml.c_str());
    return raw;
}

std::string six::toXMLString(const Data* data,
                             const six::XMLControlRegistry *xmlRegistry)
{
    if (!xmlRegistry)
    {
        xmlRegistry = &XMLControlFactory::getInstance();
    }

    const std::auto_ptr<XMLControl>
        xmlControl(xmlRegistry->newXMLControl(data->getDataType()));
    const std::auto_ptr<xml::lite::Document> doc(xmlControl->toXML(data));

    io::ByteStream bs;
    doc->getRootElement()->print(bs);

    return bs.stream().str();
}

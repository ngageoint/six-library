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

#include <logging/NullLogger.h>
#include <six/XMLControl.h>

namespace six
{
XMLControl::XMLControl(logging::Logger* log, bool ownLog) :
    mLog(NULL),
    mOwnLog(false)
{
    setLogger(log, ownLog);
}

XMLControl::~XMLControl()
{
    if (mLog && mOwnLog)
    {
        delete mLog;
    }
}

void XMLControl::setLogger(logging::Logger* log, bool own)
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

std::string XMLControl::getDefaultURI(const Data& data)
{
    const std::string dataTypeStr =
            dataTypeToString(data.getDataType(), false);

    return ("urn:" + dataTypeStr + ":" + data.getVersion());
}

std::string XMLControl::getVersionFromURI(const xml::lite::Document* doc)
{
    const std::string uri = doc->getRootElement()->getUri();
    if (!(str::startsWith(uri, "urn:SICD:") ||
          str::startsWith(uri, "urn:SIDD:")))
    {
        throw except::Exception(Ctxt(
                "Unable to transform XML DES: Invalid XML namespace URI: " +
                uri));
    }

    return uri.substr(9); // remove "urn:SIxx:" from beginning
}

void XMLControl::getVersionFromURI(const xml::lite::Document* doc,
                                   std::vector<std::string>& version)
{
    splitVersion(getVersionFromURI(doc), version);
}

void XMLControl::splitVersion(const std::string& versionStr,
                              std::vector<std::string>& version)
{
    version = str::split(versionStr, ".");

    if (version.size() < 2)
    {
        throw except::Exception(Ctxt("Invalid version string: " + versionStr));
    }
}

xml::lite::Document* XMLControl::toXML(const Data* data)
{
    return toXMLImpl(data);
}

Data* XMLControl::fromXML(const xml::lite::Document* doc)
{
    Data* const data = fromXMLImpl(doc);
    data->setVersion(getVersionFromURI(doc));
    return data;
}

std::string XMLControl::dataTypeToString(DataType dataType, bool appendXML)
{
    std::string str;

    switch (dataType)
    {
    case DataType::COMPLEX:
        str = "SICD";
        break;
    case DataType::DERIVED:
        str = "SIDD";
        break;
    default:
        throw except::Exception(Ctxt("Invalid data type " +
                                         str::toString(dataType)));
    }

    if (appendXML)
    {
        str += "_XML";
    }

    return str;
}
}

/* =========================================================================
 * This file is part of six.sicd-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
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

#include <six/sicd/ComplexXMLControl.h>
#include <six/sicd/ComplexData.h>
#include <six/sicd/ComplexXMLParser040.h>
#include <six/sicd/ComplexXMLParser041.h>
#include <six/sicd/ComplexXMLParser10x.h>

namespace six
{
namespace sicd
{
ComplexXMLControl::ComplexXMLControl(logging::Logger* log, bool ownLog) :
    XMLControl(log, ownLog)
{
}

Data* ComplexXMLControl::fromXMLImpl(const xml::lite::Document* doc)
{
    return getParser(getVersionFromURI(doc))->fromXML(doc);
}

xml::lite::Document* ComplexXMLControl::toXMLImpl(const Data* data)
{
    if (data->getDataType() != DataType::COMPLEX)
    {
        throw except::Exception(Ctxt("Data must be SICD"));
    }

    const ComplexData* const sicd(reinterpret_cast<const ComplexData*>(data));
    return getParser(data->getVersion())->toXML(sicd);
}

std::auto_ptr<ComplexXMLParser>
ComplexXMLControl::getParser(const std::string& version) const
{
    std::auto_ptr<ComplexXMLParser> parser;

    std::vector<std::string> versionParts;
    splitVersion(version, versionParts);

    if (versionParts.size() != 3)
    {
        throw except::Exception(
            Ctxt("Unsupported SICD Version: " + version));
    }

    const std::string& majorVersion(versionParts.at(0));
    const std::string& minorVersion(versionParts.at(1));
    const std::string& patchVersion(versionParts.at(2));

    // six.sicd only currently supports --
    //   SICD 0.4.0
    //   SICD 0.4.1
    //   SICD 1.0.0
    if (majorVersion == "0")
    {
        if (minorVersion == "4")
        {
            if (patchVersion == "0")
                parser.reset(new ComplexXMLParser040(version, mLog));
            else if (patchVersion == "1")
                parser.reset(new ComplexXMLParser041(version, mLog));
        }
    }
    else if (majorVersion == "1")
    {
        if (minorVersion == "0")
        {
            if (patchVersion == "0")
                parser.reset(new ComplexXMLParser10x(version, mLog));
        }
    }

    // Validation check that we found a suitable Parser
    if (parser.get() == NULL)
    {
        throw except::Exception(
            Ctxt("Unsupported SICD Version: " + version));
    }

    return parser;
}
}
}

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

#include <six/sidd/DerivedXMLControl.h>
#include <six/sidd/DerivedData.h>
#include <six/sidd/DerivedXMLParser100.h>
//#include <six/sidd/DerivedXMLParser110.h>

namespace six
{
namespace sidd
{
DerivedXMLControl::DerivedXMLControl(logging::Logger* log, bool ownLog) :
    XMLControl(log, ownLog)
{
}

Data* DerivedXMLControl::fromXMLImpl(const xml::lite::Document* doc)
{
    return getParser(getVersionFromURI(doc))->fromXML(doc);
}

xml::lite::Document* DerivedXMLControl::toXMLImpl(const Data* data)
{
    if (data->getDataType() != DataType::DERIVED)
    {
        throw except::Exception(Ctxt("Data must be SIDD"));
    }

    const DerivedData* const sidd(reinterpret_cast<const DerivedData*>(data));
    return getParser(data->getVersion())->toXML(sidd);
}

std::auto_ptr<DerivedXMLParser>
DerivedXMLControl::getParser(const std::string& version) const
{
    std::auto_ptr<DerivedXMLParser> parser;

    std::vector<std::string> versionParts;
    splitVersion(version, versionParts);

    if (versionParts.size() != 3)
    {
        throw except::Exception(
            Ctxt("Unsupported SIDD Version: " + version));
    }

    const std::string& majorVersion(versionParts[0]);
    const std::string& minorVersion(versionParts[1]);
    const std::string& patchVersion(versionParts[2]);

    // six.sidd only currently supports --
    //   SIDD 1.0.0
    //   SIDD 1.1.0
    if (majorVersion == "1" && patchVersion == "0")
    {
        if (minorVersion == "0")
        {
            parser.reset(new DerivedXMLParser100(mLog));
        }
        else if (minorVersion == "1")
        {
            //parser.reset(new DerivedXMLParser110(mLog));
        }
    }

    // Validation check that we found a suitable Parser
    if (parser.get() == NULL)
    {
        throw except::Exception(
            Ctxt("Unsupported SIDD Version: " + version));
    }

    return parser;
}
}
}
